#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QSignalSpy>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"
#include "inpututils.h"
#include "geodiffutils.h"

const QString TestMerginApi::TEST_PROJECT_NAME = "TEMPORARY_TEST_PROJECT";


static MerginProjectListEntry _findProjectByName( const QString &projectNamespace, const QString &projectName, const MerginProjectList &projects )
{
  for ( MerginProjectListEntry project : projects )
  {
    if ( project.projectName == projectName && project.projectNamespace == projectNamespace )
      return project;
  }
  return MerginProjectListEntry();
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
  mLocalProjectsExtra = new LocalProjectsManager( projectsExtraDir );
  mApiExtra = new MerginApi( *mLocalProjectsExtra );
  mApiExtra->setApiRoot( mApi->apiRoot() );
  QSignalSpy spyExtra( mApiExtra, &MerginApi::authChanged );
  mApiExtra->authorize( mApi->mUsername, mApi->mPassword );
  Q_ASSERT( spyExtra.wait( LONG_REPLY ) );
  QCOMPARE( spyExtra.count(), 1 );

  // remove any projects on the server that may prevent us from creating them
  deleteRemoteProject( mApiExtra, mUsername, "testListProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testDownloadProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushAddedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushRemovedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushModifiedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateAddedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateRemovedFiles" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateRemovedVsModifiedFiles" );
  deleteRemoteProject( mApiExtra, mUsername, "testConflictRemoteUpdateLocalUpdate" );
  deleteRemoteProject( mApiExtra, mUsername, "testConflictRemoteAddLocalAdd" );
  deleteRemoteProject( mApiExtra, mUsername, "testUploadProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testCancelDownloadProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testCreateDeleteProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testMultiChunkUploadDownload" );
  deleteRemoteProject( mApiExtra, mUsername, "testUploadWithUpdate" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpload" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpdateBasic" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpdateWithRebase" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpdateWithRebaseFailed" );
}

void TestMerginApi::cleanupTestCase()
{
  delete mApiExtra;
  delete mLocalProjectsExtra;

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
  QVERIFY( !_findProjectByName( mUsername, projectName, mApi->projects() ).isValid() );
  QVERIFY( !mApi->localProjectsManager().hasMerginProject( mUsername, projectName ) );

  // create the project on the server (the content is not important)
  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // check the project exists on the server
  QSignalSpy spy( mApi, &MerginApi::listProjectsFinished );
  mApi->listProjects( QString() );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QVERIFY( _findProjectByName( mUsername, projectName, mApi->projects() ).isValid() );

  // project is not available locally, so it has no entry
  QVERIFY( !mApi->localProjectsManager().hasMerginProject( mUsername, projectName ) );
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

  // check that the local projects are updated
  QVERIFY( mApi->localProjectsManager().hasMerginProject( mUsername, projectName ) );
  LocalProjectInfo project = mApi->localProjectsManager().projectFromMerginName( projectNamespace, projectName );
  QVERIFY( project.isValid() );
  QCOMPARE( project.projectDir, mApi->projectsPath() + "/" + projectName );
  QCOMPARE( project.serverVersion, 1 );
  QCOMPARE( project.localVersion, 1 );
  QCOMPARE( project.status, UpToDate );

  bool downloadSuccessful = mProjectModel->containsProject( projectNamespace, projectName );
  QVERIFY( downloadSuccessful );

  // there should be something in the directory
  QStringList projectDirEntries = QDir( project.projectDir ).entryList( QDir::AllEntries | QDir::NoDotAndDotDot );
  QCOMPARE( projectDirEntries.count(), 2 );
}

void TestMerginApi::createRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, const QString &sourcePath )
{
  // create a project
  QSignalSpy spy( api, &MerginApi::projectCreated );
  api->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );

  // Copy data
  QString projectDir = api->projectsPath() + "/" + projectName + "/";
  InputUtils::cpDir( sourcePath, projectDir );

  // make MerginApi aware of the project and its directory
  api->localProjectsManager().addMerginProject( projectDir, projectNamespace, projectName );

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

  api->localProjectsManager().removeProject( projectDir );

  QCOMPARE( QFileInfo( projectDir ).size(), 0 );
  QVERIFY( QDir( projectDir ).isEmpty() );
}


void TestMerginApi::testCancelDownloadProject()
{
  QString projectName = "testCancelDownloadProject";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TestMerginApi::TEST_PROJECT_NAME + "/" );

  QCOMPARE( mApi->transactions().count(), 0 );

  QString projectDir = mApi->projectsPath() + "/" + projectName + "/";

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
}

