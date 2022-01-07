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
    { "", "jack", 1, "" }
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
