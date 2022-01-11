/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testutilsfunctions.h"
#include <QApplication>
#include <QDesktopWidget>

#include "qgsapplication.h"
#include "qgscoordinatereferencesystem.h"
#include "qgscoordinatetransformcontext.h"
#include "qgsproject.h"
#include "qgspoint.h"
#include "qgspointxy.h"
#include "qgis.h"
#include "qgsunittypes.h"
#include "qgsfeature.h"
#include "qgsgeometry.h"

#include "testutils.h"

#include <QtTest/QtTest>
#include <QtCore/QObject>

const int DAY_IN_SECS = 60 * 60 * 24;
const int MONTH_IN_SECS = 60 * 60 * 24 * 31;

TestUtilsFunctions::TestUtilsFunctions( InputUtils *utils )
  : mUtils( utils )
{

}

void TestUtilsFunctions::testFormatDuration()
{
  QDateTime t0 = QDateTime::currentDateTime();

  testFormatDuration( t0, -1, QStringLiteral( "Invalid datetime" ) );
  testFormatDuration( t0, 0, QStringLiteral( "just now" ) );
  testFormatDuration( t0, 1, QStringLiteral( "just now" ) );
  testFormatDuration( t0, 60, QStringLiteral( "1 minute ago" ) );
  testFormatDuration( t0, 2 * 60, QStringLiteral( "2 minutes ago" ) );
  testFormatDuration( t0, 1 * 60 * 60, QStringLiteral( "1 hour ago" ) );
  testFormatDuration( t0, 2 * 60 * 60, QStringLiteral( "2 hours ago" ) );
  testFormatDuration( t0, 1 * DAY_IN_SECS, QStringLiteral( "1 day ago" ) );
  testFormatDuration( t0, 2 * DAY_IN_SECS, QStringLiteral( "2 days ago" ) );
  testFormatDuration( t0, 7 * DAY_IN_SECS, QStringLiteral( "1 week ago" ) );
  testFormatDuration( t0, 14 * DAY_IN_SECS, QStringLiteral( "2 weeks ago" ) );
  testFormatDuration( t0, MONTH_IN_SECS, QStringLiteral( "1 month ago" ) );
  testFormatDuration( t0, 2 * MONTH_IN_SECS, QStringLiteral( "2 months ago" ) );
  testFormatDuration( t0, 12 * MONTH_IN_SECS, QStringLiteral( "1 year ago" ) );
  testFormatDuration( t0, 24 * MONTH_IN_SECS, QStringLiteral( "2 years ago" ) );
}

void TestUtilsFunctions::testFormatDuration( const QDateTime &t0, qint64 diffSecs, const QString &expectedResult )
{
  QDateTime t1 = t0.addSecs( diffSecs );
  QString str_t1 = InputUtils::formatDateTimeDiff( t0, t1 );
  QCOMPARE( str_t1, expectedResult );
}

void TestUtilsFunctions::dump_screen_info()
{
  QVERIFY( mUtils->dumpScreenInfo().contains( "screen" ) );
}

void TestUtilsFunctions::screenUnitsToMeters()
{
  QgsCoordinateReferenceSystem crsGPS = QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
  QVERIFY( crsGPS.authid() == "EPSG:4326" );

  QgsQuickMapSettings ms;
  ms.setDestinationCrs( crsGPS );
  ms.setExtent( QgsRectangle( 49, 16, 50, 17 ) );
  ms.setOutputSize( QSize( 1000, 500 ) );
  double sutm = mUtils->screenUnitsToMeters( &ms, 1 );
  COMPARENEAR( sutm, 213.5, 1.0 );
}

