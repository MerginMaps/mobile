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
}

QVariant ProjectsModel_future::data( const QModelIndex &, int ) const
{
  return QVariant( "Test data" );
}

QModelIndex ProjectsModel_future::index( int, int, const QModelIndex & ) const
{
  return QModelIndex();
}

QHash<int, QByteArray> ProjectsModel_future::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles[Roles::Project] = QStringLiteral( "project" ).toLatin1();

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

void ProjectsModel_future::onProjectSyncFinished( const QString &projectDir, const QString &projectFullName, bool successfully )
{
  Q_UNUSED( projectDir )
  Q_UNUSED( projectFullName )
  Q_UNUSED( successfully )

  qDebug() << "PMR: Project " << projectFullName << " finished sync";
}

void ProjectsModel_future::onProjectSyncProgressChanged( const QString &projectFullName, qreal progress )
{
  Q_UNUSED( projectFullName )
  Q_UNUSED( progress )

  qDebug() << "PMR: Project " << projectFullName << " changed sync progress to " << progress;
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
  qDebug() << "Model " << this << " with type " << modelTypeToFlag() << " has projects: ";
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
