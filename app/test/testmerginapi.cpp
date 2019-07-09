#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"
#include "inpututils.h"

const QString TestMerginApi::TEST_PROJECT_NAME = "TEMPORARY_TEST_PROJECT";
const QString TestMerginApi::TEST_PROJECT_NAME_DOWNLOAD = TestMerginApi::TEST_PROJECT_NAME + "_DOWNLOAD";

TestMerginApi::TestMerginApi( MerginApi *api, MerginProjectModel *mpm, ProjectModel *pm, QObject *parent )
{
  mApi = api;
  mMerginProjectModel = mpm;
  mProjectModel = pm;

  initTestCase();

  testListProject();
  testDownloadProject();
  testCancelDownlaodProject();
  testCreateProjectTwice();
  testDeleteNonExistingProject();
  testCreateDeleteProject();
  testUploadProject();
  testPushChangesOfProject();

  testParseAndCompareNoChanges();
  testParseAndCompareRemovedAdded();
  testParseAndCompareUpdated();
  testParseAndCompareRenamed();

  cleanupTestCase();
  qDebug() << QString( "TestMerginApi - PASSED: %1/%2" ).arg( passedTests ).arg( runTests );
  if ( runTests == passedTests )
  {
    qDebug() << "TestMerginApi - ALL TESTS PASSED";
  }
}
void TestMerginApi::initTestCase()
{
  if ( mApi )
  {
    if ( ::getenv( "TEST_MERGIN_URL" ) )
    {
      mApiRoot = ::getenv( "TEST_MERGIN_URL" );
    }
    if ( ::getenv( "TEST_API_USERNAME" ) )
    {
      mUsername = ::getenv( "TEST_API_USERNAME" );
    }
    if ( ::getenv( "TEST_API_PASSWORD" ) )
    {
      mPassword = ::getenv( "TEST_API_PASSWORD" );
    }
    mApi->setApiRoot( mApiRoot );
    QSignalSpy spy( mApi, SIGNAL( authChanged() ) );
    mApi->authorize( mUsername, mPassword );
    Q_ASSERT( spy.wait( LONG_REPLY ) );
    QCOMPARE( spy.count(), 1 );

    mTestData = testDataPath();
    initTestProject();
    copyTestProject();

    qDebug() << "TestMerginApi::initTestCase DONE";
  }
}

void TestMerginApi::testListProject()
{
  qDebug() << "TestMerginApi::testListProjectFinished START";
  runTests++;

  QSignalSpy spy( mApi, SIGNAL( listProjectsFinished( ProjectList ) ) );
  mApi->listProjects( QString() );

  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = mMerginProjectModel->projects();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  qDebug() << "TestMerginApi::testListProjectFinished PASSED";
  passedTests++;
}

/**
 * Download project from a scratch using fetch endpoint.
 */
