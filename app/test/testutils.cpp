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
#include <exiv2/exiv2.hpp>

#include "mmconfig.h"
#include "testutils.h"
#include "coreutils.h"
#include "inpututils.h"
#include "merginapi.h"

#include "qgsvectorlayer.h"
#include "qgsproject.h"
#include "qgslayertree.h"
#include "qgslayertreelayer.h"

void TestUtils::merginGetAuthCredentials( MerginApi *api, QString &apiRoot, QString &username, QString &password )
{
  Q_ASSERT( api );

  // Test server url needs to be set
  if ( ::getenv( "TEST_MERGIN_URL" ) == nullptr )
  {
    // if there is none, just default to the dev one
    apiRoot = "https://app.dev.merginmaps.com/";
  }
  else
  {
    apiRoot = ::getenv( "TEST_MERGIN_URL" );
    // let's make sure we do not mess with the public instance
    Q_ASSERT( apiRoot != MerginApi::sDefaultApiRoot );
  }
  api->setApiRoot( apiRoot );
  qDebug() << "MERGIN API ROOT:" << apiRoot;

  // test user needs to be set
  // check if there are environmental variables for the username and the password
  if ( ::getenv( "TEST_API_USERNAME" ) == nullptr && ::getenv( "TEST_API_PASSWORD" ) == nullptr )
  {
    // generate a random email and pasword
    // create the user on the server
    // create a workspace for the user
    generateRandomUser( api, username, password );
  }
  else
  {
    username = ::getenv( "TEST_API_USERNAME" );
    password = ::getenv( "TEST_API_PASSWORD" );
  }
}

void TestUtils::authorizeUser( MerginApi *api, const QString &username, const QString &password )
{
  // Auth this user
  QSignalSpy spyExtra( api, &MerginApi::authChanged );
  api->authorize( username, password );
  QVERIFY( spyExtra.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spyExtra.count(), 1 );
}

void TestUtils::selectFirstWorkspace( MerginApi *api, QString &workspace )
{
  // Gets his workspaces
  QSignalSpy spyExtraWs( api, &MerginApi::listWorkspacesFinished );
  api->listWorkspaces();
  QVERIFY( spyExtraWs.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spyExtraWs.count(), 1 );

  // Sets active workspace
  Q_ASSERT( !api->userInfo()->workspaces().isEmpty() );
  api->userInfo()->setActiveWorkspace( api->userInfo()->workspaces().firstKey() );

  // This user needs to have active workspace
  Q_ASSERT( !api->userInfo()->activeWorkspaceName().isEmpty() );

  workspace = api->userInfo()->activeWorkspaceName();

}

