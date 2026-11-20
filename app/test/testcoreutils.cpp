/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testcoreutils.h"
#include "coreutils.h"
#include "testutils.h"
#include "geodiffutils.h"

#include <QtTest/QtTest>

#include <qgsvectorlayer.h>

void TestCoreUtils::init()
{
}

void TestCoreUtils::cleanup()
{
}

void TestCoreUtils::testConflictFileNames()
{
  struct testcase
  {
    QString filename;
    QString username;
    int version;
    QString expectedName;
  };

  QVector<testcase> testcasesCopyConflicts =
  {
    { "/home/test/geo.gpkg", "jack", 10, "/home/test/geo (conflicted copy, jack v10).gpkg" },
    { "/home/test/g.pkg", "j", 0, "/home/test/g (conflicted copy, j v0).pkg" },
    { "home/test/geo.gpkg", "jack", 10, "home/test/geo (conflicted copy, jack v10).gpkg" },
    { "geo.gpkg", "jack", 10, "./geo (conflicted copy, jack v10).gpkg" },
    { "/home/../geo.gpkg", "jack", 10, "/home/../geo (conflicted copy, jack v10).gpkg" },
    { "/home/./geo.gpkg", "jack", 10, "/home/./geo (conflicted copy, jack v10).gpkg" },
    { "/home/test/geo.gpkg", "", 10, "/home/test/geo (conflicted copy,  v10).gpkg" },
    { "/home/test/geo.gpkg", "jack", -1, "/home/test/geo (conflicted copy, jack v-1).gpkg" },
    { "/home/test/geo.tar.gz", "jack", 100, "/home/test/geo (conflicted copy, jack v100).tar.gz" },
    { "", "jack", 1, "" },
    { "/home/test/survey.qgs", "jack", 10, "/home/test/survey (conflicted copy, jack v10).qgs~" },
    { "/home/test/survey.QGZ", "jack", 10, "/home/test/survey (conflicted copy, jack v10).QGZ~" }
  };

  for ( const auto &c : testcasesCopyConflicts )
  {
    QString actualName = CoreUtils::generateConflictedCopyFileName( c.filename, c.username, c.version );
    QCOMPARE( actualName, c.expectedName );
  }

  QVector<testcase> testcasesEditConflicts =
  {
    { "/home/test/geo.json", "jack", 10, "/home/test/geo (edit conflict, jack v10).json" },
    { "/home/test/g.jsn", "j", 0, "/home/test/g (edit conflict, j v0).json" },
    { "home/test/geo.json", "jack", 10, "home/test/geo (edit conflict, jack v10).json" },
    { "geo.json", "jack", 10, "./geo (edit conflict, jack v10).json" },
    { "/home/../geo.json", "jack", 10, "/home/../geo (edit conflict, jack v10).json" },
    { "/home/./geo.json", "jack", 10, "/home/./geo (edit conflict, jack v10).json" },
    { "/home/test/geo.json", "", 10, "/home/test/geo (edit conflict,  v10).json" },
    { "/home/test/geo.json", "jack", -1, "/home/test/geo (edit conflict, jack v-1).json" },
    { "/home/test/geo.gpkg", "jack", 10, "/home/test/geo (edit conflict, jack v10).json" },
    { "/home/test/geo.tar.gz", "jack", 100, "/home/test/geo (edit conflict, jack v100).json" },
    { "", "jack", 1, "" }
  };

  for ( const auto &c : testcasesEditConflicts )
  {
    QString actualName = CoreUtils::generateEditConflictFileName( c.filename, c.username, c.version );
    QCOMPARE( actualName, c.expectedName );
  }
}

