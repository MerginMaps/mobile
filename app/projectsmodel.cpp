/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectsmodel.h"
#include "localprojectsmanager.h"
#include "inpututils.h"
#include "merginuserauth.h"
#include "coreutils.h"

ProjectsModel::ProjectsModel( QObject *parent ) : QAbstractListModel( parent )
{
}

void ProjectsModel::initializeProjectsModel()
{
  if ( !mBackend || !mLocalProjectsManager || mModelType == EmptyProjectsModel ) // Model is not set up properly yet
    return;

  QObject::connect( mBackend, &MerginApi::syncProjectStatusChanged, this, &ProjectsModel::onProjectSyncProgressChanged );
  QObject::connect( mBackend, &MerginApi::syncProjectFinished, this, &ProjectsModel::onProjectSyncFinished );
  QObject::connect( mBackend, &MerginApi::projectDetached, this, &ProjectsModel::onProjectDetachedFromMergin );
  QObject::connect( mBackend, &MerginApi::projectAttachedToMergin, this, &ProjectsModel::onProjectAttachedToMergin );
  QObject::connect( mBackend, &MerginApi::authChanged, this, &ProjectsModel::onAuthChanged );

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsByNameFinished, this, &ProjectsModel::onListProjectsByNameFinished );
    loadLocalProjects();
  }
  else if ( mModelType != ProjectModelTypes::RecentProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsFinished, this, &ProjectsModel::onListProjectsFinished );
  }
  else
  {
    // Implement RecentProjectsModel type
  }

  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::localProjectAdded, this, &ProjectsModel::onProjectAdded );
  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::aboutToRemoveLocalProject, this, &ProjectsModel::onAboutToRemoveProject );
  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::localProjectDataChanged, this, &ProjectsModel::onProjectDataChanged );
  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::dataDirReloaded, this, &ProjectsModel::loadLocalProjects );

  emit modelInitialized();
}

QVariant ProjectsModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( index.row() < 0 || index.row() >= mProjects.size() )
    return QVariant();

  std::shared_ptr<Project> project = mProjects.at( index.row() );

  switch ( role )
  {
    case ProjectName: return QVariant( project->projectName() );
    case ProjectNamespace: return QVariant( project->projectNamespace() );
    case ProjectFullName: return QVariant( project->projectFullName() );
    case ProjectId: return QVariant( project->projectId() );
    case ProjectIsLocal: return QVariant( project->isLocal() );
    case ProjectIsMergin: return QVariant( project->isMergin() );
    case ProjectSyncStatus: return QVariant( project->isMergin() ? project->mergin->status : ProjectStatus::NoVersion );
    case ProjectFilePath: return QVariant( project->isLocal() ? project->local->qgisProjectFilePath : QString() );
    case ProjectDirectory: return QVariant( project->isLocal() ? project->local->projectDir : QString() );
    case ProjectIsValid:
    {
      if ( !project->isLocal() )
        return true; // Mergin projects are by default valid, remote error only affects syncing, not opening of a project
      return project->local->projectError.isEmpty();
    }
    case ProjectDescription:
    {
      if ( project->isLocal() )
      {
        if ( !project->local->projectError.isEmpty() )
        {
          return QVariant( project->local->projectError );
        }
        QFileInfo fi( project->local->projectDir );
        // lastModified of projectDir is not reliable - gpkg file may have modified header after opening it. See more #1320
        return QVariant( tr( "Updated %1" ).arg( InputUtils::formatDateTimeDiff( fi.lastModified().toUTC() ) ) );
      }
      else if ( project->isMergin() )
      {
        return QVariant( tr( "Updated %1" ).arg( InputUtils::formatDateTimeDiff( project->mergin->serverUpdated.toUTC() ) ) );
      }

      // This should not happen
      CoreUtils::log( "Project error", "Found project that is not downloaded nor remote" );
      return QVariant();
    }
    default:
    {
      if ( !project->isMergin() ) return QVariant();

      // Roles only for projects that has mergin part
      if ( role == ProjectPending ) return QVariant( project->mergin->pending );
      else if ( role == ProjectSyncProgress ) return QVariant( project->mergin->progress );
      else if ( role == ProjectRemoteError ) return QVariant( project->mergin->remoteError );
      return QVariant();
    }
  }
}

QModelIndex ProjectsModel::index( int row, int col, const QModelIndex &parent ) const
{
  Q_UNUSED( col )
  Q_UNUSED( parent )
  return createIndex( row, 0, nullptr );
}

QHash<int, QByteArray> ProjectsModel::roleNames() const
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

int ProjectsModel::rowCount( const QModelIndex & ) const
{
  return mProjects.count();
}

