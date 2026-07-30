/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "QtDebug"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <exiv2/exiv2.hpp>

#include "mmconfig.h"
#include "testutils.h"
#include "coreutils.h"
#include "inpututils.h"
#include "merginapi.h"

#include "qgsvectorlayer.h"
#include "qgsproject.h"
#include "qgsfeature.h"

void TestUtils::merginGetAuthCredentials( MerginApi *api, QString &apiRoot, QString &username, QString &password )
{
  Q_ASSERT( api );

  // Test server url needs to be set
  if ( ::getenv( "TEST_MERGIN_URL" ) == nullptr )
  {
    // if there is none, just default to the dev one
    apiRoot = QStringLiteral( "https://app.dev.merginmaps.com/" );
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

QString TestUtils::generateEmail()
{
#if defined(Q_OS_MACOS)
  QString prefix = QStringLiteral( "mac" );
#elif defined(Q_OS_LINUX)
  QString prefix = QStringLiteral( "lin" );
#else
  QString prefix = QStringLiteral( "mob" );
#endif
  QDateTime time = QDateTime::currentDateTime();
  QString uniqeName = time.toString( QStringLiteral( "ddMMyy-hhmmss" ) );
  // adding the prefix and the uniqueName
  return QStringLiteral( "%1-%2@lutraconsulting.co.uk" ).arg( prefix, uniqeName );
}

QString TestUtils::generatePassword()
{
  QString pass = CoreUtils::uuidWithoutBraces( QUuid::createUuid() ).right( 15 ).replace( "-", "" );
  return QStringLiteral( "_Pass12%1" ).arg( pass );
}

void TestUtils::generateRandomUser( MerginApi *api, QString &username, QString &password )
{
  // generate the test run-specific user details
  QString email = generateEmail();
  password = generatePassword();
  username = email.left( email.lastIndexOf( '@' ) );

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
  api->createWorkspace( username );
  bool workspaceSuccess = wsSpy.wait( TestUtils::LONG_REPLY );
  QVERIFY( workspaceSuccess );
  qDebug() << "CREATED NEW WORKSPACE:" << username;

  // call userInfo to set active workspace
  QSignalSpy infoSpy( api, &MerginApi::userInfoReplyFinished );
  api->getUserInfo();
  QVERIFY( infoSpy.wait( TestUtils::LONG_REPLY ) );
  QVERIFY( api->userInfo()->activeWorkspaceId() >= 0 );

  // change the data plan
  QString workspaceId = QString::number( api->userInfo()->activeWorkspaceId() );
  QSignalSpy wsStorageSpy( api, &MerginApi::updateWorkspaceService );

  // Create JSON payload to change the data plan
  QString payload = QString( R"({
                             "limits_override": {
                             "storage": %1,
                             "projects" : %2,
                             "api_allowed" : true
                             }
  })" ).arg( TEST_WORKSPACE_STORAGE_SIZE ).arg( TEST_WORKSPACE_PROJECT_NUMBER );

  api->updateWorkspaceService( workspaceId, payload );
  bool workspaceStorageModified = wsStorageSpy.wait( TestUtils::LONG_REPLY );
  if ( workspaceStorageModified )
  {
    qDebug() << "Updated the storage limit" << username;
  }

// this needs to be cleared, as the user will be authorized in the test cases.
  api->clearAuth();

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

QgsVectorLayer *TestUtils::createVRLookupLayer( int count )
{
  auto *layer = new QgsVectorLayer(
    QStringLiteral( "None?field=key:integer&field=label:string" ),
    QStringLiteral( "vr_lookup" ),
    QStringLiteral( "memory" )
  );
  if ( !layer || !layer->isValid() )
    return nullptr;

  QgsFeatureList features;
  features.reserve( count );
  for ( int i = 1; i <= count; ++i )
  {
    QgsFeature f( layer->fields() );
    f.setAttribute( QStringLiteral( "key" ),   i );
    f.setAttribute( QStringLiteral( "label" ), QStringLiteral( "Label %1" ).arg( i ) );
    features << f;
  }
  layer->dataProvider()->addFeatures( features );
  return layer;
}

QgsVectorLayer *TestUtils::createVROrderingLayer()
{
  auto *layer = new QgsVectorLayer(
    QStringLiteral( "None?field=key:integer&field=label:string" ),
    QStringLiteral( "vr_ordering" ),
    QStringLiteral( "memory" )
  );
  if ( !layer || !layer->isValid() )
    return nullptr;

  struct Row { int key; QString label; };
  const QList<Row> rows =
  {
    {3, QStringLiteral( "Gamma" )},
    {1, QStringLiteral( "Alpha" )},
    {4, QStringLiteral( "Beta" )},
    {2, QStringLiteral( "Delta" )}
  };

  QgsFeatureList features;
  features.reserve( rows.size() );
  for ( const auto &row : rows )
  {
    QgsFeature f( layer->fields() );
    f.setAttribute( QStringLiteral( "key" ),   row.key );
    f.setAttribute( QStringLiteral( "label" ), row.label );
    features << f;
  }
  layer->dataProvider()->addFeatures( features );
  return layer;
}

QgsVectorLayer *TestUtils::createFilterTestLayer( const QString &fieldName, const QString &fieldType, const QString &layerName )
{
  const QString layerUri = QStringLiteral( "None?field=%1:%2" ).arg( fieldName, fieldType );

  auto *layer = new QgsVectorLayer( layerUri, layerName, QStringLiteral( "memory" ) );
  if ( !layer->isValid() )
  {
    delete layer;
    return nullptr;
  }

  QgsProject::instance()->addMapLayer( layer );
  return layer;
}

bool TestUtils::addFeatureToLayer( QgsVectorLayer *layer, const QString &fieldName, const QVariant &value )
{
  QgsFeature f( layer->fields() );
  f.setAttribute( fieldName, value );
  return layer->dataProvider()->addFeatures( QgsFeatureList() << f );
}

QString TestUtils::setupControllerWithFilter( FilterController *controller,
    FieldFilter::FilterType filterType,
    const QString &layerId,
    const QString &fieldName,
    const QString &sqlExpression )
{
  QString filterTypeStr;
  switch ( filterType )
  {
    case FieldFilter::TextFilter:         filterTypeStr = QStringLiteral( "Text" ); break;
    case FieldFilter::NumberFilter:       filterTypeStr = QStringLiteral( "Number" ); break;
    case FieldFilter::DateFilter:         filterTypeStr = QStringLiteral( "Date" ); break;
    case FieldFilter::CheckboxFilter:     filterTypeStr = QStringLiteral( "Checkbox" ); break;
    case FieldFilter::SingleSelectFilter: filterTypeStr = QStringLiteral( "Single select" ); break;
    case FieldFilter::MultiSelectFilter:  filterTypeStr = QStringLiteral( "Multi select" ); break;
  }

  QJsonObject filterObj;
  filterObj[QStringLiteral( "filter_name" )]    = QStringLiteral( "Test Filter" );
  filterObj[QStringLiteral( "filter_type" )]    = filterTypeStr;
  filterObj[QStringLiteral( "field_name" )]     = fieldName;
  filterObj[QStringLiteral( "sql_expression" )] = sqlExpression;
  filterObj[QStringLiteral( "layer_id" )]       = layerId;
  const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayer( layerId ) );
  filterObj[QStringLiteral( "provider" )]       = layer ? layer->providerType() : QStringLiteral( "memory" );

  QJsonArray filtersArray;
  filtersArray.append( filterObj );

  const QString filtersJson = QString::fromUtf8(
                                QJsonDocument( filtersArray ).toJson( QJsonDocument::Compact ) );

  QgsProject::instance()->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "Filtering/Enabled" ), true );
  QgsProject::instance()->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "Filtering/Filters" ), filtersJson );

  controller->loadFilterConfig( QgsProject::instance() );

  const QVariantList filters = controller->getFilters();
  if ( filters.isEmpty() )
    return {};
  return filters.first().toMap().value( QStringLiteral( "filterId" ) ).toString();
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