void TestMerginApi::testCreateProjectTwice()
{
  QString projectName = "testCreateProjectTwice";
  QString projectNamespace = mUsername;

  MerginProjectList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );

  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ).isValid() );

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
  QCOMPARE( spy3.takeFirst().at( 1 ).toBool(), true );

  projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );
}

void TestMerginApi::testDeleteNonExistingProject()
{
  // Checks if projects doesn't exist
  QString projectName = "testDeleteNonExistingProject";
  QString projectNamespace = mUsername;
  MerginProjectList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );

  // Try to delete non-existing project
  QSignalSpy spy( mApi, &MerginApi::networkErrorOccurred );
  mApi->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );

  QList<QVariant> arguments = spy.takeFirst();
  QVERIFY( arguments.at( 0 ).type() == QVariant::String );
  QVERIFY( arguments.at( 1 ).type() == QVariant::String );
  QCOMPARE( arguments.at( 1 ).toString(), QStringLiteral( "Mergin API error: deleteProject" ) );
}

void TestMerginApi::testCreateDeleteProject()
{
  // Create a project
  QString projectName = "testCreateDeleteProject";
  QString projectNamespace = mUsername;
  MerginProjectList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );

  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName );
  QVERIFY( spy.wait( SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );

  projects = getProjectList();
  QVERIFY( !mMerginProjectModel->projects().isEmpty() );
  Q_ASSERT( _findProjectByName( projectNamespace, projectName, projects ).isValid() );

  // Delete created project
  QSignalSpy spy2( mApi, &MerginApi::serverProjectDeleted );
  mApi->deleteProject( projectNamespace, projectName );
  spy2.wait( SHORT_REPLY );
  QCOMPARE( spy2.takeFirst().at( 1 ).toBool(), true );

  projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );
}

