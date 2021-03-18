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

  // TODO: connect to signals from LocalProjectsManager
//  QObject::connect( mLocalProjectsManager, &LocalProjectsManager::localProjectAdded )

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsByNameFinished, this, &ProjectsModel_future::onListProjectsByNameFinished );
  }
  else if ( mModelType != ProjectModelTypes::RecentProjectsModel )
  {
    QObject::connect( mBackend, &MerginApi::listProjectsFinished, this, &ProjectsModel_future::onListProjectsFinished );
  }
  else
  {
    // Implement RecentProjectsModel type
  }

//  QObject::connect( &mLocalProjectsManager, &LocalProjectsManager::localProjectAdded, this, &ProjectsModel_future::onProjectAdded );
}

QVariant ProjectsModel_future::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

   std::shared_ptr<Project_future> project = mProjects.at( index.row() );

  switch ( role ) {
    default: return QVariant("TestData");
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
  roles[Roles::ProjectName] = QStringLiteral( "ProjectName" ).toLatin1();
  return roles;
}

int ProjectsModel_future::rowCount( const QModelIndex & ) const
{
  return mProjects.count();
}

void ProjectsModel_future::listProjects()
{
  if ( mModelType == LocalProjectsModel )
  {
    InputUtils::log( "Input", "Can not call listProjects API on LocalProjectsModel" );
    return;
  }

  mLastRequestId = mBackend->listProjects( "", modelTypeToFlag(), "", 1 ); //TODO: pagination
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

void ProjectsModel_future::mergeProjects( const MerginProjectList &merginProjects, Transactions pendingProjects )
{
  QList<LocalProjectInfo> localProjects = mLocalProjectsManager.projects();

  qDebug() << "PMR: mergeProjects(): # of local projects = " << localProjects.size() << " # of mergin projects = " << merginProjects.size();
  mProjects.clear();

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    // Keep all local projects and ignore all not downloaded remote projects
    for ( auto &localProject : localProjects )
    {
      std::shared_ptr<Project_future> project = std::shared_ptr<Project_future>( new Project_future() );
      std::unique_ptr<LocalProject_future> local = std::unique_ptr<LocalProject_future>( new LocalProject_future() );

      local->projectName = localProject.projectName;
      local->projectNamespace = localProject.projectNamespace;
      local->projectDir = localProject.projectDir;
      local->projectError = localProject.qgisProjectError;
      local->qgisProjectFilePath = localProject.qgisProjectFilePath;
      // TODO: later copy data by copy constructor
      project->local = std::move( local );

      MerginProjectListEntry remoteEntry;
      remoteEntry.projectName = project->local->projectName;
      remoteEntry.projectNamespace = project->local->projectNamespace;

      if ( merginProjects.contains( remoteEntry ) )
      {
        int i = merginProjects.indexOf( remoteEntry );
        std::unique_ptr<MerginProject_future> mergin = std::unique_ptr<MerginProject_future>( new MerginProject_future() );
        mergin->projectName = merginProjects[i].projectName;
        mergin->projectNamespace = merginProjects[i].projectNamespace;
        mergin->serverVersion = merginProjects[i].version;
        mergin->serverUpdated = merginProjects[i].serverUpdated;
        // TODO: later copy data by copy constructor
        // TODO: check for project errors (from ListByName API ~> not authorized / no rights / no version)

        if ( pendingProjects.contains( mergin->id() ) )
        {
          TransactionStatus projectTransaction = pendingProjects.value( mergin->id() );
          mergin->progress = projectTransaction.transferedSize / projectTransaction.totalSize;
          mergin->pending = true;
        }

        project->mergin = std::move( mergin );
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
      std::unique_ptr<MerginProject_future> mergin = std::unique_ptr<MerginProject_future>( new MerginProject_future() );

      mergin->projectName = remoteEntry.projectName;
      mergin->projectNamespace = remoteEntry.projectNamespace;
      // TODO: later copy data by copy constructor

      if ( pendingProjects.contains( mergin->id() ) )
      {
        TransactionStatus projectTransaction = pendingProjects.value( mergin->id() );
        mergin->progress = projectTransaction.transferedSize / projectTransaction.totalSize;
        mergin->pending = true;
      }

      project->mergin = std::move( mergin );

      // find downloaded projects
      LocalProjectInfo localProject;
      localProject.projectName = project->mergin->projectName;
      localProject.projectNamespace = project->mergin->projectNamespace;

      if ( localProjects.contains( localProject ) )
      {
        int ix = localProjects.indexOf( localProject );
        project->local = std::unique_ptr<LocalProject_future>( new LocalProject_future() );

        project->local->projectName = localProjects[ix].projectName;
        project->local->projectNamespace = localProjects[ix].projectNamespace;
        // TODO: later copy data by copy constructor
      }

      mProjects << project;
    }
  }
}

void ProjectsModel_future::onListProjectsFinished( const MerginProjectList &merginProjects, Transactions pendingProjects, int projectCount, int page, QString requestId )
{
  qDebug() << "PMR: onListProjectsFinished(): received response with requestId = " << requestId;
  if ( mLastRequestId != requestId )
  {
    qDebug() << "PMR: onListProjectsFinished(): ignoring request with id " << requestId;
    return;
  }

  Q_UNUSED( projectCount );
  Q_UNUSED( page );

  qDebug() << "PMR: onListProjectsFinished(): project count =  " << projectCount << " but mergin projects emited: " << merginProjects.size();

  beginResetModel();
  mergeProjects( merginProjects, pendingProjects );
  printProjects();
  endResetModel();
}

void ProjectsModel_future::onListProjectsByNameFinished( const MerginProjectList &merginProjects, Transactions pendingProjects, QString requestId )
{
  qDebug() << "PMR: onListProjectsByNameFinished(): received response with requestId = " << requestId;
  if ( mLastRequestId != requestId )
  {
    qDebug() << "PMR: onListProjectsByNameFinished(): ignoring request with id " << requestId;
    return;
  }

  Q_UNUSED( merginProjects );
  Q_UNUSED( pendingProjects );
  Q_UNUSED( requestId );

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
    qDebug() << "PMR: project" << MerginApi::getFullProjectName(projectNamespace, projectName) << "is already ";
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
  std::shared_ptr<Project_future> newProject = std::shared_ptr<Project_future>( new Project_future() );
  newProject->local = std::unique_ptr<LocalProject_future>( new LocalProject_future( project ) );
}

void ProjectsModel_future::onProjectDeleted( const QString &projectFullName )
{
  Q_UNUSED( projectFullName )
}

void ProjectsModel_future::onProjectDetachedFromMergin( const QString &projectFullName )
{
  Q_UNUSED( projectFullName )
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

QStringList ProjectsModel_future::projectNames() const // TODO: use local projects instead
{
  QStringList projectNames;
  QList<LocalProjectInfo> projects = mLocalProjectsManager.projects();

  for ( const auto &proj : projects )
  {
    if ( !proj.projectName.isEmpty() && !proj.projectNamespace.isEmpty() )
      projectNames << MerginApi::getFullProjectName( proj.projectNamespace, proj.projectName );
  }

  return projectNames;
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