void ProjectsModel::listProjects( const QString &searchExpression, int page )
{
  if ( mModelType == LocalProjectsModel )
  {
    listProjectsByName();
    return;
  }

  mLastRequestId = mBackend->listProjects( searchExpression, modelTypeToFlag(), "", page );

  if ( !mLastRequestId.isEmpty() )
  {
    setModelIsLoading( true );
    // clear only after requesting the very first page, otherwise we want to append results to the model
    if ( page == 1 )
      clearProjects();
  }
}

void ProjectsModel::listProjectsByName()
{
  if ( mModelType != LocalProjectsModel )
  {
    return;
  }

  mLastRequestId = mBackend->listProjectsByName( projectNames() );

  if ( !mLastRequestId.isEmpty() )
  {
    setModelIsLoading( true );
    clearProjects();
  }
}

bool ProjectsModel::hasMoreProjects() const
{
  return ( mProjects.size() < mServerProjectsCount );
}

void ProjectsModel::fetchAnotherPage( const QString &searchExpression )
{
  listProjects( searchExpression, mPaginatedPage + 1 );
}

void ProjectsModel::onListProjectsFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, int projectsCount, int page, QString requestId )
{
  if ( mLastRequestId != requestId )
  {
    return;
  }

  if ( page == 1 )
  {
    // if we are populating first page, reset model and throw away previous projects
    beginResetModel();
    mergeProjects( merginProjects, pendingProjects, false );
    endResetModel();
  }
  else
  {
    // paginating next page, keep previous projects and emit model add items
    beginInsertRows( QModelIndex(), mProjects.size(), mProjects.size() + merginProjects.size() - 1 );
    mergeProjects( merginProjects, pendingProjects, true );
    endInsertRows();
  }

  mServerProjectsCount = projectsCount;
  mPaginatedPage = page;
  emit hasMoreProjectsChanged();

  setModelIsLoading( false );
}

void ProjectsModel::onListProjectsByNameFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId )
{
  if ( mLastRequestId != requestId )
  {
    return;
  }

  beginResetModel();
  mergeProjects( merginProjects, pendingProjects );
  endResetModel();

  setModelIsLoading( false );
}

void ProjectsModel::mergeProjects( const MerginProjectsList &merginProjects, Transactions pendingProjects, bool keepPrevious )
{
  const LocalProjectsList localProjects = mLocalProjectsManager->projects();

  if ( !keepPrevious )
    mProjects.clear();

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    // Keep all local projects and ignore all not downloaded remote projects
    for ( const auto &localProject : localProjects )
    {
      std::shared_ptr<Project> project = std::shared_ptr<Project>( new Project() );
      project->local = std::unique_ptr<LocalProject>( localProject.clone() );

      const auto res = std::find_if( merginProjects.begin(), merginProjects.end(), [&project]( const MerginProject & me )
      {
        return ( project->local->projectName == me.projectName && project->local->projectNamespace == me.projectNamespace );
      } );

      if ( res != merginProjects.end() )
      {
        project->mergin = std::unique_ptr<MerginProject>( res->clone() );

        if ( pendingProjects.contains( project->mergin->id() ) )
        {
          TransactionStatus transaction = pendingProjects.value( project->mergin->id() );
          project->mergin->progress = transaction.totalSize != 0 ? transaction.transferedSize / transaction.totalSize : 0;
          project->mergin->pending = true;
          pendingProjects.remove( project->mergin->id() );
        }
        project->mergin->status = ProjectStatus::projectStatus( project );
      }
      else if ( project->local->localVersion > -1 )
      {
        // this is indeed a Mergin project, it has metadata folder in it
        project->mergin = std::unique_ptr<MerginProject>( new MerginProject() );
        project->mergin->projectName = project->local->projectName;
        project->mergin->projectNamespace = project->local->projectNamespace;
        project->mergin->status = ProjectStatus::projectStatus( project );
      }

      mProjects << project;
    }

    // lets check also for projects that are currently being downloaded and add them to local projects list
    Transactions::const_iterator i = pendingProjects.constBegin();

    while ( i != pendingProjects.constEnd() )
    {
      // all projects that are left in transactions are those being downloaded, so we add all of them
      std::shared_ptr<Project> project = std::make_shared<Project>();
      project->mergin = std::unique_ptr<MerginProject>( new MerginProject() );

      MerginApi::extractProjectName( i.key(), project->mergin->projectNamespace, project->mergin->projectName );
      project->mergin->progress = i.value().totalSize != 0 ? i.value().transferedSize / i.value().totalSize : 0;
      project->mergin->pending = true;
      project->mergin->status = ProjectStatus::projectStatus( project );

      mProjects << project;
      ++i;
    }
  }
  else if ( mModelType != ProjectModelTypes::RecentProjectsModel )
  {
    // Keep all remote projects and ignore all non mergin projects from local projects
    for ( const auto &remoteEntry : merginProjects )
    {
      std::shared_ptr<Project> project = std::shared_ptr<Project>( new Project() );
      project->mergin = std::unique_ptr<MerginProject>( remoteEntry.clone() );

      if ( pendingProjects.contains( project->mergin->id() ) )
      {
        TransactionStatus projectTransaction = pendingProjects.value( project->mergin->id() );
        project->mergin->progress = projectTransaction.transferedSize / projectTransaction.totalSize;
        project->mergin->pending = true;
      }

      const auto res = std::find_if( localProjects.begin(), localProjects.end(), [&project]( const LocalProject & le )
      {
        return ( project->mergin->projectName == le.projectName && project->mergin->projectNamespace == le.projectNamespace );
      } );

      if ( res != localProjects.end() )
      {
        project->local = std::unique_ptr<LocalProject>( res->clone() );
      }
      project->mergin->status = ProjectStatus::projectStatus( project );

      mProjects << project;
    }
  }
}