void TestMerginApi::testUploadProject()
{
  QString projectName = "testUploadProject";
  QString projectNamespace = mUsername;
  QString projectDir = mApi->projectsPath() + "/" + projectName;

  QSignalSpy spy0( mApiExtra, &MerginApi::projectCreated );
  mApiExtra->createProject( projectNamespace, projectName );
  QVERIFY( spy0.wait( LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );
  QCOMPARE( spy0.takeFirst().at( 1 ).toBool(), true );

  MerginProjectList projects = getProjectList();
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ).isValid() );

  // copy project's test data to the new project directory
  QVERIFY( InputUtils::cpDir( mTestDataPath + "/" + TEST_PROJECT_NAME, projectDir ) );
  mApi->localProjectsManager().addMerginProject( projectDir, projectNamespace, projectName );

  // project info does not have any version information yet
  LocalProjectInfo project0 = mApi->getLocalProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QCOMPARE( project0.serverVersion, -1 );
  QCOMPARE( project0.localVersion, -1 );

  //
  // try to upload, but cancel it immediately afterwards
  // (this verifies we can cancel upload before a transaction is started)
  //

  QSignalSpy spy( mApi, &MerginApi::syncProjectFinished );
  mApi->uploadProject( projectNamespace, projectName );
  mApi->uploadCancel( MerginApi::getFullProjectName( projectNamespace, projectName ) );

  // no need to wait for the signal here - as we call abort() the reply's finished() signal is immediately emitted
  QCOMPARE( spy.count(), 1 );
  QList<QVariant> arguments = spy.takeFirst();
  QVERIFY( !arguments.at( 2 ).toBool() );

  // server version is still not available (cancelled before project info)
  LocalProjectInfo project1 = mApi->getLocalProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QCOMPARE( project1.serverVersion, -1 );
  QCOMPARE( project1.localVersion, -1 );

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

  // server version is now available (cancelled after project info)
  LocalProjectInfo project2 = mApi->getLocalProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QCOMPARE( project2.serverVersion, 0 );
  QCOMPARE( project2.localVersion, -1 );

  //
  // try to upload - and let the upload finish successfully
  //

  mApi->uploadProject( projectNamespace, projectName );
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );

  QVERIFY( spy2.wait( LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  LocalProjectInfo project3 = mApi->getLocalProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QCOMPARE( project3.serverVersion, 1 );
  QCOMPARE( project3.localVersion, 1 );
  QCOMPARE( project3.status, UpToDate );
}

void TestMerginApi::testMultiChunkUploadDownload()
{
  // this will try to upload a file that needs to be split into multiple chunks
  // and then also download it correctly again in a clean new download

  QString projectName = "testMultiChunkUploadDownload";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  // create a big file (21mb)
  QString bigFilePath = mApi->projectsPath() + "/" + projectName + "/" + "big_file.dat";
  QFile bigFile( bigFilePath );
  QVERIFY( bigFile.open( QIODevice::WriteOnly ) );
  for ( int i = 0; i < 21; ++i )   // 21 times 1mb -> should be three chunks when chunk size == 10mb
    bigFile.write( QByteArray( 1024 * 1024, static_cast<char>( 'A' + i ) ) );   // AAAA.....BBBB.....CCCC.....
  bigFile.close();

  QByteArray checksum = MerginApi::getChecksum( bigFilePath );
  QVERIFY( !checksum.isEmpty() );

  // upload
  uploadRemoteProject( mApi, mUsername, projectName );

  // download again
  deleteLocalProject( mApi, mUsername, projectName );
  QVERIFY( !QFileInfo::exists( bigFilePath ) );
  downloadRemoteProject( mApi, mUsername, projectName );

  // verify it's there and with correct content
  QByteArray checksum2 = MerginApi::getChecksum( bigFilePath );
  QVERIFY( QFileInfo::exists( bigFilePath ) );
  QCOMPARE( checksum, checksum2 );
}


void TestMerginApi::testPushAddedFile()
{
  QString projectName = "testPushAddedFile";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project0 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project0.serverVersion, 1 );
  QCOMPARE( project0.localVersion, 1 );
  QCOMPARE( project0.status, UpToDate );

  // add a single file
  QString newFilePath = mApi->projectsPath() + "/" + projectName + "/added.txt";
  QFile file( newFilePath );
  QVERIFY( file.open( QIODevice::WriteOnly ) );
  file.write( "added file content\n" );
  file.close();

  // check that the status is "modified"
  mApi->localProjectsManager().updateProjectStatus( mApi->projectsPath() + "/" + projectName );  // force update of status
  LocalProjectInfo project1 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project1.serverVersion, 1 );
  QCOMPARE( project1.localVersion, 1 );
  QCOMPARE( project1.status, Modified );

  // upload
  uploadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project2 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project2.serverVersion, 2 );
  QCOMPARE( project2.localVersion, 2 );
  QCOMPARE( project2.status, UpToDate );

  deleteLocalProject( mApi, mUsername, projectName );

  downloadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project3 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project3.serverVersion, 2 );
  QCOMPARE( project3.localVersion, 2 );
  QCOMPARE( project3.status, UpToDate );

  // check it has the new file
  QFileInfo fi( newFilePath );
  QVERIFY( fi.exists() );
}

void TestMerginApi::testPushRemovedFile()
{
  // download a project, then remove a file locally and upload the project.
  // we then check that the file is really removed on the subsequent download.

  QString projectName = "testPushRemovedFile";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project0 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project0.serverVersion, 1 );
  QCOMPARE( project0.localVersion, 1 );
  QCOMPARE( project0.status, UpToDate );

  // Remove file
  QString removedFilePath = mApi->projectsPath() + "/" + projectName + "/test1.txt";
  QFile file( removedFilePath );
  QVERIFY( file.exists() );
  file.remove();
  QVERIFY( !file.exists() );

  // check that it is considered as modified now
  mApi->localProjectsManager().updateProjectStatus( mApi->projectsPath() + "/" + projectName );  // force update of status
  LocalProjectInfo project1 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project1.serverVersion, 1 );
  QCOMPARE( project1.localVersion, 1 );
  QCOMPARE( project1.status, Modified );

  // upload changes

  uploadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project2 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project2.serverVersion, 2 );
  QCOMPARE( project2.localVersion, 2 );
  QCOMPARE( project2.status, UpToDate );

  deleteLocalProject( mApi, mUsername, projectName );

  downloadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project3 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project3.serverVersion, 2 );
  QCOMPARE( project3.localVersion, 2 );
  QCOMPARE( project3.status, UpToDate );

  // check it has the new file
  QFileInfo fi( removedFilePath );
  QVERIFY( !fi.exists() );

  QStringList projectDirEntries = QDir( mApi->projectsPath() + "/" + projectName ).entryList( QDir::AllEntries | QDir::NoDotAndDotDot );
  QCOMPARE( projectDirEntries.count(), 1 );  // just the .qgs file left
}

