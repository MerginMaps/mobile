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
  connect( this, &ProjectsModel::merginApiChanged, this, &ProjectsModel::initializeProjectsModel );
  connect( this, &ProjectsModel::modelTypeChanged, this, &ProjectsModel::initializeProjectsModel );
  connect( this, &ProjectsModel::syncManagerChanged, this, &ProjectsModel::initializeProjectsModel );
  connect( this, &ProjectsModel::localProjectsManagerChanged, this, &ProjectsModel::initializeProjectsModel );
}

void ProjectsModel::initializeProjectsModel()
{
  if ( !mSyncManager || !mBackend || !mLocalProjectsManager || mModelType == EmptyProjectsModel ) // Model is not set up properly yet
    return;

  QObject::connect( mSyncManager, &SynchronizationManager::syncStarted, this, &ProjectsModel::onProjectSyncStarted );
  QObject::connect( mSyncManager, &SynchronizationManager::syncFinished, this, &ProjectsModel::onProjectSyncFinished );
  QObject::connect( mSyncManager, &SynchronizationManager::syncCancelled, this, &ProjectsModel::onProjectSyncCancelled );
  QObject::connect( mSyncManager, &SynchronizationManager::syncProgressChanged, this, &ProjectsModel::onProjectSyncProgressChanged );

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

  const Project project = mProjects.at( index.row() );

  switch ( role )
  {
    case ProjectName: return QVariant( project.projectName() );
    case ProjectNamespace: return QVariant( project.projectNamespace() );
    case ProjectFullName: return QVariant( project.fullName() );
    case ProjectId: return QVariant( project.id() );
    case ProjectIsLocal: return QVariant( project.isLocal() );
    case ProjectIsMergin: return QVariant( project.isMergin() );
    case ProjectStatus: return QVariant( project.isMergin() ? project.mergin.status : ProjectStatus::NoVersion );
    case ProjectFilePath: return QVariant( project.isLocal() ? project.local.qgisProjectFilePath : QString() );
    case ProjectDirectory: return QVariant( project.isLocal() ? project.local.projectDir : QString() );
    case ProjectIsValid:
    {
      if ( !project.isLocal() )
        return true; // Mergin projects are by default valid, remote error only affects syncing, not opening of a project
      return project.local.projectError.isEmpty();
    }
    case ProjectDescription:
    {
      if ( project.isLocal() )
      {
        if ( !project.local.projectError.isEmpty() )
        {
          return QVariant( project.local.projectError );
        }
        else
        {
          ProjectStatus::Status status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );

          if ( status == ProjectStatus::NeedsSync )
          {
            return QVariant( tr( "Pending changes to synchronise" ) );
          }

          if ( status == ProjectStatus::NoVersion )
          {
            return QVariant( tr( "Local project, not uploaded" ) );
          }
        }

        QFileInfo fi( project.local.projectDir );

        // Up to date
        // lastModified of projectDir is not reliable - gpkg file may have modified header after opening it. See more #1320
        return QVariant( tr( "Updated %1" ).arg( InputUtils::formatDateTimeDiff( fi.lastModified().toUTC() ) ) );
      }
      else if ( project.isMergin() )
      {
        return QVariant( tr( "Updated %1" ).arg( InputUtils::formatDateTimeDiff( project.mergin.serverUpdated.toUTC() ) ) );
      }

      // This should not happen
      CoreUtils::log( "Project error", "Found project that is not downloaded nor remote" );
      return QVariant();
    }
    case ProjectIsActiveProject:
    {
      return QVariant( project.id() == mActiveProjectId );
    }
    default:
    {
      if ( !project.isMergin() ) return QVariant();

      // Roles only for projects that has mergin part
      if ( role == ProjectSyncPending ) return QVariant( mSyncManager->hasPendingSync( project.fullName() ) );
      else if ( role == ProjectSyncProgress ) return QVariant( mSyncManager->syncProgress( project.fullName() ) );
      else if ( role == ProjectRemoteError ) return QVariant( project.mergin.remoteError );
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
  roles[Roles::ProjectStatus]       = QStringLiteral( "ProjectStatus" ).toLatin1();
  roles[Roles::ProjectIsValid]      = QStringLiteral( "ProjectIsValid" ).toLatin1();
  roles[Roles::ProjectFilePath]     = QStringLiteral( "ProjectFilePath" ).toLatin1();
  roles[Roles::ProjectDescription]  = QStringLiteral( "ProjectDescription" ).toLatin1();
  roles[Roles::ProjectSyncPending]  = QStringLiteral( "ProjectSyncPending" ).toLatin1();
  roles[Roles::ProjectSyncProgress] = QStringLiteral( "ProjectSyncProgress" ).toLatin1();
  roles[Roles::ProjectRemoteError]  = QStringLiteral( "ProjectRemoteError" ).toLatin1();
  roles[Roles::ProjectIsActiveProject]  = QStringLiteral( "ProjectIsActiveProject" ).toLatin1();
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

  mLastRequestId = mBackend->listProjects( searchExpression, modelTypeToFlag(), page );

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

void ProjectsModel::onListProjectsFinished( const MerginProjectsList &merginProjects, int projectsCount, int page, QString requestId )
{
  if ( mLastRequestId != requestId )
  {
    return;
  }

  if ( page == 1 )
  {
    // if we are populating first page, reset model and throw away previous projects
    beginResetModel();
    mergeProjects( merginProjects, MergeStrategy::DiscardPrevious );
    endResetModel();
  }
  else
  {
    // paginating next page, keep previous projects and emit model add items
    beginInsertRows( QModelIndex(), mProjects.size(), mProjects.size() + merginProjects.size() - 1 );
    mergeProjects( merginProjects, MergeStrategy::KeepPrevious );
    endInsertRows();
  }

  mServerProjectsCount = projectsCount;
  mPaginatedPage = page;
  emit hasMoreProjectsChanged();
  setModelIsLoading( false );
}

void ProjectsModel::onListProjectsByNameFinished( const MerginProjectsList &merginProjects, QString requestId )
{
  if ( mLastRequestId != requestId )
  {
    return;
  }

  beginResetModel();
  mergeProjects( merginProjects );
  endResetModel();

  setModelIsLoading( false );
}

void ProjectsModel::mergeProjects( const MerginProjectsList &merginProjects, MergeStrategy mergeStrategy )
{
  const LocalProjectsList localProjects = mLocalProjectsManager->projects();

  if ( mergeStrategy == DiscardPrevious )
  {
    mProjects.clear();
  }

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    // Keep all local projects and ignore all not downloaded remote projects
    for ( const LocalProject &localProject : localProjects )
    {
      Project project;
      project.local = localProject;

      const auto res = std::find_if( merginProjects.begin(), merginProjects.end(), [&project]( const MerginProject & me )
      {
        return ( project.id() == me.id() );
      } );

      if ( res != merginProjects.end() )
      {
        project.mergin = *res;
        project.mergin.status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );
      }
      else if ( project.local.hasMerginMetadata() )
      {
        // App is starting - loads all local projects from a device
        // OR
        // We do not have server info because the project was ignored
        // (listProjectsByName API limits response to max 50 projects)
        project.mergin.projectName = project.local.projectName;
        project.mergin.projectNamespace = project.local.projectNamespace;
        project.mergin.status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );
      }

      mProjects << project;
    }

    // lets check also for projects that are currently being downloaded and add them to local projects list
    QList<QString> pendingProjects = mSyncManager->pendingProjects();

    for ( const QString &pendingProjectName : pendingProjects )
    {
      const auto &match = std::find_if( mProjects.begin(), mProjects.end(), [&pendingProjectName]( const Project & mp )
      {
        return ( mp.id() == pendingProjectName );
      } );

      bool alreadyIncluded = match != mProjects.end();
      if ( !alreadyIncluded )
      {
        Project project;

        MerginApi::extractProjectName( pendingProjectName, project.mergin.projectNamespace, project.mergin.projectName );
        project.mergin.status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );

        mProjects << project;
      }
    }
  }
  else if ( mModelType != ProjectModelTypes::RecentProjectsModel )
  {
    // Keep all remote projects and ignore all non mergin projects from local projects
    for ( const auto &remoteEntry : merginProjects )
    {
      Project project;
      project.mergin = remoteEntry;

      const auto match = std::find_if( localProjects.begin(), localProjects.end(), [&project]( const LocalProject & le )
      {
        return ( project.id() == le.id() );
      } );

      if ( match != localProjects.end() )
      {
        project.local = *match;
      }
      project.mergin.status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );

      mProjects << project;
    }
  }
}

