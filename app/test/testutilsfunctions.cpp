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
#include <QScreen>
#include <QLocale>

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
#include "qgsmemoryproviderutils.h"
#include "qgsrasterlayer.h"
#include "qgspolygon.h"

#include "mmstyle.h"

#include "testutils.h"

#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QTemporaryDir>
#include <QLocale>
#include <QUrl>

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
  testFormatDuration( t0, 60, QStringLiteral( "1 minute(s) ago" ) );
  testFormatDuration( t0, 2 * 60, QStringLiteral( "2 minute(s) ago" ) );
  testFormatDuration( t0, 1 * 60 * 60, QStringLiteral( "1 hour(s) ago" ) );
  testFormatDuration( t0, 2 * 60 * 60, QStringLiteral( "2 hour(s) ago" ) );
  testFormatDuration( t0, 1 * DAY_IN_SECS, QStringLiteral( "1 day(s) ago" ) );
  testFormatDuration( t0, 2 * DAY_IN_SECS, QStringLiteral( "2 day(s) ago" ) );
  testFormatDuration( t0, 7 * DAY_IN_SECS, QStringLiteral( "1 week(s) ago" ) );
  testFormatDuration( t0, 14 * DAY_IN_SECS, QStringLiteral( "2 week(s) ago" ) );
  testFormatDuration( t0, MONTH_IN_SECS, QStringLiteral( "1 month(s) ago" ) );
  testFormatDuration( t0, 2 * MONTH_IN_SECS, QStringLiteral( "2 month(s) ago" ) );
  testFormatDuration( t0, 12 * MONTH_IN_SECS, QStringLiteral( "1 year(s) ago" ) );
  testFormatDuration( t0, 24 * MONTH_IN_SECS, QStringLiteral( "2 year(s) ago" ) );
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

  InputMapSettings ms;
  ms.setDestinationCrs( crsGPS );
  ms.setExtent( QgsRectangle( 49, 16, 50, 17 ) );
  ms.setOutputSize( QSize( 1000, 500 ) );
  double sutm = mUtils->screenUnitsToMeters( &ms, 1 );
  COMPARENEAR( sutm, 213.5, 1.0 );
}

void TestUtilsFunctions::transformedPointXY()
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

  QgsPointXY transformedPoint =  mUtils->transformPointXY( crsGPS,
                                 crs3857,
                                 QgsCoordinateTransformContext(),
                                 pointXY );
  COMPARENEAR( transformedPoint.x(), 5554843, 1.0 );
  COMPARENEAR( transformedPoint.y(), 1839491, 1.0 );

  // Check transformation within the same CRS
  transformedPoint = mUtils->transformPointXY(
                       crsGPS,
                       crsGPS,
                       QgsCoordinateTransformContext(),
                       pointXY
                     );

  QVERIFY( !transformedPoint.isEmpty() );
  COMPARENEAR( transformedPoint.x(), 49.9, 1e-4 );
  COMPARENEAR( transformedPoint.y(), 16.3, 1e-4 );
}

void TestUtilsFunctions::transformedPoint()
{
  QgsPoint point = mUtils->point( 49.9, 16.3 );
  COMPARENEAR( point.x(), 49.9, 1e-4 );
  COMPARENEAR( point.y(), 16.3, 1e-4 );

  QgsCoordinateReferenceSystem crs3857 = QgsCoordinateReferenceSystem::fromEpsgId( 3857 );
  QVERIFY( crs3857.authid() == "EPSG:3857" );

  QgsCoordinateReferenceSystem crsGPS = QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
  QVERIFY( crsGPS.authid() == "EPSG:4326" );

  QgsPoint transformedPoint =  mUtils->transformPoint( crsGPS,
                               crs3857,
                               QgsCoordinateTransformContext(),
                               point );
  COMPARENEAR( transformedPoint.x(), 5554843, 1.0 );
  COMPARENEAR( transformedPoint.y(), 1839491, 1.0 );

  // Check transformation within the same CRS
  transformedPoint = mUtils->transformPoint(
                       crsGPS,
                       crsGPS,
                       QgsCoordinateTransformContext(),
                       point
                     );

  QVERIFY( !transformedPoint.isEmpty() );
  COMPARENEAR( transformedPoint.x(), 49.9, 1e-4 );
  COMPARENEAR( transformedPoint.y(), 16.3, 1e-4 );
}

void TestUtilsFunctions::formatPoint()
{
  QgsPoint point( -2.234521, 34.4444421 );
  QString point2str =  mUtils->formatPoint( point );
  QString expected =
    QStringLiteral( "-2" ) + QLocale().decimalPoint() + QStringLiteral( "235" )
    + QgsCoordinateFormatter::separator()
    + QStringLiteral( "34" ) + QLocale().decimalPoint() + QStringLiteral( "444" );
  QCOMPARE( point2str, expected );
}

