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

ProjectsModel_future::ProjectsModel_future( QObject *parent ) : QAbstractListModel( parent )
{
  qDebug() << "PMR: Instantiated ProjectsModel! " << this << "MerginAPI: " << mBackend << "LPM:" << mLocalProjectsManager << "Type: " << mModelType;
}

void ProjectsModel_future::initializeProjectsModel()
{
  if ( !mBackend || !mLocalProjectsManager || mModelType == EmptyProjectsModel ) // Model is not set up properly yet
    return;

  qDebug() << "PMR: initializing projects model " << this;

  QObject::connect( mBackend, &MerginApi::syncProjectStatusChanged, this, &ProjectsModel_future::onProjectSyncProgressChanged );
  QObject::connect( mBackend, &MerginApi::syncProjectFinished, this, &ProjectsModel_future::onProjectSyncFinished );
  QObject::connect( mBackend, &MerginApi::projectDetached, this, &ProjectsModel_future::onProjectDetachedFromMergin );
  QObject::connect( mBackend, &MerginApi::projectAttachedToMergin, this, &ProjectsModel_future::onProjectAttachedToMergin );

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsByNameFinished, this, &ProjectsModel_future::onListProjectsByNameFinished );
    loadLocalProjects();
  }
  else if ( mModelType != ProjectModelTypes::RecentProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsFinished, this, &ProjectsModel_future::onListProjectsFinished );
  }
  else
  {
    // Implement RecentProjectsModel type
  }

  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::localProjectAdded, this, &ProjectsModel_future::onProjectAdded );
  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::aboutToRemoveLocalProject, this, &ProjectsModel_future::onAboutToRemoveProject );
  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::localProjectDataChanged, this, &ProjectsModel_future::onProjectDataChanged );
  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::dataDirReloaded, this, &ProjectsModel_future::loadLocalProjects );

  emit modelInitialized();
}