bool TestUtils::needsToAuthorizeAgain( MerginApi *api, const QString &username )
{
  Q_ASSERT( api );
  // no auth at all
  if ( !api->userAuth()->hasAuthData() )
  {
    return true;
  }

  // wrong user
  if ( api->userAuth()->login() != username )
  {
    return true;
  }

  // no workspace
  if ( api->userInfo()->activeWorkspaceName().isEmpty() )
  {
    return true;
  }

  // invalid token
  if ( api->userAuth()->authToken().isEmpty() || api->userAuth()->tokenExpiration() < QDateTime().currentDateTime().toUTC() )
  {
    return true;
  }

  // we are OK
  return false;
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

QString TestUtils::generateWorkspaceName( const QString &username )
{
  /*
  * Create a workspace name from the generated username
  * Output: a workspace name: mmta-DayMonthYear-HourSeconds
  */
  static const QRegularExpression regex( R"(merginautotest(\d{6})-(\d{4})\d{2}-\d{3})" );
  const QRegularExpressionMatch match = regex.match( username );

  if ( match.hasMatch() )
  {
    const QString date = match.captured( 1 ); // Day Month Year
    const QString time = match.captured( 2 ); // Hour Second
    return QString( "mmat-%1-%2" ).arg( date, time );
  }
  return {};
}

void TestUtils::generateRandomUser( MerginApi *api, QString &username, QString &password )
{
  // generate the test run-specific user details
  QString email = generateEmail();
  password = generatePassword();
  username = email.split( '@' ).first();
  username.remove( "+" );

  // create the account for the test run user
  api->clearAuth();
  QSignalSpy spy( api, &MerginApi::registrationSucceeded );
  QSignalSpy spy2( api, &MerginApi::registrationFailed );
  api->registerUser( email, password, true );
  // check that the account has been created.
  bool success = spy.wait( TestUtils::LONG_REPLY );
  if ( !success )
  {
    qDebug() << "Failed registration" << spy2.takeFirst();
    QVERIFY( false );
  }

  // check that the user can be authorized
  QSignalSpy spyAuth( api->userAuth(), &MerginUserAuth::authChanged );
  api->authorize( email, password );
  QVERIFY( spyAuth.wait( TestUtils::LONG_REPLY * 5 ) );

  // create workspace
  QSignalSpy wsSpy( api, &MerginApi::workspaceCreated );
  // create the workspace name
  QString workspace = generateWorkspaceName( username );
  api->createWorkspace( workspace );
  bool workspaceSuccess = wsSpy.wait( TestUtils::LONG_REPLY );
  if ( workspaceSuccess )
  {
    qDebug() << "CREATED NEW WORKSPACE:" << workspace;

    // call userInfo to set active workspace
    QSignalSpy infoSpy( api, &MerginApi::userInfoReplyFinished );
    api->getUserInfo();
    QVERIFY( infoSpy.wait( TestUtils::LONG_REPLY ) );
    QVERIFY( api->userInfo()->activeWorkspaceId() >= 0 );

    // change the data plan
    QString workspaceId = QString::number( api->userInfo()->activeWorkspaceId() );
    QSignalSpy wsStorageSpy( api, &MerginApi::updateWorkspaceStorageProjectLimit );
    api->updateWorkspaceStorageProjectLimit( workspaceId, TestUtils::TEST_WORKSPACE_STORAGE_SIZE, TestUtils::TEST_WORKSPACE_PROJECT_NUMER );
    bool workspaceStorageModified = wsStorageSpy.wait( TestUtils::LONG_REPLY );
    if ( workspaceStorageModified )
    {
      qDebug() << "Updated the storage limit" << workspace;
    }

    // this needs to be cleared, as the user will be authorized in the test cases.
    api->clearAuth();
  }
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

void TestUtils::testLayerHasGeometry()
{
  // null layer => should be false
  QCOMPARE( InputUtils::layerHasGeometry( nullptr ), false );

  // invalid layer => should be false
  QgsVectorLayer *invalidLayer = new QgsVectorLayer( "", "InvalidLayer", "none" );
  QVERIFY( invalidLayer->isValid() == false );
  QCOMPARE( InputUtils::layerHasGeometry( invalidLayer ), false );
  delete invalidLayer;

  // valid memory layer with geometry
  QgsVectorLayer *pointLayer = new QgsVectorLayer( "Point?crs=EPSG:4326", "ValidPointLayer", "memory" );
  QVERIFY( pointLayer->isValid() );
  QCOMPARE( InputUtils::layerHasGeometry( pointLayer ), true );

  // layer with NoGeo => should be false
  QgsVectorLayer *noGeomLayer = new QgsVectorLayer( "None", "NoGeometryLayer", "memory" );
  QVERIFY( noGeomLayer->isValid() );
  QCOMPARE( InputUtils::layerHasGeometry( noGeomLayer ), false );

  delete pointLayer;
  delete noGeomLayer;
}

void TestUtils::testLayerVisible()
{
  QgsProject *project = new QgsProject();
  project->clear();

  // null layer => should be false
  QCOMPARE( InputUtils::isLayerVisible( nullptr, project ), false );

  // valid memory layer
  QgsVectorLayer *layer = new QgsVectorLayer( "LineString?crs=EPSG:4326", "VisibleLineLayer", "memory" );
  QVERIFY( layer->isValid() );

  // won't appear in the layer tree => false
  QCOMPARE( InputUtils::isLayerVisible( layer, project ), false );

  // added to project => true
  project->addMapLayer( layer );
  QCOMPARE( InputUtils::isLayerVisible( layer, project ), true );

  // hide layer => false
  QgsLayerTree *root = project->layerTreeRoot();
  QgsLayerTreeLayer *layerTree = root->findLayer( layer );
  QVERIFY( layerTree );
  layerTree->setItemVisibilityChecked( false );
  QCOMPARE( InputUtils::isLayerVisible( layer, project ), false );

  delete project;
}

void TestUtils::testIsPositionTrackingLayer()
{
  QCOMPARE( InputUtils::isPositionTrackingLayer( nullptr, nullptr ), false );

  QgsProject *project = new QgsProject();
  QgsVectorLayer *layer = new QgsVectorLayer( "Point?crs=EPSG:4326", "TrackingLayer", "memory" );
  project->addMapLayer( layer );
  QCOMPARE( InputUtils::isPositionTrackingLayer( layer, project ), false );

  // tracking layer ID => true
  QString layerId = layer->id();
  project->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), layerId );
  QCOMPARE( InputUtils::isPositionTrackingLayer( layer, project ), true );

  // not tracking layer ID => false
  project->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), QString( "some-other-id" ) );
  QCOMPARE( InputUtils::isPositionTrackingLayer( layer, project ), false );

  delete project;
}

