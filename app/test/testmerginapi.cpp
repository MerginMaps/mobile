#include <QtTest/QtTest>
#include <QtCore/QObject>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include "testmerginapi.h"
#include "inpututils.h"
#include "coreutils.h"
#include "geodiffutils.h"
#include "testutils.h"
#include "merginuserauth.h"
#include "merginuserinfo.h"

const QString TestMerginApi::TEST_PROJECT_NAME = "TEMPORARY_TEST_PROJECT";
const QString TestMerginApi::TEST_EMPTY_FILE_NAME = "test_empty_file.md";


static MerginProject _findProjectByName( const QString &projectNamespace, const QString &projectName, const MerginProjectsList &projects )
{
  for ( MerginProject project : projects )
  {
    if ( project.projectName == projectName && project.projectNamespace == projectNamespace )
      return project;
  }
  return MerginProject();
}


TestMerginApi::TestMerginApi( MerginApi *api )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API

  mLocalProjectsModel = std::unique_ptr<ProjectsModel>( new ProjectsModel );
  mLocalProjectsModel->setModelType( ProjectsModel::LocalProjectsModel );
  mLocalProjectsModel->setMerginApi( mApi );
  mLocalProjectsModel->setLocalProjectsManager( &mApi->localProjectsManager() );

  mCreatedProjectsModel = std::unique_ptr<ProjectsModel>( new ProjectsModel );
  mCreatedProjectsModel->setModelType( ProjectsModel::CreatedProjectsModel );
  mCreatedProjectsModel->setMerginApi( mApi );
  mCreatedProjectsModel->setLocalProjectsManager( &mApi->localProjectsManager() );
}

void TestMerginApi::initTestCase()
{
  QString apiRoot, username, password;
  TestUtils::mergin_auth( mApi, apiRoot, username, password );

  QSignalSpy spy( mApi, &MerginApi::authChanged );
  mApi->authorize( username, password );
  QVERIFY( spy.wait( TestUtils::LONG_REPLY ) );
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
  mApiExtra->authorize( username, password );
  QVERIFY( spyExtra.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spyExtra.count(), 1 );

  // remove any projects on the server that may prevent us from creating them
  deleteRemoteProject( mApiExtra, mUsername, "testListProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testDownloadProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testDownloadProjectSpecChars" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushAddedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushRemovedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushModifiedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testPushNoChanges" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateAddedFile" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateRemovedFiles" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateRemovedVsModifiedFiles" );
  deleteRemoteProject( mApiExtra, mUsername, "testConflictRemoteUpdateLocalUpdate" );
  deleteRemoteProject( mApiExtra, mUsername, "testConflictRemoteAddLocalAdd" );
  deleteRemoteProject( mApiExtra, mUsername, "testEditConflictScenario" );
  deleteRemoteProject( mApiExtra, mUsername, "testUploadProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testCancelDownloadProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testCreateProjectTwice" );
  deleteRemoteProject( mApiExtra, mUsername, "testCreateDeleteProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testMultiChunkUploadDownload" );
  deleteRemoteProject( mApiExtra, mUsername, "testEmptyFileUploadDownload" );
  deleteRemoteProject( mApiExtra, mUsername, "testUploadWithUpdate" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpload" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffSubdirsUpload" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpdateBasic" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpdateWithRebase" );
  deleteRemoteProject( mApiExtra, mUsername, "testDiffUpdateWithRebaseFailed" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateWithDiffs" );
  deleteRemoteProject( mApiExtra, mUsername, "testUpdateWithMissedVersion" );
  deleteRemoteProject( mApiExtra, mUsername, "testMigrateProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testMigrateProjectAndSync" );
  deleteRemoteProject( mApiExtra, mUsername, "testMigrateDetachProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testSelectiveSync" );
  deleteRemoteProject( mApiExtra, mUsername, "testSelectiveSyncSubfolder" );
  deleteRemoteProject( mApiExtra, mUsername, "testSelectiveSyncAddConfigToExistingProject" );
  deleteRemoteProject( mApiExtra, mUsername, "testSelectiveSyncRemoveConfig" );
  deleteRemoteProject( mApiExtra, mUsername, "testSelectiveSyncChangeSyncFolder" );
  deleteRemoteProject( mApiExtra, mUsername, "testSelectiveSyncDisabledInConfig" );
  deleteRemoteProject( mApiExtra, mUsername, "testSelectiveSyncCorruptedFormat" );

  deleteLocalDir( mApi, "testExcludeFromSync" );
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

  // check that there's no testListProject
  MerginProjectsList projects = getProjectList();

  QVERIFY( !_findProjectByName( mUsername, projectName, projects ).isValid() );
  QVERIFY( !mApi->localProjectsManager().projectFromMerginName( mUsername, projectName ).isValid() );

  // create the project on the server (the content is not important)
  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // check the project exists on the server
  projects = getProjectList();

  QVERIFY( _findProjectByName( mUsername, projectName, projects ).isValid() );

  // project is not available locally, so it has no entry
  QVERIFY( !mApi->localProjectsManager().projectFromMerginName( mUsername, projectName ).isValid() );
}

void TestMerginApi::testListProjectsByName()
{
  QString projectName = "testListProjectByName";

  // create the project on the server with other client
  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  // let's invalidate main client's auth token and see if the listProjectsByName gets new one
  // set token's expiration to 3 secs ago
  QDateTime now = QDateTime::currentDateTimeUtc().addSecs( -3 );
  mApi->userAuth()->setTokenExpiration( now );

  QByteArray oldToken = mApi->userAuth()->authToken();

  QStringList projects;
  projects.append( MerginApi::getFullProjectName( mUsername, projectName ) );

  QSignalSpy responseReceived( mApi, &MerginApi::listProjectsByNameFinished );
  mApi->listProjectsByName( projects );
  QVERIFY( responseReceived.wait( TestUtils::SHORT_REPLY ) );

  QVERIFY( oldToken != mApi->userAuth()->authToken() );

  MerginProjectsList receivedProjects = projectListFromSpy( responseReceived );
  QVERIFY( receivedProjects.count() == 1 );

  MerginProject ourProject = receivedProjects.at( 0 );
  QVERIFY( ourProject.remoteError.isEmpty() );
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
  QVERIFY( spy.wait( TestUtils::LONG_REPLY * 5 ) );
  QCOMPARE( spy.count(), 1 );

  QCOMPARE( mApi->transactions().count(), 0 );

  // check that the local projects are updated
  QVERIFY( mApi->localProjectsManager().projectFromMerginName( mUsername, projectName ).isValid() );

  // update model to have latest info
  refreshProjectsModel( ProjectsModel::LocalProjectsModel );

  std::shared_ptr<Project> project = mLocalProjectsModel->projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project && project->isLocal() && project->isMergin() );
  QCOMPARE( project->local->projectDir, mApi->projectsPath() + "/" + projectName );
  QCOMPARE( project->local->localVersion, 1 );
  QCOMPARE( project->mergin->serverVersion, 1 );
  QCOMPARE( project->mergin->status, ProjectStatus::UpToDate );

  bool downloadSuccessful = mApi->localProjectsManager().projectFromMerginName( projectNamespace, projectName ).isValid();
  QVERIFY( downloadSuccessful );

  // there should be something in the directory
  QStringList projectDirEntries = QDir( project->local->projectDir ).entryList( QDir::AllEntries | QDir::NoDotAndDotDot );
  QCOMPARE( projectDirEntries.count(), 2 );

  // verify that download in progress file is erased
  QVERIFY( !QFile::exists( CoreUtils::downloadInProgressFilePath( mTestDataPath + "/" + TEST_PROJECT_NAME ) ) );
}

void TestMerginApi::testDownloadProjectSpecChars()
{
  // Create and upload project with project file containing special chars in its name.
  // Especially testing a name containing "+" sign which was converted into a space when a download query gets to Mergin server
  // https://doc.qt.io/qt-5/qurlquery.html#handling-of-spaces-and-plus
  QString projectName = "testDownloadProjectSpecChars";
  QString projectNamespace = mUsername;
  QString projectDir = mApi->projectsPath() + "/" + projectName + "/";

  // create an empty project on the server
  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName, true );
  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );
  // make MerginApi aware of the project and its directory
  mApi->localProjectsManager().addMerginProject( projectDir, projectNamespace, projectName );

  // Copy data
  QString sourcePath = mTestDataPath + "/" + TestMerginApi::TEST_PROJECT_NAME + "/";
  InputUtils::cpDir( sourcePath, projectDir );

  // Add special characters in the project file name
  QFile projectFile( projectDir + "/project.qgs" );
  QVERIFY( projectFile.exists() );
  QString specChars( "+?%@&" );
  QString newProjectFileName = QString( "%1.qgs" ).arg( specChars );
  QVERIFY( projectFile.rename( projectDir + "/" + newProjectFileName ) );

  // Upload data
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );
  mApi->uploadProject( projectNamespace, projectName );
  QVERIFY( spy2.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );
  QList<QVariant> arguments = spy2.takeFirst();
  QVERIFY( arguments.at( 2 ).toBool() );

  // Download project and check if the project file is there
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;
  QFile projectFileExtra( projectDirExtra + "/" + newProjectFileName );
  QVERIFY( projectFileExtra.exists() );
}