QVariant ProjectsModel_future::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( index.row() < 0 || index.row() >= mProjects.size() )
    return QVariant();

  std::shared_ptr<Project_future> project = mProjects.at( index.row() );

  switch ( role ) {
    case ProjectName: return QVariant( project->projectName() );
    case ProjectNamespace: return QVariant( project->projectNamespace() );
    case ProjectFullName: return QVariant( project->projectId() );
    case ProjectId: return QVariant( project->projectId() );
    case ProjectIsLocal: return QVariant( project->isLocal() );
    case ProjectIsMergin: return QVariant( project->isMergin() );
    case ProjectSyncStatus: return QVariant( project->isMergin() ? project->mergin->status : ProjectStatus::NoVersion );
    case ProjectFilePath: return QVariant( project->isLocal() ? project->local->qgisProjectFilePath : QString() );
    case ProjectDirectory: return QVariant( project->isLocal() ? project->local->projectDir : QString() );
    case ProjectIsValid: {
      if ( !project->isLocal() )
        return true; // Mergin projects are by default valid, remote error only affects syncing, not opening of a project
      return project->local->projectError.isEmpty();
    }
    case ProjectDescription: {
      if ( project->isLocal() )
      {
        if ( !project->local->projectError.isEmpty() )
        {
          return QVariant( project->local->projectError );
        }
        QFileInfo fi( project->local->projectDir );
        return QVariant( fi.lastModified().toLocalTime() ); // Maybe use better timestamp format https://doc.qt.io/qt-5/qdatetime.html#toString-3
      }
      else if ( project->isMergin() )
      {
        return QVariant( project->mergin->serverUpdated );
      }
      return QVariant(); // This should not happen
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
  roles[Roles::ProjectId]           = QStringLiteral( "ProjectId" ).toLatin1();
  roles[Roles::ProjectDirectory]    = QStringLiteral( "ProjectDirectory" ).toLatin1();
  roles[Roles::ProjectIsLocal]      = QStringLiteral( "ProjectIsLocal" ).toLatin1();
  roles[Roles::ProjectIsMergin]     = QStringLiteral( "ProjectIsMergin" ).toLatin1();
  roles[Roles::ProjectSyncStatus]   = QStringLiteral( "ProjectSyncStatus" ).toLatin1();
  roles[Roles::ProjectIsValid]      = QStringLiteral( "ProjectIsValid" ).toLatin1();
  roles[Roles::ProjectFilePath]     = QStringLiteral( "ProjectFilePath" ).toLatin1();
  roles[Roles::ProjectDescription]  = QStringLiteral( "ProjectDescription" ).toLatin1();
  roles[Roles::ProjectPending]      = QStringLiteral( "ProjectPending" ).toLatin1();
  roles[Roles::ProjectSyncProgress] = QStringLiteral( "ProjectSyncProgress" ).toLatin1();
  roles[Roles::ProjectRemoteError]  = QStringLiteral( "ProjectRemoteError" ).toLatin1();
  return roles;
}

int ProjectsModel_future::rowCount( const QModelIndex & ) const
{
  return mProjects.count();
}

void ProjectsModel_future::listProjects( const QString &searchExpression, int page )
{
  if ( mModelType == LocalProjectsModel )
  {
    listProjectsByName();
    return;
  }

  mLastRequestId = mBackend->listProjects( searchExpression, modelTypeToFlag(), "", page );
}

void ProjectsModel_future::listProjectsByName()
{
  if ( mModelType != LocalProjectsModel )
  {
    return;
  }

  mLastRequestId = mBackend->listProjectsByName( projectNames() );
}

bool ProjectsModel_future::hasMoreProjects() const
{
  if ( mProjects.size() < mServerProjectsCount )
    return true;

  return false;
}

void ProjectsModel_future::fetchAnotherPage( const QString &searchExpression )
{
  listProjects( searchExpression, mPaginatedPage + 1 );
}

QVariant ProjectsModel_future::dataFrom( int fromRole, QVariant fromValue, int desiredRole ) const
{
  switch ( fromRole )
  {
    case ProjectId:
    {
      std::shared_ptr<Project_future> project = projectFromId( fromValue.toString() );
      if ( project )
      {
        QModelIndex ix = index( mProjects.indexOf( project ) );
        return data( ix, desiredRole );
      }
      return QVariant();
    }

    case ProjectFilePath:
    {
      for ( int i = 0; i < mProjects.size(); i++ )
      {
        if ( mProjects[i]->isLocal() && mProjects[i]->local->qgisProjectFilePath == fromValue.toString() )
        {
          QModelIndex ix = index( i );
          return data( ix, desiredRole );
        }
      }
    }
    default: return QVariant();
  }

  return QVariant();
}

void ProjectsModel_future::onListProjectsFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, int projectsCount, int page, QString requestId )
{
  qDebug() << "PMR: onListProjectsFinished(): received response with requestId = " << requestId;
  if ( mLastRequestId != requestId )
  {
    qDebug() << "PMR: onListProjectsFinished(): ignoring request with id " << requestId;
    return;
  }

  qDebug() << "PMR: onListProjectsFinished(): project count =  " << projectsCount << " but mergin projects emited: " << merginProjects.size();

  if ( page == 1 )
  {
    // if we are populating first page, reset model and throw away previous projects
    beginResetModel();
    mergeProjects( merginProjects, pendingProjects, false );
    printProjects();
    endResetModel();
  }
  else
  {
    // paginating next page, keep previous projects and emit model add items
    beginInsertRows( QModelIndex(), mProjects.size(), mProjects.size() + merginProjects.size() - 1 );
    mergeProjects( merginProjects, pendingProjects, true );
    printProjects();
    endInsertRows();
  }

  mServerProjectsCount = projectsCount;
  mPaginatedPage = page;
  emit hasMoreProjectsChanged();
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

void ProjectsModel_future::mergeProjects( const MerginProjectsList &merginProjects, Transactions pendingProjects, bool keepPrevious )
{
  LocalProjectsList localProjects = mLocalProjectsManager->projects();

  qDebug() << "PMR: mergeProjects(): # of local projects = " << localProjects.size() << " # of mergin projects = " << merginProjects.size();

  if ( !keepPrevious )
    mProjects.clear();

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    // Keep all local projects and ignore all not downloaded remote projects
    for ( const auto &localProject : localProjects )
    {
      std::shared_ptr<Project_future> project = std::shared_ptr<Project_future>( new Project_future() );
      project->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( localProject ) );

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
        project->mergin->status = ProjectStatus::projectStatus( project );
      }
      else if ( project->local->localVersion > -1 )
      {
        // this is indeed a Mergin project, it has metadata folder in it
        project->mergin = std::unique_ptr<MerginProject_future>( new MerginProject_future() );
        project->mergin->projectName = project->local->projectName;
        project->mergin->projectNamespace = project->local->projectNamespace;
        project->mergin->status = ProjectStatus::projectStatus( project );
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
      project->mergin->status = ProjectStatus::projectStatus( project );

      mProjects << project;
    }
  }
}

void ProjectsModel_future::syncProject( const QString &projectId )
{
  std::shared_ptr<Project_future> project = projectFromId( projectId );

  if ( project == nullptr )
  {
    qDebug() << "PMR: project" << projectId << "not in projects list";
    return;
  }

  if ( !project->isMergin() )
  {
    qDebug() << "PMR: project" << projectId << "is not a mergin project";
    return;
  }

  if ( project->mergin->pending )
  {
    qDebug() << "PMR: project" << projectId << "is already syncing";
    return;
  }

  if ( project->mergin->status == ProjectStatus::NoVersion || project->mergin->status == ProjectStatus::OutOfDate )
  {
    qDebug() << "PMR: updating project:" << project->mergin->id();

    bool useAuth = !mBackend->userAuth()->hasAuthData() && mModelType == ProjectModelTypes::PublicProjectsModel;
    mBackend->updateProject( project->mergin->projectNamespace, project->mergin->projectName, useAuth );
  }
  else if ( project->mergin->status == ProjectStatus::Modified )
  {
    qDebug() << "PMR: uploading project:" << project->mergin->id();
    mBackend->uploadProject( project->mergin->projectNamespace, project->mergin->projectName );
  }
}