void ProjectsModel::syncProject( const QString &projectId )
{
  std::shared_ptr<Project> project = projectFromId( projectId );

  if ( project == nullptr || !project->isMergin() || project->mergin->pending )
  {
    return;
  }

  if ( project->mergin->status == ProjectStatus::NoVersion || project->mergin->status == ProjectStatus::OutOfDate )
  {
    bool useAuth = !mBackend->userAuth()->hasAuthData() && mModelType == ProjectModelTypes::PublicProjectsModel;
    mBackend->updateProject( project->mergin->projectNamespace, project->mergin->projectName, useAuth );
  }
  else if ( project->mergin->status == ProjectStatus::Modified )
  {
    mBackend->uploadProject( project->mergin->projectNamespace, project->mergin->projectName );
  }
}

void ProjectsModel::stopProjectSync( const QString &projectId )
{
  std::shared_ptr<Project> project = projectFromId( projectId );

  if ( project == nullptr || !project->isMergin() || !project->mergin->pending )
  {
    return;
  }

  if ( project->mergin->status == ProjectStatus::NoVersion || project->mergin->status == ProjectStatus::OutOfDate )
  {
    mBackend->updateCancel( project->mergin->id() );
  }
  else if ( project->mergin->status == ProjectStatus::Modified )
  {
    mBackend->uploadCancel( project->mergin->id() );
  }
}

void ProjectsModel::removeLocalProject( const QString &projectId )
{
  mLocalProjectsManager->removeLocalProject( projectId );
}

void ProjectsModel::migrateProject( const QString &projectId )
{
  // if it is indeed a local project
  std::shared_ptr<Project> project = projectFromId( projectId );

  if ( project->isLocal() )
    mBackend->migrateProjectToMergin( project->local->projectName );
}

void ProjectsModel::onProjectSyncFinished( const QString &projectDir, const QString &projectFullName, bool successfully, int newVersion )
{
  Q_UNUSED( projectDir )

  std::shared_ptr<Project> project = projectFromId( projectFullName );
  if ( !project || !project->isMergin() )
    return;

  if ( successfully )
  {
    project->mergin->pending = false;
    project->mergin->progress = 0;
    project->mergin->serverVersion = newVersion;
    project->mergin->status = ProjectStatus::projectStatus( project );

    QModelIndex ix = index( mProjects.indexOf( project ) );
    emit dataChanged( ix, ix );
  }
  else if ( !successfully && mModelType == LocalProjectsModel && !project->isLocal() )
  {
    // remove project from localProjectsModel when first time download was cancelled or failed
    int removeIndex = mProjects.indexOf( project );

    beginRemoveRows( QModelIndex(), removeIndex, removeIndex );
    mProjects.removeOne( project );
    endRemoveRows();
  }
}

void ProjectsModel::onProjectSyncProgressChanged( const QString &projectFullName, qreal progress )
{
  std::shared_ptr<Project> project = projectFromId( projectFullName );
  if ( !project || !project->isMergin() )
    return;

  project->mergin->pending = progress >= 0;
  project->mergin->progress = progress >= 0 ? progress : 0;

  QModelIndex ix = index( mProjects.indexOf( project ) );
  emit dataChanged( ix, ix );
}

void ProjectsModel::onProjectAdded( const LocalProject &project )
{
  // Check if such project is already in project list
  std::shared_ptr<Project> proj = projectFromId( project.id() );
  if ( proj )
  {
    // add local information ~ project downloaded
    proj->local = std::unique_ptr<LocalProject>( project.clone() );
    if ( proj->isMergin() )
      proj->mergin->status = ProjectStatus::projectStatus( proj );

    QModelIndex ix = index( mProjects.indexOf( proj ) );
    emit dataChanged( ix, ix );
  }
  else if ( mModelType == LocalProjectsModel )
  {
    // add project to project list ~ project created
    std::shared_ptr<Project> newProject = std::make_shared<Project>();
    newProject->local = std::unique_ptr<LocalProject>( project.clone() );

    int insertIndex = mProjects.size();

    beginInsertRows( QModelIndex(), insertIndex, insertIndex );
    mProjects << newProject;
    endInsertRows();
  }
}

