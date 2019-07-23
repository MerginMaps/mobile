#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"
#include "inpututils.h"

const QString TestMerginApi::TEST_PROJECT_NAME = "TEMPORARY_TEST_PROJECT";
const QString TestMerginApi::TEST_PROJECT_NAME_DOWNLOAD = TestMerginApi::TEST_PROJECT_NAME + "_DOWNLOAD";


static std::shared_ptr<MerginProject> _findProjectByName( const QString &projectNamespace, const QString &projectName, const ProjectList &projects )
{
  for ( std::shared_ptr<MerginProject> project : projects )
  {
    if ( project->name == projectName && project->projectNamespace == projectNamespace )
      return project;
  }
  return nullptr;
}


TestMerginApi::TestMerginApi( MerginApi *api, MerginProjectModel *mpm, ProjectModel *pm )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API
  mMerginProjectModel = mpm;
  mProjectModel = pm;
}

void TestMerginApi::initTestCase()
{
  // these env variables really need to be set!
  QVERIFY( ::getenv( "TEST_MERGIN_URL" ) );
  QVERIFY( ::getenv( "TEST_API_USERNAME" ) );
  QVERIFY( ::getenv( "TEST_API_PASSWORD" ) );

  QString apiRoot = ::getenv( "TEST_MERGIN_URL" );
  QString username = ::getenv( "TEST_API_USERNAME" );
  QString password = ::getenv( "TEST_API_PASSWORD" );

  qDebug() << "MERGIN API ROOT:" << apiRoot;
  qDebug() << "MERGIN USERNAME:" << username;

  // let's make sure we do not mess with the public instance
  QVERIFY( apiRoot != MerginApi::defaultApiRoot() );

  mApi->setApiRoot( apiRoot );
  QSignalSpy spy( mApi, &MerginApi::authChanged );
  mApi->authorize( username, password );
  Q_ASSERT( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  mUsername = username;  // keep for later

  QDir testDataDir( STR( INPUT_TEST_DATA_DIR ) );  // #defined in input.pro
  mTestDataPath = testDataDir.canonicalPath();  // get rid of any ".." that may cause problems later
  qDebug() << "test data dir:" << mTestDataPath;

  // prepare empty projects directory
  QDir testProjectsExtraDir( testDataDir.path() + "/../temp_extra_projects" );
  if ( testProjectsExtraDir.exists() )
    testProjectsExtraDir.removeRecursively();
  QDir( testDataDir.path() + "/.." ).mkpath( "temp_extra_projects" );
  QString projectsExtraDir = testProjectsExtraDir.canonicalPath() + "/";

  // create extra API to do requests we are not testing (as if some other user did those)
  mApiExtra = new MerginApi( projectsExtraDir );
  mApiExtra->setApiRoot( mApi->apiRoot() );
  QSignalSpy spyExtra( mApiExtra, &MerginApi::authChanged );
  mApiExtra->authorize( mApi->mUsername, mApi->mPassword );
  Q_ASSERT( spyExtra.wait( LONG_REPLY ) );
  QCOMPARE( spyExtra.count(), 1 );

  // remove any projects on the server that may prevent us from creating them
  deleteRemoteProject( mApiExtra, mUsername, "testListProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testDownloadProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushAddedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushModifiedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateAddedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateRemovedFiles" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateRemovedVsModifiedFiles" );
  deleteRemoteProject( mApiExtra, mUsername, "testConflictRemoteUpdateLocalUpdate" );
  deleteRemoteProject( mApiExtra, mUsername, "testConflictRemoteAddLocalAdd" );
  deleteRemoteProject( mApiExtra, mUsername, TestMerginApi::TEST_PROJECT_NAME );
  deleteRemoteProject( mApiExtra, mUsername, TestMerginApi::TEST_PROJECT_NAME_DOWNLOAD );

  initTestProject();

  qDebug() << "TestMerginApi::initTestCase DONE";
}

void TestMerginApi::cleanupTestCase()
{
  delete mApiExtra;

  // do not remove test projects on the server - keep them around for inspection (deleted when tests are starting)

  // do not remove project data locally - keep them around so it's possible to inspect their final state if needed
}

void TestMerginApi::testListProject()
{
  QString projectName = "testListProject";

  // check that there's no hello world project
  QSignalSpy spy0( mApi, &MerginApi::listProjectsFinished );
  mApi->listProjects( QString() );
  QVERIFY( spy0.wait( SHORT_REPLY ) );
  QCOMPARE( spy0.count(), 1 );
  QVERIFY( !_findProjectByName( mUsername, projectName, mMerginProjectModel->projects() ) );

  // create the project on the server (the content is not important)
  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // check the project exists on the server
  QSignalSpy spy( mApi, &MerginApi::listProjectsFinished );
  mApi->listProjects( QString() );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QVERIFY( _findProjectByName( mUsername, projectName, mMerginProjectModel->projects() ) );
}

/**
 * Download project from a scratch using fetch endpoint.
 */
void TestMerginApi::testDownloadProject()
{
  // create the project on the server (the content is not important)
  QString projectName = "testDownloadProject";
  QString projectNamespace = mUsername;
  createRemoteProject( mApiExtra, projectNamespace, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // add an entry about this project to main API - otherwise it fails
  QCOMPARE( mApi->transactions().count(), 0 );

  // try to download the project
  QSignalSpy spy( mApi, &MerginApi::syncProjectFinished );
  mApi->updateProject( projectNamespace, projectName );
  QCOMPARE( mApi->transactions().count(), 1 );
  QVERIFY( spy.wait( LONG_REPLY * 5 ) );
  QCOMPARE( spy.count(), 1 );

  QCOMPARE( mApi->transactions().count(), 0 );

  ProjectList projects = mMerginProjectModel->projects();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );

  bool downloadSuccessful = mProjectModel->containsProject( projectNamespace, projectName );
  QVERIFY( downloadSuccessful );

  std::shared_ptr<MerginProject> project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project );
  QCOMPARE( project->name, projectName );
  QCOMPARE( project->projectNamespace, projectNamespace );
  QCOMPARE( project->projectDir, mApi->projectsPath() + projectName );  // assuming no name clash that would require renaming of the dir
  QCOMPARE( project->version, "v2" );  // for some unknown reason, first version in Mergin is v2 :-)
  QCOMPARE( project->filesCount, 2 );  // .qgs + .txt file
  QCOMPARE( project->status, UpToDate );

  // TODO: bug - project info of the mergin project is not updated when starting download (e.g. serverUpdated is lost)
  //QCOMPARE( mApi->getProjectStatus( project, mApi->getLastModifiedFileDateTime( project->projectDir ) ), UpToDate );

  // there should be something in the directory
  QStringList projectDirEntries = QDir( project->projectDir ).entryList( QDir::AllEntries | QDir::NoDotAndDotDot );
  QCOMPARE( projectDirEntries.count(), 2 );
}

void TestMerginApi::createRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, const QString &sourcePath )
{
  // create a project
  QSignalSpy spy( api, &MerginApi::projectCreated );
  api->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  // Copy data
  QString projectDir = api->projectsPath() + projectName + "/";
  InputUtils::cpDir( sourcePath, projectDir );

  // make MerginApi aware of the project and its directory
  std::shared_ptr<MerginProject> p = std::make_shared<MerginProject>();
  p->name = projectName;
  p->projectNamespace = projectNamespace;
  p->projectDir = projectDir;
  api->addProject( p );

  // Upload data
  QSignalSpy spy3( api, &MerginApi::syncProjectFinished );
  api->uploadProject( projectNamespace, projectName );
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
  api->clearProject( p );

  QCOMPARE( QFileInfo( projectDir ).size(), 0 );
  QVERIFY( QDir( projectDir ).isEmpty() );
}