void ProjectsModel::syncProject( const QString &projectId )
{
  int ix = projectIndexFromId( projectId );

  if ( ix < 0 )
    return;

  if ( mSyncManager )
  {
    if ( mModelType == ProjectModelTypes::PublicProjectsModel )
    {
      mSyncManager->syncProject( mProjects[ix], SyncOptions::AuthOptional );
    }
    else
    {
      mSyncManager->syncProject( mProjects[ix] );
    }
  }
}

void ProjectsModel::stopProjectSync( const QString &projectId )
{
  if ( mSyncManager )
  {
    mSyncManager->stopProjectSync( projectId );
  }
}

void ProjectsModel::removeLocalProject( const QString &projectId )
{
  mLocalProjectsManager->removeLocalProject( projectId );
}

void ProjectsModel::migrateProject( const QString &projectId )
{
  int ix = projectIndexFromId( projectId );

  if ( ix < 0 )
    return;

  mSyncManager->migrateProjectToMergin( mProjects[ix].local.projectName );
}

void ProjectsModel::onProjectSyncStarted( const QString &projectFullName )
{
  int ix = projectIndexFromId( projectFullName );

  if ( ix < 0 )
    return;

  QModelIndex changeIndex = index( ix );
  emit dataChanged( changeIndex, changeIndex, { ProjectSyncPending, ProjectSyncProgress } );
}