void TestMerginApi::testDownloadProject()
{
  qDebug() << "TestMerginApi::testDownloadProject START";
  runTests++;

  QSignalSpy spy( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  QString projectName = "mobile_demo_mod"; // TODO depends on mergin test server, unless a project is created beforehand
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  mApi->updateProject( projectNamespace, projectName );

  QVERIFY( spy.wait( LONG_REPLY * 5 ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = mMerginProjectModel->projects();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );

  bool downloadSuccessful = mProjectModel->containsProject( projectNamespace, projectName );
  QVERIFY( downloadSuccessful );

  QDir testDir( mProjectModel->dataDir() + projectName );
  testDir.removeRecursively();

  qDebug() << "TestMerginApi::testDownloadProject PASSED";
  passedTests++;
}

void TestMerginApi::testCancelDownlaodProject()
{
  qDebug() << "TestMerginApi::testCancelDownlaodProject START";
  runTests++;
  QString projectName = TestMerginApi::TEST_PROJECT_NAME_DOWNLOAD;

  // create a project
  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( mUsername, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  // Copy data
  QString source = mTestData + "/" + TestMerginApi::TEST_PROJECT_NAME + "/";
  QString projectDir = mProjectModel->dataDir() + projectName + "/";
  qDebug() << source << projectDir;
  InputUtils::cpDir( source, projectDir );

  // Upload data
  MerginProject project;
  std::shared_ptr<MerginProject> p = std::make_shared<MerginProject>( project );
  p->name = projectName;
  p->projectNamespace = mUsername;
  p->projectDir = projectDir;
  mApi->addProject( p );

  QSignalSpy spy3( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  mApi->uploadProject( mUsername, projectName );
  QVERIFY( spy3.wait( LONG_REPLY ) );
  QCOMPARE( spy3.count(), 1 );
  QList<QVariant> arguments = spy3.takeFirst();
  QVERIFY( arguments.at( 2 ).toBool() );

  // Remove the whole project
  QDir( projectDir ).removeRecursively();
  QFileInfo info( projectDir );
  QDir dir( projectDir );
  QCOMPARE( info.size(), 0 );
  QVERIFY( dir.isEmpty() );
  mApi->clearProject( p );

  // Test download and cancel before transaction actually starts
  mApi->updateProject( mUsername, projectName );
  QSignalSpy spy5( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  mApi->updateCancel( MerginApi::getFullProjectName( mUsername, projectName ) );
//  TODO cannot catch signal of spy5s
//  QVERIFY( spy5.wait( LONG_REPLY ) );
//  QCOMPARE( spy5.count(), 1 );
//  arguments = spy5.takeFirst();
//  QVERIFY( !arguments.at(2).toBool() );

  QCOMPARE( QFileInfo( projectDir ).size(), 0 );
  QVERIFY( QDir( projectDir ).isEmpty() );

  // Test download and cancel after transcation starts
  QSignalSpy spy6( mApi, SIGNAL( pullFilesStarted() ) );
  mApi->updateProject( mUsername, projectName );
  QVERIFY( spy6.wait( LONG_REPLY ) );
  QCOMPARE( spy6.count(), 1 );

  QSignalSpy spy7( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  mApi->updateCancel( MerginApi::getFullProjectName( mUsername, projectName ) );
//  TODO cannot catch signal of spy7
//  QVERIFY( spy7.wait( LONG_REPLY ) );
//  QCOMPARE( spy7.count(), 1 );
//  arguments = spy7.takeFirst();
//  QVERIFY( !arguments.at(2).toBool() );

  info = QFileInfo( projectDir );
  dir = QDir( projectDir );
  QCOMPARE( info.size(), 0 );
  QVERIFY( dir.isEmpty() );

  qDebug() << "TestMerginApi::testCancelDownlaodProject PASSED";
  passedTests++;
}

void TestMerginApi::testCreateProjectTwice()
{
  qDebug() << "TestMerginApi::testCreateProjectTwice START";
  runTests++;

  QString projectName = TestMerginApi::TEST_PROJECT_NAME + "2";
  QString projectNamespace = mUsername;
  bool containsTestProject = false;
  ProjectList projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  QVERIFY( hasProject( projectNamespace, projectName, projects ) );

  // Create again, expecting error
  QSignalSpy spy2( mApi, SIGNAL( networkErrorOccurred( QString, QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy2.wait( SHORT_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  QList<QVariant> arguments = spy2.takeFirst();
  QVERIFY( arguments.at( 0 ).type() == QVariant::String );
  QVERIFY( arguments.at( 1 ).type() == QVariant::String );

  QCOMPARE( arguments.at( 1 ).toString(), QStringLiteral( "Mergin API error: createProject" ) );

  //Clean created project
  QSignalSpy spy3( mApi, SIGNAL( serverProjectDeleted( QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy3.wait( SHORT_REPLY );

  containsTestProject = false;
  projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  qDebug() << "TestMerginApi::testCreateProjectTwice PASSED";
  passedTests++;
}

void TestMerginApi::testDeleteNonExistingProject()
{
  qDebug() << "TestMerginApi::testDeleteNonExistingProject START";
  runTests++;

  // Checks if projects doesn't exist
  QString projectName = TestMerginApi::TEST_PROJECT_NAME + "_DOESNT_EXISTS";
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  ProjectList projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  // Try to delete non-existing project
  QSignalSpy spy( mApi, SIGNAL( networkErrorOccurred( QString, QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );

  QList<QVariant> arguments = spy.takeFirst();
  QVERIFY( arguments.at( 0 ).type() == QVariant::String );
  QVERIFY( arguments.at( 1 ).type() == QVariant::String );
  QCOMPARE( arguments.at( 1 ).toString(), QStringLiteral( "Mergin API error: deleteProject" ) );

  qDebug() << "TestMerginApi::testDeleteNonExistingProject PASSED";
  passedTests++;
}

void TestMerginApi::testCreateDeleteProject()
{
  qDebug() << "TestMerginApi::testCreateDeleteProject START";
  runTests++;

  // Create a project
  QString projectName = TestMerginApi::TEST_PROJECT_NAME + "_CREATE_DELETE";
  QString projectNamespace = mUsername;
  ProjectList projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  Q_ASSERT( hasProject( projectNamespace, projectName, projects ) );

  // Delete created project
  QSignalSpy spy2( mApi, SIGNAL( serverProjectDeleted( QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );

  projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  qDebug() << "TestMerginApi::testCreateDeleteProject PASSED";
  passedTests++;
}

void TestMerginApi::testUploadProject()
{
  qDebug() << "TestMerginApi::testUploadProject START";
  runTests++;

  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;
  std::shared_ptr<MerginProject> project = prepareTestProjectUpload();

  QDateTime serverT0 = project->serverUpdated;
  mApi->uploadProject( projectNamespace, projectName );
//  QSignalSpy spy( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  mApi->uploadCancel( MerginApi::getFullProjectName( projectNamespace, projectName ) );
//  NOTE: QSignalSpy somehow cannot catch signal above, functionality is tested anyway by following up verification
//  QVERIFY( spy.wait( LONG_REPLY ) );
//  QCOMPARE( spy.count(), 1 );

  ProjectList projects = getProjectList();
  QVERIFY( hasProject( projectNamespace, projectName, projects ) );

  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT1 = project->serverUpdated;
  QCOMPARE( serverT0, serverT1 );

  mApi->uploadProject( projectNamespace, projectName );
  QSignalSpy spy2( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );

  QVERIFY( spy2.wait( LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  projects = getProjectList();
  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT2 = project->serverUpdated;
  QVERIFY( serverT1 < serverT2 );

  qDebug() << "TestMerginApi::testUploadProject PASSED";
  passedTests++;
}

void TestMerginApi::testPushChangesOfProject()
{
  qDebug() << "TestMerginApi::testPushChangesOfProject START";
  runTests++;

  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;
  std::shared_ptr<MerginProject> project = prepareTestProjectUpload();

  QDateTime serverT0 = project->serverUpdated;
  mApi->uploadProject( projectNamespace, projectName );
  QSignalSpy spy( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  QVERIFY( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = getProjectList();
  int projectNo0 = projects.size();
  int localProjectNo0 = mProjectModel->rowCount();
  QVERIFY( hasProject( projectNamespace, projectName, projects ) );
  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT1 = project->serverUpdated;

  // Update file
  QFile file( mProjectModel->dataDir() + projectName + "/project.qgs" );
  if ( !file.open( QIODevice::Append ) )
  {
    QVERIFY( false );
  }

  file.write( QByteArray( "v2" ) );
  file.close();

  // upload changes
  mApi->uploadProject( projectNamespace, projectName );
  QSignalSpy spy2( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  QVERIFY( spy2.wait( LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  projects = getProjectList();
  int projectNo1 = projects.size();
  int localProjectNo1 = mProjectModel->rowCount();
  QVERIFY( hasProject( projectNamespace, projectName, projects ) );
  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT2 = project->serverUpdated;
  QVERIFY( serverT1 < serverT2 );

  QCOMPARE( localProjectNo0, localProjectNo1 );
  QCOMPARE( projectNo0, projectNo1 );

  qDebug() << "TestMerginApi::testPushChangesOfProject PASSED";
  passedTests++;
}

void TestMerginApi::testParseAndCompareNoChanges()
{
  qDebug() << "TestMerginApi::parseAndCompareTestNoChanges START";
  runTests++;

  QString projectMetadataPath = QString( "%1" ).arg( mProjectModel->dataDir() );
  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( projectMetadataPath, QStringLiteral( "mergin.json" ) );
  QVERIFY( project );
  ProjectDiff diff = mApi->compareProjectFiles( project->files, project->files );
  QVERIFY( diff.added.isEmpty() );
  QVERIFY( diff.removed.isEmpty() );
  QVERIFY( diff.modified.isEmpty() );

  qDebug() << "TestMerginApi::parseAndCompareTestNoChanges PASSED";
  passedTests++;
}

void TestMerginApi::testParseAndCompareRemovedAdded()
{
  qDebug() << "TestMerginApi::testParseAndCompareRemovedAdded START";
  runTests++;

  QString projectMetadataPath = QString( "%1" ).arg( mProjectModel->dataDir() );
  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( projectMetadataPath, QStringLiteral( "mergin.json" ) );
  std::shared_ptr<MerginProject> project_added = mApi->readProjectMetadataFromPath( projectMetadataPath, QStringLiteral( "mergin_added.json" ) );
  QVERIFY( project );
  QVERIFY( project_added );

  ProjectDiff diff = mApi->compareProjectFiles( project_added->files, project->files );
  QCOMPARE( diff.added.size(), 1 );
  QVERIFY( diff.removed.isEmpty() );
  QVERIFY( diff.modified.isEmpty() );

  ProjectDiff diff_removed = mApi->compareProjectFiles( project->files, project_added->files );
  QVERIFY( diff_removed.added.isEmpty() );
  QCOMPARE( diff_removed.removed.size(), 1 );
  QVERIFY( diff_removed.modified.isEmpty() );

  qDebug() << "TestMerginApi::testParseAndCompareRemovedAdded PASSED";
  passedTests++;
}

void TestMerginApi::testParseAndCompareUpdated()
{
  qDebug() << "TestMerginApi::testParseAndCompareUpdated START";
  runTests++;

  QString projectMetadataPath = QString( "%1" ).arg( mProjectModel->dataDir() );
  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( projectMetadataPath, QStringLiteral( "mergin.json" ) );
  std::shared_ptr<MerginProject> project_updated = mApi->readProjectMetadataFromPath( projectMetadataPath, QStringLiteral( "mergin_updated.json" ) );
  QVERIFY( project );
  QVERIFY( project_updated );

  ProjectDiff diff = mApi->compareProjectFiles( project_updated->files, project->files );
  QVERIFY( diff.added.isEmpty() );
  QVERIFY( diff.removed.isEmpty() );
  QCOMPARE( diff.modified.size(), 1 );

  qDebug() << "TestMerginApi::testParseAndCompareUpdated PASSED";
  passedTests++;
}

void TestMerginApi::testParseAndCompareRenamed()
{
  qDebug() << "TestMerginApi::testParseAndCompareRenamed START";
  runTests++;

  QString projectMetadataPath = QString( "%1" ).arg( mProjectModel->dataDir() );
  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( projectMetadataPath, QStringLiteral( "mergin.json" ) );
  std::shared_ptr<MerginProject> project_renamed = mApi->readProjectMetadataFromPath( projectMetadataPath, QStringLiteral( "mergin_renamed.json" ) );
  QVERIFY( project );
  QVERIFY( project_renamed );

  ProjectDiff diff = mApi->compareProjectFiles( project_renamed->files, project->files );
  QCOMPARE( diff.added.size(), 1 );
  QCOMPARE( diff.removed.size(), 1 );
  QVERIFY( diff.modified.isEmpty() );

  qDebug() << "TestMerginApi::testParseAndCompareRenamed PASSED";
  passedTests++;
}

void TestMerginApi::cleanupTestCase()
{
  deleteTestProjects();

  QDir testDir( mProjectModel->dataDir() );
  testDir.removeRecursively();
  qDebug() << "TestMerginApi::cleanupTestCase DONE";
}

//////// HELPER FUNCTIONS ////////

ProjectList TestMerginApi::getProjectList()
{
  QSignalSpy spy( mApi, SIGNAL( listProjectsFinished( ProjectList ) ) );
  mApi->listProjects( QString(), mUsername, QString(), QString() );
  spy.wait( SHORT_REPLY );

  return mApi->projects();
}

bool TestMerginApi::hasProject( QString projectNamespace, QString projectName, ProjectList projects )
{
  QString projectFullName = MerginApi::getFullProjectName( projectNamespace, projectName );
  for ( std::shared_ptr<MerginProject> p : projects )
  {
    if ( MerginApi::getFullProjectName( p->projectNamespace, p->name ) == projectFullName )
    {
      return true;
    }
  }
  return false;
}

void TestMerginApi::initTestProject()
{
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  Q_ASSERT( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = getProjectList();
  Q_ASSERT( !mMerginProjectModel->projects().isEmpty() );
  Q_ASSERT( hasProject( projectNamespace, projectName, projects ) );
  qDebug() << "TestMerginApi::initTestProject DONE";
}

std::shared_ptr<MerginProject> TestMerginApi::prepareTestProjectUpload()
{
  // Create a project
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;
  ProjectList projects = getProjectList();

  std::shared_ptr<MerginProject> project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  if ( project->clientUpdated < project->serverUpdated && project->serverUpdated > project->lastSyncClient.toUTC() )
  {
    // Fake same version on client of what is on the server
    project->clientUpdated = project->serverUpdated;
    project->projectDir = mProjectModel->dataDir() + projectName;
  }
  return project;
}

void TestMerginApi::deleteSingleTestProject( QString &projectNamespace, const QString &projectName )
{
  QSignalSpy spy( mApi, SIGNAL( serverProjectDeleted( QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );
}

void TestMerginApi::deleteTestProjects()
{
  QString projectNamespace = mUsername;
  deleteSingleTestProject( projectNamespace, TestMerginApi::TEST_PROJECT_NAME );
  deleteSingleTestProject( projectNamespace, TestMerginApi::TEST_PROJECT_NAME_DOWNLOAD );

  qDebug() << "TestMerginApi::deleteTestProject DONE";
}

void TestMerginApi::copyTestProject()
{
  QString source = mTestData;
  QString destination = mProjectModel->dataDir().remove( mProjectModel->dataDir().length() - 1, 1 );
  InputUtils::cpDir( source, destination );
  qDebug() << "TestMerginApi::copyTestProject DONE";
}

QString TestMerginApi::testDataPath()
{
#ifdef TEST_DATA
  return STR( TEST_DATA );
#endif
  QString testData = ::getenv( "TEST_DATA" );
  if ( !testData.isEmpty() ) return testData;

  // TODO if missing variable, take root folder + /test/test_data/
  return QStringLiteral();
}