void TestMerginApi::testCancelDownloadProject()
{
  qDebug() << "TestMerginApi::testCancelDownlaodProject START";
  QString projectName = TestMerginApi::TEST_PROJECT_NAME_DOWNLOAD;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TestMerginApi::TEST_PROJECT_NAME + "/" );

  QCOMPARE( mApi->transactions().count(), 0 );

  QString projectDir = mApi->projectsPath() + projectName + "/";

  // Test download and cancel before transaction actually starts
  QSignalSpy spy5( mApi, &MerginApi::syncProjectFinished );
  mApi->updateProject( mUsername, projectName );
  QCOMPARE( mApi->transactions().count(), 1 );
  mApi->updateCancel( MerginApi::getFullProjectName( mUsername, projectName ) );

  // no need to wait for the signal here - as we call abort() the reply's finished() signal is immediately emitted
  QCOMPARE( spy5.count(), 1 );
  QList<QVariant> arguments = spy5.takeFirst();
  QVERIFY( !arguments.at( 2 ).toBool() );

  QCOMPARE( QFileInfo( projectDir ).size(), 0 );
  QVERIFY( QDir( projectDir ).isEmpty() );

  QCOMPARE( mApi->transactions().count(), 0 );

  // Test download and cancel after transcation starts
  QSignalSpy spy6( mApi, &MerginApi::pullFilesStarted );
  mApi->updateProject( mUsername, projectName );
  QVERIFY( spy6.wait( LONG_REPLY ) );
  QCOMPARE( spy6.count(), 1 );

  QSignalSpy spy7( mApi, &MerginApi::syncProjectFinished );
  mApi->updateCancel( MerginApi::getFullProjectName( mUsername, projectName ) );

  // no need to wait for the signal here - as we call abort() the reply's finished() signal is immediately emitted
  QCOMPARE( spy7.count(), 1 );
  arguments = spy7.takeFirst();
  QVERIFY( !arguments.at( 2 ).toBool() );

  QFileInfo info( projectDir );
  QDir dir( projectDir );
  QCOMPARE( info.size(), 0 );
  QVERIFY( dir.isEmpty() );

  qDebug() << "TestMerginApi::testCancelDownlaodProject PASSED";
}

