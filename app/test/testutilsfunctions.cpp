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
#include <QSignalSpy>

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
#include "qgslayertree.h"
#include "qgslayertreelayer.h"

#include "mmstyle.h"
#include "inputmapcanvasmap.h"
#include "compass.h"

#include "testutils.h"

#include <QtTest/QtTest>
#include <QtCore/QObject>
#include <QTemporaryDir>
#include <QTranslator>
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
  if ( !QGuiApplication::topLevelWindows().isEmpty() )
  {
    QVERIFY( mUtils->dumpScreenInfo().contains( "screen" ) );
  }
  else
  {
    QCOMPARE( mUtils->dumpScreenInfo(), QStringLiteral( "Application is not initialized!" ) );
  }
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

  QString path3 = QUrl( path2 ).toString();
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
  QString ABSOLUTE_DEFAULT_ROOT( "/absolute/default/root/path" ); // can be not existing path

  QgsProject *activeProject = nullptr;
  QVariantMap config;

  // prepare project to be able evaluate expression
  QString projectDir = homePath + "/planes";
  QgsProject::instance()->read( projectDir + "/quickapp_project.qgs" );
  FeatureLayerPair pair; // Can be dummy pair

  // case 1: empty config, no expression
  QString resultDir = mUtils->resolveTargetDir( homePath, config, pair, FeatureLayerPair(), activeProject );
  QCOMPARE( resultDir, homePath );

  // case 2: defined default root config as a relative path, no expression - resolved against homePath
  config.insert( QStringLiteral( "DefaultRoot" ), DEFAULT_ROOT );
  QString resultDir2 = mUtils->resolveTargetDir( homePath, config, pair, FeatureLayerPair(), activeProject );
  QCOMPARE( resultDir2, QStringLiteral( "%1/%2" ).arg( homePath, DEFAULT_ROOT ) );
  config.clear();

  // case 2b: defined default root config as an already-absolute path, no expression - stays unchanged
  config.insert( QStringLiteral( "DefaultRoot" ), ABSOLUTE_DEFAULT_ROOT );
  QString resultDir2b = mUtils->resolveTargetDir( homePath, config, pair, FeatureLayerPair(), activeProject );
  QCOMPARE( resultDir2b, ABSOLUTE_DEFAULT_ROOT );
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

  QString resultDir3 = mUtils->resolveTargetDir( homePath, config, pair, FeatureLayerPair(), QgsProject::instance() );
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
  positionKit.setPositionProvider( positionKit.constructProvider( "internal", "simulated", "simulated" ) );
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

void TestUtilsFunctions::testWhereToPanWhenIdentifying()
{
  // bottomOffset and the returned screen point are in logical pixels,
  // so the result must not depend on the device pixel ratio
  const QVector<qreal> dprs = { 1.0, 2.0 };
  for ( const qreal dpr : dprs )
  {
    InputMapSettings ms;
    ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 4326 ) );
    ms.setDevicePixelRatio( dpr );
    ms.setOutputSize( QSize( 400, 620 ) );
    ms.setExtent( QgsRectangle( 0, 0, 40, 62 ) ); // 0.1 map units per logical pixel

    const double bottomOffset = 200; // drawer covers screen y in [420, 620] ~ map y in [0, 20]

    // point hidden under the drawer -> map should pan so that the point
    // ends up centered in the unobstructed part of the canvas
    const QgsGeometry hiddenPoint = QgsGeometry::fromPointXY( QgsPointXY( 20, 10 ) );
    QPointF pan = InputUtils::whereToPanWhenIdentifying( hiddenPoint, &ms, bottomOffset, 0, QPointF( 20, 10 ) );
    COMPARENEAR( pan.x(), 200.0, 1e-4 );
    COMPARENEAR( pan.y(), 620.0, 1e-4 );

    // point well inside the unobstructed area -> no pan needed
    const QgsGeometry visiblePoint = QgsGeometry::fromPointXY( QgsPointXY( 20, 40 ) );
    pan = InputUtils::whereToPanWhenIdentifying( visiblePoint, &ms, bottomOffset, 0, QPointF( 20, 40 ) );
    QVERIFY( std::isnan( pan.x() ) );
    QVERIFY( std::isnan( pan.y() ) );
  }

  // rotated map: the visible part is checked on the screen, not in map coordinates
  const QVector<double> rotations = { 45, 90, 180, -135 };
  for ( const double rotation : rotations )
  {
    InputMapSettings ms;
    ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 4326 ) );
    ms.setOutputSize( QSize( 400, 620 ) );
    ms.setExtent( QgsRectangle( 0, 0, 40, 62 ) );
    ms.setRotation( rotation );

    const double bottomOffset = 200; // drawer covers screen y in [420, 620]

    // point well inside the unobstructed area -> no pan needed
    const QgsPoint visibleOnMap = ms.screenToCoordinate( QPointF( 200, 200 ) );
    const QgsGeometry visiblePoint = QgsGeometry::fromPointXY( QgsPointXY( visibleOnMap.x(), visibleOnMap.y() ) );
    QPointF pan = InputUtils::whereToPanWhenIdentifying( visiblePoint, &ms, bottomOffset, 0, QPointF( visibleOnMap.x(), visibleOnMap.y() ) );
    QVERIFY2( std::isnan( pan.x() ) && std::isnan( pan.y() ), qPrintable( QStringLiteral( "rotation %1" ).arg( rotation ) ) );

    // point hidden under the drawer -> pan so that it is centered in the unobstructed part
    const QgsPoint hiddenOnMap = ms.screenToCoordinate( QPointF( 200, 520 ) );
    const QgsGeometry hiddenPoint = QgsGeometry::fromPointXY( QgsPointXY( hiddenOnMap.x(), hiddenOnMap.y() ) );
    pan = InputUtils::whereToPanWhenIdentifying( hiddenPoint, &ms, bottomOffset, 0, QPointF( hiddenOnMap.x(), hiddenOnMap.y() ) );
    COMPARENEAR( pan.x(), 200.0, 1e-4 );
    COMPARENEAR( pan.y(), 620.0, 1e-4 );
  }
}

