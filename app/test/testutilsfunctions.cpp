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

#include "testutils.h"

#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QTemporaryDir>
#include <QLocale>

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
  QVector<QPair< QgsMapLayer *, QString > > testcases =
  {
    { QgsMemoryProviderUtils::createMemoryLayer( "P1", QgsFields(), Qgis::WkbType::Point ), QStringLiteral( "qrc:/mIconPointLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "P2", QgsFields(), Qgis::WkbType::PointZ ), QStringLiteral( "qrc:/mIconPointLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "P3", QgsFields(), Qgis::WkbType::PointM ), QStringLiteral( "qrc:/mIconPointLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "P4", QgsFields(), Qgis::WkbType::PointZM ), QStringLiteral( "qrc:/mIconPointLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "P5", QgsFields(), Qgis::WkbType::MultiPoint ), QStringLiteral( "qrc:/mIconPointLayer.svg" ) },

    { QgsMemoryProviderUtils::createMemoryLayer( "L1", QgsFields(), Qgis::WkbType::LineString ), QStringLiteral( "qrc:/mIconLineLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "L2", QgsFields(), Qgis::WkbType::LineStringZ ), QStringLiteral( "qrc:/mIconLineLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "L3", QgsFields(), Qgis::WkbType::LineStringM ), QStringLiteral( "qrc:/mIconLineLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "L4", QgsFields(), Qgis::WkbType::LineStringZM ), QStringLiteral( "qrc:/mIconLineLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "L5", QgsFields(), Qgis::WkbType::MultiLineString ), QStringLiteral( "qrc:/mIconLineLayer.svg" ) },

    { QgsMemoryProviderUtils::createMemoryLayer( "PO1", QgsFields(), Qgis::WkbType::Polygon ), QStringLiteral( "qrc:/mIconPolygonLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO2", QgsFields(), Qgis::WkbType::PolygonZ ), QStringLiteral( "qrc:/mIconPolygonLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO3", QgsFields(), Qgis::WkbType::PolygonM ), QStringLiteral( "qrc:/mIconPolygonLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO4", QgsFields(), Qgis::WkbType::PolygonZM ), QStringLiteral( "qrc:/mIconPolygonLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "PO5", QgsFields(), Qgis::WkbType::MultiPolygon ), QStringLiteral( "qrc:/mIconPolygonLayer.svg" ) },

    { QgsMemoryProviderUtils::createMemoryLayer( "N1", QgsFields(), Qgis::WkbType::Unknown ), QStringLiteral( "qrc:/mIconTableLayer.svg" ) },
    { QgsMemoryProviderUtils::createMemoryLayer( "N2", QgsFields(), Qgis::WkbType::NoGeometry ), QStringLiteral( "qrc:/mIconTableLayer.svg" ) },

    { new QgsRasterLayer(), QStringLiteral( "qrc:/mIconRasterLayer.svg" ) }
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

  QVector< QPair< QgsFeature, QString > > testcases_features =
  {
    { pointFeature, QStringLiteral( "qrc:/mIconPointLayer.svg" ) },
    { lineFeature, QStringLiteral( "qrc:/mIconLineLayer.svg" ) },
    { polygonFeature, QStringLiteral( "qrc:/mIconPolygonLayer.svg" ) },
    { emptyFeature, QStringLiteral( "qrc:/mIconTableLayer.svg" ) }
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
