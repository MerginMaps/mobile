/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testactiveproject.h"
#include "testutils.h"
#include "inpututils.h"
#include "coreutils.h"
#include "activeproject.h"

#include <QtTest/QtTest>

TestActiveProject::TestActiveProject( MerginApi *api )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API
}

TestActiveProject::~TestActiveProject() = default;

void TestActiveProject::init()
{
}

void TestActiveProject::cleanup()
{
}

void TestActiveProject::testProjectValidations()
{
  QString projectDir = TestUtils::testDataDir() + "/project-with-missing-layer-and-invalid-crs";
  QString projectFilename = "bad_layer.qgz";

  AppSettings as;
  ActiveLayer activeLayer;
  ActiveProject activeProject( as, activeLayer, mApi->localProjectsManager() );

  QSignalSpy spyReportIssues( &activeProject, &ActiveProject::reportIssue );
  QSignalSpy spyErrorsFound( &activeProject, &ActiveProject::loadingErrorFound );

  mApi->localProjectsManager().addLocalProject( projectDir, projectFilename );
  QVERIFY( activeProject.load( projectDir + "/" + projectFilename ) );

  QCOMPARE( spyErrorsFound.count(), 1 );
  QCOMPARE( spyReportIssues.count(), 3 ); // invalid project CRS, invalid layer CRS, missing layer Survey

  const QString id = mApi->localProjectsManager().projectId( projectDir + "/" + projectFilename );
  mApi->localProjectsManager().removeLocalProject( id );
}

void TestActiveProject::testProjectLoadFailure()
{
  QString projectname = QStringLiteral( "testProjectLoadFailure" );
  QString projectdir = QDir::tempPath() + "/" + projectname;
  QString projectfilename = "broken_project.qgs";

  InputUtils::cpDir( TestUtils::testDataDir() + "/load_failure", projectdir );

  AppSettings as;
  ActiveLayer activeLayer;
  ActiveProject activeProject( as, activeLayer, mApi->localProjectsManager() );

  mApi->localProjectsManager().addLocalProject( projectdir, projectname );

  QSignalSpy spy( &activeProject, &ActiveProject::projectReadingFailed );
  QVERIFY( !activeProject.load( projectdir + "/" + projectfilename ) );
  QVERIFY( !activeProject.localProject().isValid() );
  QVERIFY( spy.count() );

  const QString id = mApi->localProjectsManager().projectId( projectdir + "/" + projectdir );
  mApi->localProjectsManager().removeLocalProject( id );
}

void TestActiveProject::testPositionTrackingFlag()
{
  // Load two different projects (one with tracking, one without) and make sure that
  // the position tracking availability is correctly set

  AppSettings as;
  ActiveLayer activeLayer;
  ActiveProject activeProject( as, activeLayer, mApi->localProjectsManager() );

  // project "planes" - tracking not enabled
  QString projectDir = TestUtils::testDataDir() + "/planes/";
  QString projectName = "quickapp_project.qgs";

  mApi->localProjectsManager().addLocalProject( projectDir, projectName );

  QSignalSpy spy( &activeProject, &ActiveProject::positionTrackingSupportedChanged );

  activeProject.load( projectDir + projectName );

  QCOMPARE( spy.count(), 1 );
  QCOMPARE( activeProject.positionTrackingSupported(), false );

  QString id = mApi->localProjectsManager().projectId( projectDir + "/" + projectName );
  mApi->localProjectsManager().removeLocalProject( id );

  // project "tracking" - tracking enabled
  projectDir = TestUtils::testDataDir() + "/tracking/";
  projectName = "tracking-project.qgz";

  mApi->localProjectsManager().addLocalProject( projectDir, projectName );

  activeProject.load( projectDir + projectName );

  QCOMPARE( spy.count(), 2 );
  QCOMPARE( activeProject.positionTrackingSupported(), true );

  id = mApi->localProjectsManager().projectId( projectDir + "/" + projectName );
  mApi->localProjectsManager().removeLocalProject( id );
}