void TestMerginApi::testCreateProjectTwice()
{
  qDebug() << "TestMerginApi::testCreateProjectTwice START";

  QString projectName = TestMerginApi::TEST_PROJECT_NAME + "2";
  QString projectNamespace = mUsername;
  ProjectList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ) );

  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ) );

  // Create again, expecting error
  QSignalSpy spy2( mApi, &MerginApi::networkErrorOccurred );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy2.wait( SHORT_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  QList<QVariant> arguments = spy2.takeFirst();
  QVERIFY( arguments.at( 0 ).type() == QVariant::String );
  QVERIFY( arguments.at( 1 ).type() == QVariant::String );

  QCOMPARE( arguments.at( 1 ).toString(), QStringLiteral( "Mergin API error: createProject" ) );

  //Clean created project
  QSignalSpy spy3( mApi, &MerginApi::serverProjectDeleted );
  mApi->deleteProject( projectNamespace, projectName );
  spy3.wait( SHORT_REPLY );

  projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ) );

  qDebug() << "TestMerginApi::testCreateProjectTwice PASSED";
}

void TestMerginApi::testDeleteNonExistingProject()
{
  qDebug() << "TestMerginApi::testDeleteNonExistingProject START";

  // Checks if projects doesn't exist
  QString projectName = TestMerginApi::TEST_PROJECT_NAME + "_DOESNT_EXISTS";
  QString projectNamespace = mUsername;
  ProjectList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ) );

  // Try to delete non-existing project
  QSignalSpy spy( mApi, &MerginApi::networkErrorOccurred );
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
  QString projectName = TestMerginApi::TEST_PROJECT_NAME + "_CREATE_DELETE";
  QString projectNamespace = mUsername;
  ProjectList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ) );

  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  Q_ASSERT( _findProjectByName( projectNamespace, projectName, projects ) );

  // Delete created project
  QSignalSpy spy2( mApi, &MerginApi::serverProjectDeleted );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );

  projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ) );

  qDebug() << "TestMerginApi::testCreateDeleteProject PASSED";
}