void TestUtilsFunctions::testHtmlLink()
{
  struct testcase
  {
    QString text;
    QString url;
    QString url2;
    bool underline;
    bool bold;
    QColor color;
    QString expectedResult;
  };

  QVector<testcase> testcases =
  {
    { "home page", "https://merginmaps.com", "", true, false, QColor( "red" ), "<a style='text-decoration:underline; color:#ff0000;' href='https://merginmaps.com'>home page</a>" },
    { "home page", "https://merginmaps.com", "", false, false, QColor( "red" ), "<a style='text-decoration:none; color:#ff0000;' href='https://merginmaps.com'>home page</a>" },
    { "this is my %1cool link%2 to show", "https://merginmaps.com", "", false, false, QColor( "red" ), "this is my <a style='text-decoration:none; color:#ff0000;' href='https://merginmaps.com'>cool link</a> to show" },
    { "%1cool link%2 to show", "https://merginmaps.com", "", false, false, QColor( "red" ), "<a style='text-decoration:none; color:#ff0000;' href='https://merginmaps.com'>cool link</a> to show" },
    { "this is my %1cool link%2", "https://merginmaps.com", "", false, false, QColor( "red" ), "this is my <a style='text-decoration:none; color:#ff0000;' href='https://merginmaps.com'>cool link</a>" },
    { "this is my %1cool link 1%3 and %2cool link 2%3 wow!", "https://merginmaps.com", "https://merginmaps.com/login", false, true, QColor( "red" ), "this is my <a style='text-decoration:none; font-weight: 600; color:#ff0000;' href='https://merginmaps.com'>cool link 1</a> and <a style='text-decoration:none; font-weight: 600; color:#ff0000;' href='https://merginmaps.com/login'>cool link 2</a> wow!" }
  };

  for ( const auto &c : testcases )
  {
    QString ret = mUtils->htmlLink( c.text, c.color, c.url, c.url2, c.underline, c.bold );
    QCOMPARE( ret, c.expectedResult );
  }
}