void ProjectsModel::onProjectSyncCancelled( const QString &projectFullName )
{
  int ix = projectIndexFromId( projectFullName );

  if ( ix < 0 )
    return;

  QModelIndex changeIndex = index( ix );
  emit dataChanged( changeIndex, changeIndex, { ProjectSyncPending, ProjectSyncProgress, ProjectStatus } );
}

void ProjectsModel::onProjectSyncFinished( const QString &projectFullName, bool successfully, int newVersion )
{
  int ix = projectIndexFromId( projectFullName );

  if ( ix < 0 )
    return;

  if ( !mProjects[ix].isMergin() )
    return;

  Project &project = mProjects[ix];

  if ( successfully )
  {
    project.mergin.serverVersion = newVersion;
  }

  project.mergin.status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );

  QModelIndex changeIndex = index( ix );
  emit dataChanged( changeIndex, changeIndex, { ProjectSyncPending, ProjectSyncProgress, ProjectStatus } );

  // remove project from list of projects if this was a first-time download of remote project in local projects list
  if ( !successfully && mModelType == LocalProjectsModel )
  {
    if ( !project.isLocal() )
    {
      beginRemoveRows( QModelIndex(), ix, ix );
      mProjects.removeAt( ix );
      endRemoveRows();
    }
  }
}

void ProjectsModel::onProjectSyncProgressChanged( const QString &projectFullName, qreal progress )
{
  Q_UNUSED( progress )

  int ix = projectIndexFromId( projectFullName );

  if ( ix < 0 )
    return;

  if ( !mProjects[ix].isMergin() )
    return;

  QModelIndex changeIndex = index( ix );
  emit dataChanged( changeIndex, changeIndex, { ProjectSyncPending, ProjectSyncProgress } );
}

void ProjectsModel::onProjectAdded( const LocalProject &localProject )
{
  // Check if such project is already in project list
  int ix = projectIndexFromId( localProject.id() );
  if ( ix >= 0 )
  {
    // add local information ~ project downloaded
    Project &project = mProjects[ix];

    project.local = localProject;
    if ( project.isMergin() )
    {
      project.mergin.status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );
    }

    QModelIndex modelIx = index( ix );
    emit dataChanged( modelIx, modelIx );
  }
  else if ( mModelType == LocalProjectsModel )
  {
    // add project to project list ~ project created
    Project project;
    project.local = localProject;

    int insertIndex = mProjects.size();

    beginInsertRows( QModelIndex(), insertIndex, insertIndex );
    mProjects << project;
    endInsertRows();
  }
}

