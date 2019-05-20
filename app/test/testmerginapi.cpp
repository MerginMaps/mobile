#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"

TestMerginApi::TestMerginApi( MerginApi *api, MerginProjectModel *mpm, ProjectModel *pm, QObject *parent )
{
  mApi = api;
  mMerginProjectModel = mpm;
  mProjectModel = pm;

  initTestCase();

  testListProject();
  testDownloadProject();

  // TODO Fix all related with create/delete project
  //testCreateProjectTwice();
  //testDeleteNonExistingProject();
  //testCreateDeleteProject();

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
    QVERIFY( spy.wait( SHORT_REPLY ) );
    QCOMPARE( spy.count(), 1 );
  }
}

void TestMerginApi::testListProject()
{
  qDebug() << "TestMerginApi::testListProjectFinished START";
  QSignalSpy spy( mApi, SIGNAL( listProjectsFinished( ProjectList ) ) );
  mApi->listProjects( QString() );

  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = mMerginProjectModel->projects();
  Q_ASSERT( !mMerginProjectModel->projects().isEmpty() );
  qDebug() << "TestMerginApi::testListProjectFinished PASSED";
}

void TestMerginApi::testDownloadProject()
{
  qDebug() << "TestMerginApi::testDownloadProject START";
  QSignalSpy spy( mApi, SIGNAL( syncProjectFinished( QString, QString, bool ) ) );
  QString projectName = "mobile_demo_mod"; // TODO depends on mergin test server, unless a project is created beforehand
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  mApi->downloadProject( projectNamespace, projectName );

  QVERIFY( spy.wait( 5000 ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = mMerginProjectModel->projects();
  Q_ASSERT( !mMerginProjectModel->projects().isEmpty() );
  //projectName = mMerginProjectModel->projects().at( 0 )->name;

  bool downloadSuccessful = mProjectModel->containsProject( projectName );
  Q_ASSERT( downloadSuccessful );
  qDebug() << "TestMerginApi::testDownloadProject PASSED";
}

void TestMerginApi::testCreateProjectTwice()
{
  qDebug() << "TestMerginApi::testCreateProjectTwice START";
  QString projectName = "TEMPORARY_TEST_PROJECT";
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  bool containsTestProject = false;
  ProjectList projects = getProjectList();
  Q_ASSERT( !hasProject( projectName, projects ) );

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  Q_ASSERT( !mMerginProjectModel->projects().isEmpty() );
  Q_ASSERT( hasProject( projectName, projects ) );

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
  Q_ASSERT( !hasProject( projectName, projects ) );

  qDebug() << "TestMerginApi::testCreateProjectTwice PASSED";
}

void TestMerginApi::testDeleteNonExistingProject()
{
  qDebug() << "TestMerginApi::testDeleteNonExistingProject START";
  // Checks if projects doesn't exist
  QString projectName = "TEMPORARY_TEST_PROJECT";
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  ProjectList projects = getProjectList();
  Q_ASSERT( !hasProject( projectName, projects ) );

  // Try to delete non-existing project
  QSignalSpy spy( mApi, SIGNAL( networkErrorOccurred( QString, QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );

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
  QString projectName = "TEMPORARY_TEST_PROJECT";
  QString projectNamespace = mUsername; // TODO depends on mergin test server, unless a project is created beforehand
  ProjectList projects = getProjectList();
  Q_ASSERT( !hasProject( projectName, projects ) );

  QSignalSpy spy( mApi, SIGNAL( projectCreated( QString ) ) );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  Q_ASSERT( !mMerginProjectModel->projects().isEmpty() );
  Q_ASSERT( hasProject( projectName, projects ) );

  // Delete created project
  QSignalSpy spy2( mApi, SIGNAL( serverProjectDeleted( QString ) ) );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );

  projects = getProjectList();

  Q_ASSERT( !hasProject( projectName, projects ) );
  qDebug() << "TestMerginApi::testCreateDeleteProject PASSED";
}

void TestMerginApi::cleanupTestCase()
{
  QDir testDir( mProjectModel->dataDir() );
  testDir.removeRecursively();
}

ProjectList TestMerginApi::getProjectList()
{
  QSignalSpy spy( mApi, SIGNAL( listProjectsFinished( ProjectList ) ) );
  mApi->listProjects( QString() );
  spy.wait( SHORT_REPLY );

  return mApi->projects();
}

bool TestMerginApi::hasProject( QString projectName, ProjectList projects )
{
  for ( std::shared_ptr<MerginProject> project : projects )
  {
    if ( project->name == projectName ) return true;
  }
  return false;
}