void TestCoreUtils::testFindUniquePath()
{
  QString projectPath = QDir::tempPath() + QStringLiteral( "/testFindUniquePath" );

  QDir projectDir( projectPath );
  if ( projectDir.exists() )
    projectDir.removeRecursively();

  projectDir.mkpath( projectPath );

  QVERIFY( projectDir.exists() );
  QVERIFY( projectDir.entryList( QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs ).isEmpty() );

  // each object is a folder, "files" lists the files created in it
  const QString structure = R"json({
    "folderA": {
      "files": [ "fileA.txt", "fileA (1).txt", "fileB.txt", "dotted.name.txt" ],
      "folderAB": {},
      "folderAB (1)": {}
    },
    "folder.with.dots": {
      "files": [ "file.txt" ]
    },
    "dir.txt": {},
    "files": [
      "file.txt",
      "another.txt",
      "another (1).txt",
      "another (2).txt",
      "arch.tar.bz2",
      "my.archive.tar.bz2",
      "image_my.notes.jpg",
      "image_my.notes (1).jpg",
      "photo_2024.01.05.jpg",
      "IMG.JPG",
      "my photo.jpg",
      "gap.txt",
      "gap (2).txt",
      "data (conflicted copy, user v2).gpkg",
      "README",
      ".gitignore"
    ]
  })json";

  QVERIFY( TestUtils::generateProjectFolder( projectPath, QJsonDocument::fromJson( structure.toUtf8() ) ) );

  struct testcase
  {
    QString path;
    QString expectedOutput;
  };

  QVector<testcase> testcases =
  {
    { "file.txt", "file (1).txt" },
    { "another.txt", "another (3).txt" },
    { "folderA", "folderA (1)" },
    { "non.txt", "non.txt" },
    { "data.gpkg", "data.gpkg" },
    { "arch.tar.bz2", "arch (1).tar.bz2" },
    { "my.archive.tar.bz2", "my.archive (1).tar.bz2" },
    { "image_my.notes.jpg", "image_my.notes (2).jpg" },
    { "image_my.other.jpg", "image_my.other.jpg" },
    { "folder.with.dots", "folder.with.dots (1)" },
    { "folder.with.dots/file.txt", "folder.with.dots/file (1).txt" },
    { "dir.txt", "dir.txt (1)" },
    { "photo_2024.01.05.jpg", "photo_2024.01.05 (1).jpg" },
    { "IMG.JPG", "IMG (1).JPG" },
    { "my photo.jpg", "my photo (1).jpg" },
    { "gap.txt", "gap (1).txt" },
    { "another (1).txt", "another (1) (1).txt" },
    { "data (conflicted copy, user v2).gpkg", "data (conflicted copy, user v2) (1).gpkg" },
    { "README", "README (1)" },
    { ".gitignore", ".gitignore (1)" },
    { "folderA/folder", "folderA/folder" },
    { "folderA/fileA.txt", "folderA/fileA (2).txt" },
    { "folderA/fileB.txt", "folderA/fileB (1).txt" },
    { "folderA/fileC.txt", "folderA/fileC.txt" },
    { "folderA/folderAB", "folderA/folderAB (2)" },
    { "folderA/dotted.name.txt", "folderA/dotted.name (1).txt" },
  };

  for ( const auto &c : testcases )
  {
    qInfo() << QStringLiteral( "Testing path %1" ).arg( c.path );

    QString foundPath = CoreUtils::findUniquePath( projectPath + "/" + c.path );
    QCOMPARE( foundPath, projectPath + "/" + c.expectedOutput );
  }
}

void TestCoreUtils::testHasProjectFileExtension()
{
  QVERIFY( CoreUtils::hasProjectFileExtension( QStringLiteral( "project.qgs" ) ) );
  QVERIFY( CoreUtils::hasProjectFileExtension( QStringLiteral( "project.QGZ" ) ) );
  QVERIFY( !CoreUtils::hasProjectFileExtension( QStringLiteral( "project.qg" ) ) );
}

