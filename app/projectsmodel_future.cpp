/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectsmodel_future.h"
#include "localprojectsmanager.h"
#include "inpututils.h"
#include "merginuserauth.h"

ProjectsModel_future::ProjectsModel_future(
  MerginApi *merginApi,
  ProjectModelTypes modelType,
  LocalProjectsManager &localProjectsManager,
  QObject *parent ) :
  QAbstractListModel( parent ),
  mBackend( merginApi ),
  mLocalProjectsManager( localProjectsManager ),
  mModelType( modelType )
{
  QObject::connect( mBackend, &MerginApi::syncProjectStatusChanged, this, &ProjectsModel_future::onProjectSyncProgressChanged );
  QObject::connect( mBackend, &MerginApi::syncProjectFinished, this, &ProjectsModel_future::onProjectSyncFinished );
  QObject::connect( mBackend, &MerginApi::projectDetached, this, &ProjectsModel_future::onProjectDetachedFromMergin );
  QObject::connect( mBackend, &MerginApi::projectAttachedToMergin, this, &ProjectsModel_future::onProjectAttachedToMergin );


  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsByNameFinished, this, &ProjectsModel_future::onListProjectsByNameFinished );
    loadLocalProjects(); // at app start, we need to fill model with local projects
  }
  else if ( mModelType != ProjectModelTypes::RecentProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsFinished, this, &ProjectsModel_future::onListProjectsFinished );
  }
  else
  {
    // Implement RecentProjectsModel type
  }

  QObject::connect( &mLocalProjectsManager, &LocalProjectsManager::localProjectAdded, this, &ProjectsModel_future::onProjectAdded );
  QObject::connect( &mLocalProjectsManager, &LocalProjectsManager::aboutToRemoveLocalProject, this, &ProjectsModel_future::onAboutToRemoveProject );
  QObject::connect( &mLocalProjectsManager, &LocalProjectsManager::localProjectDataChanged, this, &ProjectsModel_future::onProjectDataChanged );
  QObject::connect( &mLocalProjectsManager, &LocalProjectsManager::dataDirReloaded, this, &ProjectsModel_future::loadLocalProjects );
}

QVariant ProjectsModel_future::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  std::shared_ptr<Project_future> project = mProjects.at( index.row() );

  switch ( role ) {
    case ProjectName: return QVariant( project->projectName() );
    case ProjectNamespace: return QVariant( project->projectNamespace() );
    case ProjectFullName: return QVariant( project->projectId() );
    case ProjectIsLocal: return QVariant( project->isLocal() );
    case ProjectIsMergin: return QVariant( project->isMergin() );
    case ProjectStatus: return QVariant( LocalProjectsManager::currentProjectStatus( project ) );
    case ProjectIsValid: return QVariant( !project->isLocal() || ( project->isLocal() && project->local->projectError.isEmpty() ) );
    case ProjectDescription: {
      if ( project->isLocal() && !project->local->projectError.isEmpty() )
        return project->local->projectError;

      QFileInfo fi( project->local->projectDir );
      return fi.lastModified();
    }
    default: {
      if ( !project->isMergin() ) return QVariant();

      // Roles only for projects that has mergin part
      if ( role == ProjectPending ) return QVariant( project->mergin->pending );
      else if ( role == ProjectSyncProgress ) return QVariant( project->mergin->progress );
      else if ( role == ProjectRemoteError ) return QVariant( project->mergin->remoteError );
      return QVariant();
    }
  }
}

QModelIndex ProjectsModel_future::index( int row, int col, const QModelIndex &parent ) const
{
  Q_UNUSED( col )
  Q_UNUSED( parent )
  return createIndex( row, 0, nullptr );
}

QHash<int, QByteArray> ProjectsModel_future::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[Roles::ProjectName]         = QStringLiteral( "ProjectName" ).toLatin1();
  roles[Roles::ProjectNamespace]    = QStringLiteral( "ProjectNamespace" ).toLatin1();
  roles[Roles::ProjectFullName]     = QStringLiteral( "ProjectFullName" ).toLatin1();
  roles[Roles::ProjectIsLocal]      = QStringLiteral( "ProjectIsLocal" ).toLatin1();
  roles[Roles::ProjectIsMergin]     = QStringLiteral( "ProjectIsMergin" ).toLatin1();
  roles[Roles::ProjectDescription]  = QStringLiteral( "ProjectDescription" ).toLatin1();
  roles[Roles::ProjectPending]      = QStringLiteral( "ProjectPending" ).toLatin1();
  roles[Roles::ProjectSyncProgress] = QStringLiteral( "ProjectSyncProgress" ).toLatin1();
  return roles;
}