void TestUtils::testMapLayerFromName()
{
  QCOMPARE( InputUtils::mapLayerFromName( "Anything", nullptr ), static_cast<QgsMapLayer *>( nullptr ) );

  // empty layerName => nullptr
  QgsProject *project = new QgsProject();
  QCOMPARE( InputUtils::mapLayerFromName( "", project ), static_cast<QgsMapLayer *>( nullptr ) );

  // added a named layer to project and check => should succeed
  QgsVectorLayer *layer = new QgsVectorLayer( "Point?crs=EPSG:4326", "MyTestLayer", "memory" );
  QVERIFY( layer->isValid() );
  project->addMapLayer( layer );
  QgsMapLayer *found = InputUtils::mapLayerFromName( "MyTestLayer", project );
  QVERIFY( found != nullptr );
  QCOMPARE( found->name(), QString( "MyTestLayer" ) );

  // non-existing name => nullptr
  QCOMPARE( InputUtils::mapLayerFromName( "NoSuchName", project ), static_cast<QgsMapLayer *>( nullptr ) );

  delete project;
}

void TestUtils::testIsValidUrl()
{
  // valid urls
  QVERIFY( InputUtils::isValidUrl( "http://www.example.com" ) );
  QVERIFY( InputUtils::isValidUrl( "https://example.com/path?query=1" ) );
  QVERIFY( InputUtils::isValidUrl( "ftp://ftp.example.com/resource" ) );
  QVERIFY( InputUtils::isValidUrl( "www.example.com" ) );

  // invalid urls
  QVERIFY( !InputUtils::isValidUrl( "htp://www.example.com" ) );
  QVERIFY( !InputUtils::isValidUrl( "http//missingcolon.com" ) );
  QVERIFY( !InputUtils::isValidUrl( "://example.com" ) );
  QVERIFY( !InputUtils::isValidUrl( "http://exa mple.com" ) );
  QVERIFY( !InputUtils::isValidUrl( "" ) ); // empty url is considered valid by QUrl but not by us
}

bool TestUtils::testExifPositionMetadataExists( const QString &imageSource )
{
  if ( !QFileInfo::exists( imageSource ) )
    return false;

  try
  {
    const std::unique_ptr srcImage( Exiv2::ImageFactory::open( imageSource.toStdString() ) );
    if ( !srcImage )
      return false;

    srcImage->readMetadata();
    Exiv2::ExifData &exifData = srcImage->exifData();
    if ( exifData.empty() )
    {
      return false;
    }

    const auto iterator = exifData.findKey( Exiv2::ExifKey( "Exif.GPSInfo.GPSLatitude" ) );
    const auto iterator2 = exifData.findKey( Exiv2::ExifKey( "Exif.GPSInfo.GPSLongitude" ) );
    if ( iterator == exifData.end() || iterator2 == exifData.end() )
    {
      return false;
    }
    return true;
  }
  catch ( ... )
  {
    CoreUtils::log( "TestUtils", QStringLiteral( "Exception, while checking EXIF position metadata" ) );
    return false;
  }
}