void TestUtilsFunctions::testDrawerCompensatedExtent()
{
  // bottomOffset is in logical pixels, so the result must not depend on the device pixel ratio
  const QVector<qreal> dprs = { 1.0, 2.0 };
  for ( const qreal dpr : dprs )
  {
    InputMapSettings ms;
    ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 4326 ) );
    ms.setDevicePixelRatio( dpr );
    ms.setOutputSize( QSize( 400, 620 ) );
    ms.setExtent( QgsRectangle( 0, 0, 40, 62 ) ); // 0.1 map units per logical pixel

    const double bottomOffset = 200; // drawer covers screen y in [420, 620]

    // point geometry -> scale is kept, center shifts down by bottomOffset / 2
    // in screen space so the point is centered in the unobstructed part
    const QgsGeometry point = QgsGeometry::fromPointXY( QgsPointXY( 20, 10 ) );
    QgsRectangle extent = InputUtils::drawerCompensatedExtent( point, &ms, bottomOffset );
    COMPARENEAR( extent.width(), 40.0, 1e-4 );
    COMPARENEAR( extent.height(), 62.0, 1e-4 );
    COMPARENEAR( extent.center().x(), 20.0, 1e-4 );
    COMPARENEAR( extent.center().y(), 0.0, 1e-4 );

    // non-empty bounding box -> zoom so the padded bbox fits the part of the
    // canvas not covered by the drawer, centered in it
    const QgsGeometry line = QgsGeometry::fromPolylineXY( { QgsPointXY( 10, 10 ), QgsPointXY( 30, 20 ) } );
    extent = InputUtils::drawerCompensatedExtent( line, &ms, bottomOffset );

    // padded bbox is 23.6 x 11.8 -> 0.059 map units per logical pixel to fit 400 x 420
    COMPARENEAR( extent.width(), 400 * 0.059, 1e-4 );
    COMPARENEAR( extent.height(), 620 * 0.059, 1e-4 );
    COMPARENEAR( extent.center().x(), 20.0, 1e-4 );
    COMPARENEAR( extent.center().y(), 15.0 - bottomOffset / 2.0 * 0.059, 1e-4 );
  }

  // rotated map: the geometry ends up centered in the unobstructed part of the screen, the scale of a point is kept
  const QVector<double> rotations = { 30, 90, 180, -135 };
  for ( const double rotation : rotations )
  {
    const QString message = QStringLiteral( "rotation %1" ).arg( rotation );

    InputMapSettings ms;
    ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 4326 ) );
    ms.setOutputSize( QSize( 400, 620 ) );
    ms.setExtent( QgsRectangle( 0, 0, 40, 62 ) );
    ms.setRotation( rotation );

    const double bottomOffset = 200; // drawer covers screen y in [420, 620], the unobstructed part is centered at y 210

    const QgsGeometry point = QgsGeometry::fromPointXY( QgsPointXY( 20, 10 ) );
    QgsRectangle extent = InputUtils::drawerCompensatedExtent( point, &ms, bottomOffset );
    QVERIFY2( qgsDoubleNear( extent.width(), 40.0, 1e-4 ), qPrintable( message ) ); // not zoomed out
    QVERIFY2( qgsDoubleNear( extent.height(), 62.0, 1e-4 ), qPrintable( message ) );

    ms.setExtent( extent );
    QPointF screenPt = ms.coordinateToScreen( QgsPoint( 20, 10 ) );
    QVERIFY2( qgsDoubleNear( screenPt.x(), 200.0, 1e-3 ), qPrintable( message ) );
    QVERIFY2( qgsDoubleNear( screenPt.y(), 210.0, 1e-3 ), qPrintable( message ) );

    // the same extent again does not zoom out (it used to grow with every selection)
    extent = InputUtils::drawerCompensatedExtent( point, &ms, bottomOffset );
    QVERIFY2( qgsDoubleNear( extent.width(), 40.0, 1e-4 ), qPrintable( message ) );

    // a line fits into the unobstructed part, centered in it
    const QgsGeometry line = QgsGeometry::fromPolylineXY( { QgsPointXY( 10, 10 ), QgsPointXY( 30, 20 ) } );
    extent = InputUtils::drawerCompensatedExtent( line, &ms, bottomOffset );
    ms.setExtent( extent );

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();
    for ( const QgsPoint &corner : { QgsPoint( 10, 10 ), QgsPoint( 10, 20 ), QgsPoint( 30, 10 ), QgsPoint( 30, 20 ) } )
    {
      const QPointF pt = ms.coordinateToScreen( corner );
      minX = std::min( minX, pt.x() );
      minY = std::min( minY, pt.y() );
      maxX = std::max( maxX, pt.x() );
      maxY = std::max( maxY, pt.y() );
    }
    QVERIFY2( minX >= -1e-3 && maxX <= 400 + 1e-3 && minY >= -1e-3 && maxY <= 420 + 1e-3, qPrintable( message ) );
    QVERIFY2( qgsDoubleNear( ( minX + maxX ) / 2, 200.0, 1e-3 ), qPrintable( message ) );
    QVERIFY2( qgsDoubleNear( ( minY + maxY ) / 2, 210.0, 1e-3 ), qPrintable( message ) );
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
  QCOMPARE( str, QStringLiteral( "file:///simple/valid/filename.ext" ) );

  // unchanged - url prefix with slash
  str = QStringLiteral( "file:///simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "file:///simple/valid/filename.ext" ) );

  // unchanged
  str = QStringLiteral( "/complex/valid/Φ!l@#äme$%^&()-_=+[]{}`~;',.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "/complex/valid/Φ!l@_äme$%^&()-_=+[]{}`~;',.ext" ) );

  // unchanged with partition letter on Windows
  str = QStringLiteral( "C:/Users/simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "C:/Users/simple/valid/filename.ext" ) );

  // unchanged with partition letter on Windows and file prefix
  str = QStringLiteral( "file:///C:/Users/simple/valid/filename.ext" );
  InputUtils::sanitizePath( str );
  QCOMPARE( str, QStringLiteral( "file:///C:/Users/simple/valid/filename.ext" ) );

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

void TestUtilsFunctions::testUniqueString()
{
  const QStringList similarStrings( {QStringLiteral( "lutra" ), QStringLiteral( "lutra (1)" ), QStringLiteral( "lutra (256)" ), QStringLiteral( "lutra (257)" )} );

  const QString testString1( QStringLiteral( "lutra" ) );
  QString result = InputUtils::getUniqueString( testString1, similarStrings );
  QCOMPARE( result, QStringLiteral( "lutra (2)" ) );

  const QString testString2( QStringLiteral( "lutra            " ) );
  result = InputUtils::getUniqueString( testString2, similarStrings );
  QCOMPARE( result, QStringLiteral( "lutra (2)" ) );

  const QString testString3( QStringLiteral( "            lutra" ) );
  result = InputUtils::getUniqueString( testString3, similarStrings );
  QCOMPARE( result, QStringLiteral( "lutra (2)" ) );

  const QString testString4( QStringLiteral( "lutra (1)" ) );
  result = InputUtils::getUniqueString( testString4, similarStrings );
  QCOMPARE( result, QStringLiteral( "lutra (2)" ) );

  const QString testString5( QStringLiteral( "lutra (256)" ) );
  result = InputUtils::getUniqueString( testString5, similarStrings );
  QCOMPARE( result, QStringLiteral( "lutra (258)" ) );

  const QString testString6( QStringLiteral( "mulutram" ) );
  result = InputUtils::getUniqueString( testString6, similarStrings );
  QCOMPARE( result, QStringLiteral( "mulutram" ) );
}

void TestUtilsFunctions::testLayerHasGeometry()
{
  // null layer => should be false
  QCOMPARE( InputUtils::layerHasGeometry( nullptr ), false );

  // invalid layer => should be false
  QgsVectorLayer *invalidLayer = new QgsVectorLayer( "", "InvalidLayer", "none" );
  QVERIFY( invalidLayer->isValid() == false );
  QCOMPARE( InputUtils::layerHasGeometry( invalidLayer ), false );
  delete invalidLayer;

  // valid memory layer with geometry
  QgsVectorLayer *pointLayer = new QgsVectorLayer( "Point?crs=EPSG:4326", "ValidPointLayer", "memory" );
  QVERIFY( pointLayer->isValid() );
  QCOMPARE( InputUtils::layerHasGeometry( pointLayer ), true );

  // layer with NoGeo => should be false
  QgsVectorLayer *noGeomLayer = new QgsVectorLayer( "None", "NoGeometryLayer", "memory" );
  QVERIFY( noGeomLayer->isValid() );
  QCOMPARE( InputUtils::layerHasGeometry( noGeomLayer ), false );

  delete pointLayer;
  delete noGeomLayer;
}

void TestUtilsFunctions::testLayerVisible()
{
  QgsProject *project = new QgsProject();
  project->clear();

  // null layer => should be false
  QCOMPARE( InputUtils::isLayerVisible( nullptr, project ), false );

  // valid memory layer
  QgsVectorLayer *layer = new QgsVectorLayer( "LineString?crs=EPSG:4326", "VisibleLineLayer", "memory" );
  QVERIFY( layer->isValid() );

  // won't appear in the layer tree => false
  QCOMPARE( InputUtils::isLayerVisible( layer, project ), false );

  // added to project => true
  project->addMapLayer( layer );
  QCOMPARE( InputUtils::isLayerVisible( layer, project ), true );

  // hide layer => false
  QgsLayerTree *root = project->layerTreeRoot();
  QgsLayerTreeLayer *layerTree = root->findLayer( layer );
  QVERIFY( layerTree );
  layerTree->setItemVisibilityChecked( false );
  QCOMPARE( InputUtils::isLayerVisible( layer, project ), false );

  delete project;
}

void TestUtilsFunctions::testIsPositionTrackingLayer()
{
  QCOMPARE( InputUtils::isPositionTrackingLayer( nullptr, nullptr ), false );

  QgsProject *project = new QgsProject();
  QgsVectorLayer *layer = new QgsVectorLayer( "Point?crs=EPSG:4326", "TrackingLayer", "memory" );
  project->addMapLayer( layer );
  QCOMPARE( InputUtils::isPositionTrackingLayer( layer, project ), false );

  // tracking layer ID => true
  QString layerId = layer->id();
  project->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), layerId );
  QCOMPARE( InputUtils::isPositionTrackingLayer( layer, project ), true );

  // not tracking layer ID => false
  project->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), QString( "some-other-id" ) );
  QCOMPARE( InputUtils::isPositionTrackingLayer( layer, project ), false );

  delete project;
}

void TestUtilsFunctions::testMapLayerFromName()
{
  QCOMPARE( InputUtils::mapLayerFromName( "Anything", nullptr ), static_cast<QgsMapLayer *>( nullptr ) );

  // empty layerName => nullptr
  QgsProject *project = new QgsProject();
  QCOMPARE( InputUtils::mapLayerFromName( "", project ), static_cast<QgsMapLayer *>( nullptr ) );

  // added a named layer to project and check => should succeed
  QgsVectorLayer *layer = new QgsVectorLayer( "Point?crs=EPSG:4326", "MyTestLayer", "memory" );
  QVERIFY( layer->isValid() );
  project->addMapLayer( layer );
  QgsMapLayer *found = InputUtils::mapLayerFromName( "MyTestLayer", project );
  QVERIFY( found != nullptr );
  QCOMPARE( found->name(), QString( "MyTestLayer" ) );

  // non-existing name => nullptr
  QCOMPARE( InputUtils::mapLayerFromName( "NoSuchName", project ), static_cast<QgsMapLayer *>( nullptr ) );

  delete project;
}

void TestUtilsFunctions::testIsValidUrl()
{
  // valid urls
  QVERIFY( InputUtils::isValidUrl( "http://www.example.com" ) );
  QVERIFY( InputUtils::isValidUrl( "https://example.com/path?query=1" ) );
  QVERIFY( InputUtils::isValidUrl( "ftp://ftp.example.com/resource" ) );
  QVERIFY( InputUtils::isValidUrl( "www.example.com" ) );

  // invalid urls
  // QVERIFY( !InputUtils::isValidUrl( "htp://www.example.com" ) );
  // QVERIFY( !InputUtils::isValidUrl( "http//missingcolon.com" ) );
  QVERIFY( !InputUtils::isValidUrl( "://example.com" ) );
  QVERIFY( !InputUtils::isValidUrl( "http://exa mple.com" ) );
  QVERIFY( !InputUtils::isValidUrl( "" ) ); // empty url is considered valid by QUrl but not by us
}

void TestUtilsFunctions::testParentLocale()
{
  QCOMPARE( InputUtils::parentLocale( QLocale( "pt_AO" ) ), QStringLiteral( "pt_PT" ) );
  QCOMPARE( InputUtils::parentLocale( QLocale( "pt_MZ" ) ), QStringLiteral( "pt_PT" ) );
  QCOMPARE( InputUtils::parentLocale( QLocale( "es_MX" ) ), QStringLiteral( "es_419" ) );
  QCOMPARE( InputUtils::parentLocale( QLocale( "es_US" ) ), QStringLiteral( "es_419" ) );

  // locales that are their own parent or have no parent we ship
  QVERIFY( InputUtils::parentLocale( QLocale( "pt_BR" ) ).isEmpty() );
  QVERIFY( InputUtils::parentLocale( QLocale( "pt_PT" ) ).isEmpty() );
  QVERIFY( InputUtils::parentLocale( QLocale( "es_ES" ) ).isEmpty() );
  QVERIFY( InputUtils::parentLocale( QLocale( "en_US" ) ).isEmpty() );
}

void TestUtilsFunctions::testTranslationFilePath_data()
{
  // uiLanguages are lists as returned by QLocale::uiLanguages() on macOS/iOS and Android
  // for the given device languages (and region), Android lists are shortened
  QTest::addColumn<QStringList>( "uiLanguages" );
  QTest::addColumn<QString>( "expected" );

  // a later language must not win over the base form of a more preferred one (#4524)
  QTest::newRow( "en-GB, sk-SK [region SK]" )
      << QStringList { "en-Latn-GB", "en-GB", "en-Latn-SK", "en-SK", "sk-Latn-SK", "sk-SK", "sk-Latn", "sk", "en-Latn", "en" }
      << "en";
  QTest::newRow( "en-GB, sk-SK (Android)" )
      << QStringList { "en-Latn-GB", "en-GB", "sk-Latn-SK", "sk-SK", "sk-Latn", "sk", "en-AU-u-fw-mon-mu-celsius", "en-Latn-AU", "en-AU-u-fw-mon-mu", "en-AU-u", "en-AU", "en-Latn", "en" }
      << "en";
  QTest::newRow( "de-AT, en-US (Android)" )
      << QStringList { "de-Latn-AT", "de-AT", "en-Latn-US", "en-US", "en-Latn", "en", "de-Latn-BE", "de-BE", "de-Latn", "de" }
      << "de";
  QTest::newRow( "sk-SK, en-GB" )
      << QStringList { "sk-Latn-SK", "sk-SK", "sk-Latn", "sk", "en-Latn-GB", "en-GB", "en-Latn", "en" }
      << "sk_SK";
  QTest::newRow( "sk-SK-u-mu-celsius, en-GB (Android)" )
      << QStringList { "sk-SK-u-mu-celsius", "sk-Latn-SK", "sk-Latn", "sk", "sk-SK-u-mu", "en-Latn-GB", "en-GB", "en-Latn", "en" }
      << "sk_SK";

  // Traditional Chinese must not get the Simplified Chinese translation
  QTest::newRow( "zh-Hant-TW, en-US" )
      << QStringList { "zh-Hant-TW", "zh-TW", "zh-Hant", "en-Latn-US", "en-US", "en-Latn", "en", "zh" }
      << "en";
  QTest::newRow( "zh-Hant-TW, ja-JP" )
      << QStringList { "zh-Hant-TW", "zh-TW", "zh-Hant", "ja-Jpan-JP", "ja-JP", "ja-Jpan", "ja", "zh" }
      << "ja_JP";
  QTest::newRow( "zh-Hant-TW" )
      << QStringList { "zh-Hant-TW", "zh-TW", "zh-Hant", "zh" }
      << "en";
  QTest::newRow( "zh-Hant-TW [region US]" )
      << QStringList { "zh-Hant-TW", "zh-TW", "zh-Hant", "zh-Hant-US", "zh-US", "zh" }
      << "en";
  QTest::newRow( "zh-TW, en-US (Android)" )
      << QStringList { "zh-Hant-TW", "zh-TW", "zh-Hant", "en-Latn-US", "en-US", "en-Latn", "en", "zh" }
      << "en";
  QTest::newRow( "zh-Hant-HK, en-GB (Android)" )
      << QStringList { "zh-Hant-HK", "zh-HK", "zh-Hant", "en-Latn-GB", "en-GB", "en-AU-u-fw-mon-mu-celsius", "en-Latn-AU", "en-AU", "sk-Latn-SK", "sk-SK", "sk-Latn", "sk", "zh", "en-Latn", "en" }
      << "en";
  QTest::newRow( "zh-Hant-TW, zh-Hans-CN" )
      << QStringList { "zh-Hant-TW", "zh-TW", "zh-Hant", "zh-Hans-CN", "zh-CN", "zh-Hans", "zh" }
      << "zh_CN";
  QTest::newRow( "zh-Hans-CN, en-US" )
      << QStringList { "zh-Hans-CN", "zh-CN", "zh-Hans", "zh", "en-Latn-US", "en-US", "en-Latn", "en" }
      << "zh_CN";
  QTest::newRow( "zh-Hans-SG, en-US [region US]" )
      << QStringList { "zh-Hans-SG", "zh-SG", "zh-Hans-US", "en-Latn-US", "en-US", "en-Latn", "en", "zh-Hans", "zh" }
      << "zh";

  // Cyrillic Azerbaijani must not get the Latin translation
  QTest::newRow( "az-Cyrl-AZ, en-US" )
      << QStringList { "az-Cyrl-AZ", "az-Cyrl", "en-Latn-US", "en-US", "en-Latn", "en", "az" }
      << "en";
  QTest::newRow( "az-Cyrl-AZ" )
      << QStringList { "az-Cyrl-AZ", "az-Cyrl", "az" }
      << "en";
  QTest::newRow( "az-Cyrl-AZ, az-Latn-AZ" )
      << QStringList { "az-Cyrl-AZ", "az-Cyrl", "az-Latn-AZ", "az-AZ", "az-Latn", "az" }
      << "az_AZ";

  // regional variants
  QTest::newRow( "es-MX, es-419" )
      << QStringList { "es-Latn-MX", "es-MX", "es-Latn-419", "es-419", "es-Latn", "es" }
      << "es_419";
  QTest::newRow( "es-MX" )
      << QStringList { "es-Latn-MX", "es-MX", "es-Latn", "es" }
      << "es_419";
  QTest::newRow( "es-ES" )
      << QStringList { "es-Latn-ES", "es-ES", "es-Latn", "es" }
      << "es_ES";
  QTest::newRow( "pt-AO" )
      << QStringList { "pt-Latn-AO", "pt-AO", "pt-Latn", "pt" }
      << "pt_PT";
  QTest::newRow( "pt-BR" )
      << QStringList { "pt-Latn-BR", "pt-BR", "pt-Latn", "pt" }
      << "pt_BR";
  QTest::newRow( "ar-SA" )
      << QStringList { "ar-Arab-SA", "ar-SA", "ar-Arab", "ar" }
      << "ar";
  QTest::newRow( "ar-EG" )
      << QStringList { "ar-Arab-EG", "ar-EG", "ar-Arab", "ar" }
      << "ar_EG";
  QTest::newRow( "ja-US, de-DE [region US]" )
      << QStringList { "ja-Jpan-US", "ja-US", "ja-Jpan", "ja", "de-Latn-DE", "de-DE", "de-Latn", "de" }
      << "ja_JP";
  QTest::newRow( "in-ID, en-US (Android legacy code)" )
      << QStringList { "id-Latn-ID", "id-ID", "id-Latn", "id", "en-Latn-US", "en-US", "en-Latn", "en" }
      << "id_ID";

  // nothing translated
  QTest::newRow( "ru-RU, uk-UA" )
      << QStringList { "ru-Cyrl-RU", "ru-RU", "ru-Cyrl", "ru", "uk-Cyrl-UA", "uk-UA", "uk-Cyrl", "uk" }
      << "en";
  QTest::newRow( "empty" ) << QStringList() << "en";
  QTest::newRow( "invalid tags" ) << QStringList { "", "C", "xx-YY" } << "en";
}

void TestUtilsFunctions::testTranslationFilePath()
{
  QFETCH( QStringList, uiLanguages );
  QFETCH( QString, expected );

  // fixed set of translation files, so the test does not depend on translations we currently ship
  const QStringList translations =
  {
    "ar", "ar_EG", "az", "az_AZ", "de", "en", "es", "es_419", "es_ES", "id_ID", "ja_JP",
    "pt_BR", "pt_PT", "sk", "sk_SK", "zh", "zh_CN"
  };

  QTemporaryDir dir;
  QVERIFY( dir.isValid() );
  for ( const QString &translation : translations )
  {
    QFile file( dir.filePath( QStringLiteral( "input_%1.qm" ).arg( translation ) ) );
    QVERIFY( file.open( QIODevice::WriteOnly ) );
  }

  const QString path = InputUtils::translationFilePath( uiLanguages, dir.path() );
  QCOMPARE( path, dir.filePath( QStringLiteral( "input_%1.qm" ).arg( expected ) ) );
}

void TestUtilsFunctions::testTranslationFilePathWithoutEnglish()
{
  QTemporaryDir dir;
  QVERIFY( dir.isValid() );
  QFile file( dir.filePath( QStringLiteral( "input_sk.qm" ) ) );
  QVERIFY( file.open( QIODevice::WriteOnly ) );
  file.close();

  QCOMPARE( InputUtils::translationFilePath( { "sk-Latn-SK", "sk-SK", "sk-Latn", "sk" }, dir.path() ), file.fileName() );
  QVERIFY( InputUtils::translationFilePath( { "de-Latn-DE", "de-DE", "de-Latn", "de" }, dir.path() ).isEmpty() );
}

void TestUtilsFunctions::testTranslationFromResources()
{
  // the translations bundled in the app resources can be resolved and loaded
  const QString path = InputUtils::translationFilePath( { "en-Latn-GB", "en-GB", "en-Latn-SK", "en-SK", "sk-Latn-SK", "sk-SK", "sk-Latn", "sk", "en-Latn", "en" } );
  QCOMPARE( path, QStringLiteral( ":/input_en.qm" ) );

  QTranslator translator;
  QVERIFY( translator.load( InputUtils::translationFilePath( { "sk-Latn-SK", "sk-SK", "sk-Latn", "sk" } ) ) );
  QVERIFY( !translator.isEmpty() );
  QCOMPARE( translator.filePath(), QStringLiteral( ":/input_sk_SK.qm" ) );
}


void TestUtilsFunctions::testMapRotation()
{
  InputMapSettings ms;
  QCOMPARE( ms.rotation(), 0.0 );

  QSignalSpy spy( &ms, &InputMapSettings::rotationChanged );

  // the rotation is normalized to (-180, 180]
  ms.setRotation( 190 );
  QCOMPARE( ms.rotation(), -170.0 );
  QCOMPARE( spy.count(), 1 );

  ms.setRotation( 350 );
  QCOMPARE( ms.rotation(), -10.0 );

  ms.setRotation( -180 );
  QCOMPARE( ms.rotation(), 180.0 );

  ms.setRotation( 360 );
  QCOMPARE( ms.rotation(), 0.0 );

  ms.setRotation( 720 + 45 );
  QCOMPARE( ms.rotation(), 45.0 );

  // floating point noise around north is dropped
  ms.setRotation( 1e-9 );
  QCOMPARE( ms.rotation(), 0.0 );

  // setting the same rotation again does not emit
  spy.clear();
  ms.setRotation( 0 );
  QCOMPARE( spy.count(), 0 );
  ms.setRotation( 30 );
  ms.setRotation( 30 );
  QCOMPARE( spy.count(), 1 );
}

void TestUtilsFunctions::testSetExtentToGeomRotated()
{
  InputMapSettings ms;
  ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 4326 ) );
  ms.setOutputSize( QSize( 400, 400 ) );
  ms.setExtent( QgsRectangle( 0, 0, 100, 100 ) );

  // a box 10 wide and 20 high, centered in (5, 10)
  const QgsGeometry geom = QgsGeometry::fromRect( QgsRectangle( 0, 0, 10, 20 ) );
  constexpr double SCALE_FACTOR = 1.18;

  // not rotated: the extent fits the box
  mUtils->setExtentToGeom( geom, &ms );
  QVERIFY( qgsDoubleNear( ms.extent().width(), 10 * SCALE_FACTOR, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().height(), 20 * SCALE_FACTOR, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().center().x(), 5, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().center().y(), 10, 1e-6 ) );

  // rotated by 90 degrees: the box looks 20 wide and 10 high on the screen, so the extent is swapped
  ms.setRotation( 90 );
  mUtils->setExtentToGeom( geom, &ms );
  QVERIFY( qgsDoubleNear( ms.extent().width(), 20 * SCALE_FACTOR, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().height(), 10 * SCALE_FACTOR, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().center().x(), 5, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().center().y(), 10, 1e-6 ) );

  // a point only moves the center, the scale stays
  const double width = ms.extent().width();
  mUtils->setExtentToGeom( QgsGeometry::fromPointXY( QgsPointXY( 50, 60 ) ), &ms );
  QVERIFY( qgsDoubleNear( ms.extent().width(), width, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().center().x(), 50, 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms.extent().center().y(), 60, 1e-6 ) );
}

void TestUtilsFunctions::testMapSettingsTransform()
{
  // without map settings we get the identity
  const QVariantList identity = mUtils->mapSettingsTransform( nullptr );
  QCOMPARE( identity, QVariantList( { 1, 0, 0, 1, 0, 0 } ) );

  InputMapSettings ms;
  ms.setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 3857 ) );
  ms.setOutputSize( QSize( 400, 800 ) );

  auto verifyTransform = [&]( const QgsPointXY & center, double mapUnitsPerPixel, double rotation ) -> bool
  {
    const double halfWidth = 200 * mapUnitsPerPixel;
    const double halfHeight = 400 * mapUnitsPerPixel;
    ms.setRotation( rotation );
    ms.setExtent( QgsRectangle( center.x() - halfWidth, center.y() - halfHeight, center.x() + halfWidth, center.y() + halfHeight ) );

    const QVariantList t = mUtils->mapSettingsTransform( &ms );
    if ( t.count() != 6 )
      return false;

    // points around the map (the center, the corners and a point out of the view)
    const QList<QgsPointXY> points =
    {
      center,
      QgsPointXY( center.x() - halfWidth, center.y() - halfHeight ),
      QgsPointXY( center.x() + halfWidth, center.y() + halfHeight ),
      QgsPointXY( center.x() - halfWidth, center.y() + halfHeight ),
      QgsPointXY( center.x() + 3 * halfWidth, center.y() - 2 * halfHeight )
    };

    for ( const QgsPointXY &point : points )
    {
      const QPointF expected = ms.coordinateToScreen( QgsPoint( point ) );
      const double x = t[0].toDouble() * point.x() + t[1].toDouble() * point.y() + t[4].toDouble();
      const double y = t[2].toDouble() * point.x() + t[3].toDouble() * point.y() + t[5].toDouble();

      if ( !qgsDoubleNear( x, expected.x(), 0.01 ) || !qgsDoubleNear( y, expected.y(), 0.01 ) )
        return false;
    }
    return true;
  };

  QVERIFY( verifyTransform( QgsPointXY( 1000, 2000 ), 1, 0 ) );
  QVERIFY( verifyTransform( QgsPointXY( 1000, 2000 ), 1, 30 ) );
  QVERIFY( verifyTransform( QgsPointXY( 1000, 2000 ), 0.5, -135 ) );
  QVERIFY( verifyTransform( QgsPointXY( 1000, 2000 ), 2, 180 ) );

  // big coordinates and a map zoomed in a lot, the transform has to stay precise
  QVERIFY( verifyTransform( QgsPointXY( 19000000, 6500000 ), 0.003, 0 ) );
  QVERIFY( verifyTransform( QgsPointXY( 19000000, 6500000 ), 0.003, 77 ) );
  QVERIFY( verifyTransform( QgsPointXY( -19000000, -6500000 ), 0.003, -20 ) );
}

void TestUtilsFunctions::testMapCanvasRotate()
{
  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  ms->setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 3857 ) );
  ms->setOutputSize( QSize( 400, 800 ) );
  ms->setExtent( QgsRectangle( 0, 0, 400, 800 ) );

  QSignalSpy rotationSpy( ms, &InputMapSettings::rotationChanged );
  QSignalSpy extentSpy( ms, &InputMapSettings::extentChanged );
  QSignalSpy visibleExtentSpy( ms, &InputMapSettings::visibleExtentChanged );

  // the map point under the pivot stays under it
  const QPointF pivot( 120, 610 );
  const QgsPoint pivotOnMap = ms->screenToCoordinate( pivot );

  canvas.rotate( pivot, 40 );
  QCOMPARE( ms->rotation(), 40.0 );
  QCOMPARE( rotationSpy.count(), 1 );
  QCOMPARE( extentSpy.count(), 1 );
  QCOMPARE( visibleExtentSpy.count(), 1 ); // listeners are updated once per rotation step

  // nothing changes, nothing is announced (e.g. the fingers hold the map snapped to north)
  canvas.rotate( pivot, 0 );
  QCOMPARE( rotationSpy.count(), 1 );
  QCOMPARE( extentSpy.count(), 1 );
  QCOMPARE( visibleExtentSpy.count(), 1 );

  QPointF pivotNow = ms->coordinateToScreen( pivotOnMap );
  QVERIFY( qgsDoubleNear( pivotNow.x(), pivot.x(), 1e-6 ) );
  QVERIFY( qgsDoubleNear( pivotNow.y(), pivot.y(), 1e-6 ) );

  // the scale is not changed by the rotation
  QVERIFY( qgsDoubleNear( ms->mapUnitsPerPixel(), 1, 1e-9 ) );

  // the angle is normalized
  canvas.rotate( pivot, 150 );
  QCOMPARE( ms->rotation(), -170.0 );

  pivotNow = ms->coordinateToScreen( pivotOnMap );
  QVERIFY( qgsDoubleNear( pivotNow.x(), pivot.x(), 1e-6 ) );
  QVERIFY( qgsDoubleNear( pivotNow.y(), pivot.y(), 1e-6 ) );

  // rotation around the center of the map does not move the center
  const QgsPoint center = ms->center();
  canvas.rotate( QPointF( 200, 400 ), 33 );
  QVERIFY( qgsDoubleNear( ms->center().x(), center.x(), 1e-6 ) );
  QVERIFY( qgsDoubleNear( ms->center().y(), center.y(), 1e-6 ) );

  // rotation set directly still updates the visible extent
  visibleExtentSpy.clear();
  ms->setRotation( 50 );
  QCOMPARE( visibleExtentSpy.count(), 1 );
}

void TestUtilsFunctions::testMapCanvasRotationRenderRest()
{
  // the pause is as long as the last rendering
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 300, 5, 0 ), qint64( 300 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 300, 5, 100 ), qint64( 200 ) );

  // the pause is over
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 300, 5, 300 ), qint64( 0 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 300, 5, 5000 ), qint64( 0 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 0, 0, 0 ), qint64( 0 ) );

  // the pause after a very long rendering is limited
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 2000, 5, 0 ), qint64( 2000 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 9000, 5, 0 ), qint64( 2000 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 9000, 5, 1500 ), qint64( 500 ) );

  // the start of the rendering blocked the UI, the pause is at least 1 s
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 300, 29, 0 ), qint64( 300 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 300, 30, 0 ), qint64( 1000 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 300, 30, 400 ), qint64( 600 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 1500, 30, 0 ), qint64( 1500 ) );
  QCOMPARE( InputMapCanvasMap::rotationRenderRest( 9000, 160, 0 ), qint64( 2000 ) );
}

void TestUtilsFunctions::testMapCanvasRotationBreak()
{
  // angles are compared the shorter way around the circle
  QCOMPARE( InputMapCanvasMap::angleDistance( 10, 4 ), 6.0 );
  QCOMPARE( InputMapCanvasMap::angleDistance( 4, 10 ), 6.0 );
  QCOMPARE( InputMapCanvasMap::angleDistance( 179, -179 ), 2.0 );
  QCOMPARE( InputMapCanvasMap::angleDistance( 350, 10 ), 20.0 );
  QCOMPARE( InputMapCanvasMap::angleDistance( 0, 180 ), 180.0 );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  ms->setDestinationCrs( QgsCoordinateReferenceSystem::fromEpsgId( 3857 ) );
  ms->setOutputSize( QSize( 400, 800 ) );
  ms->setExtent( QgsRectangle( 0, 0, 400, 800 ) );
  const QPointF center( 200, 400 );

  canvas.setRenderAfterBreak( true );
  QVERIFY( !canvas.mRotationBreakTimer.isActive() ); // nothing to render yet

  // a turn starts a break
  canvas.rotate( center, 10 );
  QVERIFY( canvas.mRotationBreakTimer.isActive() );
  QCOMPARE( canvas.mRotationBreakStart, 10.0 );

  // a wobble within the break does not start a new one
  canvas.rotate( center, 2 );
  canvas.rotate( center, -4 );
  QCOMPARE( canvas.mRotationBreakStart, 10.0 );

  // a bigger turn starts a new break
  canvas.rotate( center, 10 );
  QCOMPARE( canvas.mRotationBreakStart, 18.0 );

  // across the south too
  canvas.rotate( center, 160 );
  QCOMPARE( canvas.mRotationBreakStart, 178.0 );
  canvas.rotate( center, 3 ); // -179
  QCOMPARE( canvas.mRotationBreakStart, 178.0 );
  canvas.rotate( center, 2 ); // -177
  QCOMPARE( canvas.mRotationBreakStart, -177.0 );

  // at the end of the break the turned map is rendered
  QVERIFY( !canvas.mRotationRenderRequested );
  canvas.onRotationBreak();
  QVERIFY( canvas.mRotationRenderRequested );
  QVERIFY( !canvas.mRotationBreakTimer.isActive() );

  // the other mode does not use the break
  canvas.setRenderAfterBreak( false );
  QVERIFY( !canvas.mRotationBreakTimer.isActive() );
  canvas.rotate( center, 20 );
  QVERIFY( !canvas.mRotationBreakTimer.isActive() );
}

void TestUtilsFunctions::testCompassUserOrientation()
{
  using Reading = QOrientationReading;

  const QList<Reading::Orientation> deviceOrientations =
  {
    Reading::Undefined, Reading::TopUp, Reading::TopDown, Reading::LeftUp, Reading::RightUp, Reading::FaceUp, Reading::FaceDown
  };
  const QList<int> userOrientations = { 0, 90, 180, 270 };

  // UI in the native orientation or upside down: the physical orientation must not change the direction
  for ( const Reading::Orientation deviceOrientation : deviceOrientations )
  {
    for ( const int current : userOrientations )
    {
      QCOMPARE( Compass::userOrientation( Qt::PortraitOrientation, Qt::PortraitOrientation, deviceOrientation, current ), 0 );
      QCOMPARE( Compass::userOrientation( Qt::InvertedPortraitOrientation, Qt::PortraitOrientation, deviceOrientation, current ), 180 );

      QCOMPARE( Compass::userOrientation( Qt::LandscapeOrientation, Qt::LandscapeOrientation, deviceOrientation, current ), 0 );
      QCOMPARE( Compass::userOrientation( Qt::InvertedLandscapeOrientation, Qt::LandscapeOrientation, deviceOrientation, current ), 180 );
    }
  }

  // UI a quarter turn from native: the sensor tells which side
  const QList<QPair<Qt::ScreenOrientation, Qt::ScreenOrientation>> quarters =
  {
    { Qt::LandscapeOrientation, Qt::PortraitOrientation }, { Qt::InvertedLandscapeOrientation, Qt::PortraitOrientation },
    { Qt::PortraitOrientation, Qt::LandscapeOrientation }, { Qt::InvertedPortraitOrientation, Qt::LandscapeOrientation }
  };
  for ( const auto &[screenOrientation, nativeOrientation] : quarters )
  {
    for ( const int current : userOrientations )
    {
      QCOMPARE( Compass::userOrientation( screenOrientation, nativeOrientation, Reading::RightUp, current ), 90 );
      QCOMPARE( Compass::userOrientation( screenOrientation, nativeOrientation, Reading::LeftUp, current ), 270 );
    }

    // the device lies flat, the known side is kept
    QCOMPARE( Compass::userOrientation( screenOrientation, nativeOrientation, Reading::FaceUp, 90 ), 90 );
    QCOMPARE( Compass::userOrientation( screenOrientation, nativeOrientation, Reading::FaceUp, 270 ), 270 );
    QCOMPARE( Compass::userOrientation( screenOrientation, nativeOrientation, Reading::FaceDown, 90 ), 90 );
    QCOMPARE( Compass::userOrientation( screenOrientation, nativeOrientation, Reading::Undefined, 270 ), 270 );
  }

  // side unknown, the screen decides (verified on iOS only)
  QCOMPARE( Compass::userOrientation( Qt::LandscapeOrientation, Qt::PortraitOrientation, Reading::FaceUp, 0 ), 270 );
  QCOMPARE( Compass::userOrientation( Qt::LandscapeOrientation, Qt::PortraitOrientation, Reading::TopUp, 180 ), 270 );
  QCOMPARE( Compass::userOrientation( Qt::InvertedLandscapeOrientation, Qt::PortraitOrientation, Reading::FaceUp, 0 ), 90 );
  QCOMPARE( Compass::userOrientation( Qt::InvertedLandscapeOrientation, Qt::PortraitOrientation, Reading::TopUp, 180 ), 90 );
}

void TestUtilsFunctions::testCompassSmoothing()
{
  // the smoothed azimuth is in range (-180, 180], the test compares it in range [0, 360)
  auto smoothed = []( const Compass & compass )
  {
    const qreal azimuth = compass.smoothedAzimuth();
    return azimuth < 0 ? azimuth + 360 : azimuth;
  };

  // rotation (counter-clockwise) becomes a clockwise azimuth in (-180, 180]
  QCOMPARE( Compass::azimuthFromRotation( 0 ), 0.0 );
  QCOMPARE( Compass::azimuthFromRotation( -90 ), 90.0 ); // east
  QCOMPARE( Compass::azimuthFromRotation( 90 ), -90.0 ); // west
  QCOMPARE( Compass::azimuthFromRotation( 180 ), 180.0 );
  QCOMPARE( Compass::azimuthFromRotation( -180 ), 180.0 );
  QCOMPARE( Compass::azimuthFromRotation( 10 ), -10.0 );

  // the first azimuth is taken as it is
  {
    Compass compass;
    compass.addAzimuth( 123, 0 );
    QVERIFY( qgsDoubleNear( smoothed( compass ), 123, 1e-6 ) );
    compass.addAzimuth( -100, 60000 );
    QVERIFY( qgsDoubleNear( compass.smoothedAzimuth(), -100, 1e-3 ) ); // west is negative, as from the compass on Android
  }

  // noise around the south (Android jumps between 180 and -180) stays at the south, a plain average would point north
  {
    Compass compass;
    for ( int i = 0; i < 50; ++i )
    {
      compass.addAzimuth( i % 2 ? 175 : -175, 20 );
      const qreal azimuth = smoothed( compass );
      QVERIFY2( azimuth >= 175 && azimuth <= 185, qPrintable( QString::number( azimuth ) ) );
    }
    QVERIFY( qgsDoubleNear( smoothed( compass ), 180, 1 ) );
  }

  // the same around the north, the result is in range [0, 360)
  {
    Compass compass;
    for ( int i = 0; i < 50; ++i )
    {
      compass.addAzimuth( i % 2 ? 5 : -5, 20 );
      const qreal azimuth = smoothed( compass );
      QVERIFY2( azimuth <= 5 || ( azimuth >= 355 && azimuth < 360 ), qPrintable( QString::number( azimuth ) ) );
    }
  }

  // the noise is reduced
  {
    Compass compass;
    compass.addAzimuth( 90, 0 );
    compass.addAzimuth( 100, 20 );
    QVERIFY( smoothed( compass ) > 90 && smoothed( compass ) < 91 );
  }

  // a real turn is followed the shorter way (350 -> 20 over north)
  {
    Compass compass;
    compass.addAzimuth( 350, 0 );
    compass.addAzimuth( 20, 20 );
    const qreal azimuth = smoothed( compass );
    QVERIFY2( azimuth > 350 && azimuth < 360, qPrintable( QString::number( azimuth ) ) );

    for ( int i = 0; i < 100; ++i )
      compass.addAzimuth( 20, 20 );
    QVERIFY( qgsDoubleNear( smoothed( compass ), 20, 0.1 ) );
  }

  // after a long pause the new azimuth is taken (almost) as it is
  {
    Compass compass;
    compass.addAzimuth( 10, 0 );
    compass.addAzimuth( 200, 60000 );
    QVERIFY( qgsDoubleNear( smoothed( compass ), 200, 1e-3 ) );
  }
}

void TestUtilsFunctions::testCompassNormalizedDirection()
{
  // the direction is in range [0, 360) on all platforms, e.g. for @position_direction
  QCOMPARE( Compass::normalizedDirection( 0 ), 0.0 );
  QCOMPARE( Compass::normalizedDirection( 90 ), 90.0 );
  QCOMPARE( Compass::normalizedDirection( -90 ), 270.0 ); // west from the Android sensors
  QCOMPARE( Compass::normalizedDirection( -179.5 ), 180.5 );
  QCOMPARE( Compass::normalizedDirection( 180 ), 180.0 );
  QCOMPARE( Compass::normalizedDirection( 360 ), 0.0 );
  QCOMPARE( Compass::normalizedDirection( 450 ), 90.0 ); // azimuth with the user orientation added
  QCOMPARE( Compass::normalizedDirection( -360 ), 0.0 );

  // a tiny negative direction is not 360
  const qreal tiny = Compass::normalizedDirection( -1e-13 );
  QVERIFY2( tiny >= 0 && tiny < 360, qPrintable( QString::number( tiny, 'g', 17 ) ) );

  // any azimuth from the sensors plus any user orientation ends in range
  const int userOrientations[] = { 0, 90, 180, 270 };
  for ( int rotation = -360; rotation <= 360; rotation += 5 )
  {
    for ( const int userOrientation : userOrientations )
    {
      const qreal direction = Compass::normalizedDirection( Compass::azimuthFromRotation( rotation ) + userOrientation );
      QVERIFY2( direction >= 0 && direction < 360, qPrintable( QString::number( direction ) ) );
    }
  }
}