void ProjectsModel_future::stopProjectSync( const QString &projectId )
{
  std::shared_ptr<Project_future> project = projectFromId( projectId );

  if ( project == nullptr )
  {
    qDebug() << "PMR: project" << projectId << "not in projects list";
    return;
  }

  if ( !project->isMergin() )
  {
    qDebug() << "PMR: project" << projectId << "is not a mergin project";
    return;
  }

  if ( !project->mergin->pending )
  {
    qDebug() << "PMR: project" << projectId << "is not pending";
    return;
  }

  if ( project->mergin->status == ProjectStatus::NoVersion || project->mergin->status == ProjectStatus::OutOfDate )
  {
    qDebug() << "PMR: cancelling update of project:" << project->mergin->id();
    mBackend->updateCancel( project->mergin->id() );
  }
  else if ( project->mergin->status == ProjectStatus::Modified )
  {
    qDebug() << "PMR: cancelling upload of project:" << project->mergin->id();
    mBackend->uploadCancel( project->mergin->id() );
  }
}

void ProjectsModel_future::removeLocalProject( const QString &projectId )
{
  mLocalProjectsManager->removeLocalProject( projectId );
}

void ProjectsModel_future::migrateProject( const QString &projectId )
{
  // if it is indeed a local project
  std::shared_ptr<Project_future> project = projectFromId( projectId );

  if ( project->isLocal() )
    mBackend->migrateProjectToMergin( project->local->projectName );
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
    if ( proj->isMergin() )
      proj->mergin->status = ProjectStatus::projectStatus( proj );

    QModelIndex ix = index( mProjects.indexOf( proj ) );
    emit dataChanged( ix, ix );
  }
  else if ( mModelType == LocalProjectsModel )
  {
    // add project to project list ~ project created
    std::shared_ptr<Project_future> newProject = std::shared_ptr<Project_future>( new Project_future() );
    newProject->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( project ) );

    int insertIndex = mProjects.size();

    beginInsertRows( QModelIndex(), insertIndex, insertIndex );
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
    if ( mModelType == LocalProjectsModel )
    {
      int removeIndex = mProjects.indexOf( proj );

      beginRemoveRows( QModelIndex(), removeIndex, removeIndex );
      mProjects.removeOne( proj );
      endRemoveRows();

      qDebug() << "PMR: Deleted project" << project.id();
    }
    else
    {
      // just remove local part
      proj->local.reset();

      if ( proj->isMergin() )
        proj->mergin->status = ProjectStatus::projectStatus( proj );

      QModelIndex ix = index( mProjects.indexOf( proj ) );
      emit dataChanged( ix, ix );
    }
  }
}

void ProjectsModel_future::onProjectDataChanged( const LocalProject_future &project )
{
  std::shared_ptr<Project_future> proj = projectFromId( project.id() );

  if ( proj )
  {
    proj->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( project ) );
    if ( proj->isMergin() )
      proj->mergin->status = ProjectStatus::projectStatus( proj );

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
    proj->mergin.reset();
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

void ProjectsModel_future::setMerginApi( MerginApi *merginApi )
{
  if ( !merginApi || mBackend == merginApi )
    return;

  mBackend = merginApi;
  qDebug() << "PMR: New MA: " << mBackend;
  initializeProjectsModel();
}

void ProjectsModel_future::setLocalProjectsManager( LocalProjectsManager *localProjectsManager )
{
  if ( !localProjectsManager || mLocalProjectsManager == localProjectsManager )
    return;

  mLocalProjectsManager = localProjectsManager;
  qDebug() << "PMR: New LPM: " << mLocalProjectsManager;
  initializeProjectsModel();
}

void ProjectsModel_future::setModelType( ProjectsModel_future::ProjectModelTypes modelType )
{
  if ( mModelType == modelType )
    return;

  mModelType = modelType;
  qDebug() << "PMR: New Type: " << mModelType;
  initializeProjectsModel();
}

QString ProjectsModel_future::modelTypeToFlag() const
{
  switch ( mModelType )
  {
    case CreatedProjectsModel:
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
  LocalProjectsList projects = mLocalProjectsManager->projects();

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
    printProjects();
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

ProjectsModel_future::ProjectModelTypes ProjectsModel_future::modelType() const
{
    return mModelType;
}
