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
    { "/home/test/geo.gpkg", "jack", 10, "/home/test/geo_copy_conflict_jack_v10.gpkg" },
    { "/home/test/g.pkg", "j", 0, "/home/test/g_copy_conflict_j_v0.pkg" },
    { "home/test/geo.gpkg", "jack", 10, "home/test/geo_copy_conflict_jack_v10.gpkg" },
    { "geo.gpkg", "jack", 10, "./geo_copy_conflict_jack_v10.gpkg" },
    { "/home/../geo.gpkg", "jack", 10, "/home/../geo_copy_conflict_jack_v10.gpkg" },
    { "/home/./geo.gpkg", "jack", 10, "/home/./geo_copy_conflict_jack_v10.gpkg" },
    { "/home/test/geo.gpkg", "", 10, "/home/test/geo_copy_conflict__v10.gpkg" },
    { "/home/test/geo.gpkg", "jack", -1, "/home/test/geo_copy_conflict_jack_v-1.gpkg" },
    { "/home/test/geo.tar.gz", "jack", -1, "/home/test/geo_copy_conflict_jack_v-1.tar.gz" },
    { "", "jack", 1, "" }
  };

  for ( const auto &c : testcasesCopyConflicts )
  {
    QString actualName = CoreUtils::generateCopyConflictFileName( c.filename, c.username, c.version );
    QCOMPARE( actualName, c.expectedName );
  }

  QVector<testcase> testcasesEditConflicts =
  {
    { "/home/test/geo.gpkg", "jack", 10, "/home/test/geo_edit_conflict_jack_v10.gpkg" },
    { "/home/test/g.pkg", "j", 0, "/home/test/g_edit_conflict_j_v0.pkg" },
    { "home/test/geo.gpkg", "jack", 10, "home/test/geo_edit_conflict_jack_v10.gpkg" },
    { "geo.gpkg", "jack", 10, "./geo_edit_conflict_jack_v10.gpkg" },
    { "/home/../geo.gpkg", "jack", 10, "/home/../geo_edit_conflict_jack_v10.gpkg" },
    { "/home/./geo.gpkg", "jack", 10, "/home/./geo_edit_conflict_jack_v10.gpkg" },
    { "/home/test/geo.gpkg", "", 10, "/home/test/geo_edit_conflict__v10.gpkg" },
    { "/home/test/geo.gpkg", "jack", -1, "/home/test/geo_edit_conflict_jack_v-1.gpkg" },
    { "/home/test/geo.tar.gz", "jack", -1, "/home/test/geo_edit_conflict_jack_v-1.tar.gz" },
    { "", "jack", 1, "" }
  };

  for ( const auto &c : testcasesEditConflicts )
  {
    QString actualName = CoreUtils::generateEditConflictFileName( c.filename, c.username, c.version );
    QCOMPARE( actualName, c.expectedName );
  }
}
