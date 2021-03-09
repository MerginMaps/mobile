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
    // TODO: implement RecentProjectsModel type
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
  mLastRequestId = mBackend->listProjects( "", modelTypeToFlag(), "", mPopulatedPage );
}

void ProjectsModel_future::mergeProjects( const MerginProjectList &merginProjects, Transactions pendingProjects )
{
  QList<LocalProjectInfo> localProjects = mLocalProjectsManager.projects();

  qDebug() << "ProjectsModel_future: mergeProjects(): # of local projects = " << localProjects.size();
  mProjects.clear();

  if ( mModelType == ProjectModelTypes::LocalProjectsModel )
  {
    // Keep all local projects and ignore all not downloaded remote projects
    for ( auto &localProject : localProjects )
    {
      std::shared_ptr<Project_future> project = std::shared_ptr<Project_future>( new Project_future() );

      std::unique_ptr<LocalProject_future> local = std::unique_ptr<LocalProject_future>( new LocalProject_future() );

      project->local = std::move( local );

      project->local->projectName = localProject.projectName;
      project->local->projectNamespace = localProject.projectNamespace;
      project->local->projectDir = localProject.projectDir;
      // TODO: later copy data by copy constructor

      MerginProjectListEntry remoteEntry;
      remoteEntry.projectName = project->local->projectName;
      remoteEntry.projectNamespace = project->local->projectNamespace;

      if ( merginProjects.contains( remoteEntry ) )
      {
        int i = merginProjects.indexOf( remoteEntry );
        std::unique_ptr<MerginProject_future> mergin = std::unique_ptr<MerginProject_future>( new MerginProject_future() );
        mergin->projectName = merginProjects[i].projectName;
        mergin->projectNamespace = merginProjects[i].projectNamespace;
        // TODO: later copy data by copy constructor
        // TODO: check for project errors (from ListByName API ~> not authorized / no rights / no version)

        if ( pendingProjects.contains( mergin->projectIdentifier() ) )
        {
          TransactionStatus projectTransaction = pendingProjects.value( mergin->projectIdentifier() );
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
    for ( const auto &remoteEntry: merginProjects )
    {
      std::shared_ptr<Project_future> project = std::shared_ptr<Project_future>( new Project_future() );
      std::unique_ptr<MerginProject_future> mergin = std::unique_ptr<MerginProject_future>( new MerginProject_future() );

      mergin->projectName = remoteEntry.projectName;
      mergin->projectNamespace = remoteEntry.projectNamespace;
      // TODO: later copy data by copy constructor

      if ( pendingProjects.contains( mergin->projectIdentifier() ) )
      {
        TransactionStatus projectTransaction = pendingProjects.value( mergin->projectIdentifier() );
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
  qDebug() << "ProjectsModel_future: onListProjectsFinished(): received response with requestId = " << requestId;
  if ( mLastRequestId != requestId )
  {
    qDebug() << "ProjectsModel_future: onListProjectsFinished(): should ignore request with id " << requestId << ", disabled for a while";
//    return;
  }
  Q_UNUSED( pendingProjects );
  Q_UNUSED( projectCount );
  Q_UNUSED( page );

  qDebug() << "ProjectsModel_future: onListProjectsFinished(): project count =  " << projectCount << " but mergin projects emited: " << merginProjects.size();

  mergeProjects( merginProjects, pendingProjects );
}

void ProjectsModel_future::onListProjectsByNameFinished()
{

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