void TestActiveProject::testRecordingAllowed()
{
  QString projectDir = TestUtils::testDataDir() + "/tracking/";
  QString projectFilename = "tracking-project.qgz";

  AppSettings as;
  ActiveLayer activeLayer;
  ActiveProject activeProject( as, activeLayer, mApi->localProjectsManager() );

  mApi->localProjectsManager().addLocalProject( projectDir, projectFilename );
  QVERIFY( activeProject.load( projectDir + "/" + projectFilename ) );

  // 1: null layer should return false
  QCOMPARE( activeProject.recordingAllowed( nullptr ), false );

  QgsVectorLayer *validLayer = new QgsVectorLayer( "Polygon?crs=EPSG:4326", "PolygonLayer", "memory" );
  QVERIFY( validLayer->isValid() );

  QgsProject *project = activeProject.qgsProject();
  project->addMapLayer( validLayer );

  // 2: valid and writable layer should return true
  QCOMPARE( activeProject.recordingAllowed( validLayer ), true );

  // 3: read-only layer should return false
  QgsVectorLayer *readOnlyLayer = new QgsVectorLayer( projectDir + "/tracking_layer.gpkg", "ReadOnlyLayer", "ogr" );
  QVERIFY( readOnlyLayer->isValid() );
  project->addMapLayer( readOnlyLayer );
  readOnlyLayer->setReadOnly( true );

  QVERIFY( readOnlyLayer->readOnly() );
  QCOMPARE( activeProject.recordingAllowed( readOnlyLayer ), false );

  // 4: no geometry should return false
  QgsVectorLayer *noGeomLayer = new QgsVectorLayer( "None", "NoGeomLayer", "memory" );
  QVERIFY( noGeomLayer->isValid() );
  project->addMapLayer( noGeomLayer );
  QCOMPARE( activeProject.recordingAllowed( noGeomLayer ), false );

  // 5: position tracking layer should return false
  project->writeEntry( "Mergin", "PositionTracking/TrackingLayer", validLayer->id() );
  QCOMPARE( activeProject.recordingAllowed( validLayer ), false );

  // restore position tracking layer id and it should return true
  project->writeEntry( "Mergin", "PositionTracking/TrackingLayer", QString() );
  QCOMPARE( activeProject.recordingAllowed( validLayer ), true );

  const QString id = mApi->localProjectsManager().projectId( projectDir + "/" + projectFilename );
  mApi->localProjectsManager().removeLocalProject( id );
}

void TestActiveProject::testLoadingFlagFileExpiration()
{
  AppSettings as;
  ActiveLayer activeLayer;
  ActiveProject activeProject( as, activeLayer, mApi->localProjectsManager() );

  // project "planes" - tracking not enabled
  QString projectDir = TestUtils::testDataDir() + "/planes/";
  QString projectName = "quickapp_project.qgs";

  mApi->localProjectsManager().addLocalProject( projectDir, projectName );

  activeProject.load( projectDir + projectName );

  QSignalSpy spyLoadingStarted( &activeProject, &ActiveProject::loadingStarted );

  // check that loading flag file is created
  QFile flagFile( ActiveProject::LOADING_FLAG_FILE_PATH );
  QVERIFY( flagFile.exists() );

  // wait for expiration + some margin
  QTest::qWait( ActiveProject::LOADING_FLAG_FILE_EXPIRATION_MS + 1000 );

  QVERIFY( !flagFile.exists() );
}

void TestActiveProject::testLoadingAuthFileFromConfiguration()
{
  AppSettings appSettings;
  ActiveLayer activeLayer;
  ActiveProject activeProject( appSettings, activeLayer, mApi->localProjectsManager() );
  QString projectDir = TestUtils::testDataDir() + QStringLiteral( "/project_auth_file/" );
  QString projectName = QStringLiteral( "auth-test.qgz" );
  QString authFile = QDir( projectDir ).filePath( CoreUtils::AUTH_CONFIG_FILENAME );

  QgsAuthManager *authManager = QgsApplication::authManager();

  mApi->localProjectsManager().addLocalProject( projectDir, projectName );
  activeProject.load( projectDir + projectName );

  QSignalSpy spyLoadingStarted( &activeProject, &ActiveProject::loadingStarted );

  // we expect the configuration import to fail
  int count = authManager->configIds().count();
  QCOMPARE( count, 0 );

  QFileInfo cfgFileInfo( authFile );
  if ( cfgFileInfo.exists() && cfgFileInfo.isFile() )
  {
    // we still check that the configuration can be imported
    bool ok = authManager->importAuthenticationConfigsFromXml( authFile, AUTH_CONFIG_PASSWORD, true );

    QCOMPARE( ok, true );
    count = authManager->configIds().count();
    QCOMPARE( count, 1 );
  }
}