void TestUtilsFunctions::formatDistance()
{
  QString dist2str =  mUtils->formatDistance( 1222.234, Qgis::DistanceUnit::Meters,  2 );
  QVERIFY( dist2str == "1.22 km" );

  dist2str =  mUtils->formatDistance( 1222.234, Qgis::DistanceUnit::Meters, 1 );
  QVERIFY( dist2str == "1.2 km" );

  dist2str =  mUtils->formatDistance( 1222.234, Qgis::DistanceUnit::Meters, 0 );
  QVERIFY( dist2str == "1 km" );

  dist2str =  mUtils->formatDistance( 700.22, Qgis::DistanceUnit::Meters, 1 );
  QVERIFY( dist2str == "700.2 m" );

  dist2str =  mUtils->formatDistance( 0.22, Qgis::DistanceUnit::Meters, 0 );
  QVERIFY( dist2str == "22 cm" );

  dist2str =  mUtils->formatDistance( -0.22, Qgis::DistanceUnit::Meters, 0 );
  QVERIFY( dist2str == "0 mm" );

  dist2str =  mUtils->formatDistance( 1.222234, Qgis::DistanceUnit::Kilometers,  2 );
  QVERIFY( dist2str == "1.22 km" );

  /////////////////////////////////////////////////////////
  dist2str =  mUtils->formatDistance( 6000, Qgis::DistanceUnit::Feet, 1, Qgis::SystemOfMeasurement::Imperial );
  QVERIFY( dist2str == "1.1 mi" );

  dist2str =  mUtils->formatDistance( 5, Qgis::DistanceUnit::Feet, 1, Qgis::SystemOfMeasurement::Imperial );
  QVERIFY( dist2str == "1.7 yd" );

  /////////////////////////////////////////////////////////
  dist2str =  mUtils->formatDistance( 7000, Qgis::DistanceUnit::Feet, 1, Qgis::SystemOfMeasurement::USCS );
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
  QUrl dummy =  mUtils->getFormEditorType( "dummy" );
  QCOMPARE( dummy.path(), QString( "../form/editors/MMFormTextEditor.qml" ) );

  QUrl valuemap =  mUtils->getFormEditorType( "valuemap" );
  QCOMPARE( valuemap.path(), QString( "../form/editors/MMFormValueMapEditor.qml" ) );
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

  QString path3 = QStringLiteral( "file:///" ) + path2;
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
  QString expectedDir = QStringLiteral( "%1/photos" ).arg( projectDir );
  mUtils->sanitizeFileName( expectedDir );
  QCOMPARE( resultDir3, expectedDir );
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

void TestUtilsFunctions::testStakeoutPathExtent()
{
  QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem::fromEpsgId( 4326 );

  InputMapSettings ms;
  ms.setDestinationCrs( crs );
  ms.setExtent( QgsRectangle( 49, 16, 50, 17 ) );
  ms.setOutputSize( QSize( 400, 620 ) );

  PositionKit positionKit;
  positionKit.setPositionProvider( PositionKit::constructProvider( "internal", "simulated", "simulated" ) );
  AbstractPositionProvider *provider = positionKit.positionProvider();

  MapPosition mapPositioner;
  mapPositioner.setMapSettings( &ms );
  mapPositioner.setPositionKit( &positionKit );

  QgsVectorLayer pointsLayer( QStringLiteral( "point?crs=%1" ).arg( "EPSG:4326" ), "pointsLayer", "memory" );

  QgsPoint *target = new QgsPoint( 47.48117696934868, 19.064282309621444 );

  // We want to test if extent has correct scale and center
  struct testcase
  {
    QgsPoint gpsPosition;
    QgsPoint expectedCenter;
    int expectedScale;
  };

  QVector< testcase > testcases =
  {
    { QgsPoint( 48.1141526157956, 17.267434685006886 ),  QgsPoint( 48.1141526157956, 17.267434685006886 ),  204 }, // far far away
    { QgsPoint( 47.48127984125111, 19.064443912646784 ), QgsPoint( 47.48127984125111, 19.064443912646784 ), 204 }, // > 20m from target
    { QgsPoint( 47.481122740577486, 19.06433009357165 ), QgsPoint( 47.481122740577486, 19.06433009357165 ), 105 }, // ~ 8m from target
    { QgsPoint( 47.481185467, 19.064302897 ),            QgsPoint( 47.481185467, 19.064302897 ),            55  }, // ~ 2m from target
    { QgsPoint( 47.481177379, 19.064283071 ),            QgsPoint( 47.48117696934868, 19.064282309621444 ), 25  }  // ~ 10cm from target
  };

  QgsFeature feature;
  QgsGeometry geom;
  geom.set( target );
  feature.setGeometry( geom );

  FeatureLayerPair pair( feature, &pointsLayer );

  for ( const auto &test : testcases )
  {
    provider->setPosition( test.gpsPosition );
    QgsRectangle extent = mUtils->stakeoutPathExtent( &mapPositioner, pair, &ms, 0 );

    QVERIFY( mUtils->equals( extent.center(), test.expectedCenter ) );

    ms.setExtent( extent );

    // On some platforms the scale is not exactly the same
    COMPARENEAR( ms.mapSettings().scale(), test.expectedScale, 1.5 );
  }

  // Test stakeout distance2scale
  struct testcaseDistance2Scale
  {
    qreal distance;
    qreal expectedScale;
  };

  QVector<testcaseDistance2Scale> testcasesDistance2Scale =
  {
    { 150, 205 },
    { 15, 205 },
    { 10.1, 205 },
    { 8, 105 },
    { 4, 105 },
    { 2, 55 },
    { 1.5, 55 },
    { 1.10320432, 55 },
    { 0.9, 25 },
    { 0.1, 25 },
    { 0, 25 },
    { -15, 25 }
  };

  for ( const auto &test : testcasesDistance2Scale )
  {
    COMPARENEAR( mUtils->distanceToScale( test.distance ), test.expectedScale, 0.1 );
  }
}

void TestUtilsFunctions::testDistanceBetweenGpsAndFeature()
{
  QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem::fromEpsgId( 4326 );

  InputMapSettings ms;
  ms.setDestinationCrs( crs );

  QgsPoint gpsPos( 36.77320625296759, 3.060085717615282 );
  QgsPoint *point = new QgsPoint( 36.77440939232914, 3.0596565641869136 );

  QgsVectorLayer pointsLayer( QStringLiteral( "point?crs=%1" ).arg( "EPSG:4326" ), "pointsLayer", "memory" );

  QgsFeature feature;
  QgsGeometry geom;
  geom.set( point );
  feature.setGeometry( geom );

  FeatureLayerPair pair( feature, &pointsLayer );
  qreal distance = mUtils->distanceBetweenGpsAndFeature( gpsPos, pair, &ms );
  QCOMPARE( mUtils->formatNumber( distance, 2 ), "142.20" );
}

void TestUtilsFunctions::testAngleBetweenGpsAndFeature()
{
  QgsCoordinateReferenceSystem featureCRS = QgsCoordinateReferenceSystem::fromEpsgId( 3857 );

  InputMapSettings ms;
  ms.setDestinationCrs( featureCRS );

  QgsPoint gpsPos( 14.7067, 46.6842 );
  QgsPoint *point = new QgsPoint( 524780, 5887742 );

  QgsVectorLayer pointsLayer( QStringLiteral( "point?crs=%1" ).arg( "EPSG:3857" ), "pointsLayer", "memory" );

  QgsFeature feature;
  QgsGeometry geom;
  geom.set( point );
  feature.setGeometry( geom );

  FeatureLayerPair pair( feature, &pointsLayer );
  qreal angle = mUtils->angleBetweenGpsAndFeature( gpsPos, pair, &ms );
  QCOMPARE( mUtils->formatNumber( angle, 3 ), "-1.573" );
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

void TestUtilsFunctions::testMapPointToGps()
{
  // get a point from map crs and compare it to GPS crs (4326)

  InputMapSettings ms;
  ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 3857 ) );
  ms.setExtent( QgsRectangle( 1638326.6916276202537119, 5886831.7859314000234008, 1653505.5471235010772943, 5901042.9167255694046617 ) );
  ms.setOutputSize( QSize( 1000, 500 ) );

  struct scenario
  {
    double x; // original
    double y; // original
    double lat; // expected
    double lon; // expected
  };

  double nan = std::numeric_limits<double>::quiet_NaN();

  QVector<scenario> scenarios =
  {
    { 191.162109375, 364.65240478515625, 14.7067, 46.6842 },
    { 181.21180725097656, 289.0494384765625, 14.7041, 46.6974 },
    { 262.84490966796875, 418.75543212890625, 14.725, 46.6747 },
    { 253.0792236328125, 199.93780517578125, 14.7225, 46.713 },
    { nan, nan, nan, nan }
  };

  for ( const auto &s : scenarios )
  {
    QPointF mapPoint( s.x, s.y );
    QgsPoint gpsPoint = InputUtils::mapPointToGps( mapPoint, &ms );

    if ( std::isnan( s.x ) )
    {
      QVERIFY( gpsPoint.isEmpty() );
    }
    else
    {
      COMPARENEAR( gpsPoint.x(), s.lat, 0.001 );
      COMPARENEAR( gpsPoint.y(), s.lon, 0.001 );
    }
  }
}