void ProjectsModel::onAboutToRemoveProject( const LocalProject project )
{
  std::shared_ptr<Project> proj = projectFromId( project.id() );

  if ( proj )
  {
    if ( mModelType == LocalProjectsModel )
    {
      int removeIndex = mProjects.indexOf( proj );

      beginRemoveRows( QModelIndex(), removeIndex, removeIndex );
      mProjects.removeOne( proj );
      endRemoveRows();
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

void ProjectsModel::onProjectDataChanged( const LocalProject &project )
{
  std::shared_ptr<Project> proj = projectFromId( project.id() );

  if ( proj )
  {
    proj->local = std::unique_ptr<LocalProject>( project.clone() );
    if ( proj->isMergin() )
      proj->mergin->status = ProjectStatus::projectStatus( proj );

    QModelIndex editIndex = index( mProjects.indexOf( proj ) );

    emit dataChanged( editIndex, editIndex );
  }
}

void ProjectsModel::onProjectDetachedFromMergin( const QString &projectFullName )
{
  std::shared_ptr<Project> proj = projectFromId( projectFullName );

  if ( proj )
  {
    proj->mergin.reset();
    proj->local->projectNamespace = "";
    QModelIndex editIndex = index( mProjects.indexOf( proj ) );

    emit dataChanged( editIndex, editIndex );

    // This project should also be removed from project list for remote project model types,
    // however, currently one needs to click on "My projects/Shared/Explore" and that sends
    // another listProjects request. In new list this project will not be shown.
  }
}

void ProjectsModel::onProjectAttachedToMergin( const QString & )
{
  // To ensure project will be in sync with server, send listProjectByName request.
  // In theory we could send that request only for this one project.
  listProjectsByName();
}

void ProjectsModel::onAuthChanged()
{
  if ( !mBackend->userAuth() || !mBackend->userAuth()->hasAuthData() ) // user logged out, clear created and shared lists
  {
    if ( mModelType == CreatedProjectsModel || mModelType == SharedProjectsModel )
    {
      clearProjects();
    }
  }
}

void ProjectsModel::setMerginApi( MerginApi *merginApi )
{
  if ( !merginApi || mBackend == merginApi )
    return;

  mBackend = merginApi;
  initializeProjectsModel();
}

void ProjectsModel::setLocalProjectsManager( LocalProjectsManager *localProjectsManager )
{
  if ( !localProjectsManager || mLocalProjectsManager == localProjectsManager )
    return;

  mLocalProjectsManager = localProjectsManager;
  initializeProjectsModel();
}

void ProjectsModel::setModelType( ProjectsModel::ProjectModelTypes modelType )
{
  if ( mModelType == modelType )
    return;

  mModelType = modelType;
  initializeProjectsModel();
}

QString ProjectsModel::modelTypeToFlag() const
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

QStringList ProjectsModel::projectNames() const
{
  QStringList projectNames;
  const LocalProjectsList projects = mLocalProjectsManager->projects();

  for ( const auto &proj : projects )
  {
    if ( !proj.projectName.isEmpty() && !proj.projectNamespace.isEmpty() )
      projectNames << proj.id();
  }

  return projectNames;
}

void ProjectsModel::clearProjects()
{
  beginResetModel();
  mProjects.clear();
  mServerProjectsCount = -1;
  endResetModel();

  emit hasMoreProjectsChanged();
}

void ProjectsModel::loadLocalProjects()
{
  if ( mModelType == LocalProjectsModel )
  {
    beginResetModel();
    mergeProjects( MerginProjectsList(), Transactions() ); // Fills model with local projects
    endResetModel();
  }
}

bool ProjectsModel::containsProject( QString projectId ) const
{
  std::shared_ptr<Project> proj = projectFromId( projectId );
  return proj != nullptr;
}

std::shared_ptr<Project> ProjectsModel::projectFromId( QString projectId ) const
{
  for ( const std::shared_ptr<Project> &it : mProjects )
  {
    if ( it->isMergin() && it->mergin->id() == projectId )
      return it;
    else if ( it->isLocal() && it->local->id() == projectId )
      return it;
  }
  return nullptr;
}

bool ProjectsModel::isLoading() const
{
  return mModelIsLoading;
}

void ProjectsModel::setModelIsLoading( bool state )
{
  mModelIsLoading = state;
  emit isLoadingChanged( mModelIsLoading );
}

ProjectsModel::ProjectModelTypes ProjectsModel::modelType() const
{
  return mModelType;
}