void TestMerginApi::testPushModifiedFile()
{
  QString projectName = "testPushModifiedFile";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  // need to sleep at least for a second so that last modified time
  // has later timestamp than the last sync (seems there's one second resolution)
  QTest::qSleep( 1000 );

  // modify a single file
  QString filename = mApi->projectsPath() + "/" + projectName + "/project.qgs";
  QFile file( filename );
  QVERIFY( file.open( QIODevice::WriteOnly ) );
  file.write( QByteArray( "v2" ) );
  file.close();

  // check that the status is "modified"
  mApi->localProjectsManager().updateProjectStatus( mApi->projectsPath() + "/" + projectName );  // force update of status
  LocalProjectInfo project1 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project1.serverVersion, 1 );
  QCOMPARE( project1.localVersion, 1 );
  QCOMPARE( project1.status, Modified );

  // upload
  uploadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project2 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project2.serverVersion, 2 );
  QCOMPARE( project2.localVersion, 2 );
  QCOMPARE( project2.status, UpToDate );

  // verify the remote project has updated file

  deleteLocalProject( mApi, mUsername, projectName );

  QVERIFY( !file.open( QIODevice::ReadOnly ) );  // it should not exist at this point

  downloadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project3 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project3.serverVersion, 2 );
  QCOMPARE( project3.localVersion, 2 );
  QCOMPARE( project3.status, UpToDate );

  QVERIFY( file.open( QIODevice::ReadOnly ) );
  QCOMPARE( file.readAll(), QByteArray( "v2" ) );
  file.close();
}

void TestMerginApi::testUpdateAddedFile()
{
  // this test downloads a project, then a file gets added on the server
  // and we check whether the update was correct (i.e. the file got added too)

  QString projectName = "testUpdateAddedFile";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // download initial version
  downloadRemoteProject( mApi, mUsername, projectName );
  QVERIFY( !QFile::exists( projectDir + "/test-remote-new.txt" ) );

  LocalProjectInfo project0 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project0.serverVersion, 1 );
  QCOMPARE( project0.localVersion, 1 );
  QCOMPARE( project0.status, UpToDate );

  // remove a file on the server
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( extraProjectDir + "/test-remote-new.txt", QByteArray( "my new content" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );
  QVERIFY( QFile::exists( extraProjectDir + "/test-remote-new.txt" ) );

  // list projects - just so that we can figure out we are behind
  getProjectList();

  LocalProjectInfo project1 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project1.serverVersion, 2 );
  QCOMPARE( project1.localVersion, 1 );
  QCOMPARE( project1.status, OutOfDate );

  // now try to update
  downloadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project2 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project2.serverVersion, 2 );
  QCOMPARE( project2.localVersion, 2 );
  QCOMPARE( project2.status, UpToDate );

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
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;

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
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;

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
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;
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
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;
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

