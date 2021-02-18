#include "projectsmodel_future.h"


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
  // TODO: connect to signals from LocalProjectsManager and MerginAPI
}

void ProjectsModel_future::listProjects()
{
  mLastRequestId = mBackend->listProjects( "", modelTypeToFlag(), "", mPopulatedPage );
}

void ProjectsModel_future::mergeProjects()
{
  QList<LocalProjectInfo> localProjects = mLocalProjectsManager.projects();
}

void ProjectsModel_future::listProjectsFinished()
{
  mergeProjects();
}

void ProjectsModel_future::projectSyncFinished()
{

}

void ProjectsModel_future::projectSyncProgressChanged()
{

}

QString ProjectsModel_future::modelTypeToFlag() const
{
  switch ( mModelType ) {
    case MyProjectsModel:
      return QStringLiteral( "created" );
    case SharedProjectsModel:
      return QStringLiteral( "shared" );
    default:
      return QStringLiteral( "" );
  }
}