void TestUtilsFunctions::testEquals()
{
  // Test different InputUtils::equals overloads

  struct testcaseQPointF
  {
    QPointF a;
    QPointF b;
    qreal epsilon;
    bool shouldEqual;
  };

  QVector<testcaseQPointF> testcasesQPointFs =
  {
    { QPointF(),             QPointF(),       0.001, true  },
    { QPointF( 1, 1 ),       QPointF( 1, 1 ), 0.001, true  },
    { QPointF( 1, 1 ),       QPointF(),       0.001, false },
    { QPointF(),             QPointF( 1, 1 ), 0.001, false },
    { QPointF( 0, -5 ),      QPointF( 0, 5 ), 0.1,   false },
    { QPointF( 1.15005, 5 ), QPointF( 1.15, 5 ), 0.01,    true  },
    { QPointF( 1.15005, 5 ), QPointF( 1.15, 5 ), 0.001,   true  },
    { QPointF( 1.15005, 5 ), QPointF( 1.15, 5 ), 0.00001, false },
  };

  for ( const auto &test : testcasesQPointFs )
  {
    QCOMPARE( InputUtils::equals( test.a, test.b, test.epsilon ), test.shouldEqual );
  }

  struct testcaseQgsPointXY
  {
    QgsPointXY a;
    QgsPointXY b;
    qreal epsilon;
    bool shouldEqual;
  };

  QVector<testcaseQgsPointXY> testcasesQgsPointXY =
  {
    { QgsPointXY(),        QgsPointXY(),        0.0001, true  },
    { QgsPointXY(),        QgsPointXY( 1, 6 ),  0.0001, false },
    { QgsPointXY( -5, 5 ), QgsPointXY(),        0.0001, false },
    { QgsPointXY( -5, 5 ), QgsPointXY( -5, 5 ), 0.0001, true  },
    { QgsPointXY( -5, 5 ), QgsPointXY( 5, 5 ),  0.1,    false },
    { QgsPointXY( 1.15005, 5 ), QgsPointXY( 1.15, 5 ), 0.001,   true  },
    { QgsPointXY( 1.15005, 5 ), QgsPointXY( 1.15, 5 ), 0.00001, false },
  };

  for ( const auto &test : testcasesQgsPointXY )
  {
    QCOMPARE( InputUtils::equals( test.a, test.b, test.epsilon ), test.shouldEqual );
  }

  struct testcaseQgsPoint
  {
    QgsPoint a;
    QgsPoint b;
    qreal epsilon;
    bool shouldEqual;
  };

  QVector<testcaseQgsPoint> testcasesQgsPoint =
  {
    { QgsPoint(),        QgsPoint(),        0.0001, true  },
    { QgsPoint(),        QgsPoint( 1, 6 ),  0.0001, false },
    { QgsPoint( -5, 5 ), QgsPoint(),        0.0001, false },
    { QgsPoint( -5, 5 ), QgsPoint( -5, 5 ), 0.0001, true  },
    { QgsPoint( -5, 5 ), QgsPoint( 5, 5 ),  0.1,    false },
    { QgsPoint( 1.15005, 5 ), QgsPoint( 1.15, 5 ), 0.001,   true  },
    { QgsPoint( 1.15005, 5 ), QgsPoint( 1.15, 5 ), 0.00001, false },
  };

  for ( const auto &test : testcasesQgsPoint )
  {
    QCOMPARE( InputUtils::equals( test.a, test.b, test.epsilon ), test.shouldEqual );
  }
}