void TestMerginApi::testUploadWithUpdate()
{
  // this test triggers the situation when the request to upload a project
  // first needs to do an update and only afterwards it uploads changes

  QString projectName = "testUploadWithUpdate";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;
  QString filenameLocal = projectDir + "/test-new-local-file.txt";
  QString filenameRemote = projectDir + "/test-new-remote-file.txt";
  QString extraFilenameRemote = extraProjectDir + "/test-new-remote-file.txt";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( extraFilenameRemote, QByteArray( "new remote content" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  writeFileContent( filenameLocal, QByteArray( "new local content" ) );

  qDebug() << "now do both update + upload";
  uploadRemoteProject( mApi, mUsername, projectName );

  QCOMPARE( readFileContent( filenameLocal ), QByteArray( "new local content" ) );
  QCOMPARE( readFileContent( filenameRemote ), QByteArray( "new remote content" ) );

  // try to re-download the project and see if everything went fine
  deleteLocalProject( mApi, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );

  LocalProjectInfo project3 = mApi->getLocalProject( MerginApi::getFullProjectName( mUsername, projectName ) );
  QCOMPARE( project3.serverVersion, 3 );
  QCOMPARE( project3.localVersion, 3 );
  QCOMPARE( project3.status, UpToDate );

  QCOMPARE( readFileContent( filenameLocal ), QByteArray( "new local content" ) );
  QCOMPARE( readFileContent( filenameRemote ), QByteArray( "new remote content" ) );
}

void TestMerginApi::testDiffUpload()
{
  QString projectName = "testDiffUpload";
  QString projectDir = mApi->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + "diff_project" + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  QVERIFY( QFileInfo::exists( projectDir + "/.mergin/base.gpkg" ) );

  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected

  // replace gpkg with a new version with a modified geometry
  QFile::remove( projectDir + "/base.gpkg" );
  QFile::copy( mTestDataPath + "/modified_1_geom.gpkg", projectDir + "/base.gpkg" );

  ProjectDiff diff = MerginApi::localProjectChanges( projectDir );
  ProjectDiff expectedDiff;
  expectedDiff.localUpdated = QSet<QString>() << "base.gpkg";
  QCOMPARE( diff, expectedDiff );

  GeodiffUtils::ChangesetSummary expectedSummary;
  expectedSummary["simple"] = GeodiffUtils::TableSummary( 0, 1, 0 );
  expectedSummary["gpkg_contents"] = GeodiffUtils::TableSummary( 0, 1, 0 );
  QString changes = GeodiffUtils::diffableFilePendingChanges( projectDir, "base.gpkg", true );
  GeodiffUtils::ChangesetSummary summary = GeodiffUtils::parseChangesetSummary( changes );
  QCOMPARE( summary, expectedSummary );

  uploadRemoteProject( mApi, mUsername, projectName );

  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected
}

void TestMerginApi::testDiffUpdateBasic()
{
  // test case where there is no local change in a gpkg, it is only modified on the server
  // and the local file should get the new stuff from server

  QString projectName = "testDiffUpdateBasic";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + "diff_project" + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  QVERIFY( QFileInfo::exists( projectDir + "/.mergin/base.gpkg" ) );
  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected

  QgsVectorLayer *vl0 = new QgsVectorLayer( projectDir + "/base.gpkg|layername=simple", "base", "ogr" );
  QVERIFY( vl0->isValid() );
  QCOMPARE( vl0->featureCount(), 3 );
  delete vl0;

  //
  // download with mApiExtra + modify + upload
  //

  downloadRemoteProject( mApiExtra, mUsername, projectName );
  bool r0 = QFile::remove( projectDirExtra + "/base.gpkg" );
  bool r1 = QFile::copy( mTestDataPath + "/added_row.gpkg", projectDirExtra + "/base.gpkg" );
  QVERIFY( r0 && r1 );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  // update our local version now
  downloadRemoteProject( mApi, mUsername, projectName );

  //
  // check the result
  //

  QgsVectorLayer *vl = new QgsVectorLayer( projectDir + "/base.gpkg|layername=simple", "base", "ogr" );
  QVERIFY( vl->isValid() );
  QCOMPARE( vl->featureCount(), 4 );
  delete vl;

  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected

  QVERIFY( !GeodiffUtils::hasPendingChanges( projectDir, "base.gpkg" ) );
}

void TestMerginApi::testDiffUpdateWithRebase()
{
  // a test case where there is a local change in a gpkg that is also modified on the server
  // and the local change will get rebased on top of the server's change

  QString projectName = "testDiffUpdateWithRebase";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + "diff_project" + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  QVERIFY( QFileInfo::exists( projectDir + "/.mergin/base.gpkg" ) );
  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected

  //
  // download with mApiExtra + modify + upload
  //

  downloadRemoteProject( mApiExtra, mUsername, projectName );
  bool r0 = QFile::remove( projectDirExtra + "/base.gpkg" );
  bool r1 = QFile::copy( mTestDataPath + "/added_row.gpkg", projectDirExtra + "/base.gpkg" );
  QVERIFY( r0 && r1 );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  //
  // do a local update of the file
  //

  QgsVectorLayer *vl0 = new QgsVectorLayer( projectDir + "/base.gpkg|layername=simple", "base", "ogr" );
  QVERIFY( vl0->isValid() );
  QCOMPARE( vl0->featureCount(), 3 );
  vl0->startEditing();
  QgsFeature f( vl0->fields() );
  f.setAttribute( "name", "def" );
  f.setAttribute( "rating", 456 );
  f.setGeometry( QgsGeometry::fromPointXY( QgsPointXY( 4, 5 ) ) );
  vl0->addFeature( f );
  vl0->commitChanges();
  QCOMPARE( vl0->featureCount(), 4 );
  delete vl0;

  // check that the file is marked as changed
  ProjectDiff diff = MerginApi::localProjectChanges( projectDir );
  ProjectDiff expectedDiff;
  expectedDiff.localUpdated = QSet<QString>() << "base.gpkg";
  QCOMPARE( diff, expectedDiff );

  // check that geodiff knows there was one added feature
  GeodiffUtils::ChangesetSummary expectedSummary;
  expectedSummary["simple"] = GeodiffUtils::TableSummary( 1, 0, 0 );
  expectedSummary["gpkg_contents"] = GeodiffUtils::TableSummary( 0, 1, 0 );
  QString changes = GeodiffUtils::diffableFilePendingChanges( projectDir, "base.gpkg", true );
  GeodiffUtils::ChangesetSummary summary = GeodiffUtils::parseChangesetSummary( changes );
  QCOMPARE( summary, expectedSummary );

  // update our local version now
  downloadRemoteProject( mApi, mUsername, projectName );

  //
  // check the result
  //

  QgsVectorLayer *vl = new QgsVectorLayer( projectDir + "/base.gpkg|layername=simple", "base", "ogr" );
  QVERIFY( vl->isValid() );
  QCOMPARE( vl->featureCount(), 5 );
  delete vl;

  // like before the update - there should be locally modified base.gpkg with the changes we did
  QCOMPARE( MerginApi::localProjectChanges( projectDir ), expectedDiff );
  QCOMPARE( GeodiffUtils::parseChangesetSummary( changes ), expectedSummary );
}

void TestMerginApi::testDiffUpdateWithRebaseFailed()
{
  // a test case where the local change is something that geodiff does not support
  // and thus cannot rebase the changes (should create a conflict file instead)

  // a test case where there is a local change in a gpkg that is also modified on the server
  // and the local change will get rebased on top of the server's change

  QString projectName = "testDiffUpdateWithRebaseFailed";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + "diff_project" + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  QVERIFY( QFileInfo::exists( projectDir + "/.mergin/base.gpkg" ) );
  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected

  //
  // download with mApiExtra + modify + upload
  //

  downloadRemoteProject( mApiExtra, mUsername, projectName );
  bool r0 = QFile::remove( projectDirExtra + "/base.gpkg" );
  bool r1 = QFile::copy( mTestDataPath + "/added_row.gpkg", projectDirExtra + "/base.gpkg" );
  QVERIFY( r0 && r1 );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  //
  // do a local update of the file
  //

  bool r2 = QFile::remove( projectDir+ "/base.gpkg" );
  bool r3 = QFile::copy( mTestDataPath + "/added_column.gpkg", projectDir + "/base.gpkg" );
  QVERIFY( r2 && r3 );

  // check that the file is marked as changed
  ProjectDiff diff = MerginApi::localProjectChanges( projectDir );
  ProjectDiff expectedDiff;
  expectedDiff.localUpdated = QSet<QString>() << "base.gpkg";
  qDebug() << diff.dump();
  QCOMPARE( diff, expectedDiff );

  // check that geodiff knows there was one added feature
  QString changes = GeodiffUtils::diffableFilePendingChanges( projectDir, "base.gpkg", true );
  QCOMPARE( changes, "ERROR" );  // local diff should fail

  // update our local version now
  downloadRemoteProject( mApi, mUsername, projectName );

  //
  // check the result
  //

  QVERIFY( QFile::exists( projectDir + "/base.gpkg" ) );
  QVERIFY( QFile::exists( projectDir + "/base.gpkg_conflict" ) );

  // there should be a new "conflict" file
  ProjectDiff expectedDiffFinal;
  expectedDiffFinal.localAdded = QSet<QString>() << "base.gpkg_conflict";
  QCOMPARE( MerginApi::localProjectChanges( projectDir ), expectedDiffFinal );
}

//////// HELPER FUNCTIONS ////////

MerginProjectList TestMerginApi::getProjectList()
{
  QSignalSpy spy( mApi,  &MerginApi::listProjectsFinished );
  mApi->listProjects( QString(), mUsername, QString() );
  spy.wait( SHORT_REPLY );

  return mApi->projects();
}


void TestMerginApi::deleteRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  QSignalSpy spy( api, &MerginApi::serverProjectDeleted );
  api->deleteProject( projectNamespace, projectName );
  spy.wait( SHORT_REPLY );
}

void TestMerginApi::deleteLocalProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  LocalProjectInfo project = api->getLocalProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project.isValid() );
  QVERIFY( project.projectDir.startsWith( api->projectsPath() ) );  // just to make sure we don't delete something wrong (-:
  QDir projectDir( project.projectDir );
  projectDir.removeRecursively();

  api->localProjectsManager().removeProject( project.projectDir );
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