int ProjectsModel_future::rowCount( const QModelIndex & ) const
{
  return mProjects.count();
}

void ProjectsModel_future::listProjects( int page, QString searchExpression )
{
  if ( mModelType == LocalProjectsModel )
  {
    InputUtils::log( "Input", "Can not call listProjects API on LocalProjectsModel" );
    // maybe call listProjectsByName();
    return;
  }

  mLastRequestId = mBackend->listProjects( "", modelTypeToFlag(), searchExpression, page );
}

void ProjectsModel_future::listProjectsByName()
{
  if ( mModelType != LocalProjectsModel )
  {
    InputUtils::log( "Input", "Can not call listProjectsByName API on not LocalProjectsModel" );
    return;
  }

  mLastRequestId = mBackend->listProjectsByName( projectNames() );
}

void ProjectsModel_future::mergeProjects( const MerginProjectsList &merginProjects, Transactions pendingProjects, bool keepPrevious )
{
  LocalProjectsList localProjects = mLocalProjectsManager.projects();

  qDebug() << "PMR: mergeProjects(): # of local projects = " << localProjects.size() << " # of mergin projects = " << merginProjects.size();

  if ( !keepPrevious )
    mProjects.clear();

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    // Keep all local projects and ignore all not downloaded remote projects
    for ( const auto &localProject : localProjects )
    {
      std::shared_ptr<Project_future> project = std::shared_ptr<Project_future>( new Project_future() );
      std::unique_ptr<LocalProject_future> local = std::unique_ptr<LocalProject_future>( new LocalProject_future( localProject ) );

      project->local = std::move( local );

      MerginProject_future remoteEntry;
      remoteEntry.projectName = project->local->projectName;
      remoteEntry.projectNamespace = project->local->projectNamespace;

      if ( merginProjects.contains( remoteEntry ) )
      {
        int i = merginProjects.indexOf( remoteEntry );
        project->mergin = std::unique_ptr<MerginProject_future>( new MerginProject_future( merginProjects[i] ) );

        if ( pendingProjects.contains( project->mergin->id() ) )
        {
          TransactionStatus projectTransaction = pendingProjects.value( project->mergin->id() );
          project->mergin->progress = projectTransaction.transferedSize / projectTransaction.totalSize;
          project->mergin->pending = true;
        }
      }

      mProjects << project;
    }
  }
  else if ( mModelType != ProjectModelTypes::RecentProjectsModel )
  {
    // Keep all remote projects and ignore all non mergin projects from local projects
    for ( const auto &remoteEntry : merginProjects )
    {
      std::shared_ptr<Project_future> project = std::shared_ptr<Project_future>( new Project_future() );
      project->mergin = std::unique_ptr<MerginProject_future>( new MerginProject_future( remoteEntry ) );

      if ( pendingProjects.contains( project->mergin->id() ) )
      {
        TransactionStatus projectTransaction = pendingProjects.value( project->mergin->id() );
        project->mergin->progress = projectTransaction.transferedSize / projectTransaction.totalSize;
        project->mergin->pending = true;
      }

      // find downloaded projects
      LocalProject_future localProject;
      localProject.projectName = project->mergin->projectName;
      localProject.projectNamespace = project->mergin->projectNamespace;

      if ( localProjects.contains( localProject ) )
      {
        int ix = localProjects.indexOf( localProject );
        project->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( localProjects[ix] ) );
      }

      mProjects << project;
    }
  }
}

int ProjectsModel_future::serverProjectsCount() const
{
  return mServerProjectsCount;
}

void ProjectsModel_future::onListProjectsFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, int projectsCount, int page, QString requestId )
{
  qDebug() << "PMR: onListProjectsFinished(): received response with requestId = " << requestId;
  if ( mLastRequestId != requestId )
  {
    qDebug() << "PMR: onListProjectsFinished(): ignoring request with id " << requestId;
    return;
  }

  setServerProjectsCount( projectsCount );

  qDebug() << "PMR: onListProjectsFinished(): project count =  " << projectsCount << " but mergin projects emited: " << merginProjects.size();

  beginResetModel();
  mergeProjects( merginProjects, pendingProjects, page != 1 ); // throw projects only if paginating first page
  printProjects();
  endResetModel();
}