void TestUtilsFunctions::testGeometryIcons()
{
  QVector<QPair< QgsMapLayer *, QUrl > > testcases =
  {
    { QgsMemoryProviderUtils::createMemoryLayer( "P1", QgsFields(), Qgis::WkbType::Point ), MMStyle::pointLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "P2", QgsFields(), Qgis::WkbType::PointZ ), MMStyle::pointLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "P3", QgsFields(), Qgis::WkbType::PointM ), MMStyle::pointLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "P4", QgsFields(), Qgis::WkbType::PointZM ), MMStyle::pointLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "P5", QgsFields(), Qgis::WkbType::MultiPoint ), MMStyle::pointLayerNoColorOverlayIcon() },

    { QgsMemoryProviderUtils::createMemoryLayer( "L1", QgsFields(), Qgis::WkbType::LineString ), MMStyle::lineLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "L2", QgsFields(), Qgis::WkbType::LineStringZ ), MMStyle::lineLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "L3", QgsFields(), Qgis::WkbType::LineStringM ), MMStyle::lineLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "L4", QgsFields(), Qgis::WkbType::LineStringZM ), MMStyle::lineLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "L5", QgsFields(), Qgis::WkbType::MultiLineString ), MMStyle::lineLayerNoColorOverlayIcon() },

    { QgsMemoryProviderUtils::createMemoryLayer( "PO1", QgsFields(), Qgis::WkbType::Polygon ), MMStyle::polygonLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO2", QgsFields(), Qgis::WkbType::PolygonZ ), MMStyle::polygonLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO3", QgsFields(), Qgis::WkbType::PolygonM ), MMStyle::polygonLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO4", QgsFields(), Qgis::WkbType::PolygonZM ), MMStyle::polygonLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO5", QgsFields(), Qgis::WkbType::MultiPolygon ), MMStyle::polygonLayerNoColorOverlayIcon() },

    { QgsMemoryProviderUtils::createMemoryLayer( "N1", QgsFields(), Qgis::WkbType::Unknown ), MMStyle::tableLayerNoColorOverlayIcon() },
    { QgsMemoryProviderUtils::createMemoryLayer( "N2", QgsFields(), Qgis::WkbType::NoGeometry ), MMStyle::tableLayerNoColorOverlayIcon() },

    { new QgsRasterLayer(), MMStyle::rasterLayerNoColorOverlayIcon() }
  };

  for ( const auto &test : testcases )
  {
    QCOMPARE( mUtils->loadIconFromLayer( test.first ), test.second );
    delete test.first;
  }

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

  QgsFeature polygonFeature;
  QgsGeometry polygonGeom;
  QgsPolygon *polygon = new QgsPolygon( new QgsLineString( QVector<QgsPointXY>() << QgsPointXY( 0, 0 ) << QgsPointXY( 1, 1 ) ) );
  polygonGeom.set( polygon );
  polygonFeature.setGeometry( polygonGeom );

  QgsFeature emptyFeature;
  QgsGeometry emptyGeom;
  emptyFeature.setGeometry( emptyGeom );

  QVector< QPair< QgsFeature, QUrl > > testcases_features =
  {
    { pointFeature, MMStyle::pointLayerNoColorOverlayIcon() },
    { lineFeature, MMStyle::lineLayerNoColorOverlayIcon() },
    { polygonFeature, MMStyle::polygonLayerNoColorOverlayIcon() },
    { emptyFeature, MMStyle::tableLayerNoColorOverlayIcon() }
  };

  for ( const auto &test : testcases_features )
  {
    QCOMPARE( mUtils->loadIconFromFeature( test.first ), test.second );
  }
}

