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

void TestUtils::mergin_setup_auth( MerginApi *api, QString &apiRoot, QString &username, QString &password )
{
  Q_ASSERT( api );

  // Test server url needs to be set
  Q_ASSERT( ::getenv( "TEST_MERGIN_URL" ) );

  apiRoot = ::getenv( "TEST_MERGIN_URL" );
  api->setApiRoot( apiRoot );
  qDebug() << "MERGIN API ROOT:" << apiRoot;

  // let's make sure we do not mess with the public instance
  Q_ASSERT( apiRoot != MerginApi::sDefaultApiRoot );

  username = ::getenv( "TEST_API_USERNAME" );
  password = ::getenv( "TEST_API_PASSWORD" );

  if ( username.isEmpty() )
  {
    // we need to register new user for tests and assign its credentials to env vars
    username = generateUsername();
    password = generatePassword();
    QString email = generateEmail();

    qDebug() << "REGISTERING NEW TEST USER:" << username;

    QSignalSpy spy( api,  &MerginApi::registrationSucceeded );
    api->registerUser( username, email, password, password, true );
    QVERIFY( spy.wait( TestUtils::LONG_REPLY ) );
    QCOMPARE( spy.count(), 1 );

    // put it so in next local test run we can take it from
    // the environment and we do not create another user
    qputenv( "TEST_API_USERNAME", username.toLatin1() );
    qputenv( "TEST_API_PASSWORD", password.toLatin1() );

    QSignalSpy authSpy( api, &MerginApi::authChanged );
    api->authorize( username, password );
    QVERIFY( authSpy.wait( TestUtils::LONG_REPLY ) );
    QVERIFY( !authSpy.isEmpty() );

    // we also need to create a workspace for this user
    QSignalSpy wsSpy( api, &MerginApi::workspaceCreated );
    api->createWorkspace( username );
    QVERIFY( wsSpy.wait( TestUtils::LONG_REPLY ) );
    QCOMPARE( wsSpy.takeFirst().at( 1 ), true );

    qDebug() << "CREATED NEW WORKSPACE:" << username;

    // call userInfo to set active workspace
    QSignalSpy infoSpy( api, &MerginApi::userInfoReplyFinished );
    api->getUserInfo();
    QVERIFY( infoSpy.wait( TestUtils::LONG_REPLY ) );

    QVERIFY( api->userInfo()->activeWorkspaceId() >= 0 );
    qDebug() << "WORKING WITH WORKSPACE:" << api->userInfo()->activeWorkspaceName() << api->userInfo()->activeWorkspaceId();

    // we need to subscribe to some reasonable plan with workspace
  }

  Q_ASSERT( ::getenv( "TEST_API_USERNAME" ) );
  Q_ASSERT( ::getenv( "TEST_API_PASSWORD" ) );

  qDebug() << "MERGIN USERNAME:" << username;
}

void TestUtils::mergin_setup_pro_subscription( MerginApi *api, TestingPurchasingBackend *purchasingBackend )
{
  QSignalSpy spy2( api, &MerginApi::subscriptionInfoChanged );
  api->getServiceInfo();
  QVERIFY( spy2.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  if ( api->subscriptionInfo()->planProductId() != TIER02_PLAN_ID )
  {
    // always start from PRO subscription
    runPurchasingCommand( api, purchasingBackend, TestingPurchasingBackend::NonInteractiveBuyProfessionalPlan, TIER02_PLAN_ID );
  }

  QCOMPARE( api->subscriptionInfo()->planProductId(), TIER02_PLAN_ID );
  QCOMPARE( api->workspaceInfo()->storageLimit(), TIER02_STORAGE );
  QCOMPARE( api->subscriptionInfo()->ownsActiveSubscription(), true );
  QCOMPARE( api->subscriptionInfo()->subscriptionStatus(), MerginSubscriptionStatus::ValidSubscription );
  QCOMPARE( api->subscriptionInfo()->planProvider(), MerginSubscriptionType::TestSubscriptionType );

  qDebug() << "MERGIN SUBSCRIPTION:" << api->subscriptionInfo()->planProductId();
}

void TestUtils::runPurchasingCommand( MerginApi *api, TestingPurchasingBackend *purchasingBackend, TestingPurchasingBackend::NextPurchaseResult result, const QString &planId, bool waitForWorkspaceInfoChanged )
{
  purchasingBackend->setNextPurchaseResult( result );

  QSignalSpy spy0( api, &MerginApi::subscriptionInfoChanged );
  QVERIFY( !planId.isEmpty() );
  QSignalSpy spy1( api->workspaceInfo(), &MerginWorkspaceInfo::workspaceInfoChanged );

  purchasingBackend->purchasing()->purchase( planId );
  QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );

  if ( waitForWorkspaceInfoChanged )
  {
    QVERIFY( spy1.wait( TestUtils::LONG_REPLY ) );
  }
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
