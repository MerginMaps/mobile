#include "variablesmanager.h"

#include "qgsexpressioncontextutils.h"

VariablesManager::VariablesManager( MerginApi *merginApi, QObject *parent )
  : QObject( parent )
  , mMerginApi( merginApi )
{
  QgsExpressionContextUtils::setGlobalVariable( QStringLiteral( "mergin_url" ),  mMerginApi->apiRoot() );

  QObject::connect( mMerginApi, &MerginApi::apiRootChanged, this, &VariablesManager::apiRootChanged );
  QObject::connect( merginApi, &MerginApi::authChanged, this, &VariablesManager::authChanged );
}

void VariablesManager::apiRootChanged()
{
  QgsExpressionContextUtils::setGlobalVariable( QStringLiteral( "mergin_url" ),  mMerginApi->apiRoot() );
}

void VariablesManager::authChanged()
{
  QgsExpressionContextUtils::setGlobalVariable( QStringLiteral( "mergin_username" ),  mMerginApi->merginUserName() );
}

void VariablesManager::merginProjectChanged( QgsProject *project )
{
  QString filePath = project->fileName();
  LocalProjectInfo info = mMerginApi->localProjectsManager().projectByProjectFilePath( filePath );

  MerginProjectMetadata metadata = MerginProjectMetadata::fromCachedJson( info.projectDir + "/" + MerginApi::sMetadataFile );
  if ( metadata.isValid() )
  {
    info.projectName = metadata.name;
    info.projectNamespace = metadata.projectNamespace;
    info.localVersion = metadata.version;
    QgsExpressionContextUtils::setProjectVariable( project, QStringLiteral( "mergin_project_name" ),  info.projectName );
    QgsExpressionContextUtils::setProjectVariable( project, QStringLiteral( "mergin_project_full_name" ),  mMerginApi->getFullProjectName( info.projectNamespace, info.projectName ) );
    QgsExpressionContextUtils::setProjectVariable( project, QStringLiteral( "mergin_project_version" ), metadata.version );
    QgsExpressionContextUtils::setProjectVariable( project, QStringLiteral( "mergin_project_owner" ),  info.projectNamespace );
  }
}