void TestUtilsFunctions::testCreateGeometryForLayer()
{
  QVector< QPair< QString, Qgis::WkbType > > testcases =
  {
    { QStringLiteral( "Point" ), Qgis::WkbType::Point },
    { QStringLiteral( "MultiPoint" ), Qgis::WkbType::MultiPoint },
    { QStringLiteral( "PointZ" ), Qgis::WkbType::PointZ },
    { QStringLiteral( "LineString" ), Qgis::WkbType::LineString },
    { QStringLiteral( "MultiLineString" ), Qgis::WkbType::MultiLineString },
    { QStringLiteral( "LineStringM" ), Qgis::WkbType::LineStringM },
    { QStringLiteral( "MultiLineStringZM" ), Qgis::WkbType::MultiLineStringZM },
    { QStringLiteral( "Polygon" ), Qgis::WkbType::Polygon },
    { QStringLiteral( "MultiPolygon" ), Qgis::WkbType::MultiPolygon },
    { QStringLiteral( "MultiPolygonM" ), Qgis::WkbType::MultiPolygonM },
  };

  QgsGeometry geom;

  for ( const auto &test : testcases )
  {
    QgsVectorLayer *layer = new QgsVectorLayer( QStringLiteral( "%1?crs=epsg:4326" ).arg( test.first ), "layer", "memory" );
    geom = InputUtils::createGeometryForLayer( layer );
    QVERIFY( geom.isEmpty() );
    QCOMPARE( geom.wkbType(), test.second );
    delete layer;
  }
}

void TestUtilsFunctions::testInvalidGeometryWarning()
{
  QVector< QPair< QString, int > > testcases =
  {
    { QStringLiteral( "Point" ), 1 },
    { QStringLiteral( "MultiPoint" ), 1 },
    { QStringLiteral( "LineString" ), 2 },
    { QStringLiteral( "MultiLineString" ), 2 },
    { QStringLiteral( "Polygon" ), 3 },
    { QStringLiteral( "MultiPolygon" ), 3 },
  };

  QString msg;

  for ( const auto &test : testcases )
  {
    QgsVectorLayer *layer = new QgsVectorLayer( QStringLiteral( "%1?crs=epsg:4326" ).arg( test.first ), "layer", "memory" );
    msg = InputUtils::invalidGeometryWarning( layer );

    if ( QgsWkbTypes::isMultiType( layer->wkbType() ) )
    {
      QCOMPARE( msg, QStringLiteral( "You need to add at least %1 point(s) to every part." ).arg( test.second ) );
    }
    else
    {
      QCOMPARE( msg, QStringLiteral( "You need to add at least %1 point(s)." ).arg( test.second ) );
    }

    delete layer;
  }
}

void TestUtilsFunctions::testAttribution()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QgsProject::instance()->read( projectDir + "/quickapp_project.qgs" );

  QList< QgsMapLayer * > layers = QgsProject::instance()->mapLayersByName( QStringLiteral( "airports" ) );
  QVERIFY( !layers.isEmpty() );
  QgsMapLayer *layer = layers.at( 0 );
  QVERIFY( layer->isValid() );
  QString attr = InputUtils::layerAttribution( layer );
  QVERIFY( attr.isEmpty() );
  QgsLayerMetadata metadata;
  metadata.setRights( QStringList() << QStringLiteral( "test" ) );
  layer->setMetadata( metadata );
  attr = InputUtils::layerAttribution( layer );
  QCOMPARE( attr, QStringLiteral( "test" ) );
}

void TestUtilsFunctions::testParsePositionUpdates()
{
  //
  // InputUtils::parsePositionUpdates parses position updates from file data to list of QgsPoints
  // example: "10 20 30 40\n" -> QgsPoint( x:10, y:20, z:30, m:40 )
  //

  QList<std::pair<QString, QList<QgsPoint>>> testcases =
  {
    { QString(), QList<QgsPoint>() },
    { "", QList<QgsPoint>() },
    { "1", QList<QgsPoint>() },
    { "1 1", QList<QgsPoint>() },
    { "1 1 1", QList<QgsPoint>() },
    { "1 1 1 1", QList<QgsPoint> { QgsPoint( 1, 1, 1, 1 ) } },
    { "1 1 1 1\n", QList<QgsPoint> { QgsPoint( 1, 1, 1, 1 ) } },
    { "1 1 1 1\n2 2 2\n3 3 3 3", QList<QgsPoint> { QgsPoint( 1, 1, 1, 1 ), QgsPoint( 3, 3, 3, 3 ) } }
  };

  for ( int i = 0; i < testcases.size(); ++i )
  {
    auto actual = InputUtils::parsePositionUpdates( testcases.at( i ).first );
    auto expected = testcases.at( i ).second;

    if ( actual.isEmpty() )
    {
      QCOMPARE( actual.isEmpty(), expected.isEmpty() );
    }
    else
    {
      QCOMPARE( actual.size(), expected.size() );

      for ( int z = 0; z < actual.size(); ++z )
      {
        QVERIFY( InputUtils::equals( actual.at( z ), expected.at( z ) ) );
      }
    }
  }
}