void TestMerginApi::createRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, const QString &sourcePath )
{
  // create a project
  QSignalSpy spy( api, &MerginApi::projectCreated );
  api->createProject( projectNamespace, projectName, true );
  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
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
  QVERIFY( spy3.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy3.count(), 1 );
  QList<QVariant> arguments = spy3.takeFirst();
  QVERIFY( arguments.at( 2 ).toBool() );

  // Remove the whole project
  QDir( projectDir ).removeRecursively();
  QFileInfo info( projectDir );
  QDir dir( projectDir );
  QCOMPARE( info.size(), 0 );
  QVERIFY( dir.isEmpty() );

  api->localProjectsManager().removeLocalProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );

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
  QVERIFY( spy6.wait( TestUtils::LONG_REPLY ) );
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

  MerginProjectsList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );

  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName, true );
  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );

  projects = getProjectList();
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

  QVERIFY( mCreatedProjectsModel->rowCount() );
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ).isValid() );

  // Create again, expecting error
  QSignalSpy spy2( mApi, &MerginApi::networkErrorOccurred );
  mApi->createProject( projectNamespace, projectName, true );
  QVERIFY( spy2.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  QList<QVariant> arguments = spy2.takeFirst();
  QVERIFY( arguments.at( 0 ).type() == QVariant::String );
  QVERIFY( arguments.at( 1 ).type() == QVariant::String );

  QCOMPARE( arguments.at( 1 ).toString(), QStringLiteral( "Mergin API error: createProject" ) );

  //Clean created project
  QSignalSpy spy3( mApi, &MerginApi::serverProjectDeleted );
  mApi->deleteProject( projectNamespace, projectName );
  QVERIFY( spy3.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy3.takeFirst().at( 1 ).toBool(), true );

  projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );
}

void TestMerginApi::testDeleteNonExistingProject()
{
  // Checks if projects doesn't exist
  QString projectName = "testDeleteNonExistingProject";
  QString projectNamespace = mUsername;
  MerginProjectsList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );

  // Try to delete non-existing project
  QSignalSpy spy( mApi, &MerginApi::networkErrorOccurred );
  mApi->deleteProject( projectNamespace, projectName );
  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );

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
  MerginProjectsList projects = getProjectList();
  QVERIFY( !_findProjectByName( projectNamespace, projectName, projects ).isValid() );

  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  mApi->createProject( projectNamespace, projectName, true );
  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );

  projects = getProjectList();
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

  QVERIFY( mCreatedProjectsModel->rowCount() );
  Q_ASSERT( _findProjectByName( projectNamespace, projectName, projects ).isValid() );

  // Delete created project
  QSignalSpy spy2( mApi, &MerginApi::serverProjectDeleted );
  mApi->deleteProject( projectNamespace, projectName );
  QVERIFY( spy2.wait( TestUtils::SHORT_REPLY ) );
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
  mApiExtra->createProject( projectNamespace, projectName, true );
  QVERIFY( spy0.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy0.count(), 1 );
  QCOMPARE( spy0.takeFirst().at( 1 ).toBool(), true );

  MerginProjectsList projects = getProjectList();
  QVERIFY( _findProjectByName( projectNamespace, projectName, projects ).isValid() );

  // copy project's test data to the new project directory
  QVERIFY( InputUtils::cpDir( mTestDataPath + "/" + TEST_PROJECT_NAME, projectDir ) );
  mApi->localProjectsManager().addMerginProject( projectDir, projectNamespace, projectName );

  // project info does not have any version information yet
  std::shared_ptr<Project> project0 = mLocalProjectsModel->projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project0 && project0->isLocal() && !project0->isMergin() );
  QCOMPARE( project0->local->localVersion, -1 );

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
  std::shared_ptr<Project> project1 = mLocalProjectsModel->projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project1 && project1->isLocal() && !project1->isMergin() );
  QCOMPARE( project1->local->localVersion, -1 );

  //
  // try to upload, but cancel it after started to upload files
  // (so that we test also cancellation of transaction)
  //

  QSignalSpy spyX( mApi, &MerginApi::syncProjectFinished );
  QSignalSpy spyY( mApi, &MerginApi::pushFilesStarted );
  mApi->uploadProject( projectNamespace, projectName );
  QVERIFY( spyY.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spyY.count(), 1 );

  QSignalSpy spyCancel( mApi, &MerginApi::uploadCanceled );
  mApi->uploadCancel( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( spyCancel.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spyCancel.count(), 1 );

  // no need to wait for the signal here - as we call abort() the reply's finished() signal is immediately emitted
  QCOMPARE( spyX.count(), 1 );
  QList<QVariant> argumentsX = spyX.takeFirst();
  QVERIFY( !argumentsX.at( 2 ).toBool() );

  // server version is now available (cancelled after project info), but after projects model refresh
  std::shared_ptr<Project> project2 = mLocalProjectsModel->projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project2 && project2->isLocal() && !project2->isMergin() );
  QCOMPARE( project2->local->localVersion, -1 );

  refreshProjectsModel( ProjectsModel::LocalProjectsModel );

  project2 = mLocalProjectsModel->projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project2 && project2->isLocal() && project2->isMergin() );
  QCOMPARE( project2->local->localVersion, -1 );
  QCOMPARE( project2->mergin->serverVersion, 0 );

  //
  // try to upload - and let the upload finish successfully
  //

  mApi->uploadProject( projectNamespace, projectName );
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );

  QVERIFY( spy2.wait( TestUtils::LONG_REPLY ) );
  QCOMPARE( spy2.count(), 1 );

  std::shared_ptr<Project> project3 = mLocalProjectsModel->projectFromId( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project3 && project3->isLocal() && project3->isMergin() );
  QCOMPARE( project3->local->localVersion, 1 );
  QCOMPARE( project3->mergin->serverVersion, 1 );
  QCOMPARE( project3->mergin->status, ProjectStatus::UpToDate );
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

void TestMerginApi::testEmptyFileUploadDownload()
{
  // test will try to upload a project with empty file

  QString projectName = QStringLiteral( "testEmptyFileUploadDownload" );

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  QString emptyFileDestinationPath = mApi->projectsPath() + "/" + projectName + "/" + TEST_EMPTY_FILE_NAME;

  // copy empty file to project
  QFile::copy( mTestDataPath + "/" + TEST_EMPTY_FILE_NAME, emptyFileDestinationPath );
  QVERIFY( QFileInfo::exists( emptyFileDestinationPath ) );

  QByteArray checksum = MerginApi::getChecksum( emptyFileDestinationPath );
  QVERIFY( !checksum.isEmpty() );

  //upload
  uploadRemoteProject( mApi, mUsername, projectName );

  //download again
  deleteLocalProject( mApi, mUsername, projectName );
  QVERIFY( !QFileInfo::exists( emptyFileDestinationPath ) );
  downloadRemoteProject( mApi, mUsername, projectName );

  // verify it's there and with correct content
  QByteArray checksum2 = MerginApi::getChecksum( emptyFileDestinationPath );
  QVERIFY( QFileInfo::exists( emptyFileDestinationPath ) );
  QCOMPARE( checksum, checksum2 );
}