void TestMerginApi::testUploadProject()
{
  qDebug() << "TestMerginApi::testUploadProject START";

  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;
  std::shared_ptr<MerginProject> project = prepareTestProjectUpload();

  //
  // try to upload, but cancel it immediately afterwards
  // (this verifies we can cancel upload before a transaction is started)
  //

  QDateTime serverT0 = project->serverUpdated;
  QSignalSpy spy( mApi, &MerginApi::syncProjectFinished );
  mApi->uploadProject( projectNamespace, projectName );
  mApi->uploadCancel( MerginApi::getFullProjectName( projectNamespace, projectName ) );

  // no need to wait for the signal here - as we call abort() the reply's finished() signal is immediately emitted
  QCOMPARE( spy.count(), 1 );
  QList<QVariant> arguments = spy.takeFirst();
  QVERIFY( !arguments.at( 2 ).toBool() );

  ProjectList projects = getProjectList();
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ) );

  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT1 = project->serverUpdated;
  QCOMPARE( serverT0, serverT1 );

  //
  // try to upload, but cancel it after started to upload files
  // (so that we test also cancellation of transaction)
  //

  QSignalSpy spyX( mApi, &MerginApi::syncProjectFinished );
  QSignalSpy spyY( mApi, &MerginApi::pushFilesStarted );
  mApi->uploadProject( projectNamespace, projectName );
  spyY.wait( LONG_REPLY );
  QCOMPARE( spyY.count(), 1 );

  mApi->uploadCancel( MerginApi::getFullProjectName( projectNamespace, projectName ) );

  // no need to wait for the signal here - as we call abort() the reply's finished() signal is immediately emitted
  QCOMPARE( spyX.count(), 1 );
  QList<QVariant> argumentsX = spyX.takeFirst();
  QVERIFY( !argumentsX.at( 2 ).toBool() );

  projects = getProjectList();
  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  serverT1 = project->serverUpdated;
  QCOMPARE( serverT0, serverT1 );

  //
  // try to upload - and let the upload finish successfully
  //

  mApi->uploadProject( projectNamespace, projectName );
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );

  QVERIFY( spy2.wait( LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  projects = getProjectList();
  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT2 = project->serverUpdated;
  QVERIFY( serverT1 < serverT2 );

  qDebug() << "TestMerginApi::testUploadProject PASSED";
}


void TestMerginApi::testPushAddedFile()
{
  QString projectName = "testPushAddedFile";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  // add a single file
  QString newFilePath = mApi->projectsPath() + projectName + "/added.txt";
  QFile file( newFilePath );
  QVERIFY( file.open( QIODevice::WriteOnly ) );
  file.write( "added file content\n" );
  file.close();

  // TODO: check that the status is "modified"

  // upload
  uploadRemoteProject( mApi, mUsername, projectName );

  deleteLocalProject( mApi, mUsername, projectName );

  downloadRemoteProject( mApi, mUsername, projectName );

  // check it has the new file
  QFileInfo fi( newFilePath );
  QVERIFY( fi.exists() );
}

void TestMerginApi::testPushRemovedFile()
{
  qDebug() << "TestMerginApi::testPushRemovedFile START";

  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;
  std::shared_ptr<MerginProject> project = prepareTestProjectUpload();

  QDateTime serverT0 = project->serverUpdated;
  mApi->uploadProject( projectNamespace, projectName );
  QSignalSpy spy( mApi, &MerginApi::syncProjectFinished );
  QVERIFY( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = getProjectList();
  int projectNo0 = projects.size();
  int localProjectNo0 = mProjectModel->rowCount();
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ) );
  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT1 = project->serverUpdated;
  // Remove file
  QFile file( mApi->projectsPath() + projectName + "/test1.txt" );
  QVERIFY( file.exists() );
  file.remove();
  QVERIFY( !file.exists() );

  // upload changes
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );
  mApi->uploadProject( projectNamespace, projectName );
  QVERIFY( spy2.wait( LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );
  QList<QVariant> arguments = spy2.takeFirst();
  QVERIFY( arguments.at( 2 ).toBool() );

  projects = getProjectList();
  int projectNo1 = projects.size();
  int localProjectNo1 = mProjectModel->rowCount();
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ) );
  project = mApi->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QDateTime serverT2 = project->serverUpdated;
  QVERIFY( serverT1 < serverT2 );

  QCOMPARE( localProjectNo0, localProjectNo1 );
  QCOMPARE( projectNo0, projectNo1 );

  qDebug() << "TestMerginApi::testPushRemovedFile PASSED";
}

