/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "variablesmanager.h"

#include "qgsexpressioncontextutils.h"

VariablesManager::VariablesManager( MerginApi *merginApi, QObject *parent )
  : QObject( parent )
  , mMerginApi( merginApi )
{
  apiRootChanged();
  authChanged();

  QObject::connect( mMerginApi, &MerginApi::apiRootChanged, this, &VariablesManager::apiRootChanged );
  QObject::connect( mMerginApi, &MerginApi::authChanged, this, &VariablesManager::authChanged );
  QObject::connect( mMerginApi, &MerginApi::projectDataChanged, this, &VariablesManager::setVersionVariable );
}

void VariablesManager::removeMerginProjectVariables( QgsProject *project )
{
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_name" ) );
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_full_name" ) );
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_version" ) );
  QgsExpressionContextUtils::removeProjectVariable( project, QStringLiteral( "mergin_project_owner" ) );
}

void VariablesManager::apiRootChanged()
{
  QgsExpressionContextUtils::setGlobalVariable( QStringLiteral( "mergin_url" ),  mMerginApi->apiRoot() );
}

void VariablesManager::authChanged()
{
  QgsExpressionContextUtils::setGlobalVariable( QStringLiteral( "mergin_username" ),  mMerginApi->merginUserName() );
}

void VariablesManager::setVersionVariable( const QString &projectFullName )
{
  if ( mCurrentProject->customVariables().value( QStringLiteral( "mergin_project_full_name" ) ).toString() == projectFullName )
    setProjectVariables();
}

void VariablesManager::merginProjectChanged( QgsProject *project )
{
  mCurrentProject = project;
  setProjectVariables();
}

void VariablesManager::setProjectVariables()
{
  if ( !mCurrentProject )
    return;


  QString filePath = mCurrentProject->fileName();
  QString projectDir = mMerginApi->localProjectsManager().projectFromProjectFilePath( filePath ).projectDir;
  if ( projectDir.isEmpty() )
  {
    removeMerginProjectVariables( mCurrentProject );
    return;
  }

  MerginProjectMetadata metadata = MerginProjectMetadata::fromCachedJson( projectDir + "/" + MerginApi::sMetadataFile );
  if ( metadata.isValid() )
  {
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_version" ), metadata.version );
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_name" ),  metadata.name );
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_full_name" ),  mMerginApi->getFullProjectName( metadata.projectNamespace,  metadata.name ) );
    QgsExpressionContextUtils::setProjectVariable( mCurrentProject, QStringLiteral( "mergin_project_owner" ),   metadata.projectNamespace );
  }
  else
  {
    removeMerginProjectVariables( mCurrentProject );
  }
}