void TestCoreUtils::testNameValidation()
{
  QVector< QPair< QString, bool > > testcases =
  {
    { QStringLiteral( "project" ), true },
    { QStringLiteral( "ProJect" ), true },
    { QStringLiteral( "Pro123ject" ), true },
    { QStringLiteral( "123PROJECT" ), true },
    { QStringLiteral( "PROJECT" ), true },
    { QStringLiteral( "project " ), true },
    { QStringLiteral( "pro ject" ), true },
    { QStringLiteral( "proj-ect" ), true },
    { QStringLiteral( "-project" ), true },
    { QStringLiteral( "proj_ect" ), true },
    { QStringLiteral( "proj.ect" ), true },
    { QStringLiteral( "proj!ect" ), false },

    { QStringLiteral( " project" ), false },
    { QStringLiteral( ".project" ), false },
    { QStringLiteral( "proj~ect" ), false },
    { QStringLiteral( "pro\\ject" ), false },
    { QStringLiteral( "pro/ject" ), false },
    { QStringLiteral( "pro|ject" ), false },
    { QStringLiteral( "pro+ject" ), false },
    { QStringLiteral( "pro=ject" ), false },
    { QStringLiteral( "pro>ject" ), false },
    { QStringLiteral( "pro<ject" ), false },
    { QStringLiteral( "pro@ject" ), false },
    { QStringLiteral( "pro#ject" ), false },
    { QStringLiteral( "pro$ject" ), false },
    { QStringLiteral( "pro%ject" ), false },
    { QStringLiteral( "pro^ject" ), false },
    { QStringLiteral( "pro&ject" ), false },
    { QStringLiteral( "pro*ject" ), false },
    { QStringLiteral( "pro?ject" ), false },
    { QStringLiteral( "pro:ject" ), false },
    { QStringLiteral( "pro;ject" ), false },
    { QStringLiteral( "pro,ject" ), false },
    { QStringLiteral( "pro`ject" ), false },
    { QStringLiteral( "pro'ject" ), false },
    { QStringLiteral( "pro\"ject" ), false },
    { QStringLiteral( "projectz" ), true },
    { QStringLiteral( "projectZ" ), true },
    { QStringLiteral( "project0" ), true },

    { QStringLiteral( "pro(ject" ), false },
    { QStringLiteral( "pro)ject" ), false },
    { QStringLiteral( "pro{ject" ), false },
    { QStringLiteral( "pro}ject" ), false },
    { QStringLiteral( "pro[ject" ), false },
    { QStringLiteral( "pro]ject" ), false },
    { QStringLiteral( "pro]ject" ), false },

    { QStringLiteral( "CON" ), false },
    { QStringLiteral( "NUL" ), false },
    { QStringLiteral( "NULL" ), true },
    { QStringLiteral( "PRN" ), false },
    { QStringLiteral( "LPT0" ), false },
    { QStringLiteral( "lpt0" ), false },
    { QStringLiteral( "LPT1" ), false },
    { QStringLiteral( "lpt1" ), false },
    { QStringLiteral( "COM1" ), false },
    { QStringLiteral( "com1" ), false },
    { QStringLiteral( "AUX" ), false },
    { QStringLiteral( "AuX" ), false },
    { QStringLiteral( "projAUXect" ), true },
    { QStringLiteral( "CONproject" ), true },
    { QStringLiteral( "projectCON" ), true },
    { QStringLiteral( "項目" ), true },
    { QStringLiteral( "פּרוֹיֶקט" ), true },

    { QStringLiteral( "support" ), false },
    { QStringLiteral( "helpdesk" ), false },
    { QStringLiteral( "input" ), false },
    { QStringLiteral( "lutraconsulting" ), false },
    { QStringLiteral( "lutra" ), false },
    { QStringLiteral( "merginmaps" ), false },
    { QStringLiteral( "mergin" ), false },
    { QStringLiteral( "sales" ), false },
    { QStringLiteral( "admin" ), false },
  };

  for ( const auto &test : testcases )
  {
    QCOMPARE( CoreUtils::isValidName( test.first ), test.second );
  }
}

void TestCoreUtils::testReservedName()
{
  QVector< QPair< QString, bool > > testcases =
  {
    { QStringLiteral( "admin" ), true },
    { QStringLiteral( "ADMIN" ), true },
    { QStringLiteral( "mergin" ), true },
    { QStringLiteral( "CON" ), true },
    { QStringLiteral( "com1" ), true },
    { QStringLiteral( "administrator" ), false },
    { QStringLiteral( "my admin" ), false },
    { QStringLiteral( "project" ), false },
    { QStringLiteral( "pro/ject" ), false },
  };

  for ( const auto &test : testcases )
  {
    QCOMPARE( CoreUtils::isReservedName( test.first ), test.second );
  }
}