void TestUtilsFunctions::transformedPoint()
{
  QgsPointXY pointXY = mUtils->pointXY( 49.9, 16.3 );
  COMPARENEAR( pointXY.x(), 49.9, 1e-4 );
  COMPARENEAR( pointXY.y(), 16.3, 1e-4 );

  QgsPoint point =  mUtils->point( 1.0, -1.0 );
  COMPARENEAR( point.x(), 1.0, 1e-4 );
  COMPARENEAR( point.y(), -1.0, 1e-4 );

  QgsCoordinateReferenceSystem crs3857 = QgsCoordinateReferenceSystem::fromEpsgId( 3857 );
  QVERIFY( crs3857.authid() == "EPSG:3857" );

  QgsCoordinateReferenceSystem crsGPS = QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
  QVERIFY( crsGPS.authid() == "EPSG:4326" );

  QgsPointXY transformedPoint =  mUtils->transformPoint( crsGPS,
                                 crs3857,
                                 QgsCoordinateTransformContext(),
                                 pointXY );
  COMPARENEAR( transformedPoint.x(), 5554843, 1.0 );
  COMPARENEAR( transformedPoint.y(), 1839491, 1.0 );
}

void TestUtilsFunctions::formatPoint()
{
  QgsPoint point( -2.234521, 34.4444421 );
  QString point2str =  mUtils->formatPoint( point );
  QVERIFY( point2str == "-2.235,34.444" );
}

void TestUtilsFunctions::formatDistance()
{
  QString dist2str =  mUtils->formatDistance( 1222.234, QgsUnitTypes::DistanceMeters,  2 );
  QVERIFY( dist2str == "1.22 km" );

  dist2str =  mUtils->formatDistance( 1222.234, QgsUnitTypes::DistanceMeters, 1 );
  QVERIFY( dist2str == "1.2 km" );

  dist2str =  mUtils->formatDistance( 1222.234, QgsUnitTypes::DistanceMeters, 0 );
  QVERIFY( dist2str == "1 km" );

  dist2str =  mUtils->formatDistance( 700.22, QgsUnitTypes::DistanceMeters, 1 );
  QVERIFY( dist2str == "700.2 m" );

  dist2str =  mUtils->formatDistance( 0.22, QgsUnitTypes::DistanceMeters, 0 );
  QVERIFY( dist2str == "22 cm" );

  dist2str =  mUtils->formatDistance( -0.22, QgsUnitTypes::DistanceMeters, 0 );
  QVERIFY( dist2str == "0 mm" );

  dist2str =  mUtils->formatDistance( 1.222234, QgsUnitTypes::DistanceKilometers,  2 );
  QVERIFY( dist2str == "1.22 km" );

  /////////////////////////////////////////////////////////
  dist2str =  mUtils->formatDistance( 6000, QgsUnitTypes::DistanceFeet, 1, QgsUnitTypes::ImperialSystem );
  QVERIFY( dist2str == "1.1 mi" );

  dist2str =  mUtils->formatDistance( 5, QgsUnitTypes::DistanceFeet, 1, QgsUnitTypes::ImperialSystem );
  QVERIFY( dist2str == "1.7 yd" );

  /////////////////////////////////////////////////////////
  dist2str =  mUtils->formatDistance( 7000, QgsUnitTypes::DistanceFeet, 1, QgsUnitTypes::USCSSystem );
  QVERIFY( dist2str == "1.2 NM" );
}

void TestUtilsFunctions::loadIcon()
{
  QUrl url =  mUtils->getThemeIcon( "ic_save_white" );
  QCOMPARE( url.toString(), QLatin1String( "qrc:/ic_save_white.svg" ) );

  QFileInfo fileInfo( url.toString() );
  QString fileName( fileInfo.fileName() );
  QCOMPARE( fileName, QLatin1String( "ic_save_white.svg" ) );
}

void TestUtilsFunctions::fileExists()
{
  QString path = TestUtils::testDataDir() + "/planes/quickapp_project.qgs";
  QVERIFY( mUtils->fileExists( path ) );
}


void TestUtilsFunctions::loadQmlComponent()
{
  QUrl dummy =  mUtils->getEditorComponentSource( "dummy" );
  QCOMPARE( dummy.path(), QString( "../editor/inputtextedit.qml" ) );

  QUrl valuemap =  mUtils->getEditorComponentSource( "valuemap" );
  QCOMPARE( valuemap.path(), QString( "../editor/inputvaluemap.qml" ) );
}