void ProjectsModel_future::onListProjectsByNameFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId )
{
  qDebug() << "PMR: onListProjectsByNameFinished(): received response with requestId = " << requestId;
  if ( mLastRequestId != requestId )
  {
    qDebug() << "PMR: onListProjectsByNameFinished(): ignoring request with id " << requestId;
    return;
  }

  beginResetModel();
  mergeProjects( merginProjects, pendingProjects );
  printProjects();
  endResetModel();
}

void ProjectsModel_future::syncProject( QString projectNamespace, QString projectName )
{
  std::shared_ptr<Project_future> project = projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );

  if ( project == nullptr )
  {
    qDebug() << "PMR: project" << MerginApi::getFullProjectName(projectNamespace, projectName) << "not in projects list";
    return;
  }

  if ( !project->isMergin() )
  {
    qDebug() << "PMR: project" << MerginApi::getFullProjectName(projectNamespace, projectName) << "is not a mergin project";
    return;
  }

  if ( project->mergin->pending )
  {
    qDebug() << "PMR: project" << MerginApi::getFullProjectName(projectNamespace, projectName) << "is already syncing";
    return;
  }

  if ( project->mergin->status == _NoVersion || project->mergin->status == _OutOfDate )
  {
    qDebug() << "PMR: updating project:" << project->mergin->id();

    bool useAuth = !mBackend->userAuth()->hasAuthData() && mModelType == ProjectModelTypes::ExploreProjectsModel;
    mBackend->updateProject( projectNamespace, projectName, useAuth );
  }
  else if ( project->mergin->status == _Modified )
  {
    qDebug() << "PMR: uploading project:" << project->mergin->id();
    mBackend->uploadProject( projectNamespace, projectName );
  }
}

void ProjectsModel_future::stopProjectSync( QString projectNamespace, QString projectName )
{
  std::shared_ptr<Project_future> project = projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );

  if ( project == nullptr )
  {
    qDebug() << "PMR: project" << MerginApi::getFullProjectName(projectNamespace, projectName) << "not in projects list";
    return;
  }

  if ( !project->isMergin() )
  {
    qDebug() << "PMR: project" << MerginApi::getFullProjectName(projectNamespace, projectName) << "is not a mergin project";
    return;
  }

  if ( !project->mergin->pending )
  {
    qDebug() << "PMR: project" << MerginApi::getFullProjectName(projectNamespace, projectName) << "is not pending";
    return;
  }

  if ( project->mergin->status == _NoVersion || project->mergin->status == _OutOfDate )
  {
    qDebug() << "PMR: cancelling update of project:" << project->mergin->id();
    mBackend->updateCancel( project->mergin->id() );
  }
  else if ( project->mergin->status == _Modified )
  {
    qDebug() << "PMR: cancelling upload of project:" << project->mergin->id();
    mBackend->uploadCancel( project->mergin->id() );
  }
}

void ProjectsModel_future::onProjectSyncFinished( const QString &projectDir, const QString &projectFullName, bool successfully )
{
  Q_UNUSED( projectDir )

  std::shared_ptr<Project_future> project = projectFromId( projectFullName );
  if ( !project || !project->isMergin() || !successfully )
    return;

  project->mergin->pending = false;
  project->mergin->progress = 0;

  QModelIndex ix = index( mProjects.indexOf( project ) );
  emit dataChanged( ix, ix );

  qDebug() << "PMR: Project " << projectFullName << " finished sync";
}

void ProjectsModel_future::onProjectSyncProgressChanged( const QString &projectFullName, qreal progress )
{
  std::shared_ptr<Project_future> project = projectFromId( projectFullName );
  if ( !project || !project->isMergin() )
    return;

  project->mergin->pending = progress >= 0;
  project->mergin->progress = progress >= 0 ? progress : 0;

  QModelIndex ix = index( mProjects.indexOf( project ) );
  emit dataChanged( ix, ix );

  qDebug() << "PMR: Project " << projectFullName << " changed sync progress to " << progress;
}

void ProjectsModel_future::onProjectAdded( const LocalProject_future &project )
{
  // Check if such project is already in project list
  std::shared_ptr<Project_future> proj = projectFromId( project.id() );
  if ( proj )
  {
    // add local information ~ project downloaded
    proj->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( project ) );

    QModelIndex ix = index( mProjects.indexOf( proj ) );
    emit dataChanged( ix, ix );
  }
  else if ( mModelType == LocalProjectsModel )
  {
    // add project to project list ~ project created
    std::shared_ptr<Project_future> newProject = std::shared_ptr<Project_future>( new Project_future() );
    newProject->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( project ) );

    int insertIndex = mProjects.size() == 0 ? 0 : mProjects.size() - 1;
    beginInsertRows( QModelIndex(), insertIndex, insertIndex + 1 );
    mProjects << newProject;
    endInsertRows();
  }

  qDebug() << "PMR: Added project" << project.id();
}