void TestCoreUtils::testNameAbbr()
{
  QVector< QPair< QString, QString > > testcases =
  {
    { QStringLiteral( "" ), QStringLiteral( "" ) },
    { QStringLiteral( "Chuck Brave|chuck@example.com" ), QStringLiteral( "CB" ) },
    { QStringLiteral( "Chuck Norris|chuck@example.com" ), QStringLiteral( "CN" ) },
    { QStringLiteral( "Chuck Norris Carlos|chuck@example.com" ), QStringLiteral( "CC" ) },
    { QStringLiteral( "Chuck|chuck@example.com" ), QStringLiteral( "CH" ) },
    { QStringLiteral( "|chuck@example.com" ), QStringLiteral( "CH" ) },
    { QStringLiteral( "Chuck|" ), QStringLiteral( "CH" ) },
    { QStringLiteral( "C|" ), QStringLiteral( "C" ) },
  };

  for ( const auto &test : testcases )
  {
    QStringList nameAndEmail = test.first.split( '|' );
    QString name = nameAndEmail.size() > 0 ? nameAndEmail.at( 0 ) : "";
    QString email = nameAndEmail.size() > 1 ? nameAndEmail.at( 1 ) : "";

    QCOMPARE( CoreUtils::nameAbbr( name, email ), test.second );
  }
}

void TestCoreUtils::testReplaceValueInJson()
{
  // temporary test file
  QString testFilePath = QDir::tempPath() + "/test_replace_value.json";

  // basic replacement in valid JSON with int value
  {
    QFile file( testFilePath );
    QVERIFY( file.open( QIODevice::WriteOnly ) );
    file.write( R"({"name": "test", "value": 123})" );
    file.close();

    QVERIFY( CoreUtils::replaceValueInJson( testFilePath, "value", 456 ) );

    // verify
    QVERIFY( file.open( QIODevice::ReadOnly ) );
    QJsonDocument doc = QJsonDocument::fromJson( file.readAll() );
    file.close();
    QVERIFY( doc.isObject() );
    QJsonObject obj = doc.object();
    QCOMPARE( obj["value"].toInt(), 456 );
    QCOMPARE( obj["name"].toString(), QString( "test" ) );
  }
  // valid JSON with string value
  {
    QFile file( testFilePath );
    QVERIFY( file.open( QIODevice::WriteOnly ) );
    file.write( R"({"name": "test", "status": "active"})" );
    file.close();

    QVERIFY( CoreUtils::replaceValueInJson( testFilePath, "status", "inactive" ) );

    // verify replacement
    QVERIFY( file.open( QIODevice::ReadOnly ) );
    QJsonDocument doc = QJsonDocument::fromJson( file.readAll() );
    file.close();
    QVERIFY( doc.isObject() );
    QJsonObject obj = doc.object();
    QCOMPARE( obj["status"].toString(), QString( "inactive" ) );
    QCOMPARE( obj["name"].toString(), QString( "test" ) );
  }

  // add new key-value pair
  {
    QFile file( testFilePath );
    QVERIFY( file.open( QIODevice::WriteOnly ) );
    file.write( R"({"name": "test"})" );
    file.close();

    QVERIFY( CoreUtils::replaceValueInJson( testFilePath, "newKey", "newValue" ) );

    // verify the addition
    QVERIFY( file.open( QIODevice::ReadOnly ) );
    QJsonDocument doc = QJsonDocument::fromJson( file.readAll() );
    file.close();
    QVERIFY( doc.isObject() );
    QJsonObject obj = doc.object();
    QCOMPARE( obj["newKey"].toString(), QString( "newValue" ) );
    QCOMPARE( obj["name"].toString(), QString( "test" ) );
  }

  // invalid JSON file
  {
    QFile file( testFilePath );
    QVERIFY( file.open( QIODevice::WriteOnly ) );
    file.write( "invalid json content" );
    file.close();

    QVERIFY( !CoreUtils::replaceValueInJson( testFilePath, "key", "value" ) );
  }

  // non-existent file
  {
    QString nonExistentPath = QDir::tempPath() + "/non_existent.json";
    QVERIFY( !CoreUtils::replaceValueInJson( nonExistentPath, "key", "value" ) );
  }

  QFile::remove( testFilePath );
}

