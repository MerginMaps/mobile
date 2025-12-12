#include "testauthimport.h"
#include "testutils.h"
#include <QTextStream>

TestAuthImport::TestAuthImport()
  : QObject( nullptr )
  , mPass( "1234" )
{
}

void TestAuthImport::initTestCase()
{
  if ( !QgsApplication::instance() )
  {
    static int argc = 1;
    static char *argv[] = { ( char * )"test_app", nullptr };
    new QgsApplication( argc, argv, true, "TestApp" );
  }

  // create a temporary folder
  mAppTempDir.reset( new QTemporaryDir() );
  QVERIFY( mAppTempDir->isValid() );
  qputenv( "QGIS_AUTH_DB_DIR_PATH", mAppTempDir->path().toLatin1() );

  // init QGIS
  QgsApplication::initQgis();

  // check that qca is correctly linked and the authentication system is not disabled
  QVERIFY2( !QgsApplication::authManager()->isDisabled(),
            "Authentication system is DISABLED. Check QCA/OpenSSL dependency deployment." );

  QgsAuthManager *authMngr = QgsApplication::authManager();
  QVERIFY( authMngr->setMasterPassword( QStringLiteral( "merginMaps" ), true ) );
  QVERIFY( authMngr->masterPasswordIsSet() );
}

void TestAuthImport::cleanupTestCase()
{
  QgsApplication::authManager()->clearMasterPassword();
  QgsApplication::authManager()->removeAllAuthenticationConfigs();
  mAppTempDir.reset();
  QgsApplication::exitQgis();
}

void TestAuthImport::cleanup()
{
  // Clear configs after each test to ensure isolation
  QgsApplication::authManager()->removeAllAuthenticationConfigs();
}

void TestAuthImport::mockProjectMetadata( const QString &projectDir )
{
  // method to create a mock .mergin/metadata.json file with TEST_PROJECT_ID
  QString metadataPath = CoreUtils::getProjectMetadataPath( projectDir );
  QDir().mkpath( QFileInfo( metadataPath ).path() );

  QFile file( metadataPath );
  if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
  {
    QTextStream stream( &file );
    stream << "{\"projectId\":\"" << TEST_PROJECT_ID << "\"}";
    file.close();
    QVERIFY( true );
  }
}

void TestAuthImport::copyTestAuthFile( const QString &projectDir )
{
  const QString sourcePath = TestUtils::testDataDir() + "/" + ( TEST_XML_FILENAME );
  const QString destPath = QDir( projectDir ).filePath( AUTH_CONFIG_FILENAME );

  QVERIFY2( QFile::exists( sourcePath ),
            qPrintable( QString( "Source XML file missing: %1" ).arg( sourcePath ) ) );

  // copy the pre-built, encrypted cfg XML file into the temporary project directory
  QVERIFY2( QFile::copy( sourcePath, destPath ),
            qPrintable( QString( "Failed to copy test XML from %1 to %2" ).arg( sourcePath, destPath ) ) );
}

void TestAuthImport::test_successful_of_authFile()
{
  // authentication DB config file is present. Should succeed.

  // setup project dir, mock metadata, and copy the encrypted XML file
  QTemporaryDir projectTempDir;
  const QString projectDir = projectTempDir.path();
  mockProjectMetadata( projectDir );
  copyTestAuthFile( projectDir );

  // initialise the QGIS authentication manager
  QgsAuthManager *authMngr = QgsApplication::authManager();
  QString authFile = QDir( projectDir ).filePath( AUTH_CONFIG_FILENAME );

  bool ok = authMngr->importAuthenticationConfigsFromXml( authFile, mPass, true );

  QVERIFY2( ok, "Importing auth config XML failed when Master Password was set." );
  int count = authMngr->configIds().count();
  QCOMPARE( count, 1 );
}

void TestAuthImport::test_import_fails_authFile_missing()
{
  // authentication DB config fileis missing, negative test case.

  // setup project dir and mock metadata (NO call to copyTestAuthFile)
  QTemporaryDir projectTempDir;
  const QString projectDir = projectTempDir.path();
  mockProjectMetadata( projectDir );

  // initialise the QGIS authentication manager
  QgsAuthManager *authMngr = QgsApplication::authManager();
  QString authFile = QDir( projectDir ).filePath( AUTH_CONFIG_FILENAME );

  bool ok = authMngr->importAuthenticationConfigsFromXml( authFile, mPass, true );

  // import must fail as the auth manager fails to open the file
  QVERIFY2( !ok, "Importing should have failed because the auth file was missing." );

  // no configs should have been loaded
  QCOMPARE( authMngr->configIds().count(), 0 );
}

void TestAuthImport::test_import_fails_master_password_not_set()
{
  // authentication DB config file is present, master Password is NOT set, import must fail

  // setup project dir, mock metadata, and copy the encrypted XML file
  QTemporaryDir projectTempDir;
  const QString projectDir = projectTempDir.path();
  mockProjectMetadata( projectDir );
  copyTestAuthFile( projectDir );

  // make sure that the master Password is NOT set
  QgsAuthManager *authMngr = QgsApplication::authManager();
  authMngr->clearMasterPassword();
  QVERIFY( !authMngr->masterPasswordIsSet() );

  QString authFile = QDir( projectDir ).filePath( AUTH_CONFIG_FILENAME );
  bool ok = authMngr->importAuthenticationConfigsFromXml( authFile, mPass, true );

  // the import method does not return false if no master password is set
  // however it does not load any configurations, thus the import failed
  QVERIFY2( ok, "Importing should have failed because the Master Password was not set." );
  QCOMPARE( authMngr->configIds().count(), 0 );

  // we set master password again so that the following test does not fail
  QVERIFY( authMngr->setMasterPassword( QStringLiteral( "merginMaps" ), true ) );
  QVERIFY( authMngr->masterPasswordIsSet() );
}

void TestAuthImport::test_reload_after_configuration_update()
{
  // import works, then reload works, as overwrite=true flag is effective, when the cfg xml file is updated

  // setup project dir, mock metadata, copy XML, and set Master Password
  QTemporaryDir projectTempDir;
  const QString projectDir = projectTempDir.path();
  mockProjectMetadata( projectDir );
  copyTestAuthFile( projectDir );

  QgsAuthManager *authMngr = QgsApplication::authManager();
  QString authFile = QDir( projectDir ).filePath( AUTH_CONFIG_FILENAME );

  QVERIFY( authMngr->importAuthenticationConfigsFromXml( authFile, mPass, true ) );
  QCOMPARE( authMngr->configIds().count(), 1 );

  // run the import call again, which happends after the configuration file has been resynchronised
  bool ok = authMngr->importAuthenticationConfigsFromXml( authFile, mPass, true );

  QVERIFY2( ok, "Reloading/overwriting the existing auth config failed." );

  // the number of configurations should remain 1 (it was overwritten/refreshed)
  int count = authMngr->configIds().count();
  QCOMPARE( count, 1 );
}