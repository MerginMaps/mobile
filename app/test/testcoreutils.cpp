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

#include <QtTest/QtTest>

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

  //
  // Create test files in the following structure:
  //
  // - folderA
  //   |- fileA.txt
  //   |- fileA (1).txt
  //   |- fileB.txt
  //   |- folderAB
  //   |- folderAB (1)
  //
  // - file.txt
  // - another.txt
  // - another (1).txt
  // - another (2).txt
  // - arch.tar.gz
  //

  QString structure = "{"
                      " \"folderA\": {"
                      "  \"files\": ["
                      "   \"fileA.txt\", "
                      "   \"fileA (1).txt\", "
                      "   \"fileB.txt\""
                      "  ],"
                      "  \"folderAB\": {},"
                      "  \"folderAB (1)\": {}"
                      " },"
                      " \"files\": [ "
                      "  \"file.txt\", "
                      "  \"another.txt\", "
                      "  \"another (1).txt\", "
                      "  \"another (2).txt\", "
                      "  \"arch.tar.gz\""
                      " ]"
                      "}";

  QVERIFY( TestUtils::generateProjectFolder( projectPath, QJsonDocument::fromJson( structure.toUtf8() ) ) );

  struct combination
  {
    QString path;
    QString expectedOutput;
  };

  QVector<combination> testcases =
  {
    { "file.txt", "file (1).txt" },
    { "another.txt", "another (3).txt" },
    { "folderA", "folderA (1)" },
    { "non.txt", "non.txt" },
    { "data.gpkg", "data.gpkg" },
    { "arch.tar.gz", "arch (1).tar.gz" },
    { "folderA/folder", "folderA/folder" },
    { "folderA/fileA.txt", "folderA/fileA (2).txt" },
    { "folderA/fileB.txt", "folderA/fileB (1).txt" },
    { "folderA/fileC.txt", "folderA/fileC.txt" },
    { "folderA/folderAB", "folderA/folderAB (2)" },
  };

  for ( const auto &c : testcases )
  {
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
    { QStringLiteral( "proj!ect" ), true },

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