void TestCoreUtils::testCopyWalAware()
{
  // a copy of a gpkg in WAL mode includes the data still in the -wal file and is itself in rollback mode
  // (header bytes 18-19 == 1), without any -wal/-shm files

  const QString dir = QDir::tempPath() + QStringLiteral( "/testCopyWalAware" );
  QDir( dir ).removeRecursively();
  QVERIFY( QDir().mkpath( dir ) );

  auto headerVersions = []( const QString & path )
  {
    QFile f( path );
    if ( !f.open( QIODevice::ReadOnly ) )
      return QByteArray();
    return f.read( 20 ).mid( 18, 2 );
  };

  const QString src = dir + QStringLiteral( "/src.gpkg" );
  QVERIFY( QFile::copy( TestUtils::testDataDir() + QStringLiteral( "/diff_project/base.gpkg" ), src ) );
  QFile::setPermissions( src, QFile::ReadOwner | QFile::WriteOwner );

  QgsVectorLayer *vl = new QgsVectorLayer( src + QStringLiteral( "|layername=simple" ), QStringLiteral( "src" ), QStringLiteral( "ogr" ) );
  QVERIFY( vl->isValid() );
  const long featureCount = vl->featureCount();
  vl->startEditing();
  QgsFeature f( vl->fields() );
  vl->addFeature( f );
  // save but don't stop editing so -wal file is preserved. The added feature now lives in the -wal file
  vl->commitChanges( false );
  QVERIFY( QFile::exists( src + QStringLiteral( "-wal" ) ) );
  QCOMPARE( headerVersions( src ), QByteArray( 2, 2 ) );

  // copy while the source is still open with data in its -wal file
  const QString dst = dir + QStringLiteral( "/dst.gpkg" );
  QVERIFY( GeodiffUtils::copyWalAware( src, dst ) );
  delete vl;

  QCOMPARE( headerVersions( dst ), QByteArray( 2, 1 ) );
  QVERIFY( !QFile::exists( dst + QStringLiteral( "-wal" ) ) );
  QVERIFY( !QFile::exists( dst + QStringLiteral( "-shm" ) ) );

  QgsVectorLayer *vlCopy = new QgsVectorLayer( dst + QStringLiteral( "|layername=simple" ), QStringLiteral( "dst" ), QStringLiteral( "ogr" ) );
  QVERIFY( vlCopy->isValid() );
  QCOMPARE( vlCopy->featureCount(), featureCount + 1 );
  delete vlCopy;

  // a copy of a gpkg in rollback mode stays in rollback mode
  const QString dst2 = dir + QStringLiteral( "/dst2.gpkg" );
  QCOMPARE( headerVersions( TestUtils::testDataDir() + QStringLiteral( "/diff_project/base.gpkg" ) ), QByteArray( 2, 1 ) );
  QVERIFY( GeodiffUtils::copyWalAware( TestUtils::testDataDir() + QStringLiteral( "/diff_project/base.gpkg" ), dst2 ) );
  QCOMPARE( headerVersions( dst2 ), QByteArray( 2, 1 ) );

  // non-gpkg files are copied as they are
  const QString txtSrc = dir + QStringLiteral( "/file.txt" );
  QFile txt( txtSrc );
  QVERIFY( txt.open( QIODevice::WriteOnly ) );
  txt.write( "not a database" );
  txt.close();
  QVERIFY( GeodiffUtils::copyWalAware( txtSrc, dir + QStringLiteral( "/file2.txt" ) ) );
  QCOMPARE( CoreUtils::calculateChecksum( dir + QStringLiteral( "/file2.txt" ) ), CoreUtils::calculateChecksum( txtSrc ) );

  QDir( dir ).removeRecursively();
}
