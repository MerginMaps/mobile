#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"
#include "inpututils.h"

const QString TestMerginApi::TEST_PROJECT_NAME = "TEMPORARY_TEST_PROJECT";

TestMerginApi::TestMerginApi( MerginApi *api, MerginProjectModel *mpm, ProjectModel *pm, QObject *parent )
{
  mApi = api;
  mMerginProjectModel = mpm;
  mProjectModel = pm;

  initTestCase();

  testListProject();
  testDownloadProject();
  testCreateProjectTwice();
  testDeleteNonExistingProject();
  testCreateDeleteProject();
  testUploadProject();

  cleanupTestCase();
  qDebug() << "TestMerginApi - ALL TESTS PASSED";
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

    // Init DB
    mDataDir = testDataPath();
    initTestProject();
    copyTestProject();

    qDebug() << "TestMerginApi::initTestCase DONE";
  }
}

void TestMerginApi::testListProject()
{
  qDebug() << "TestMerginApi::testListProjectFinished START";
  QSignalSpy spy( mApi, SIGNAL( listProjectsFinished( ProjectList ) ) );
  mApi->listProjects( QString() );

  QVERIFY( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = mMerginProjectModel->projects();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  qDebug() << "TestMerginApi::testListProjectFinished PASSED";
}

/**
 * Download project from a scratch using fetch endpoint.
 */
void TestMerginApi::testDownloadProject()
{
  qDebug() << "TestMerginApi::testDownloadProject START";
  QSignalSpy spy( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  QString projectName = "mobile_demo_mod"; // TODO depends on mergin test server, unless a project is created beforehand
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  mApi->updateProject( projectNamespace, projectName );

  QVERIFY( spy.wait( 5000 ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = mMerginProjectModel->projects();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );

  bool downloadSuccessful = mProjectModel->containsProject( projectNamespace, projectName );
  QVERIFY( downloadSuccessful );
  qDebug() << "TestMerginApi::testDownloadProject PASSED";
}

void TestMerginApi::testCreateProjectTwice()
{
  qDebug() << "TestMerginApi::testCreateProjectTwice START";
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  bool containsTestProject = false;
  ProjectList projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  QVERIFY( hasProject( projectNamespace, projectName, projects ) );

  // Create again, expecting error
  QSignalSpy spy2( mApi, SIGNAL( networkErrorOccurred( QString, QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy2.wait( LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  QList<QVariant> arguments = spy2.takeFirst();
  QVERIFY( arguments.at( 0 ).type() == QVariant::String );
  QVERIFY( arguments.at( 1 ).type() == QVariant::String );

  QCOMPARE( arguments.at( 1 ).toString(), QStringLiteral( "Mergin API error: createProject" ) );

  //Clean created project
  QSignalSpy spy3( mApi, SIGNAL( serverProjectDeleted( QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy3.wait( LONG_REPLY );

  containsTestProject = false;
  projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  qDebug() << "TestMerginApi::testCreateProjectTwice PASSED";
}

void TestMerginApi::testDeleteNonExistingProject()
{
  qDebug() << "TestMerginApi::testDeleteNonExistingProject START";
  // Checks if projects doesn't exist
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  ProjectList projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  // Try to delete non-existing project
  QSignalSpy spy( mApi, SIGNAL( networkErrorOccurred( QString, QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( LONG_REPLY );

  QList<QVariant> arguments = spy.takeFirst();
  QVERIFY( arguments.at( 0 ).type() == QVariant::String );
  QVERIFY( arguments.at( 1 ).type() == QVariant::String );
  QCOMPARE( arguments.at( 1 ).toString(), QStringLiteral( "Mergin API error: deleteProject" ) );

  qDebug() << "TestMerginApi::testDeleteNonExistingProject PASSED";
}

void TestMerginApi::testCreateDeleteProject()
{
  qDebug() << "TestMerginApi::testCreateDeleteProject START";
  // Create a project
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  ProjectList projects = getProjectList();
  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  QVERIFY( hasProject( projectNamespace, projectName, projects ) );

  // Delete created project
  QSignalSpy spy2( mApi, SIGNAL( serverProjectDeleted( QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( LONG_REPLY );

  projects = getProjectList();

  QVERIFY( !hasProject( projectNamespace, projectName, projects ) );
  qDebug() << "TestMerginApi::testCreateDeleteProject PASSED";
}

void TestMerginApi::testUploadProject()
{
  qDebug() << "TestMerginApi::testUploadProject START";
  // Create a project
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  ProjectList projects = getProjectList();
  QVERIFY( hasProject( projectNamespace, projectName, projects ) );

  std::shared_ptr<MerginProject> project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  if ( project->clientUpdated < project->serverUpdated && project->serverUpdated > project->lastSyncClient.toUTC() )
  {
    project->clientUpdated = project->serverUpdated;
    project->projectDir = mProjectModel->dataDir() + projectName;
  }

  QSignalSpy spy( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  mApi->uploadProject( projectNamespace, projectName );
  QVERIFY( spy.wait( LONG_REPLY * 2 ) );
  QCOMPARE( spy.count(), 1 );
  qDebug() << "TestMerginApi::testUploadProject PASSED";
}

void TestMerginApi::cleanupTestCase()
{
  deleteTestProject();

  QDir testDir( mProjectModel->dataDir() );
  testDir.removeRecursively();
  qDebug() << "TestMerginApi::cleanupTestCase DONE";
}

//////// HELPER FUNCTIONS ////////

ProjectList TestMerginApi::getProjectList()
{
  QSignalSpy spy( mApi, SIGNAL( listProjectsFinished( ProjectList ) ) );
  mApi->listProjects( QString(), mUsername, QString(), QString() );
  spy.wait( LONG_REPLY );

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
  qDebug() << "TestMerginApi::initTestProject START";
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  Q_ASSERT( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = getProjectList();
  Q_ASSERT( !mMerginProjectModel->projects().isEmpty() );
  Q_ASSERT( hasProject( projectNamespace, projectName, projects ) );
  qDebug() << "TestMerginApi::initTestProject PASSED";
}

void TestMerginApi::deleteTestProject()
{
  QString projectNamespace = mUsername;

  QSignalSpy spy( mApi, SIGNAL( serverProjectDeleted( QString ) ) );
  mApi->deleteProject( projectNamespace, TestMerginApi::TEST_PROJECT_NAME );
  spy.wait( LONG_REPLY );
  qDebug() << "TestMerginApi::deleteTestProject DONE";
}

void TestMerginApi::copyTestProject()
{
  QString source = mDataDir;
  QString destionation = mProjectModel->dataDir().remove( mProjectModel->dataDir().length() - 1, 1 );
  InputUtils::cpDir( source, destionation );
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