void TestMerginApi::testPushAddedFile()
{
  QString projectName = "testPushAddedFile";

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

  downloadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project0 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project0 && project0->isLocal() && project0->isMergin() );
  QCOMPARE( project0->local->localVersion, 1 );
  QCOMPARE( project0->mergin->serverVersion, 1 );
  QCOMPARE( project0->mergin->status, ProjectStatus::UpToDate );

  // add a single file
  QString newFilePath = mApi->projectsPath() + "/" + projectName + "/added.txt";
  QFile file( newFilePath );
  QVERIFY( file.open( QIODevice::WriteOnly ) );
  file.write( "added file content\n" );
  file.close();

  // check that the status is "modified"
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel ); // force update of status

  std::shared_ptr<Project> project1 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project1 && project1->isLocal() && project1->isMergin() );
  QCOMPARE( project1->local->localVersion, 1 );
  QCOMPARE( project1->mergin->serverVersion, 1 );
  QCOMPARE( project1->mergin->status, ProjectStatus::Modified );

  // upload
  uploadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project2 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project2 && project2->isLocal() && project2->isMergin() );
  QCOMPARE( project2->local->localVersion, 2 );
  QCOMPARE( project2->mergin->serverVersion, 2 );
  QCOMPARE( project2->mergin->status, ProjectStatus::UpToDate );

  deleteLocalProject( mApi, mUsername, projectName );

  downloadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project3 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project3 && project3->isLocal() && project3->isMergin() );
  QCOMPARE( project3->local->localVersion, 2 );
  QCOMPARE( project3->mergin->serverVersion, 2 );
  QCOMPARE( project3->mergin->status, ProjectStatus::UpToDate );

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
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

  downloadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project0 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project0 && project0->isLocal() && project0->isMergin() );
  QCOMPARE( project0->local->localVersion, 1 );
  QCOMPARE( project0->mergin->serverVersion, 1 );
  QCOMPARE( project0->mergin->status, ProjectStatus::UpToDate );

  // Remove file
  QString removedFilePath = mApi->projectsPath() + "/" + projectName + "/test1.txt";
  QFile file( removedFilePath );
  QVERIFY( file.exists() );
  file.remove();
  QVERIFY( !file.exists() );

  // check that it is considered as modified now
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel ); // force update of status

  std::shared_ptr<Project> project1 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project1 && project1->isLocal() && project1->isMergin() );
  QCOMPARE( project1->local->localVersion, 1 );
  QCOMPARE( project1->mergin->serverVersion, 1 );
  QCOMPARE( project1->mergin->status, ProjectStatus::Modified );

  // upload changes

  uploadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project2 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project2 && project2->isLocal() && project2->isMergin() );
  QCOMPARE( project2->local->localVersion, 2 );
  QCOMPARE( project2->mergin->serverVersion, 2 );
  QCOMPARE( project2->mergin->status, ProjectStatus::UpToDate );

  deleteLocalProject( mApi, mUsername, projectName );

  downloadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project3 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project3 && project3->isLocal() && project3->isMergin() );
  QCOMPARE( project3->local->localVersion, 2 );
  QCOMPARE( project3->mergin->serverVersion, 2 );
  QCOMPARE( project3->mergin->status, ProjectStatus::UpToDate );

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
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

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
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel ); // force update of status
  std::shared_ptr<Project> project1 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project1 && project1->isLocal() && project1->isMergin() );
  QCOMPARE( project1->local->localVersion, 1 );
  QCOMPARE( project1->mergin->serverVersion, 1 );
  QCOMPARE( project1->mergin->status, ProjectStatus::Modified );

  // upload
  uploadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project2 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project2 && project2->isLocal() && project2->isMergin() );
  QCOMPARE( project2->local->localVersion, 2 );
  QCOMPARE( project2->mergin->serverVersion, 2 );
  QCOMPARE( project2->mergin->status, ProjectStatus::UpToDate );

  // verify the remote project has updated file

  deleteLocalProject( mApi, mUsername, projectName );

  QVERIFY( !file.open( QIODevice::ReadOnly ) );  // it should not exist at this point

  downloadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project3 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project3 && project3->isLocal() && project3->isMergin() );
  QCOMPARE( project3->local->localVersion, 2 );
  QCOMPARE( project3->mergin->serverVersion, 2 );
  QCOMPARE( project3->mergin->status, ProjectStatus::UpToDate );

  QVERIFY( file.open( QIODevice::ReadOnly ) );
  QCOMPARE( file.readAll(), QByteArray( "v2" ) );
  file.close();
}

void TestMerginApi::testPushNoChanges()
{
  QString projectName = "testPushNoChanges";
  QString projectDir = mApi->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

  downloadRemoteProject( mApi, mUsername, projectName );

  // check that the status is still "up-to-date"
  std::shared_ptr<Project> project1 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project1 && project1->isLocal() && project1->isMergin() );
  QCOMPARE( project1->local->localVersion, 1 );
  QCOMPARE( project1->mergin->serverVersion, 1 );
  QCOMPARE( project1->mergin->status, ProjectStatus::UpToDate );

  // upload - should do nothing
  uploadRemoteProject( mApi, mUsername, projectName );


  std::shared_ptr<Project> project2 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project2 && project2->isLocal() && project2->isMergin() );
  QCOMPARE( project2->local->localVersion, 1 );
  QCOMPARE( project2->mergin->serverVersion, 1 );
  QCOMPARE( project2->mergin->status, ProjectStatus::UpToDate );

  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );
}

void TestMerginApi::testUpdateAddedFile()
{
  // this test downloads a project, then a file gets added on the server
  // and we check whether the update was correct (i.e. the file got added too)

  QString projectName = "testUpdateAddedFile";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

  // download initial version
  downloadRemoteProject( mApi, mUsername, projectName );
  QVERIFY( !QFile::exists( projectDir + "/test-remote-new.txt" ) );

  std::shared_ptr<Project> project0 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project0 && project0->isLocal() && project0->isMergin() );
  QCOMPARE( project0->local->localVersion, 1 );
  QCOMPARE( project0->mergin->serverVersion, 1 );
  QCOMPARE( project0->mergin->status, ProjectStatus::UpToDate );

  // remove a file on the server
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( extraProjectDir + "/test-remote-new.txt", QByteArray( "my new content" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );
  QVERIFY( QFile::exists( extraProjectDir + "/test-remote-new.txt" ) );

  // list projects - just so that we can figure out we are behind
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

  std::shared_ptr<Project> project1 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project1 && project1->isLocal() && project1->isMergin() );
  QCOMPARE( project1->local->localVersion, 1 );
  QCOMPARE( project1->mergin->serverVersion, 2 );
  QCOMPARE( project1->mergin->status, ProjectStatus::OutOfDate );

  // now try to update
  downloadRemoteProject( mApi, mUsername, projectName );

  std::shared_ptr<Project> project2 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project2 && project2->isLocal() && project2->isMergin() );
  QCOMPARE( project2->local->localVersion, 2 );
  QCOMPARE( project2->mergin->serverVersion, 2 );
  QCOMPARE( project2->mergin->status, ProjectStatus::UpToDate );


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
  // and the local version should go to "test1 (conflicted copy, <username> v<version>).txt"
  QString conflictFilename = projectDir + "/test1 (conflicted copy, " + mUsername + " v1).txt";
  QCOMPARE( readFileContent( filename ), QByteArray( "remote content" ) );
  QCOMPARE( readFileContent( conflictFilename ), QByteArray( "local content" ) );

  // Second conflict
  qDebug() << "modify test1.txt on the server";
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( extraFilename, QByteArray( "remote content 2" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  qDebug() << "modify test1.txt locally and do the sync";
  writeFileContent( filename, QByteArray( "local content 2" ) );
  //
  // TODO: upload should figure out it needs to run update first without this
  // (the simple check in uploadProject() likely won't be good enough to find
  // out... in upload's project info handler if there is a need for update,
  // the upload should be cancelled (or paused to update first).
  //
  downloadRemoteProject( mApi, mUsername, projectName );
  uploadRemoteProject( mApi, mUsername, projectName );

  // verify the result: the server version should be in test1.txt
  // and the local version should go to "test1 (conflicted copy, <username> v<version>).txt"
  // Note: test1.txt conflict file should still be the same
  QString conflictFilename2 = projectDir + "/test1 (conflicted copy, " + mUsername + " v3).txt";
  QCOMPARE( readFileContent( filename ), QByteArray( "remote content 2" ) );
  QCOMPARE( readFileContent( conflictFilename ), QByteArray( "local content" ) );
  QCOMPARE( readFileContent( conflictFilename2 ), QByteArray( "local content 2" ) );
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
  // and the local version should go to conflicted copy file
  QString conflictFilename = projectDir + "/test-new-file (conflicted copy, " + mUsername + " v1).txt";
  QCOMPARE( readFileContent( filename ), QByteArray( "new remote content" ) );
  QCOMPARE( readFileContent( conflictFilename ), QByteArray( "new local content" ) );
}

void TestMerginApi::testEditConflictScenario()
{
  // this test simulates creation of edit conflict when two
  // clients are trying to update the same attribute.
  // edit conflict file should be created inside project folder and synced to server

  QString projectName = "testEditConflictScenario";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString extraProjectDir = mApiExtra->projectsPath() + "/" + projectName;

  // folder rebase_edit_conflict
  QString dataProjectDir = TestUtils::testDataDir() + "/" + QStringLiteral( "rebase_edit_conflict" );
  qDebug() << "About to create the project";
  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  qDebug() << "Project has been created!";
  QString dbName = QStringLiteral( "data.gpkg" );
  QString baseDB = dataProjectDir + QStringLiteral( "/base.gpkg" );
  QString localChangeDB = dataProjectDir + QStringLiteral( "/local-change.gpkg" );
  QString remoteChangeDB = dataProjectDir + QStringLiteral( "/remote-change.gpkg" );

  downloadRemoteProject( mApi, mUsername, projectName );

  // upload base db
  InputUtils::copyFile( baseDB, projectDir + "/" + dbName );
  uploadRemoteProject( mApi, mUsername, projectName );

  // both clients now sync the project so that both of them have base gpkg
  downloadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  // both clients now make change to the same field
  InputUtils::removeFile( projectDir + "/" + dbName );
  InputUtils::removeFile( extraProjectDir + "/" + dbName );
  InputUtils::copyFile( localChangeDB, projectDir + "/" + dbName );
  InputUtils::copyFile( remoteChangeDB, extraProjectDir + "/" + dbName );

  // client B syncs his changes
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  //
  // now client A syncs, resulting in edit conflict
  //

  uploadRemoteProject( mApi, mUsername, projectName );

  QDir projDir( projectDir );

  // check the edit conflict file presence
  QVERIFY( InputUtils::fileExists( projectDir + "/" + QString( "data (edit conflict, %1 v2).json" ).arg( mUsername ) ) );

  // when client B downloads changes, he should also have that edit conflict file
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  QVERIFY( InputUtils::fileExists( projectDir + "/" + QString( "data (edit conflict, %1 v2).json" ).arg( mUsername ) ) );
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
  refreshProjectsModel( ProjectsModel::CreatedProjectsModel );

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

  std::shared_ptr<Project> project1 = mCreatedProjectsModel->projectFromId( MerginApi::getFullProjectName( mUsername, projectName ) );
  QVERIFY( project1 && project1->isLocal() && project1->isMergin() );
  QCOMPARE( project1->local->localVersion, 3 );
  QCOMPARE( project1->mergin->serverVersion, 3 );
  QCOMPARE( project1->mergin->status, ProjectStatus::UpToDate );

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
  QString changes = GeodiffUtils::diffableFilePendingChanges( projectDir, "base.gpkg", true );
  GeodiffUtils::ChangesetSummary summary = GeodiffUtils::parseChangesetSummary( changes );
  QCOMPARE( summary, expectedSummary );

  uploadRemoteProject( mApi, mUsername, projectName );

  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected
}

void TestMerginApi::testDiffSubdirsUpload()
{
  QString projectName = "testDiffSubdirsUpload";
  QString projectDir = mApi->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + "diff_project_subs" + "/" );

  downloadRemoteProject( mApi, mUsername, projectName );

  const QString base( "subdir/subsubdir/base.gpkg" );
  QVERIFY( QFileInfo::exists( projectDir + "/.mergin/" + base ) );

  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );  // no local changes expected


  // replace gpkg with a new version with a modified geometry
  QFile::remove( projectDir + "/" + base );
  QFile::copy( mTestDataPath + "/modified_1_geom.gpkg", projectDir + "/" + base );

  ProjectDiff diff = MerginApi::localProjectChanges( projectDir );
  ProjectDiff expectedDiff;
  expectedDiff.localUpdated = QSet<QString>() << base ;
  QCOMPARE( diff, expectedDiff );

  GeodiffUtils::ChangesetSummary expectedSummary;
  expectedSummary["simple"] = GeodiffUtils::TableSummary( 0, 1, 0 );
  QString changes = GeodiffUtils::diffableFilePendingChanges( projectDir, base, true );
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
  QCOMPARE( vl0->featureCount(), static_cast<long>( 3 ) );
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
  QCOMPARE( vl->featureCount(), static_cast<long>( 4 ) );
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
  QCOMPARE( vl0->featureCount(), static_cast<long>( 3 ) );
  vl0->startEditing();
  QgsFeature f( vl0->fields() );
  f.setAttribute( "name", "def" );
  f.setAttribute( "rating", 456 );
  f.setGeometry( QgsGeometry::fromPointXY( QgsPointXY( 4, 5 ) ) );
  vl0->addFeature( f );
  vl0->commitChanges();
  QCOMPARE( vl0->featureCount(), static_cast<long>( 4 ) );
  delete vl0;

  // check that the file is marked as changed
  ProjectDiff diff = MerginApi::localProjectChanges( projectDir );
  ProjectDiff expectedDiff;
  expectedDiff.localUpdated = QSet<QString>() << "base.gpkg";
  QCOMPARE( diff, expectedDiff );

  // check that geodiff knows there was one added feature
  GeodiffUtils::ChangesetSummary expectedSummary;
  expectedSummary["simple"] = GeodiffUtils::TableSummary( 1, 0, 0 );
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
  QCOMPARE( vl->featureCount(), static_cast<long>( 5 ) );
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

  bool r2 = QFile::remove( projectDir + "/base.gpkg" );
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
  QCOMPARE( changes, QString( "ERROR" ) );  // local diff should fail

  // update our local version now
  downloadRemoteProject( mApi, mUsername, projectName );

  //
  // check the result
  //
  QString conflictFilename = "base (conflicted copy, " + mUsername + " v1).gpkg";
  QVERIFY( QFile::exists( projectDir + "/base.gpkg" ) );
  QVERIFY( QFile::exists( projectDir + "/" + conflictFilename ) );

  // there should be a new "conflict" file
  ProjectDiff expectedDiffFinal;
  expectedDiffFinal.localAdded = QSet<QString>() << conflictFilename;
  QCOMPARE( MerginApi::localProjectChanges( projectDir ), expectedDiffFinal );
}