void TestUtilsFunctions::getRelativePath()
{
  QString prefixPath = QStringLiteral( "%1/" ).arg( TestUtils::testDataDir() );
  QString fileName = QStringLiteral( "quickapp_project.qgs" );
  QString path =  prefixPath + fileName;
  QString relativePath =  mUtils->getRelativePath( path, prefixPath );
  QCOMPARE( fileName, relativePath );

  QString fileName2 = QStringLiteral( "zip/test.zip" );
  QString path2 = prefixPath + fileName2;
  QString relativePath2 =  mUtils->getRelativePath( path2, prefixPath );
  QCOMPARE( fileName2, relativePath2 );

  QString path3 = QStringLiteral( "file://" ) + path2;
  QString relativePath3 =  mUtils->getRelativePath( path3, prefixPath );
  QCOMPARE( fileName2, relativePath3 );

  QString relativePath4 =  mUtils->getRelativePath( path2, QStringLiteral( "/dummy/path/" ) );
  QCOMPARE( QString(), relativePath4 );

  QString relativePath5 =  mUtils->getRelativePath( path2, QStringLiteral( "" ) );
  QCOMPARE( path2, relativePath5 );
}

void TestUtilsFunctions::resolvePhotoPath()
{
  QString homePath = TestUtils::testDataDir();
  QString imagePath( "photos/IMG001.jpg" ); // can be not existing image
  QVariantMap config;
  // prepare project to be able evaluate expression
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QgsProject::instance()->read( projectDir + "/quickapp_project.qgs" );
  FeatureLayerPair pair; // Can be dummy pair

  // Case 1: absolute path + empty default path
  config.insert( QStringLiteral( "RelativeStorage" ), QStringLiteral( "0" ) );

  QString result1 = mUtils->resolvePath( imagePath, homePath, config, pair, QgsProject::instance() );
  QString expected1( imagePath );
  QCOMPARE( result1, expected1 );
  config.clear();

  // Case 2: relative to project path + empty default path
  config.insert( QStringLiteral( "RelativeStorage" ), QStringLiteral( "1" ) );

  QString result2 = mUtils->resolvePath( imagePath, homePath, config, pair, QgsProject::instance() );
  QString expected2 = QStringLiteral( "%1/%2" ).arg( homePath ).arg( imagePath );
  QCOMPARE( result2, expected2 );
  config.clear();

  // Case 3: relative to default path + default path: @project_home + '/photos'
  config.insert( QStringLiteral( "RelativeStorage" ), QStringLiteral( "2" ) );

  QString result3 = mUtils->resolvePath( imagePath, homePath, config, pair, QgsProject::instance() );
  QString expected3 = QStringLiteral( "%1/%2" ).arg( homePath ).arg( imagePath );
  QCOMPARE( result3, expected3 );

}

void TestUtilsFunctions::resolveTargetDir()
{
  QString homePath = TestUtils::testDataDir();
  QString DEFAULT_ROOT( "DEFAULT/ROOT/PATH" ); // can be not existing path

  QgsProject *activeProject = nullptr;
  QVariantMap config;

  // prepare project to be able evaluate expression
  QString projectDir = homePath + "/planes";
  QgsProject::instance()->read( projectDir + "/quickapp_project.qgs" );
  FeatureLayerPair pair; // Can be dummy pair

  // case 1: empty config, no expression
  QString resultDir = mUtils->resolveTargetDir( homePath, config, pair, activeProject );
  QCOMPARE( resultDir, homePath );

  // case 2: defined default root config, no expression
  config.insert( QStringLiteral( "DefaultRoot" ), DEFAULT_ROOT );
  QString resultDir2 = mUtils->resolveTargetDir( homePath, config, pair, activeProject );
  QCOMPARE( resultDir2, DEFAULT_ROOT );
  config.clear();

  // case 3: defined expression in config->"PropertyCollection" -> "properties" -> "propertyRootPath" -> "expression"
  QString expression( "@project_home + '/photos'" );
  QMap<QString, QVariant> propertyRootPath;
  QMap<QString, QVariant> props;
  QMap<QString, QVariant> collection;

  propertyRootPath.insert( QStringLiteral( "expression" ), QVariant( expression ) );
  props.insert( QStringLiteral( "propertyRootPath" ), propertyRootPath );
  collection.insert( QStringLiteral( "properties" ), props );
  config.insert( QStringLiteral( "PropertyCollection" ), collection );

  QString resultDir3 = mUtils->resolveTargetDir( homePath, config, pair, QgsProject::instance() );
  QCOMPARE( resultDir3, QStringLiteral( "%1/photos" ).arg( projectDir ) );
}