void TestMerginApi::testPushModifiedFile()
{
  QString projectName = "testPushModifiedFile";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  // modify a single file
  QString filename = mApi->projectsPath() + projectName + "/project.qgs";
  QFile file( filename );
  QVERIFY( file.open( QIODevice::WriteOnly ) );
  file.write( QByteArray( "v2" ) );
  file.close();

  // TODO: check that the status is "modified"

  // upload
  uploadRemoteProject( mApi, mUsername, projectName );

  // verify the remote project has updated file

  deleteLocalProject( mApi, mUsername, projectName );

  QVERIFY( !file.open( QIODevice::ReadOnly ) );  // it should not exist at this point

  downloadRemoteProject( mApi, mUsername, projectName );

  QVERIFY( file.open( QIODevice::ReadOnly ) );
  QCOMPARE( file.readAll(), QByteArray( "v2" ) );
  file.close();
}

void TestMerginApi::testUpdateAddedFile()
{
  // this test downloads a project, then a file gets added on the server
  // and we check whether the update was correct (i.e. the file got added too)

  QString projectName = "testUpdateAddedFile";
  QString projectDir = mApi->projectsPath() + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // download initial version
  downloadRemoteProject( mApi, mUsername, projectName );
  QVERIFY( !QFile::exists( projectDir + "/test-remote-new.txt" ) );

  // remove a file on the server
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( extraProjectDir + "/test-remote-new.txt", QByteArray( "my new content" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );
  QVERIFY( QFile::exists( extraProjectDir + "/test-remote-new.txt" ) );

  // now try to update
  downloadRemoteProject( mApi, mUsername, projectName );

  // check that the added file is there
  QVERIFY( QFile::exists( projectDir + "/project.qgs" ) );
  QVERIFY( QFile::exists( projectDir + "/test1.txt" ) );
  QVERIFY( QFile::exists( projectDir + "/test-remote-new.txt" ) );
  QCOMPARE( readFileContent( projectDir + "/test-remote-new.txt" ), QByteArray( "my new content" ) );
}

void TestMerginApi::testUpdateRemovedFiles()
{
  // this tests downloads a project, then a file gets removed on the server
  // and we check whether the update was correct (i.e. the file got removed too)

  QString projectName = "testUpdateRemovedFiles";
  QString projectDir = mApi->projectsPath() + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // download initial version
  downloadRemoteProject( mApi, mUsername, projectName );
  QVERIFY( QFile::exists( projectDir + "/test1.txt" ) );

  // remove a file on the server
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  QVERIFY( QFile::remove( extraProjectDir + "/test1.txt" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  // now try to update
  downloadRemoteProject( mApi, mUsername, projectName );

  // check that the removed file is not there anymore
  QVERIFY( QFile::exists( projectDir + "/project.qgs" ) );
  QVERIFY( !QFile::exists( projectDir + "/test1.txt" ) );
}

void TestMerginApi::testUpdateRemovedVsModifiedFiles()
{
  // this test downloads a project, then a files gets removed on the server,
  // but it also gets re-created locally with different content. It should be
  // correctly detected that the file is a local modification and it should be kept

  QString projectName = "testUpdateRemovedVsModifiedFiles";
  QString projectDir = mApi->projectsPath() + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // download initial version
  downloadRemoteProject( mApi, mUsername, projectName );
  QVERIFY( QFile::exists( projectDir + "/test1.txt" ) );

  // remove a file on the server
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  QVERIFY( QFile::remove( extraProjectDir + "/test1.txt" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  // modify the same file locally
  QFile file( projectDir + "/test1.txt" );
  QVERIFY( file.open( QIODevice::WriteOnly ) );
  file.write( QByteArray( "muhaha!" ) );
  file.close();

  // now try to update
  downloadRemoteProject( mApi, mUsername, projectName );

  // check that the file removed on the server is still there, with modified content
  QVERIFY( QFile::exists( projectDir + "/project.qgs" ) );
  QVERIFY( QFile::exists( projectDir + "/test1.txt" ) );
  QVERIFY( file.open( QIODevice::ReadOnly ) );
  QCOMPARE( file.readAll(), QByteArray( "muhaha!" ) );
  file.close();
}

void TestMerginApi::testConflictRemoteUpdateLocalUpdate()
{
  // this test downloads a project, makes a local update of a file,
  // in the meanwhile it does remote update of the same file to create
  // a conflict. Finally it tries to upload the local change to test
  // the code responsible for conflict resolution (renames the local file)

  QString projectName = "testConflictRemoteUpdateLocalUpdate";
  QString projectDir = mApi->projectsPath() + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + projectName;
  QString filename = projectDir + "/test1.txt";
  QString extraFilename = extraProjectDir + "/test1.txt";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  qDebug() << "download initial version";
  downloadRemoteProject( mApi, mUsername, projectName );

  qDebug() << "modify test1.txt on the server";
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( extraFilename, QByteArray( "remote content" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  qDebug() << "modify test1.txt locally and do the sync";
  writeFileContent( filename, QByteArray( "local content" ) );
  //
  // TODO: upload should figure out it needs to run update first without this
  // (the simple check in uploadProject() likely won't be good enough to find
  // out... in upload's project info handler if there is a need for update,
  // the upload should be cancelled (or paused to update first).
  //
  downloadRemoteProject( mApi, mUsername, projectName );
  uploadRemoteProject( mApi, mUsername, projectName );

  // verify the result: the server version should be in test1.txt
  // and the local version should go to test1.txt_conflict
  QCOMPARE( readFileContent( filename ), QByteArray( "remote content" ) );
  QCOMPARE( readFileContent( filename + "_conflict" ), QByteArray( "local content" ) );
}

void TestMerginApi::testConflictRemoteAddLocalAdd()
{
  // this test downloads a project, creates a new file
  // in the meanwhile it creates the same file on the server to create
  // a conflict. Finally it tries to upload the local change to test
  // the code responsible for conflict resolution (renames the local file)

  QString projectName = "testConflictRemoteAddLocalAdd";
  QString projectDir = mApi->projectsPath() + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + projectName;
  QString filename = projectDir + "/test-new-file.txt";
  QString extraFilename = extraProjectDir + "/test-new-file.txt";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  qDebug() << "download initial version";
  downloadRemoteProject( mApi, mUsername, projectName );

  qDebug() << "create test-new-file.txt on the server";
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( extraFilename, QByteArray( "new remote content" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  qDebug() << "create test-new-file.txt locally and do the sync";
  writeFileContent( filename, QByteArray( "new local content" ) );
  //
  // TODO: upload should figure out it needs to run update first without this
  // (the simple check in uploadProject() likely won't be good enough to find
  // out... in upload's project info handler if there is a need for update,
  // the upload should be cancelled (or paused to update first).
  //
  downloadRemoteProject( mApi, mUsername, projectName );
  uploadRemoteProject( mApi, mUsername, projectName );

  // verify the result: the server version should be in test1.txt
  // and the local version should go to test1.txt_conflict
  QCOMPARE( readFileContent( filename ), QByteArray( "new remote content" ) );
  QCOMPARE( readFileContent( filename + "_conflict" ), QByteArray( "new local content" ) );
}


void TestMerginApi::testParseAndCompareNoChanges()
{
  qDebug() << "TestMerginApi::parseAndCompareTestNoChanges START";

  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( mTestDataPath, QStringLiteral( "mergin.json" ) );
  QVERIFY( project );
  ProjectDiff diff = mApi->compareProjectFiles( project->files, project->files );
  QVERIFY( diff.added.isEmpty() );
  QVERIFY( diff.removed.isEmpty() );
  QVERIFY( diff.modified.isEmpty() );

  qDebug() << "TestMerginApi::parseAndCompareTestNoChanges PASSED";
}

void TestMerginApi::testParseAndCompareRemovedAdded()
{
  qDebug() << "TestMerginApi::testParseAndCompareRemovedAdded START";

  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( mTestDataPath, QStringLiteral( "mergin.json" ) );
  std::shared_ptr<MerginProject> project_added = mApi->readProjectMetadataFromPath( mTestDataPath, QStringLiteral( "mergin_added.json" ) );
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
}

void TestMerginApi::testParseAndCompareUpdated()
{
  qDebug() << "TestMerginApi::testParseAndCompareUpdated START";

  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( mTestDataPath, QStringLiteral( "mergin.json" ) );
  std::shared_ptr<MerginProject> project_updated = mApi->readProjectMetadataFromPath( mTestDataPath, QStringLiteral( "mergin_updated.json" ) );
  QVERIFY( project );
  QVERIFY( project_updated );

  ProjectDiff diff = mApi->compareProjectFiles( project_updated->files, project->files );
  QVERIFY( diff.added.isEmpty() );
  QVERIFY( diff.removed.isEmpty() );
  QCOMPARE( diff.modified.size(), 1 );

  qDebug() << "TestMerginApi::testParseAndCompareUpdated PASSED";
}

void TestMerginApi::testParseAndCompareRenamed()
{
  qDebug() << "TestMerginApi::testParseAndCompareRenamed START";

  std::shared_ptr<MerginProject> project = mApi->readProjectMetadataFromPath( mTestDataPath, QStringLiteral( "mergin.json" ) );
  std::shared_ptr<MerginProject> project_renamed = mApi->readProjectMetadataFromPath( mTestDataPath, QStringLiteral( "mergin_renamed.json" ) );
  QVERIFY( project );
  QVERIFY( project_renamed );

  ProjectDiff diff = mApi->compareProjectFiles( project_renamed->files, project->files );
  QCOMPARE( diff.added.size(), 1 );
  QCOMPARE( diff.removed.size(), 1 );
  QVERIFY( diff.modified.isEmpty() );

  qDebug() << "TestMerginApi::testParseAndCompareRenamed PASSED";
}


//////// HELPER FUNCTIONS ////////

ProjectList TestMerginApi::getProjectList()
{
  QSignalSpy spy( mApi,  &MerginApi::listProjectsFinished );
  mApi->listProjects( QString(), mUsername, QString(), QString() );
  spy.wait( SHORT_REPLY );

  return mApi->projects();
}

void TestMerginApi::initTestProject()
{
  QString projectName = TestMerginApi::TEST_PROJECT_NAME;
  QString projectNamespace = mUsername;

  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );

  ProjectList projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ) );
  qDebug() << "TestMerginApi::initTestProject DONE";

  // copy project's test data to the new project directory
  QString source = mTestDataPath + "/" + projectName;
  QString destination = mApi->projectsPath() + projectName;
  QVERIFY( InputUtils::cpDir( source, destination ) );
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
    project->projectDir = mApi->projectsPath() + projectName;
  }
  return project;
}

void TestMerginApi::deleteRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  QSignalSpy spy( api, &MerginApi::serverProjectDeleted );
  api->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );
}

void TestMerginApi::deleteLocalProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  std::shared_ptr<MerginProject> project = api->getProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project );
  QVERIFY( project->projectDir.startsWith( api->projectsPath() ) );  // just to make sure we don't delete something wrong (-:
  QDir projectDir( project->projectDir );
  projectDir.removeRecursively();
  api->clearProject( project );
}

void TestMerginApi::downloadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  QSignalSpy spy( api, &MerginApi::syncProjectFinished );
  api->updateProject( projectNamespace, projectName );
  QCOMPARE( api->transactions().count(), 1 );
  QVERIFY( spy.wait( LONG_REPLY * 5 ) );
}

void TestMerginApi::uploadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  api->uploadProject( projectNamespace, projectName );
  QSignalSpy spy( api, &MerginApi::syncProjectFinished );
  QVERIFY( spy.wait( LONG_REPLY ) );
  QCOMPARE( spy.count(), 1 );
}

void TestMerginApi::writeFileContent( const QString &filename, const QByteArray &data )
{
  QFile f( filename );
  Q_ASSERT( f.open( QIODevice::WriteOnly ) );
  f.write( data );
  f.close();
}

QByteArray TestMerginApi::readFileContent( const QString &filename )
{
  QFile f( filename );
  Q_ASSERT( f.open( QIODevice::ReadOnly ) );
  QByteArray data = f.readAll();
  f.close();
  return data;
}