void ProjectsModel_future::onAboutToRemoveProject( const LocalProject_future project )
{
  std::shared_ptr<Project_future> proj = projectFromId( project.id() );

  if ( proj )
  {
    int removeIndex = mProjects.indexOf( proj );

    beginRemoveRows( QModelIndex(), removeIndex, removeIndex );
    mProjects.removeOne( proj );
    endRemoveRows();

    qDebug() << "PMR: Deleted project" << project.id();
  }
}

void ProjectsModel_future::onProjectDataChanged( const LocalProject_future &project )
{
  std::shared_ptr<Project_future> proj = projectFromId( project.id() );

  if ( proj )
  {
    proj->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( project ) );
    QModelIndex editIndex = index( mProjects.indexOf( proj ) );

    emit dataChanged( editIndex, editIndex );
  }
  qDebug() << "PMR: Data changed in project" << project.id();
}

void ProjectsModel_future::onProjectDetachedFromMergin( const QString &projectFullName )
{
  std::shared_ptr<Project_future> proj = projectFromId( projectFullName );

  if ( proj )
  {
    proj->mergin = nullptr;
    QModelIndex editIndex = index( mProjects.indexOf( proj ) );

    emit dataChanged( editIndex, editIndex );

    // This project should also be removed from project list for remote project model types,
    // however, currently one needs to click on "My projects/Shared/Explore" and that sends
    // another listProjects request. In new list this project will not be shown.
  }
}

void ProjectsModel_future::onProjectAttachedToMergin( const QString &projectFullName )
{
  // To ensure project will be in sync with server, send listProjectByName request.
  // In theory we could send that request only for this one project.
  listProjectsByName();

  qDebug() << "PMR: Project attached to mergin " << projectFullName;
}

void ProjectsModel_future::setServerProjectsCount( int serverProjectsCount )
{
  if ( mServerProjectsCount == serverProjectsCount )
    return;

  mServerProjectsCount = serverProjectsCount;
  emit serverProjectsCountChanged( mServerProjectsCount );
}

QString ProjectsModel_future::modelTypeToFlag() const
{
  switch ( mModelType )
  {
    case MyProjectsModel:
      return QStringLiteral( "created" );
    case SharedProjectsModel:
      return QStringLiteral( "shared" );
    default:
      return QStringLiteral( "" );
  }
}

void ProjectsModel_future::printProjects() const // TODO: Helper function, remove after refactoring is done
{
  qDebug() << "PMR: Model " << this << " with type " << modelTypeToFlag() << " has projects: ";
  for ( const auto &proj : mProjects )
  {
    QString lcl = proj->isLocal() ? "local" : "";
    QString mrgn = proj->isMergin() ? "mergin" : "";

    if ( proj->isLocal() )
    {
      qDebug() << " - " << proj->local->projectNamespace << proj->local->projectName << lcl << mrgn;
    }
    else if ( proj->isMergin() )
    {
      qDebug() << " - " << proj->mergin->projectNamespace << proj->mergin->projectName << lcl << mrgn;
    }
  }
}

QStringList ProjectsModel_future::projectNames() const
{
  QStringList projectNames;
  LocalProjectsList projects = mLocalProjectsManager.projects();

  for ( const auto &proj : projects )
  {
    if ( !proj.projectName.isEmpty() && !proj.projectNamespace.isEmpty() )
      projectNames << proj.id();
  }

  return projectNames;
}

void ProjectsModel_future::loadLocalProjects()
{
  if ( mModelType == LocalProjectsModel )
  {
    beginResetModel();
    mergeProjects( MerginProjectsList(), Transactions() ); // Fills model with local projects
    endResetModel();
  }
}

bool ProjectsModel_future::containsProject( QString projectId ) const
{
  std::shared_ptr<Project_future> proj = projectFromId( projectId );
  return proj != nullptr;
}

std::shared_ptr<Project_future> ProjectsModel_future::projectFromId( QString projectId ) const
{
  for ( int ix = 0; ix < mProjects.size(); ++ix )
  {
    if ( mProjects[ix]->isMergin() && mProjects[ix]->mergin->id() == projectId )
      return mProjects[ix];
    else if ( mProjects[ix]->isLocal() && mProjects[ix]->local->id() == projectId )
      return mProjects[ix];
  }

  return nullptr;
}

ProjectModelTypes ProjectsModel_future::modelType() const
{
    return mModelType;
}