void TestMerginApi::testUpdateWithDiffs()
{
  // a test case where we download initial version (v1), then there will be
  // two versions with diffs (v2 and v3), afterwards we try to update the local project.

  QString projectName = "testUpdateWithDiffs";
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

  // one more change + upload
  bool r2 = QFile::remove( projectDirExtra + "/base.gpkg" );
  bool r3 = QFile::copy( mTestDataPath + "/added_row_2.gpkg", projectDirExtra + "/base.gpkg" );
  QVERIFY( r2 && r3 );
  writeFileContent( projectDirExtra + "/dummy.txt", "first" );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  //
  // now update project locally
  //

  downloadRemoteProject( mApi, mUsername, projectName );

  QgsVectorLayer *vl = new QgsVectorLayer( projectDir + "/base.gpkg|layername=simple", "base", "ogr" );
  QVERIFY( vl->isValid() );
  QCOMPARE( vl->featureCount(), static_cast<long>( 5 ) );
  delete vl;

  QCOMPARE( MerginApi::localProjectChanges( projectDir ), ProjectDiff() );
  QVERIFY( !GeodiffUtils::hasPendingChanges( projectDir, "base.gpkg" ) );
}

void TestMerginApi::testUpdateWithMissedVersion()
{
  // when updating from v3 to v4, it is expected that we will get references to diffs for v2-v3 and v3-v4.
  // There was a bug where we always ignored the first one. But it could happen that there is no update in v2-v3,
  // and we ended up ignoring v3-v4, ending up with broken basefiles.

  // 1. [extra] create project, upload .gpkg (v1)
  // 3. [extra] upload a new file (v2)
  // 2. [main]  download project (v2)
  // 4. [extra] upload updated .gpkg (v3)
  // 5. [main]  update from v2 to v3

  QString projectName = "testUpdateWithMissedVersion";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  // step 1
  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + "diff_project" + "/" );
  // step 2
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  writeFileContent( projectDirExtra + "/file1.txt", QByteArray( "hello" ) );
  uploadRemoteProject( mApiExtra, mUsername, projectName );
  // step 3
  downloadRemoteProject( mApi, mUsername, projectName );
  // step 4
  bool r0 = QFile::remove( projectDirExtra + "/base.gpkg" );
  bool r1 = QFile::copy( mTestDataPath + "/added_row.gpkg", projectDirExtra + "/base.gpkg" );
  QVERIFY( r0 && r1 );
  uploadRemoteProject( mApiExtra, mUsername, projectName );
  // step 5
  downloadRemoteProject( mApi, mUsername, projectName );

  // check that added row in v3 has been added in our local file too
  QgsVectorLayer *vl = new QgsVectorLayer( projectDir + "/base.gpkg|layername=simple", "base", "ogr" );
  QVERIFY( vl->isValid() );
  QCOMPARE( vl->featureCount(), static_cast<long>( 4 ) );
  delete vl;
}

void TestMerginApi::testMigrateProject()
{
  QString projectName = "testMigrateProject";
  // make local copy of project
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  createLocalProject( projectDir );

  // reload localmanager after copying the project
  mApi->mLocalProjects.reloadDataDir();
  QStringList entryList = QDir( projectDir ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );

  // migrate project
  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );

  mApi->migrateProjectToMergin( projectName );

  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );
  QCOMPARE( mApi->transactions().count(), 1 );
  QVERIFY( spy2.wait( TestUtils::LONG_REPLY * 5 ) );

  // remove local copy of project
  deleteLocalProject( mApi, mUsername, projectName );
  QVERIFY( !QFileInfo::exists( projectDir ) );

  // download the project
  downloadRemoteProject( mApi, mUsername, projectName );

  // verify that all files have been uploaded
  QStringList entryList2 = QDir( projectDir ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );
  QCOMPARE( entryList, entryList2 );
}