void TestUtilsFunctions::testFormatDistanceInProjectUnit()
{
  QgsProject *project = QgsProject::instance();
  QVERIFY( project != nullptr );

  // Set the project distance units to meters
  project->setDistanceUnits( Qgis::DistanceUnit::Meters );

  QString dist2str = mUtils->formatDistanceInProjectUnit( 1222.234, 2, project );
  QVERIFY( dist2str == "1222.23 m" );

  dist2str = mUtils->formatDistanceInProjectUnit( 1222.234, 1, project );
  QVERIFY( dist2str == "1222.2 m" );

  dist2str = mUtils->formatDistanceInProjectUnit( 1222.234, 0, project );
  QVERIFY( dist2str == "1222 m" );

  dist2str = mUtils->formatDistanceInProjectUnit( 700.22, 1, project );
  QVERIFY( dist2str == "700.2 m" );

  dist2str = mUtils->formatDistanceInProjectUnit( 0.22, 0, project );
  QVERIFY( dist2str == "0 m" );

  dist2str = mUtils->formatDistanceInProjectUnit( -0.22, 0, project );
  QVERIFY( dist2str == "-0 m" );

  // Change project distance units to kilometers
  project->setDistanceUnits( Qgis::DistanceUnit::Kilometers );

  dist2str = mUtils->formatDistanceInProjectUnit( 1.222234, 2, project );
  QVERIFY( dist2str == "0.00 km" );

  // Change project distance units to feet
  project->setDistanceUnits( Qgis::DistanceUnit::Feet );

  dist2str = mUtils->formatDistanceInProjectUnit( 6000, 1, project );
  QVERIFY( dist2str == "19685.0 ft" );

  dist2str = mUtils->formatDistanceInProjectUnit( 5, 1, project );
  QVERIFY( dist2str == "16.4 ft" );

  dist2str = mUtils->formatDistanceInProjectUnit( 7000, 1, project );
  QVERIFY( dist2str == "22965.9 ft" );
}


void TestUtilsFunctions::testFormatAreaInProjectUnit()
{
  QgsProject *project = QgsProject::instance();
  QVERIFY( project != nullptr );

  // Set project area units to square meters
  project->setAreaUnits( Qgis::AreaUnit::SquareMeters );

  QString area2str = mUtils->formatAreaInProjectUnit( 1500.234, 2, project );
  QVERIFY( area2str == "1500.23 m²" );

  area2str = mUtils->formatAreaInProjectUnit( 1500.234, 1, project );
  QVERIFY( area2str == "1500.2 m²" );

  area2str = mUtils->formatAreaInProjectUnit( 1500.234, 0, project );
  QVERIFY( area2str == "1500 m²" );

  area2str = mUtils->formatAreaInProjectUnit( 500.22, 1, project );
  QVERIFY( area2str == "500.2 m²" );

  area2str = mUtils->formatAreaInProjectUnit( 0.22, 0, project );
  QVERIFY( area2str == "0 m²" );

  area2str = mUtils->formatAreaInProjectUnit( -0.22, 0, project );
  QVERIFY( area2str == "-0 m²" );

  // Change project area units to square kilometers
  project->setAreaUnits( Qgis::AreaUnit::SquareKilometers );

  area2str = mUtils->formatAreaInProjectUnit( 1.222234, 2, project );
  QVERIFY( area2str == "0.00 km²" );

  // Change project area units to acres
  project->setAreaUnits( Qgis::AreaUnit::Acres );

  area2str = mUtils->formatAreaInProjectUnit( 6000, 1, project );
  QVERIFY( area2str == "1.5 ac" );

  area2str = mUtils->formatAreaInProjectUnit( 5, 1, project );
  QVERIFY( area2str == "0.0 ac" );

  area2str = mUtils->formatAreaInProjectUnit( 7000, 1, project );
  QVERIFY( area2str == "1.7 ac" );
}

