/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "QtDebug"
#include <QJsonDocument>
#include <QJsonArray>
#include <QSignalSpy>

#include "testutils.h"
#include "coreutils.h"
#include "inpututils.h"
#include "merginapi.h"

void TestUtils::mergin_setup_auth( MerginApi *api, QString &apiRoot, QString &username, QString &password, QString &workspace )
{
  Q_ASSERT( api );

  // Test server url needs to be set
  Q_ASSERT( ::getenv( "TEST_MERGIN_URL" ) );

  apiRoot = ::getenv( "TEST_MERGIN_URL" );
  api->setApiRoot( apiRoot );
  qDebug() << "MERGIN API ROOT:" << apiRoot;

  // let's make sure we do not mess with the public instance
  Q_ASSERT( apiRoot != MerginApi::sDefaultApiRoot );

  // Test user needs to be set
  Q_ASSERT( ::getenv( "TEST_API_USERNAME" ) );

  // Test password needs to be set
  Q_ASSERT( ::getenv( "TEST_API_PASSWORD" ) );

  username = ::getenv( "TEST_API_USERNAME" );
  password = ::getenv( "TEST_API_PASSWORD" );

  // This user needs to have active workspace
  Q_ASSERT( !api->userInfo()->activeWorkspaceName().isEmpty() );

  workspace = api->userInfo()->activeWorkspaceName();

  qDebug() << "MERGIN USERNAME:" << username;
  qDebug() << "MERGIN WORKSPACE:" << api->userInfo()->activeWorkspaceName() << api->userInfo()->activeWorkspaceId();
}

QString TestUtils::generateUsername()
{
  QDateTime time = QDateTime::currentDateTime();
  QString uniqename = time.toString( QStringLiteral( "ddMMyy-hhmmss-z" ) );
  return QStringLiteral( "input-%1" ).arg( uniqename );
}

QString TestUtils::generateEmail()
{
  QDateTime time = QDateTime::currentDateTime();
  QString uniqename = time.toString( QStringLiteral( "ddMMyy-hhmmss-z" ) );
  return QStringLiteral( "mergin+autotest+%1@lutraconsulting.co.uk" ).arg( uniqename );
}

QString TestUtils::generatePassword()
{
  QString pass = CoreUtils::uuidWithoutBraces( QUuid::createUuid() ).right( 15 ).replace( "-", "" );
  return QStringLiteral( "_Pass12%1" ).arg( pass );
}

QString TestUtils::testDataDir()
{
  QString dataDir( TEST_DATA_DIR );
  return dataDir;
}

bool TestUtils::generateProjectFolder( const QString &rootPath, const QJsonDocument &structure )
{
  if ( !structure.isObject() )
    return false;

  if ( !QDir( rootPath ).exists() )
    return false;

  QJsonObject rootObj = structure.object();

  // generate files
  if ( rootObj.contains( "files" ) )
  {
    QJsonArray files = rootObj.value( "files" ).toArray();
    for ( int i = 0; i < files.count(); i++ )
    {
      QFile f( rootPath + "/" + files[i].toString() );
      f.open( QIODevice::WriteOnly );
      f.close();
    }
  }

  // go deeper to subfolders
  bool allGood = true;

  QStringList nodes = rootObj.keys();
  for ( int i = 0; i < nodes.count(); i++ )
  {
    if ( rootObj.value( nodes[i] ).isObject() )
    {
      QString newPath = rootPath + "/" + nodes[i];

      QDir d( newPath );
      d.mkpath( newPath );

      allGood = allGood && generateProjectFolder( rootPath + "/" + nodes[i], QJsonDocument( rootObj.value( nodes[i] ).toObject() ) );
    }
  }

  return allGood;
}

QgsProject *TestUtils::loadPlanesTestProject()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectTempDir = QDir::tempPath() + "/" + QUuid::createUuid().toString();
  QString projectName = "quickapp_project.qgs";

  // copy the project to tmp dir to not change its data
  InputUtils::cpDir( projectDir, projectTempDir );

  QgsProject *project = new QgsProject();
  project->read( projectTempDir + "/" + projectName );

  return project;
}