void TestMerginApi::testMigrateProjectAndSync()
{
  // When a new project is migrated to Mergin, creating basefiles for diffable files was omitted.
  // Therefore sync was not properly working resulting into having a conflict file.
  // Test covers creting a new project, migrating it to Mergin and both sides sync.

  // 1. [main] create project with .gpkg (v1) file
  // 2. [main] migrate the project to mergin
  // 3. [extra] download the project and make changes to .gpkg (v2)
  // 4. [main] sync the project (v2), should be valid without conflicts
  // 5. [main] make changes to .gpkg (v3) and sync
  // 6. [extra] sync the project (v3), should be valid without conflicts

  QString projectName = "testMigrateProjectAndSync";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  // step 1
  createLocalProject( projectDir );
  mApi->mLocalProjects.reloadDataDir();
  // step 2
  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );

  mApi->migrateProjectToMergin( projectName );

  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );
  QCOMPARE( mApi->transactions().count(), 1 );
  QVERIFY( spy2.wait( TestUtils::LONG_REPLY * 5 ) );

  // step 3
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  bool r0 = QFile::remove( projectDirExtra + "/base.gpkg" );
  bool r1 = QFile::copy( mTestDataPath + "/added_row.gpkg", projectDirExtra + "/base.gpkg" );
  QVERIFY( r0 && r1 );
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  // step 4
  downloadRemoteProject( mApi, mUsername, projectName );
  QVERIFY( QFile( projectDir + "/base.gpkg" ).exists() );
  QStringList projectMerginDirEntries = QDir( projectDir + "/.mergin" ).entryList( QDir::AllEntries | QDir::NoDotAndDotDot );
  for ( QString filepath : projectMerginDirEntries )
  {
    QVERIFY( !filepath.contains( QStringLiteral( "conflict" ) ) );
  }

  // step 5
  r0 = QFile::remove( projectDir + "/base.gpkg" );
  r1 = QFile::copy( mTestDataPath + "/added_row_2.gpkg", projectDir + "/base.gpkg" );
  QVERIFY( r0 && r1 );
  uploadRemoteProject( mApi, mUsername, projectName );

  // step 6
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  QVERIFY( QFile( projectDir + "/base.gpkg" ).exists() );
  QStringList projectMerginDirExtraEntries = QDir( projectDirExtra + "/.mergin" ).entryList( QDir::AllEntries | QDir::NoDotAndDotDot );
  for ( QString filepath : projectMerginDirExtraEntries )
  {
    QVERIFY( !filepath.contains( QStringLiteral( "conflict" ) ) );
  }
}

void TestMerginApi::testMigrateDetachProject()
{
  QString projectName = "testMigrateDetachProject";
  // make local copy of project
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  createLocalProject( projectDir );

  // reload localmanager after copying the project
  mApi->mLocalProjects.reloadDataDir();

  // migrate project
  QSignalSpy spy( mApi, &MerginApi::projectCreated );
  QSignalSpy spy2( mApi, &MerginApi::syncProjectFinished );

  mApi->migrateProjectToMergin( projectName );

  QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
  QCOMPARE( spy.count(), 1 );
  QCOMPARE( spy.takeFirst().at( 1 ).toBool(), true );
  QCOMPARE( mApi->transactions().count(), 1 );
  QVERIFY( spy2.wait( TestUtils::LONG_REPLY * 5 ) );

  // TEST if is mergin project
  QVERIFY( QFileInfo::exists( projectDir + "/.mergin/" ) );

  // detach project
  QString projectNamespace = mUsername;
  mApi->detachProjectFromMergin( projectNamespace, projectName );
  // TEST if is NOT mergin project
  QVERIFY( !QFileInfo::exists( projectDir + "/.mergin/" ) );
}

void TestMerginApi::testSelectiveSync()
{
  // Case: Clients have following configuration: selective sync on, selective-sync-dir empty (project dir by default)
  // Action 1: Client 1 uploads some images and Client 2 sync without downloading the images
  // Action 2: Client 2 uploads an image and do not remove not-synced images. Client 1 syncs without downloading the image, still having own images.

  // Create a project
  QString projectName = "testSelectiveSync";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApiExtra, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Create photo files
  QDir dir;
  QString photoPath( projectDir + "/subdir" );
  if ( !dir.exists( photoPath ) )
    dir.mkpath( photoPath );

  QFile file( projectDir + "/" + "photo.jpg" );
  file.open( QIODevice::WriteOnly );

  QFile file1( photoPath + "/" + "photo.jpg" );
  file1.open( QIODevice::WriteOnly );

  // Download the project and copy mergin config file containing selective sync properties
  downloadRemoteProject( mApiExtra, mUsername, projectName );
  QString configFilePathExtra( projectDirExtra + "/mergin-config.json" );
  QVERIFY( QFile::copy( mTestDataPath + "/mergin-config-project-dir.json", configFilePathExtra ) );

  // Upload config file
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  // Sync event 1:
  // Client 1 uploads images
  uploadRemoteProject( mApi, mUsername, projectName );
  // Download project and check
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  QFile fileExtra( projectDirExtra + "/photo.jpg" );
  QVERIFY( !fileExtra.exists() );

  QFile fileExtra1( projectDirExtra + "/subdir/photo.jpg" );
  QVERIFY( !fileExtra1.exists() );

  // Sync event 2:
  // Client 2 uploads an image

  QFile fileExtra2( projectDirExtra + "/" + "photoExtra.png" );
  fileExtra2.open( QIODevice::WriteOnly );

  // Client 2 uploads a new image
  uploadRemoteProject( mApiExtra, mUsername, projectName );

  // Client 1 syncs without Client 2's new image and without removing own images.
  downloadRemoteProject( mApi, mUsername, projectName );

  QVERIFY( file.exists() );
  QVERIFY( file1.exists() );
  QFile file2( projectDir + "/" + "photoExtra.png" );
  QVERIFY( !file2.exists() );
}