void TestUtilsFunctions::testDirSize()
{
  // create some test data in tmp with non ascii characters
  QString tempFolder = QDir::tempPath();

  QString project = tempFolder + QStringLiteral( "/input-project" );
  qint64 dirSize;

  dirSize = mUtils->dirSize( project );
  QCOMPARE( dirSize, 0 );

  QString subfolder = project + QStringLiteral( "/sub/sub/ž" );
  QDir subfolderDir( subfolder );
  subfolderDir.mkpath( subfolder );

  QFile file1( subfolder + QStringLiteral( "/testfile1-žriebä" ) );
  file1.open( QIODevice::WriteOnly );

  file1.write( "Loreem" );
  file1.close();

  qint64 newDirSize = mUtils->dirSize( project );

  QVERIFY( newDirSize > dirSize );

  // add hidden folder and file and see if size gets bigger
  QString hiddenFolder = project + QStringLiteral( "/.hide" );
  QDir hiddenFolderDir( hiddenFolder );
  hiddenFolderDir.mkpath( hiddenFolder );

  QFile file2( hiddenFolder + QStringLiteral( "/.mergin" ) );
  file2.open( QIODevice::WriteOnly );

  file2.write( "Loreem" );
  file2.close();

  dirSize = newDirSize;
  newDirSize = mUtils->dirSize( project );

  QVERIFY( newDirSize > dirSize );

  // test not existing folders
  dirSize = mUtils->dirSize( tempFolder + QStringLiteral( "/notexistingfilesfolder_input" ) );
  QCOMPARE( dirSize, 0 );

  // try to pass files - should ignore it and return 0
  QFile file3( tempFolder + QStringLiteral( "/.mergin" ) );
  file3.open( QIODevice::WriteOnly );

  file3.write( "Loreem" );
  file3.close();

  dirSize = mUtils->dirSize( tempFolder + QStringLiteral( "/.mergin" ) );
  QCOMPARE( dirSize, 0 );
}

void TestUtilsFunctions::testExtractPointFromFeature()
{
  FeatureLayerPair invalidPair;
  QCOMPARE( mUtils->extractPointFromFeature( invalidPair ), QgsPointXY() );

  QgsFeature feature;
  QgsGeometry geom;
  QgsPoint *pt = new QgsPoint( 1, 2, 3, 4 );
  geom.set( pt );
  feature.setGeometry( geom );

  QgsVectorLayer linesLayer( QStringLiteral( "LineString?crs=%1" ).arg( "EPSG:4326" ), "linesLayer", "memory" );
  QgsVectorLayer pointsLayer( QStringLiteral( "point?crs=%1" ).arg( "EPSG:4326" ), "pointsLayer", "memory" );

  FeatureLayerPair linePair( feature, &linesLayer );
  QCOMPARE( mUtils->extractPointFromFeature( linePair ), QgsPointXY() );

  FeatureLayerPair pointPair( feature, &pointsLayer );
  QCOMPARE( mUtils->extractPointFromFeature( pointPair ), QgsPointXY( 1, 2 ) );
}