void ProjectsModel::onAboutToRemoveProject( const LocalProject &localProject )
{
  int ix = projectIndexFromId( localProject.id() );

  if ( ix >= 0 )
  {
    if ( mModelType == LocalProjectsModel )
    {
      beginRemoveRows( QModelIndex(), ix, ix );
      mProjects.removeAt( ix );
      endRemoveRows();
    }
    else
    {
      // just remove local part
      mProjects[ix].local = LocalProject();
      mProjects[ix].mergin.status = ProjectStatus::projectStatus( mProjects[ix], mBackend->supportsSelectiveSync() );

      QModelIndex modelIx = index( ix );
      emit dataChanged( modelIx, modelIx );
    }
  }
}

void ProjectsModel::onProjectDataChanged( const LocalProject &localProject )
{
  int ix = projectIndexFromId( localProject.id() );

  if ( ix < 0 )
    return;

  Project &project = mProjects[ix];

  project.local = localProject;

  if ( project.isMergin() )
  {
    project.mergin.status = ProjectStatus::projectStatus( project, mBackend->supportsSelectiveSync() );
  }

  QModelIndex editIndex = index( ix );
  emit dataChanged( editIndex, editIndex );
}

void ProjectsModel::onProjectDetachedFromMergin( const QString &projectFullName )
{
  int ix = projectIndexFromId( projectFullName );

  if ( ix < 0 )
    return;

  Project &project = mProjects[ix];
  project.mergin = MerginProject();
  project.local.projectNamespace = QLatin1String();

  QModelIndex editIndex = index( ix );
  emit dataChanged( editIndex, editIndex );

  // This project should also be removed from project list for remote project model types,
  // however, currently one needs to click on "My projects/Shared/Explore" and that sends
  // another listProjects request. In new list this project will not be shown.
  // However, this option is not allowed in GUI anyways.

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
    if ( mModelType == WorkspaceProjectsModel )
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
  emit merginApiChanged( mBackend );
}

void ProjectsModel::setLocalProjectsManager( LocalProjectsManager *localProjectsManager )
{
  if ( !localProjectsManager || mLocalProjectsManager == localProjectsManager )
    return;

  mLocalProjectsManager = localProjectsManager;
  emit localProjectsManagerChanged( mLocalProjectsManager );
}

void ProjectsModel::setModelType( ProjectsModel::ProjectModelTypes modelType )
{
  if ( mModelType == modelType )
    return;

  mModelType = modelType;
  emit modelTypeChanged( mModelType );
}

QString ProjectsModel::modelTypeToFlag() const
{
  switch ( mModelType )
  {
    case WorkspaceProjectsModel:
      return QStringLiteral( "workspace" );
    case PublicProjectsModel:
      return QStringLiteral( "public" );
    default:
      return QLatin1String();
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
    mergeProjects( MerginProjectsList() ); // Fills model with local projects
    endResetModel();
  }
}

int ProjectsModel::projectIndexFromId( const QString &projectId ) const
{
  for ( int i = 0; i < mProjects.count(); i++ )
  {
    if ( mProjects[i].id() == projectId )
      return i;
  }

  return -1;
}

Project ProjectsModel::projectFromId( const QString &projectId ) const
{
  for ( const Project &project : mProjects )
  {
    if ( project.id() == projectId )
    {
      return project;
    }
  }
  return Project();
}

QModelIndex ProjectsModel::projectModelIndexFromId( const QString &projectId ) const
{
  int row = projectIndexFromId( projectId );
  return index( row );
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

MerginApi *ProjectsModel::merginApi() const { return mBackend; }

LocalProjectsManager *ProjectsModel::localProjectsManager() const { return mLocalProjectsManager; }

SynchronizationManager *ProjectsModel::syncManager() const
{
  return mSyncManager;
}

void ProjectsModel::setSyncManager( SynchronizationManager *newSyncManager )
{
  if ( mSyncManager == newSyncManager )
    return;

  mSyncManager = newSyncManager;
  emit syncManagerChanged( mSyncManager );
}

QString ProjectsModel::activeProjectId() const { return mActiveProjectId; }

void ProjectsModel::setActiveProjectId( const QString &projectId )
{
  mActiveProjectId = projectId;
  emit activeProjectIdChanged( mActiveProjectId );
}