void TestMerginApi::testSelectiveSyncSubfolder()
{
  /*
   * Case: Downloading project with config.
   *
   * We have following scenario:
   * {
   *   "input-selective-sync": true,
   *   "input-selective-sync-dir": "photos" // having subfolder
   * }
   *
   * Action 1: Client 1 creates project with mergin-config and uploads some images,
   *           Client 2 should sync without downloading the images.
   * Action 2: Client 2 uploads two images, one in "photos" subdirectory and second in project root.
   *           Client 1 should sync without downloading the image in "photos" subdirectory and should still have own images
   *           (they should not be deleted even though Client 2 did not have them when syncing)
   */

  // Create a project
  QString projectName = "testSelectiveSyncSubfolder";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApi, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Create photo files
  QDir dir;
  QString photoPath( projectDir + "/photos" );
  if ( !dir.exists( photoPath ) )
    dir.mkpath( photoPath );

  QFile file( photoPath + "/" + "photoA.jpg" );
  file.open( QIODevice::WriteOnly );
  file.close();

  QFile file1( photoPath + "/" + "photoB.png" );
  file1.open( QIODevice::WriteOnly );
  file1.close();

  // Add mergin-config.json to the project
  QString configFilePath( projectDir + "/mergin-config.json" );
  QVERIFY( QFile::copy( mTestDataPath + "/mergin-config-subfolder.json", configFilePath ) );

  // Upload project
  uploadRemoteProject( mApi, mUsername, projectName );

  // Client 2 in Action 1: should download project without images in subfolder "photos"
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  QString photoPathExtra( projectDirExtra + "/photos" );

  QFile fileExtra( photoPathExtra + "/" + "photoA.jpg" );
  QVERIFY( !fileExtra.exists() );

  QFile fileExtra1( photoPathExtra + "/" + "photoB.png" );
  QVERIFY( !fileExtra1.exists() );

  // ----
  // Action 2
  // ----

  // Client 2 adds 2 images, one to project root, another to "photos" subfolder
  QDir photoDirExtra( photoPathExtra );
  if ( !photoDirExtra.exists() ) // if the subfolder contained only photos, it was not even created in Client 2
    photoDirExtra.mkpath( photoPathExtra );

  QFile extraFile( photoPathExtra + "/" + "photoC.jpg" );
  extraFile.open( QIODevice::WriteOnly );
  extraFile.close();

  QFile extraRootFile( projectDirExtra + "/" + "photoD.png" );
  extraRootFile.open( QIODevice::WriteOnly );
  extraRootFile.close();

  // Client 2 uploads, Client 1 downloads
  uploadRemoteProject( mApiExtra, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Check existence of "photoD" in root and not existence of photoC in "photos"
  QFile fileExtra2( photoPath + "/" + "photoC.jpg" );
  QVERIFY( !fileExtra2.exists() );

  QFile fileExtra3( projectDir + "/" + "photoD.png" );
  QVERIFY( fileExtra3.exists() );

  // Check that photos were not deleted for Client 1
  QFile file2( photoPath + "/" + "photoA.jpg" );
  QVERIFY( file2.exists() );

  QFile file3( photoPath + "/" + "photoB.png" );
  QVERIFY( file2.exists() );
}

void TestMerginApi::testSelectiveSyncAddConfigToExistingProject()
{
  /*
   * Case: Have a project with photos without mergin config, add it when both clients are using project already to simulate
   *       users that add mergin config to existing projects.
   *
   * Procedure: Create project with photos, sync it to both clients, then let Client 1 add mergin config together with several
   *            pictures and see if the new pictures are NOT synced.
   */

  // Create a project
  QString projectName = "testSelectiveSyncAddConfigToExistingProject";
  QString projectDir = mApi->projectsPath() + "/" + projectName;
  QString projectDirExtra = mApiExtra->projectsPath() + "/" + projectName;

  createRemoteProject( mApi, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Create photo files
  QDir dir;
  QString photoPath( projectDir + "/photos" );
  if ( !dir.exists( photoPath ) )
    dir.mkpath( photoPath );

  QFile file( photoPath + "/" + "photoA.jpg" );
  file.open( QIODevice::WriteOnly );
  file.close();

  QFile file1( photoPath + "/" + "photoB.png" );
  file1.open( QIODevice::WriteOnly );
  file1.close();

  // Sync project for both clients, Client 2 should have both pictures
  uploadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  QString photoPathExtra( projectDirExtra + "/photos" );

  QFile fileExtra( photoPathExtra + "/" + "photoA.jpg" );
  QVERIFY( fileExtra.exists() );

  QFile fileExtra1( photoPathExtra + "/" + "photoB.png" );
  QVERIFY( fileExtra1.exists() );

  // Add mergin-config.json to the project together with another image
  QString configFilePath( projectDir + "/mergin-config.json" );
  QVERIFY( QFile::copy( mTestDataPath + "/mergin-config-subfolder.json", configFilePath ) );

  QFile file2( photoPath + "/" + "photoC.png" );
  file2.open( QIODevice::WriteOnly );
  file2.close();

  // Sync project for both clients
  uploadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  // With mergin-config, "photoC" should not exist for Client 2
  QFile fileExtra2( photoPathExtra + "/" + "photoC.png" );
  QVERIFY( !fileExtra2.exists() );
}

void TestMerginApi::testSelectiveSyncRemoveConfig()
{
  /*
   * Case: Remove mergin-config from an existing project with photos.
   *
   * We will create another API client that will serve as a server mirror, it will not use selective sync,
   * but will simulate as if someone manipulated project from browser via Mergin
   */

  QString serverMirrorDataPath = mApi->projectsPath() + "/" + "serverMirror";
  QDir serverMirrorDataDir( serverMirrorDataPath );
  if ( !serverMirrorDataDir.exists() )
    serverMirrorDataDir.mkpath( serverMirrorDataPath );

  LocalProjectsManager *serverMirrorProjects = new LocalProjectsManager( serverMirrorDataPath + "/" );
  MerginApi *serverMirror = new MerginApi( *serverMirrorProjects, this );
  serverMirror->setSupportsSelectiveSync( false );

  // Create a project with photos and mergin-config
  QString projectName = "testSelectiveSyncRemoveConfig";

  QString projectClient1 = mApi->projectsPath() + "/" + projectName;
  QString projectClient2 = mApiExtra->projectsPath() + "/" + projectName;
  QString projectServer = serverMirror->projectsPath() + "/" + projectName;

  createRemoteProject( mApi, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Create photo files
  QDir dir;
  QString photoPathClient1( projectClient1 + "/photos" );
  if ( !dir.exists( photoPathClient1 ) )
    dir.mkpath( photoPathClient1 );

  QFile file( photoPathClient1 + "/" + "photoA.jpg" );
  file.open( QIODevice::WriteOnly );
  file.close();

  QFile file1( photoPathClient1 + "/" + "photoB.png" );
  file1.open( QIODevice::WriteOnly );
  file1.close();

  uploadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  QString configFilePath = projectServer + "/" + "mergin-config.json";
  QVERIFY( createJsonFile( configFilePath,
  {
    { "input-selective-sync", true },
    { "input-selective-sync-dir", "photos" }
  } ) );

  uploadRemoteProject( serverMirror, mUsername, projectName );
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  QString photoPathClient2( projectClient2 + "/photos" );

  QFile fileExtra( photoPathClient2 + "/" + "photoA.jpg" );
  QVERIFY( !fileExtra.exists() );

  QFile fileExtra1( photoPathClient2 + "/" + "photoB.png" );
  QVERIFY( !fileExtra1.exists() );

  // Client 2 adds another picture
  QDir photoDirExtra( photoPathClient2 );
  if ( !photoDirExtra.exists() )
    photoDirExtra.mkpath( photoPathClient2 );

  QFile file2( photoPathClient2 + "/" + "photoC.png" );
  file2.open( QIODevice::WriteOnly );
  file2.close();

  uploadRemoteProject( mApiExtra, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  // Let's remove mergin config
  InputUtils::removeFile( configFilePath );
  QVERIFY( !InputUtils::fileExists( configFilePath ) );

  // Sync removed config
  uploadRemoteProject( serverMirror, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName ); // download back to apply the changes -> should download photos

  QFile fextra( photoPathClient2 + "/" + "photoC2-extra.png" );
  fextra.open( QIODevice::WriteOnly );
  fextra.close();

  uploadRemoteProject( mApiExtra, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );

  QString photoPathServer = serverMirrorDataPath + "/" + projectName + "/" + "photos";

  // check that all clients have all photos
  QStringList photos;
  photos << "photoA.jpg" << "photoB.png" << "photoC.png" << "photoC2-extra.png";
  for ( const QString &photo : photos )
  {
    QFile photo1( photoPathClient1 + "/" + photo );
    QFile photo2( photoPathClient2 + "/" + photo );
    QFile photo3( photoPathServer + "/" + photo );

    QVERIFY( photo1.exists() );
    QVERIFY( photo2.exists() );
    QVERIFY( photo3.exists() );
  }

  delete serverMirror;
  delete serverMirrorProjects;
}

void TestMerginApi::testSelectiveSyncDisabledInConfig()
{
  /*
   * Case: Disable selective sync in mergin-config in an existing project with photos and selective sync previously enabled.
   *
   * We will create another API client that will serve as a server mirror, it will not use selective sync,
   * but will simulate as if someone manipulated project from browser via Mergin
   */
  QString serverMirrorDataPath = mApi->projectsPath() + "/" + "serverMirror";
  QDir serverMirrorDataDir( serverMirrorDataPath );
  if ( !serverMirrorDataDir.exists() )
    serverMirrorDataDir.mkpath( serverMirrorDataPath );

  LocalProjectsManager *serverMirrorProjects = new LocalProjectsManager( serverMirrorDataPath + "/" );
  MerginApi *serverMirror = new MerginApi( *serverMirrorProjects, this );
  serverMirror->setSupportsSelectiveSync( false );

  // Create a project with photos and mergin-config
  QString projectName = "testSelectiveSyncDisabledInConfig";

  QString projectClient1 = mApi->projectsPath() + "/" + projectName;
  QString projectClient2 = mApiExtra->projectsPath() + "/" + projectName;
  QString projectServer = serverMirror->projectsPath() + "/" + projectName;

  createRemoteProject( mApi, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Create photo files
  QDir dir;
  QString photoPathClient1( projectClient1 + "/" + "photos" );
  if ( !dir.exists( photoPathClient1 ) )
    dir.mkpath( photoPathClient1 );

  QFile file( photoPathClient1 + "/" + "photoA.jpg" );
  file.open( QIODevice::WriteOnly );
  file.close();

  QFile file1( photoPathClient1 + "/" + "photoB.png" );
  file1.open( QIODevice::WriteOnly );
  file1.close();

  uploadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  QString configFilePath = projectServer + "/" + "mergin-config.json";
  QVERIFY( createJsonFile( configFilePath,
  {
    { "input-selective-sync", true },
    { "input-selective-sync-dir", "photos" }
  } ) );

  uploadRemoteProject( serverMirror, mUsername, projectName );
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  QString photoPathClient2( projectClient2 + "/photos" );

  QFile fileExtra( photoPathClient2 + "/" + "photoA.jpg" );
  QVERIFY( !fileExtra.exists() );

  QFile fileExtra1( photoPathClient2 + "/" + "photoB.png" );
  QVERIFY( !fileExtra1.exists() );

  QDir photoDirExtra( photoPathClient2 );
  if ( !photoDirExtra.exists() )
    photoDirExtra.mkpath( photoPathClient2 );

  // simulate some traffic, let both clients create few photos several times (so that project has longer history)
  for ( int i : { 1, 2, 3, 4, 5 } )
  {
    QFile f1( photoPathClient1 + "/" + QString( "photoC1-%1.png" ).arg( i ) );
    f1.open( QIODevice::WriteOnly );
    f1.close();

    QFile f2( photoPathClient2 + "/" + QString( "photoC2-%1.png" ).arg( i ) );
    f2.open( QIODevice::WriteOnly );
    f2.close();

    uploadRemoteProject( mApiExtra, mUsername, projectName );
    uploadRemoteProject( mApi, mUsername, projectName );
  }

  downloadRemoteProject( serverMirror, mUsername, projectName );

  // Let's disable selective sync
  InputUtils::removeFile( configFilePath );
  QVERIFY( !InputUtils::fileExists( configFilePath ) );

  QVERIFY( createJsonFile( configFilePath,
  {
    { "input-selective-sync", false },
    { "input-selective-sync-dir", "photos" }
  } ) );

  // Sync changed config
  uploadRemoteProject( serverMirror, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName ); // download back to apply the changes -> should download photos

  QFile fextra( photoPathClient2 + "/" + "photoC2-extra.png" );
  fextra.open( QIODevice::WriteOnly );
  fextra.close();

  uploadRemoteProject( mApiExtra, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  // check that all clients have photos
  QStringList photos;
  photos << "photoA.jpg" << "photoB.png" << "photoC1-5.png" << "photoC2-3.png" << "photoC2-extra.png";
  for ( const QString &photo : photos )
  {
    QFile photo1( photoPathClient1 + "/" + photo );
    QFile photo2( photoPathClient2 + "/" + photo );

    QVERIFY( photo1.exists() );
    QVERIFY( photo2.exists() );
  }

  // allow sync again and see if photos will no longer be downloaded
  InputUtils::removeFile( configFilePath );
  QVERIFY( !InputUtils::fileExists( configFilePath ) );

  QVERIFY( createJsonFile( configFilePath,
  {
    { "input-selective-sync", true },
    { "input-selective-sync-dir", "photos" }
  } ) );

  uploadRemoteProject( serverMirror, mUsername, projectName );

  QFile f( photoPathClient2 + "/" + "photoC2-should-not-download.png" );
  f.open( QIODevice::WriteOnly );
  f.close();

  uploadRemoteProject( mApiExtra, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  // File should be on server mirror and should not be on client 1
  QFile fverify( serverMirrorDataPath + "/" + projectName + "/" + "photos" + "/" + "photoC2-should-not-download.png" );
  QVERIFY( fverify.exists() );

  QFile fverify2( photoPathClient1 + "/" + "photoC2-should-not-download.png" );
  QVERIFY( !fverify2.exists() );

  delete serverMirror;
  delete serverMirrorProjects;
}

void TestMerginApi::testSelectiveSyncChangeSyncFolder()
{
  /*
   * Case: Change selective sync folder in mergin-config in an existing project with photos and selective sync enabled.
   *
   * We will create another API client that will serve as a server mirror, it will not use selective sync,
   * but will simulate as if someone manipulated project from browser via Mergin
   */
  QString serverMirrorDataPath = mApi->projectsPath() + "/" + "serverMirror";
  QDir serverMirrorDataDir( serverMirrorDataPath );
  if ( !serverMirrorDataDir.exists() )
    serverMirrorDataDir.mkpath( serverMirrorDataPath );

  LocalProjectsManager *serverMirrorProjects = new LocalProjectsManager( serverMirrorDataPath + "/" );
  MerginApi *serverMirror = new MerginApi( *serverMirrorProjects, this );
  serverMirror->setSupportsSelectiveSync( false );

  // Create a project with photos and mergin-config
  QString projectName = "testSelectiveSyncChangeSyncFolder";

  QString projectClient1 = mApi->projectsPath() + "/" + projectName;
  QString projectClient2 = mApiExtra->projectsPath() + "/" + projectName;
  QString projectServer = serverMirror->projectsPath() + "/" + projectName;

  createRemoteProject( mApi, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Create photo files
  QDir dir;
  QString photoPathClient1( projectClient1 + "/" + "photos" );
  if ( !dir.exists( photoPathClient1 ) )
    dir.mkpath( photoPathClient1 );

  QFile file( photoPathClient1 + "/" + "photoC1-A.jpg" );
  file.open( QIODevice::WriteOnly );
  file.close();

  QFile file1( photoPathClient1 + "/" + "photoC1-B.png" );
  file1.open( QIODevice::WriteOnly );
  file1.close();

  uploadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  QString configFilePath = projectServer + "/" + "mergin-config.json";
  QVERIFY( createJsonFile( configFilePath,
  {
    { "input-selective-sync", true },
    { "input-selective-sync-dir", "" }
  } ) );

  uploadRemoteProject( serverMirror, mUsername, projectName );
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  // client 2 adds photos to project root, client 1 to photos subfolder
  QString photoPathClient2( projectClient2 );

  QFile fileExtra( photoPathClient2 + "/" + "photoC1-A.jpg" );
  QVERIFY( !fileExtra.exists() );

  QFile fileExtra1( photoPathClient2 + "/" + "photoC1-B.png" );
  QVERIFY( !fileExtra1.exists() );

  QDir photoDirExtra( photoPathClient2 );
  if ( !photoDirExtra.exists() )
    photoDirExtra.mkpath( photoPathClient2 );

  // simulate some traffic, let both clients create few photos several times (so that project has longer history)
  for ( int i : { 1, 2, 3, 4, 5 } )
  {
    QFile f1( photoPathClient1 + "/" + QString( "photoC1-%1.png" ).arg( i ) );
    f1.open( QIODevice::WriteOnly );
    f1.close();

    QFile f2( photoPathClient2 + "/" + QString( "photoC2-%1.png" ).arg( i ) );
    f2.open( QIODevice::WriteOnly );
    f2.close();

    uploadRemoteProject( mApiExtra, mUsername, projectName );
    uploadRemoteProject( mApi, mUsername, projectName );
  }

  downloadRemoteProject( serverMirror, mUsername, projectName );

  // Let's change selective sync folder only to photos subfolder
  InputUtils::removeFile( configFilePath );
  QVERIFY( !InputUtils::fileExists( configFilePath ) );

  QVERIFY( createJsonFile( configFilePath,
  {
    { "input-selective-sync", true },
    { "input-selective-sync-dir", "photos" }
  } ) );

  // Sync changed config
  uploadRemoteProject( serverMirror, mUsername, projectName );

  // Client 1 should now download all missing files from project root directory
  downloadRemoteProject( mApi, mUsername, projectName );

  QFile fextra( photoPathClient2 + "/" + "photoC2-extra.png" );
  fextra.open( QIODevice::WriteOnly );
  fextra.close();

  uploadRemoteProject( mApiExtra, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  /*
   * Check that:
   *  1) Client 1 have all photos created by Client 2 (in project root)
   *  2) Client 2 do not have any photo in "photos" subfolder
   *  3) All photos are on the server mirror
   */

  QString serverMirrorProjectPath = serverMirrorDataPath + "/" + projectName;

  QStringList photosInRoot, photosInSubfolder;
  photosInRoot << "photoC2-1.png" << "photoC2-5.png" << "photoC2-extra.png";
  photosInSubfolder << "photoC1-A.jpg" << "photoC1-B.png" << "photoC1-1.png" << "photoC1-5.png";

  for ( const QString &photo : photosInRoot )
  {
    QFile f1( projectClient1 + "/" + photo );
    QFile f2( serverMirrorProjectPath + "/" + photo );

    QVERIFY( f1.exists() );
    QVERIFY( f2.exists() );
  }

  for ( const QString &photo : photosInSubfolder )
  {
    QFile f1( serverMirrorProjectPath + "/" + "photos" + "/" + photo );
    QFile f2( photoPathClient2 + "/" + "photos" + "/" + photo );

    QVERIFY( f1.exists() );
    QVERIFY( !f2.exists() );
  }

  // change sync folder back to project root to see if photos in root will no longer be downloaded for Client 1
  InputUtils::removeFile( configFilePath );
  QVERIFY( !InputUtils::fileExists( configFilePath ) );

  QVERIFY( createJsonFile( configFilePath,
  {
    { "input-selective-sync", true },
    { "input-selective-sync-dir", "" }
  } ) );

  uploadRemoteProject( serverMirror, mUsername, projectName );

  QFile f( photoPathClient2 + "/" + "photoC2-should-not-download.png" );
  f.open( QIODevice::WriteOnly );
  f.close();

  uploadRemoteProject( mApiExtra, mUsername, projectName );
  downloadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  // File should be on server mirror and should not be on client 1
  QFile fverify( serverMirrorProjectPath + "/" + "photoC2-should-not-download.png" );
  QVERIFY( fverify.exists() );

  QFile fverify2( projectClient1 + "/" + "photoC2-should-not-download.png" );
  QVERIFY( !fverify2.exists() );

  delete serverMirror;
  delete serverMirrorProjects;
}

void TestMerginApi::testSelectiveSyncCorruptedFormat()
{

  /*
   * Case: Test what happens when someone uploads not valid config file (not valid json)
   *
   * We will create another API client that will serve as a server mirror, it will not use selective sync,
   * but will simulate as if someone manipulated project from browser via Mergin
   */
  QString serverMirrorDataPath = mApi->projectsPath() + "/" + "serverMirror";
  QDir serverMirrorDataDir( serverMirrorDataPath );
  if ( !serverMirrorDataDir.exists() )
    serverMirrorDataDir.mkpath( serverMirrorDataPath );

  LocalProjectsManager *serverMirrorProjects = new LocalProjectsManager( serverMirrorDataPath + "/" );
  MerginApi *serverMirror = new MerginApi( *serverMirrorProjects, this );
  serverMirror->setSupportsSelectiveSync( false );

  // Create a project with photos and mergin-config
  QString projectName = "testSelectiveSyncCorruptedFormat";

  QString projectClient1 = mApi->projectsPath() + "/" + projectName;
  QString projectClient2 = mApiExtra->projectsPath() + "/" + projectName;
  QString projectServer = serverMirror->projectsPath() + "/" + projectName;

  createRemoteProject( mApi, mUsername, projectName, mTestDataPath + "/" + TEST_PROJECT_NAME + "/" );
  downloadRemoteProject( mApi, mUsername, projectName );

  // Create photo files
  QDir dir;
  QString photoPathClient1( projectClient1 + "/" + "photos" );
  if ( !dir.exists( photoPathClient1 ) )
    dir.mkpath( photoPathClient1 );

  QFile file( photoPathClient1 + "/" + "photoC1-A.jpg" );
  file.open( QIODevice::WriteOnly );
  file.close();

  QFile file1( photoPathClient1 + "/" + "photoC1-B.png" );
  file1.open( QIODevice::WriteOnly );
  file1.close();

  uploadRemoteProject( mApi, mUsername, projectName );
  downloadRemoteProject( serverMirror, mUsername, projectName );

  // add corrupted config file
  QString configFilePath = projectServer + "/" + "mergin-config.json";
  QVERIFY( QFile::copy( mTestDataPath + "/mergin-config-corrupted.json", configFilePath ) );

  uploadRemoteProject( serverMirror, mUsername, projectName );
  downloadRemoteProject( mApiExtra, mUsername, projectName );

  // client 2 should have all photos from client 1
  QString photoPathClient2( projectClient2 + "/" + "photos" );

  QFile fileExtra( photoPathClient2 + "/" + "photoC1-A.jpg" );
  QVERIFY( fileExtra.exists() );

  QFile fileExtra1( photoPathClient2 + "/" + "photoC1-B.png" );
  QVERIFY( fileExtra1.exists() );
}

void TestMerginApi::testRegister()
{
  QString password = mApi->userAuth()->password();

  // we do not have a method to delete existing user in the mApi, so for now just make sure
  // the name does not exists
  QString quiteRandom = CoreUtils::uuidWithoutBraces( QUuid::createUuid() ).right( 15 ).replace( "-", "" );
  QString username = "test_" + quiteRandom;
  QString email = username + "@nonexistant.email.com";

  qDebug() << "username:" << username;
  // do not want to be authorized
  mApi->clearAuth();

  QSignalSpy spy( mApi,  &MerginApi::registrationSucceeded );
  mApi->registerUser( username, email, password, password, true );
  QVERIFY( spy.wait( TestUtils::LONG_REPLY ) );
}

void TestMerginApi::testExcludeFromSync()
{
  // Set selective sync directory
  QString selectiveSyncDir( mApi->projectsPath() + "/testExcludeFromSync" );

  QList<QString> testFiles =
  {
    selectiveSyncDir + "/data.gpkg",
    selectiveSyncDir + "/image.png",
    selectiveSyncDir + "/image.jpg",
    selectiveSyncDir + "/image.HEIF",
    selectiveSyncDir + "/subdir/image.jpg"
  };

  for ( QString path : testFiles )
  {
    QFile file( path );
    file.open( QIODevice::WriteOnly );
  }

  MerginConfig config;
  config.selectiveSyncEnabled = true;
  config.selectiveSyncDir = selectiveSyncDir;
  config.isValid = true;

  QVERIFY( !mApi->excludeFromSync( selectiveSyncDir, config ) );
  QVERIFY( !mApi->excludeFromSync( selectiveSyncDir + "/data.gpkg", config ) );
  QVERIFY( !mApi->excludeFromSync( selectiveSyncDir + "/not-existing.file", config ) );

  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/not-existing.jpg", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/image.png", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/image.PNG", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/image.jpg", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/image.JPG", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/image.jpeg", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/image.JPEG", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/subdir/image.jpg", config ) );

  config.selectiveSyncDir = selectiveSyncDir + "/subdir";
  QVERIFY( !mApi->excludeFromSync( selectiveSyncDir + "/image.jpg", config ) );
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/subdir/image.jpg", config ) );

  config.selectiveSyncDir.clear();
  QVERIFY( mApi->excludeFromSync( selectiveSyncDir + "/image.jpg", config ) );
}

//////// HELPER FUNCTIONS ////////

MerginProjectsList TestMerginApi::getProjectList( QString tag )
{
  QSignalSpy spy( mApi,  &MerginApi::listProjectsFinished );
  mApi->listProjects( QString(), tag, QString() );
  spy.wait( TestUtils::SHORT_REPLY );

  return projectListFromSpy( spy );
}

MerginProjectsList TestMerginApi::projectListFromSpy( QSignalSpy &spy )
{
  QList<QVariant> response = spy.takeFirst();

  // get projects emited from MerginAPI, it is first argument in listProjectsFinished signal
  MerginProjectsList projects;
  if ( response.length() > 0 )
    projects = qvariant_cast<MerginProjectsList>( response.at( 0 ) );

  return projects;
}

int TestMerginApi::serverVersionFromSpy( QSignalSpy &spy )
{
  QList<QVariant> response = spy.takeFirst();

  // get version number emited from MerginApi::syncProjectFinished, it is third argument
  int serverVersion = -1;
  if ( response.length() >= 4 )
    serverVersion = response.at( 3 ).toInt();

  return serverVersion;
}

void TestMerginApi::deleteRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  QSignalSpy spy( api, &MerginApi::serverProjectDeleted );
  api->deleteProject( projectNamespace, projectName );
  spy.wait( TestUtils::SHORT_REPLY );
}

void TestMerginApi::deleteLocalProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  LocalProject project = api->getLocalProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
  QVERIFY( project.isValid() );
  QVERIFY( project.projectDir.startsWith( api->projectsPath() ) );  // just to make sure we don't delete something wrong (-:

  api->localProjectsManager().removeLocalProject( project.id() );
}

void TestMerginApi::deleteLocalDir( MerginApi *api, const QString &dirPath )
{
  QDir dir( api->projectsPath() + "/" + dirPath );
  QVERIFY( dir.removeRecursively() );
}

void TestMerginApi::downloadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  int serverVersion;
  downloadRemoteProject( api, projectNamespace, projectName, serverVersion );
}

void TestMerginApi::downloadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, int &serverVersion )
{
  QSignalSpy spy( api, &MerginApi::syncProjectFinished );
  api->updateProject( projectNamespace, projectName );
  QCOMPARE( api->transactions().count(), 1 );
  QVERIFY( spy.wait( TestUtils::LONG_REPLY * 5 ) );
  serverVersion = serverVersionFromSpy( spy );
}

void TestMerginApi::uploadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName )
{
  int serverVersion;
  uploadRemoteProject( api, projectNamespace, projectName, serverVersion );
}

void TestMerginApi::uploadRemoteProject( MerginApi *api, const QString &projectNamespace, const QString &projectName, int &serverVersion )
{
  api->uploadProject( projectNamespace, projectName );
  QSignalSpy spy( api, &MerginApi::syncProjectFinished );
  QVERIFY( spy.wait( TestUtils::LONG_REPLY * 30 ) );
  QCOMPARE( spy.count(), 1 );
  serverVersion = serverVersionFromSpy( spy );
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
  Q_ASSERT( f.exists() );
  Q_ASSERT( f.open( QIODevice::ReadOnly ) );
  QByteArray data = f.readAll();
  f.close();
  return data;
}

void TestMerginApi::createLocalProject( const QString projectDir )
{
  QDir().mkdir( projectDir );
  bool r0 = QFile::copy( mTestDataPath + "/diff_project/base.gpkg", projectDir + "/base.gpkg" );

  QVERIFY( r0 );
}

bool TestMerginApi::createJsonFile( const QString &path, const QVariantMap &params )
{
  QJsonObject json = QJsonObject::fromVariantMap( params );
  QJsonDocument doc( json );
  QByteArray data = doc.toJson();

  writeFileContent( path, data );

  QFile config( path );
  return config.exists();
}

void TestMerginApi::refreshProjectsModel( const ProjectsModel::ProjectModelTypes modelType )
{

  if ( modelType == ProjectsModel::LocalProjectsModel )
  {
    QSignalSpy spy( mApi, &MerginApi::listProjectsByNameFinished );
    mLocalProjectsModel->listProjects();
    QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
    QCOMPARE( spy.count(), 1 );
  }
  else if ( modelType == ProjectsModel::CreatedProjectsModel )
  {
    QSignalSpy spy( mApi, &MerginApi::listProjectsFinished );
    mCreatedProjectsModel->listProjects();
    QVERIFY( spy.wait( TestUtils::SHORT_REPLY ) );
    QCOMPARE( spy.count(), 1 );
  }
}