void TestUtilsFunctions::testNavigationFeatureExtent()
{
  QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem::fromEpsgId( 6326 );

  QgsQuickMapSettings ms;
  ms.setDestinationCrs( crs );
  ms.setExtent( QgsRectangle( 49, 16, 50, 17 ) );
  ms.setOutputSize( QSize( 1000, 500 ) );

  QgsPoint gpsPos( 36.77320625296759, 3.060085717615282 );
  QgsPoint *point = new QgsPoint( 36.77440939232914, 3.0596565641869136 );

  QgsVectorLayer pointsLayer( QStringLiteral( "point?crs=%1" ).arg( "EPSG:4326" ), "pointsLayer", "memory" );

  QgsFeature feature;
  QgsGeometry geom;
  geom.set( point );
  feature.setGeometry( geom );

  FeatureLayerPair pair( feature, &pointsLayer );

  QgsRectangle rect = mUtils->navigationFeatureExtent( pair, gpsPos, &ms, 0 );
  QgsRectangle acceptedExtent( 36.773085939, 3.059613648845, 36.7745297063, 3.06012863296 );
  QCOMPARE( rect.xMinimum(), acceptedExtent.xMinimum() );
  QCOMPARE( rect.yMinimum(), acceptedExtent.yMinimum() );
  QCOMPARE( rect.xMaximum(), acceptedExtent.xMaximum() );
  QCOMPARE( rect.yMaximum(), acceptedExtent.yMaximum() );
}

void TestUtilsFunctions::testDistanceToFeature()
{
  QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem::fromEpsgId( 4326 );

  QgsQuickMapSettings ms;
  ms.setDestinationCrs( crs );

  QgsPoint gpsPos( 36.77320625296759, 3.060085717615282 );
  QgsPoint *point = new QgsPoint( 36.77440939232914, 3.0596565641869136 );

  QgsVectorLayer pointsLayer( QStringLiteral( "point?crs=%1" ).arg( "EPSG:4326" ), "pointsLayer", "memory" );

  QgsFeature feature;
  QgsGeometry geom;
  geom.set( point );
  feature.setGeometry( geom );

  FeatureLayerPair pair( feature, &pointsLayer );

  QString ditance = mUtils->distanceToFeature( gpsPos, pair, &ms );
  QCOMPARE( ditance, QStringLiteral( "142.20 m" ) );
}

void TestUtilsFunctions::testIsPointLayerFeature()
{
  QgsVectorLayer pointsLayer( QStringLiteral( "point?crs=%1" ).arg( "EPSG:4326" ), "pointsLayer", "memory" );
  QgsVectorLayer linesLayer( QStringLiteral( "LineString?crs=%1" ).arg( "EPSG:4326" ), "linesLayer", "memory" );

  QgsFeature pointFeature;
  QgsGeometry geom;
  QgsPoint *pt = new QgsPoint( 1, 2, 3, 4 );
  geom.set( pt );
  pointFeature.setGeometry( geom );

  QgsFeature lineFeature;
  QgsGeometry lineGeom;
  QgsLineString *line = new QgsLineString( QVector<QgsPointXY>() << QgsPointXY( 0, 0 ) << QgsPointXY( 1, 1 ) );
  lineGeom.set( line );
  lineFeature.setGeometry( lineGeom );

  {
    FeatureLayerPair pair( pointFeature, &pointsLayer );
    QCOMPARE( InputUtils::isPointLayerFeature( pair ), true );
  }
  {
    FeatureLayerPair pair( pointFeature, &linesLayer );
    QCOMPARE( InputUtils::isPointLayerFeature( pair ), false );
  }
  {
    FeatureLayerPair pair( lineFeature, &pointsLayer );
    QCOMPARE( InputUtils::isPointLayerFeature( pair ), false );
  }
  {
    FeatureLayerPair pair( lineFeature, &linesLayer );
    QCOMPARE( InputUtils::isPointLayerFeature( pair ), false );
  }
}
