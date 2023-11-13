/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testprojectchecksumcache.h"
#include "projectchecksumcache.h"
#include "coreutils.h"
#include "testutils.h"
#include "inpututils.h"

#include <QElapsedTimer>
#include <QtTest/QtTest>

TestProjectChecksumCache::TestProjectChecksumCache() = default;

TestProjectChecksumCache::~TestProjectChecksumCache() = default;

void TestProjectChecksumCache::init()
{
}

void TestProjectChecksumCache::cleanup()
{
}

void TestProjectChecksumCache::testFilesCheckum()
{
  QString projectName = QStringLiteral( "testFilesCheckum" );
  QString projectDir = QDir::tempPath() + "/" + projectName;

  InputUtils::cpDir( TestUtils::testDataDir() + "/planes", projectDir );
  InputUtils::copyFile( TestUtils::testDataDir() + "/TreeAutumn.png", projectDir + "/TreeAutumn.png" );

  QString cacheFilePath = projectDir + "/.mergin/checksum.cache"  ;
  QString checksumDirectTxt1 = CoreUtils::calculateChecksum( projectDir + "/lines.qml" );
  QVERIFY( !checksumDirectTxt1.isEmpty() );

  QElapsedTimer timer;
  timer.start();
  QString checksumDirectBigFile = CoreUtils::calculateChecksum( projectDir + "/TreeAutumn.png" );
  qint64 elapsedForChecksumDirectBigFile = timer.elapsed();
  QVERIFY( !checksumDirectBigFile.isEmpty() );

  {
    // Cold start - delete cache file
    InputUtils::removeFile( cacheFilePath );
    ProjectChecksumCache cache( projectDir );
    QCOMPARE( cache.cacheFilePath(), cacheFilePath );

    // Test gpkg
    QString checksumDirectGpkg = CoreUtils::calculateChecksum( projectDir + "/constraint-layers.gpkg" );
    QCOMPARE( checksumDirectGpkg, "c81be103072ecea025ff92a813916db8e42b7bbb" );
    QString checksumFromCacheGpkg = cache.get( "constraint-layers.gpkg" );
    QCOMPARE( checksumDirectGpkg, checksumFromCacheGpkg );

    // Test non-existent file
    InputUtils::removeFile( projectDir + "/photo.jpg" );
    QVERIFY( cache.get( "photo.jpg" ).isEmpty() );

    // Test text file
    QString checksumFromCacheTxt = cache.get( "lines.qml" );
    QCOMPARE( checksumDirectTxt1, checksumFromCacheTxt );

    // Test photo - big file
    QString checksumFromCacheBigFile = cache.get( "TreeAutumn.png" );
    QCOMPARE( checksumDirectBigFile, checksumFromCacheBigFile );
  }

  // Test that cache is saved
  QVERIFY( QFileInfo( cacheFilePath ).exists() );
  QDateTime cacheModifiedTime = QFileInfo( cacheFilePath ).lastModified();

  // Modify txt file, remove constraint-layers.gpkg file and add photo file, do not touch TreeAutumn.png
  InputUtils::removeFile( projectDir + "/constraint-layers.gpkg" );
  InputUtils::copyFile( TestUtils::testDataDir() + "/photo.jpg", projectDir + "/photo.jpg" );
  QString checksumDirectPhoto = CoreUtils::calculateChecksum( projectDir + "/photo.jpg" );
  QVERIFY( !checksumDirectPhoto.isEmpty() );

  QFile f( projectDir + "/lines.qml" );
  if ( f.open( QIODevice::WriteOnly ) )
  {
    QTextStream stream( &f );
    stream << "something really really cool";
    f.close();
  }
  QString checksumDirectTxt2 = CoreUtils::calculateChecksum( projectDir + "/lines.qml" );
  QVERIFY( !checksumDirectTxt2.isEmpty() );
  QVERIFY( checksumDirectTxt1 != checksumDirectTxt2 );

  {
    // Start with existent cache file
    ProjectChecksumCache cache( projectDir );

    // Test non-existent gpkg - NOT taken from previous cache
    QVERIFY( cache.get( "constraint-layers.gpkg" ).isEmpty() );

    // Test new file - NOT taken from previous cache
    QString checksumDirectPhoto = CoreUtils::calculateChecksum( projectDir + "/photo.jpg" );
    QString checksumFromCachePhoto = cache.get( "photo.jpg" );
    QCOMPARE( checksumDirectPhoto, checksumFromCachePhoto );

    // Test modified file - NOT taken from previous cache
    QString checksumFromCacheTxt = cache.get( "lines.qml" );
    QCOMPARE( checksumFromCacheTxt, checksumDirectTxt2 );

    // Test bigfile - checksum taken from previous cache!
    QElapsedTimer timer2;
    timer2.start();
    QString checksumFromCacheBigFile = cache.get( "TreeAutumn.png" );
    qint64 elapsedTimeFromCache = timer2.elapsed();
    QVERIFY2( elapsedTimeFromCache < 2, QString::number( elapsedTimeFromCache ).toStdString().c_str() ); // It should be pretty much instant (<1ms)
    QVERIFY2( elapsedForChecksumDirectBigFile > 4, QString::number( elapsedForChecksumDirectBigFile ).toStdString().c_str() ); // It should take few miliseconds to read and calculate checksum of 4 MB file
    QCOMPARE( checksumDirectBigFile, checksumFromCacheBigFile );
  }

  // Test that cache is re-saved
  QVERIFY( QFileInfo( cacheFilePath ).exists() );
  QDateTime cacheModifiedTime2 = QFileInfo( cacheFilePath ).lastModified();
  QVERIFY( cacheModifiedTime != cacheModifiedTime2 );

  {
    // Start with existent cache file
    ProjectChecksumCache cache( projectDir );

    // Test geo gpkg
    QString checksumFromCacheGeoGpkg = cache.get( "TreeAutumn.png" );
    QCOMPARE( checksumDirectBigFile, checksumFromCacheGeoGpkg );
  }
  // Test that cache is NOT re-saved
  QVERIFY( QFileInfo( cacheFilePath ).exists() );
  QDateTime cacheModifiedTime3 = QFileInfo( cacheFilePath ).lastModified();
  QCOMPARE( cacheModifiedTime2, cacheModifiedTime3 );
}