void TestUtilsFunctions::testRelevantGeometryCenterToScreenCoordinates()
{
  InputMapSettings ms;
  ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 3857 ) );
  ms.setOutputSize( QSize( 436, 690 ) );
  QgsGeometry geom = QgsGeometry::fromWkt( "LineString (605540.02427726075984538 5422974.88796170614659786, 618450.11232534842565656 5430064.85434877127408981, 631042.73919192561879754 5418953.71299590915441513, 652418.458746955730021 5431228.87868097703903913)" );
  double epsilon = 0.1;

  // Case when the geometry can fully be contained within the extent
  ms.setExtent( QgsRectangle( 595290, 5.35402e+06, 661796, 5.45927e+06 ) );
  QCOMPARE( InputUtils::equals( mUtils->relevantGeometryCenterToScreenCoordinates( geom, &ms ), QPointF( 220.861, 224.065 ), epsilon ), true );

  // Case when we cut the geometry to current extent
  ms.setExtent( QgsRectangle( 599032, 5.40671e+06, 619818, 5.43961e+06 ) );
  QCOMPARE( InputUtils::equals( mUtils->relevantGeometryCenterToScreenCoordinates( geom, &ms ), QPointF( 286.257, 274.5 ), epsilon ), true );
}

void TestUtilsFunctions::testIsValidEmail()
{
  // valid emails
  QVERIFY( InputUtils::isValidEmail( "name@email.com" ) );
  QVERIFY( InputUtils::isValidEmail( "name.surname@email.com" ) );
  QVERIFY( InputUtils::isValidEmail( "name_sur-name+1@email.com" ) );

  // invalid emails
  QVERIFY( !InputUtils::isValidEmail( "broken@email" ) );
  QVERIFY( !InputUtils::isValidEmail( "@email.com" ) );
  QVERIFY( !InputUtils::isValidEmail( "brokenemail.com" ) );
  QVERIFY( !InputUtils::isValidEmail( "brokenemail" ) );
  QVERIFY( !InputUtils::isValidEmail( "" ) );
}

void TestUtilsFunctions::testSanitizeFileName()
{
  // unchanged
  QString str = QStringLiteral( "/simple/valid/filename.ext" );
  InputUtils::sanitizeFileName( str );
  QCOMPARE( str, QStringLiteral( "/simple/valid/filename.ext" ) );

  // unchanged
  str = QStringLiteral( "/complex/valid/Φ!l@#äme$%^&()-_=+[]{}`~;',.ext" );
  InputUtils::sanitizeFileName( str );
  QCOMPARE( str, QStringLiteral( "/complex/valid/Φ!l@#äme$%^&()-_=+[]{}`~;',.ext" ) );

  // sanitized
  str = QStringLiteral( "/sa ni*tized/f<i>l?n\"a:m|e .ext " );
  InputUtils::sanitizeFileName( str );
  QCOMPARE( str, QStringLiteral( "/sa ni_tized/f_i_l_n_a_m_e.ext" ) );

  // sanitized
  str = QStringLiteral( "/sa ni*tized/.f<i>l?n\"a:m|e .co .ext " );
  InputUtils::sanitizeFileName( str );
  QCOMPARE( str, QStringLiteral( "/sa ni_tized/.f_i_l_n_a_m_e .co.ext" ) );
}

void TestUtilsFunctions::testSanitizePath()
{
  // unchanged
  QString str = QStringLiteral( "/simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "/simple/valid/filename.ext" ) );

  // unchanged - no leading slash
  str = QStringLiteral( "simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "simple/valid/filename.ext" ) );

  // unchanged - url prefix
  str = QStringLiteral( "file://simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "file://simple/valid/filename.ext" ) );

  // unchanged - url prefix with slash
  str = QStringLiteral( "file:///simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "file:///simple/valid/filename.ext" ) );

  // unchanged
  str = QStringLiteral( "/complex/valid/Φ!l@#äme$%^&()-_=+[]{}`~;',.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "/complex/valid/Φ!l@#äme$%^&()-_=+[]{}`~;',.ext" ) );

  // unchanged with partition letter on Windows
  str = QStringLiteral( "C:/Users/simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "C:/Users/simple/valid/filename.ext" ) );

  // sanitized
  str = QStringLiteral( "/sa ni*tized/f<i>l?n\"a:m|e.ext " );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "/sa ni_tized/f_i_l_n_a_m_e.ext" ) );

  // sanitized
  str = QStringLiteral( "  /  sa ni*tized / f<i>l?n\"a:m|e.ext " );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "/sa ni_tized/f_i_l_n_a_m_e.ext" ) );

  // sanitized
  str = QStringLiteral( "file:///  sa ni*tized /f<i>l?n\"a:m|e .ext " );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "file:///sa ni_tized/f_i_l_n_a_m_e.ext" ) );

  // sanitized
  str = QStringLiteral( "project name / project .qgz " );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "project name/project.qgz" ) );

  // sanitized with partition letter
  str = QStringLiteral( "C:/project name / project .qgz " );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "C:/project name/project.qgz" ) );
}
