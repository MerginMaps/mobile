/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testmaptools.h"

#include "QtTest/QtTest"
#include <QSignalSpy>
#include <QList>

#include "qgspoint.h"
#include "qgslinestring.h"
#include "qgspolygon.h"
#include "qgsmultipolygon.h"
#include "qgslinestring.h"
#include "qgsgeometry.h"

#include "inputmapcanvasmap.h"
#include "inputmapsettings.h"

#include "testutils.h"

#include "inpututils.h"
#include "snaputils.h"
#include "maptools/splittingmaptool.h"
#include "maptools/recordingmaptool.h"
#include "maptools/measurementmaptool.h"

#include "featurelayerpair.h"
#include "streamingintervaltype.h"

#ifdef HAVE_BLUETOOTH
#include "position/providers/bluetoothpositionprovider.h"
#endif

void setupMapSettings( InputMapSettings *settings, QgsProject *project, QgsRectangle extent, QSize outputsize )
{
  if ( !project || !settings )
    return;

  settings->setProject( project );
  settings->setDestinationCrs( project->crs() );
  settings->setLayers( project->layers<QgsMapLayer *>().toList() );
  settings->setOutputSize( outputsize );
  settings->setExtent( extent );
}

TestMapTools::TestMapTools( PositionKit *pk, AppSettings *as )
{
  mPositionKit = pk;
  mAppSettings = as;

  mAppSettings->setGpsAntennaHeight( 0 );
  mPositionKit->setAppSettings( mAppSettings );
  mPositionKit->setPositionProvider( nullptr );
}

void TestMapTools::init()
{
}

void TestMapTools::cleanup()
{
}

void TestMapTools::testSnapping()
{
  SnapUtils su;

  QVERIFY( !su.snapped() );

  su.setDestinationLayer( nullptr );
  su.setMapSettings( nullptr );
  su.setQgsProject( nullptr );

  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";

  QgsProject *project = new QgsProject();

  QVERIFY( project->read( projectDir + "/" + projectName ) );

  QgsMapLayer *sectorL = project->mapLayersByName( QStringLiteral( "FlySector" ) ).at( 0 );
  QgsVectorLayer *flySectorLayer = static_cast<QgsVectorLayer *>( sectorL );

  QVERIFY( flySectorLayer && flySectorLayer->isValid() );

  // this should snap (map CRS): -106.961,35.929
  QgsPoint center( -107.5419, 35.0978 );

  InputMapCanvasMap canvas;

  InputMapSettings *ms = canvas.mapSettings();
  ms->setProject( project );
  ms->setDestinationCrs( project->crs() );
  ms->setOutputSize( QSize( 600, 1096 ) );
  ms->setLayers( project->layers<QgsMapLayer *>().toList() );

  QgsRectangle extent = QgsRectangle( -113.03901289716023371, 29.01993493630328658, -95.63347856920418621, 43.95809132096547955 );
  ms->setExtent( extent );

  su.setMapSettings( ms );
  su.setUseSnapping( true );
  su.setQgsProject( project );
  su.setDestinationLayer( flySectorLayer );
  su.setCenterPosition( ms->coordinateToScreen( center ) );

  su.getsnap();

  QVERIFY( su.snapped() );

  su.setUseSnapping( false );

  QVERIFY( !su.snapped() );

  delete project;
}

void TestMapTools::testSplitting()
{
  SplittingMapTool *splitTool = new SplittingMapTool();

  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectTempDir = QDir::tempPath() + "/" + QUuid::createUuid().toString();
  QString projectName = "quickapp_project.qgs";

  // copy the test project away because we will change it
  QVERIFY( InputUtils::cpDir( projectDir, projectTempDir ) );

  QgsProject *project = new QgsProject();

  QVERIFY( project->read( projectTempDir + "/" + projectName ) );

  QgsMapLayer *sectorL = project->mapLayersByName( QStringLiteral( "FlySector" ) ).at( 0 );
  QgsVectorLayer *flySectorLayer = static_cast<QgsVectorLayer *>( sectorL );

  QVERIFY( flySectorLayer && flySectorLayer->isValid() );

  // set map settings
  InputMapCanvasMap canvas;

  InputMapSettings *ms = canvas.mapSettings();
  ms->setProject( project );
  ms->setDestinationCrs( project->crs() );
  ms->setOutputSize( QSize( 600, 1096 ) );
  ms->setLayers( project->layers<QgsMapLayer *>().toList() );

  QgsRectangle extent = QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 );
  ms->setExtent( extent );

  splitTool->setMapSettings( ms );

  // set feature to split

  // split line - splits feature fid:1
  QgsPoint splitlineStart( -104.751, 32.448 );
  QgsPoint splitlineEnd( -120.844, 32.592 );
  int fidToSplit = 1;

  QgsFeature featureToSplit = flySectorLayer->getFeature( fidToSplit );
  FeatureLayerPair pairToSplit( featureToSplit, flySectorLayer );

  splitTool->setFeatureToSplit( pairToSplit );

  // add the split line
  QSignalSpy geometryChangedSpy( splitTool, &SplittingMapTool::recordedGeometryChanged );

  QVERIFY( !geometryChangedSpy.count() );

  splitTool->addPoint( splitlineStart );

  QCOMPARE( geometryChangedSpy.count(), 1 );

  splitTool->removePoint();

  QCOMPARE( geometryChangedSpy.count(), 2 );

  splitTool->addPoint( splitlineStart );
  splitTool->addPoint( splitlineEnd );

  QCOMPARE( geometryChangedSpy.count(), 4 );

  // commit split
  int previousFeaturesCount = flySectorLayer->featureCount();
  splitTool->commitSplit();

  QVERIFY( previousFeaturesCount < flySectorLayer->featureCount() );

  delete project;
  delete splitTool;
}

void TestMapTools::testRecording()
{
  RecordingMapTool *recordTool = new RecordingMapTool();

  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectTempDir = QDir::tempPath() + "/" + QUuid::createUuid().toString();
  QString projectName = "quickapp_project.qgs";

  // copy the test project away because we will change it
  QVERIFY( InputUtils::cpDir( projectDir, projectTempDir ) );

  QgsProject *project = new QgsProject();

  QVERIFY( project->read( projectTempDir + "/" + projectName ) );

  QgsMapLayer *sectorL = project->mapLayersByName( QStringLiteral( "FlySector" ) ).at( 0 );
  QgsVectorLayer *flySectorLayer = static_cast<QgsVectorLayer *>( sectorL );

  QVERIFY( flySectorLayer && flySectorLayer->isValid() );

  // set map settings
  InputMapCanvasMap canvas;

  InputMapSettings *ms = canvas.mapSettings();
  ms->setProject( project );
  ms->setDestinationCrs( project->crs() );
  ms->setOutputSize( QSize( 600, 1096 ) );
  ms->setLayers( project->layers<QgsMapLayer *>().toList() );

  QgsRectangle extent = QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 );
  ms->setExtent( extent );

  recordTool->setMapSettings( ms );

  recordTool->setCenteredToGPS( false );
  recordTool->setActiveLayer( flySectorLayer );
  recordTool->setActiveFeature( QgsFeature() );

  QCOMPARE( recordTool->recordingType(), RecordingMapTool::Manual );

  // add some points
  QSignalSpy geometryChangedSpy( recordTool, &RecordingMapTool::recordedGeometryChanged );

  QVERIFY( !geometryChangedSpy.count() );

  QVector<QgsPoint> pointsToAdd =
  {
    { -97.129, 22.602 },
    { -104.923, 24.840 },
    { -99.655, 29.891 },
    { -92.800, 26.644 }
  };

  recordTool->addPoint( pointsToAdd[0] );

  QCOMPARE( geometryChangedSpy.count(), 1 );
  QVERIFY( !recordTool->hasValidGeometry() );

  recordTool->removePoint();

  // we will end up in GRAB mode
  QVERIFY( recordTool->state() == RecordingMapTool::Grab );

  // go back to RECORDING
  recordTool->setState( RecordingMapTool::Record );

  QCOMPARE( geometryChangedSpy.count(), 2 );

  // Already existing point, skip inserting
  recordTool->addPoint( pointsToAdd[0] );

  recordTool->addPoint( pointsToAdd[1] );

  QVERIFY( !recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[2] );

  QVERIFY( recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[3] );

  QCOMPARE( geometryChangedSpy.count(), 5 );

  delete project;
  delete recordTool;
}

void TestMapTools::testMeasuring()
{
  MeasurementMapTool *measurementTool = new MeasurementMapTool();

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  setupMapSettings( ms, project, QgsRectangle( -10, -10, 10, 10 ), QSize( 600, 600 ) );

  measurementTool->setMapSettings( ms );

  QgsDistanceArea distanceArea;
  distanceArea.setEllipsoid( ms->project()->ellipsoid() );
  distanceArea.setSourceCrs( ms->destinationCrs(), ms->transformContext() );

  QPointF crosshairPoint1 = ms->coordinateToScreen( QgsPoint( 0, 0 ) );
  measurementTool->setCrosshairPoint( crosshairPoint1 );
  measurementTool->addPoint();

  measurementTool->updateDistance();
  QCOMPARE( measurementTool->lengthWithGuideline(), 0.0 );

  QPointF crosshairPoint2 = ms->coordinateToScreen( QgsPoint( 0, 1 ) );
  measurementTool->setCrosshairPoint( crosshairPoint2 );
  measurementTool->addPoint();

  measurementTool->updateDistance();

  QList<QgsPointXY> points;
  points.append( QgsPointXY( 0, 0 ) );
  points.append( QgsPointXY( 0, 1 ) );

  QgsGeometry lineGeometry = QgsGeometry::fromPolylineXY( points );
  double expectedPerimeter = distanceArea.measureLength( lineGeometry );
  QCOMPARE( measurementTool->perimeter(), expectedPerimeter );

  QPointF crosshairPoint3 = ms->coordinateToScreen( QgsPoint( 1, 1 ) );
  measurementTool->setCrosshairPoint( crosshairPoint3 );
  measurementTool->addPoint();

  measurementTool->updateDistance();
  points.append( QgsPointXY( 1, 1 ) );
  lineGeometry = QgsGeometry::fromPolylineXY( points );
  expectedPerimeter = distanceArea.measureLength( lineGeometry );
  QCOMPARE( measurementTool->perimeter(), expectedPerimeter );

  measurementTool->finalizeMeasurement( false );

  QVERIFY( measurementTool->recordedGeometry().wkbType() == Qgis::WkbType::LineString );

  measurementTool->resetMeasurement();

  measurementTool->setCrosshairPoint( crosshairPoint1 );
  measurementTool->addPoint();

  measurementTool->setCrosshairPoint( crosshairPoint2 );
  measurementTool->addPoint();

  measurementTool->setCrosshairPoint( crosshairPoint3 );
  measurementTool->addPoint();

  measurementTool->setCrosshairPoint( crosshairPoint1 );
  measurementTool->updateDistance();

  QVERIFY( measurementTool->canCloseShape() );

  measurementTool->finalizeMeasurement( true );

  QVERIFY( measurementTool->recordedGeometry().wkbType() == Qgis::WkbType::Polygon );

  QgsGeometry polygonGeometry = QgsGeometry::fromPolygonXY( QList<QList<QgsPointXY>> () << points );
  double expectedArea = distanceArea.measureArea( polygonGeometry );
  QCOMPARE( measurementTool->area(), expectedArea );

  delete project;
  delete measurementTool;
}

void TestMapTools::testExistingVertices()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QgsGeometry geometry;

  // polygon
  QgsVectorLayer *polygonLayer = new QgsVectorLayer( QStringLiteral( "Polygon?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsPolygon *polygon = new QgsPolygon( new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) << QgsPoint( 0, 0 ) ) );
  geometry.set( polygon );

  QgsFeature polyFeature;
  polyFeature.setGeometry( geometry );
  polygonLayer->dataProvider()->addFeature( polyFeature );
  QVERIFY( polygonLayer->isValid() );
  project->addMapLayer( polygonLayer );

  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( polyFeature );

  QgsGeometry vertices = mapTool.existingVertices();
  QCOMPARE( vertices.wkbType(), Qgis::WkbType::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 3 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 1, 1 ) );

  // line
  QgsVectorLayer *lineLayer = new QgsVectorLayer( QStringLiteral( "LineString?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) << QgsPoint( 2, 2 ) );
  geometry.set( line );

  QgsFeature lineFeature;
  lineFeature.setGeometry( geometry );
  lineLayer->dataProvider()->addFeature( lineFeature );
  QVERIFY( lineLayer->isValid() );
  project->addMapLayer( lineLayer );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( lineFeature );

  vertices = mapTool.existingVertices();
  QCOMPARE( vertices.wkbType(), Qgis::WkbType::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 1, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 2, 2 ) );

  // multipoint
  QgsVectorLayer *pointLayer = new QgsVectorLayer( QStringLiteral( "MultiPoint?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  geometry = QgsGeometry::fromWkt( "MultiPoint( 0 0, 1 1, 2 2)" );

  QgsFeature pointFeature;
  pointFeature.setGeometry( geometry );
  pointLayer->dataProvider()->addFeature( pointFeature );
  QVERIFY( pointLayer->isValid() );
  project->addMapLayer( pointLayer );

  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( pointFeature );

  vertices = mapTool.existingVertices();
  QCOMPARE( vertices.wkbType(), Qgis::WkbType::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 3 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 1, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 2, 2 ) );

  delete polygonLayer;
  delete lineLayer;
  delete pointLayer;
}

void TestMapTools::testMidSegmentVertices()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QgsGeometry geometry;

  // polygon
  QgsVectorLayer *polygonLayer = new QgsVectorLayer( QStringLiteral( "Polygon?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsPolygon *polygon = new QgsPolygon( new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 2 ) << QgsPoint( 2, 2 ) << QgsPoint( 2, 0 ) << QgsPoint( 0, 0 ) ) );
  geometry.set( polygon );

  QgsFeature polyFeature;
  polyFeature.setGeometry( geometry );
  polygonLayer->dataProvider()->addFeature( polyFeature );
  QVERIFY( polygonLayer->isValid() );

  project->addMapLayer( polygonLayer );

  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( polyFeature );

  QgsGeometry vertices = mapTool.midPoints();
  QCOMPARE( vertices.wkbType(), Qgis::WkbType::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 1, 2 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 2, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 1, 0 ) );

  // line
  QgsVectorLayer *lineLayer = new QgsVectorLayer( QStringLiteral( "LineString?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );

  QgsFeature lineFeature;
  lineFeature.setGeometry( geometry );
  lineLayer->dataProvider()->addFeature( lineFeature );
  QVERIFY( lineLayer->isValid() );

  project->addMapLayer( lineLayer );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( lineFeature );

  vertices = mapTool.midPoints();
  QCOMPARE( vertices.wkbType(), Qgis::WkbType::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, -0.87614105022163979 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 0.5 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 0.5, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 1.87040839992228825, 1 ) );

  // multipoint
  QgsVectorLayer *pointLayer = new QgsVectorLayer( QStringLiteral( "MultiPoint?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  geometry = QgsGeometry::fromWkt( "MultiPoint( 0 0, 1 1, 2 2)" );
  QgsFeature pointFeature;
  pointFeature.setGeometry( geometry );
  pointLayer->dataProvider()->addFeature( pointFeature );
  QVERIFY( pointLayer->isValid() );

  project->addMapLayer( pointLayer );

  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( pointFeature );
  vertices = mapTool.midPoints();
  QVERIFY( vertices.constGet()->vertexCount() == 0 );

  delete polygonLayer;
  delete lineLayer;
  delete pointLayer;
}

void TestMapTools::testHandles()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QgsGeometry geometry;

  QgsVectorLayer *lineLayer = new QgsVectorLayer( QStringLiteral( "LineString?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );
  QgsFeature lineFeature;
  lineFeature.setGeometry( geometry );
  lineLayer->dataProvider()->addFeature( lineFeature );
  QVERIFY( lineLayer->isValid() );

  project->addMapLayer( lineLayer );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( lineFeature );

  QgsGeometry handles = mapTool.handles();
  QCOMPARE( handles.wkbType(), Qgis::WkbType::MultiLineString );
  QCOMPARE( handles.constGet()->partCount(), 2 );

  // handle segments first point is a handle point and second point is
  // an existing vertex (start/end of the line)
  QVector<QString> expected =
  {
    QStringLiteral( "LineString (0 -0.87614105022164, 0 0)" ),
    QStringLiteral( "LineString (1 1, 1.87040839992229 1)" ),
  };

  const QVector<QgsGeometry> parts = handles.asGeometryCollection();
  for ( int i = 0; i < parts.count(); i++ )
  {
    QVERIFY( parts.at( i ).asWkt( 14 ) == expected.at( i ) );
  }

  delete lineLayer;
}

void TestMapTools::testLookForVertex()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QgsGeometry geometry;

  QgsVectorLayer *lineLayer = new QgsVectorLayer( QStringLiteral( "LineString?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );

  QgsFeature lineFeature;
  lineFeature.setGeometry( geometry );
  lineLayer->dataProvider()->addFeature( lineFeature );
  QVERIFY( lineLayer->isValid() );

  project->addMapLayer( lineLayer );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( lineFeature );

  // when initial geometry set we start in View state
  QCOMPARE( mapTool.state(), RecordingMapTool::MapToolState::View );

  // Start handle point. Active vertex is invalid, state changes to Record
  QPointF screenPoint = ms->coordinateToScreen( QgsPoint( -0.05, -0.83 ) );
  mapTool.lookForVertex( screenPoint );
  QVERIFY( !mapTool.activeVertex().isValid() );
  QCOMPARE( mapTool.state(), RecordingMapTool::MapToolState::Record );

  // reset state to View
  mapTool.setState( RecordingMapTool::MapToolState::View );

  // Existing geometry vertex (first point). Active vertex is valid, state changes to Grab
  screenPoint = ms->coordinateToScreen( QgsPoint( -0.01, 0.1 ) );
  mapTool.lookForVertex( screenPoint );
  QVERIFY( mapTool.activeVertex().isValid() );
  QCOMPARE( mapTool.activeVertex().type(), Vertex::Existing );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );

  // reset state to View
  mapTool.setState( RecordingMapTool::MapToolState::View );

  // Midpoint between 2nd and 3rd existing vertices (0.5, 1).
  // This creates new point with the midpoint coordinates and triggers
  // rebuild of the vertex cache (new exsting vertex will be added).
  // Active vertex is valid, state changes to Grab.
  screenPoint = ms->coordinateToScreen( QgsPoint( 0.6, 1.2 ) );
  mapTool.lookForVertex( screenPoint );
  QVERIFY( mapTool.activeVertex().isValid() );
  QCOMPARE( mapTool.activeVertex().type(), Vertex::Existing );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 2 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 0.5, 1 ) );

  // reset state to View
  mapTool.setState( RecordingMapTool::MapToolState::View );

  // Distant point. Active vertex is invalid (nothing found), View state
  screenPoint = ms->coordinateToScreen( QgsPoint( 15, 13 ) );
  mapTool.lookForVertex( screenPoint );
  QVERIFY( !mapTool.activeVertex().isValid() );
  QCOMPARE( mapTool.state(), RecordingMapTool::MapToolState::View );

  // TODO: identify:
  // - no vertex
  // - ring vertex
  // - part vertex

  delete lineLayer;
}

void TestMapTools::testAddVertexPointLayer()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *pointLayer = new QgsVectorLayer( "Point?crs=epsg:4326", "pointlayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );

  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( QgsFeature() );

  QVector<QgsPoint> pointsToAdd =
  {
    { -97.129, 22.602 },
    { -104.923, 24.840 }
  };

  //
  // Point layer should only add point when geometry is empty
  //
  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[0] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[1] );

  // geometry is updated with new point
  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == Qgis::WkbType::Point );

  // clear recorded geometry
  mapTool.setActiveLayer( nullptr );
  mapTool.setActiveLayer( pointLayer );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // if maptool is in GRAB and VIEW state, no point should be added
  mapTool.setState( RecordingMapTool::Grab );
  mapTool.addPoint( pointsToAdd[0] );

  // no point should be added
  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().isEmpty() );

  mapTool.setState( RecordingMapTool::View );
  mapTool.addPoint( pointsToAdd[0] );

  // no point should be added
  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().isEmpty() );

  delete project;
  delete pointLayer;
}

void TestMapTools::testAddVertexMultiPointLayer()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *multiPointLayer = new QgsVectorLayer( "MultiPoint?crs=epsg:4326", "mpointlayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );

  mapTool.setActiveLayer( multiPointLayer );
  mapTool.setActiveFeature( QgsFeature() );

  // ----------- MultiPoint layer ----------
  // Same as point layer, adds point only when geometry is empty

  QVector<QgsPoint> pointsToAdd =
  {
    { -97.129, 22.602 },
    { -104.923, 24.840 }
  };

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[0] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[1] );

  // adds another part
  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 2 );
  QVERIFY( mapTool.recordedGeometry().constGet()->partCount() == 2 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == Qgis::WkbType::MultiPoint );

  // clear recorded geometry
  mapTool.setActiveLayer( nullptr );
  mapTool.setActiveLayer( multiPointLayer );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // if maptool is in GRAB and VIEW state, no point should be added
  mapTool.setState( RecordingMapTool::Grab );

  mapTool.addPoint( pointsToAdd[0] );

  // no point should be added
  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().isEmpty() );

  mapTool.setState( RecordingMapTool::View );

  mapTool.addPoint( pointsToAdd[0] );

  // no point should be added
  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().isEmpty() );

  delete project;
  delete multiPointLayer;
}

void TestMapTools::testAddVertexLineLayer()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *lineLayer = new QgsVectorLayer( "LineString?crs=epsg:4326", "linelayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( QgsFeature() );

  //
  // ----------- Linestring layer ----------
  //
  QVector<QgsPoint> pointsToAdd =
  {
    { -97.129, 22.602 }, // added to end
    { -104.923, 24.840 }, // added to end
    { -108, 26 }, // added to end
    { -110, 28 }, // added to end
    { -110, 28 }, // Same as previous point should not be recorded
    { -95, 20 }, // added to start
    { -109, 27 }, // added to middle
  };

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[0] );

  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[1] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 2 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == Qgis::WkbType::LineString );

  mapTool.addPoint( pointsToAdd[2] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 3 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 2 ), pointsToAdd[2] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // nothing should be added in VIEW (neither in GRAB) state
  mapTool.setState( RecordingMapTool::View );
  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 3 );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::View );

  mapTool.setState( RecordingMapTool::Record );

  // even if active vertex would incorrectly remained set, point should be added to the end of the line
  mapTool.setActiveVertex( Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[1], Vertex::Existing ) );
  mapTool.addPoint( pointsToAdd[3] );
  mapTool.setActiveVertex( Vertex() );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[3] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[4] );
  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[3] );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[4] );

  //
  // Let's try to add point from beginning
  //
  mapTool.setInsertPolicy( RecordingMapTool::Start );

  mapTool.addPoint( pointsToAdd[5] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[5] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's add point to the middle of line
  //
  mapTool.setInsertPolicy( RecordingMapTool::End );

  Vertex addPosition = Vertex( QgsVertexId( 0, 0, 3 ), pointsToAdd[3], Vertex::Existing );
  mapTool.addPointAtPosition( addPosition, pointsToAdd[6] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 6 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[6] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // clear recorded geometry
  mapTool.setActiveLayer( nullptr );
  mapTool.setActiveLayer( lineLayer );

  //
  // Test the handles calculation logic
  //
  mapTool.addPoint( pointsToAdd[0] );
  mapTool.addPoint( pointsToAdd[0] );

  QVERIFY( mapTool.handles().isEmpty() );

  mapTool.addPoint( pointsToAdd[1] );

  QCOMPARE( mapTool.handles().vertexAt( 0 ).asWkt( 14 ), "Point (-96.22182942132511 22.34151145046518)" );
  QCOMPARE( mapTool.handles().vertexAt( 1 ), pointsToAdd[0] );

  delete project;
  delete lineLayer;
}

void TestMapTools::testAddVertexMultiLineLayer()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *multiLineLayer = new QgsVectorLayer( "MultiLineString?crs=epsg:4326", "mlinelayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveLayer( multiLineLayer );
  mapTool.setActiveFeature( QgsFeature() );

  //
  // ----------- MultiLinestring layer ----------
  //
  QVector<QgsPoint> pointsToAdd =
  {
    { -97.129, 22.602 }, // added to end
    { -104.923, 24.840 }, // added to end
    { -108, 26 }, // added to end
    { -110, 28 }, // added to end
    { -95, 20 }, // added to start
    { -109, 27 }, // added to middle
  };

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[0] );

  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[1] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 2 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == Qgis::WkbType::MultiLineString );

  mapTool.addPoint( pointsToAdd[2] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 3 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 2 ), pointsToAdd[2] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // nothing should be added in VIEW (neither in GRAB) state
  mapTool.setState( RecordingMapTool::View );
  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 3 );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::View );

  // neither add anything in GRAB state with active vertex
  mapTool.setState( RecordingMapTool::Grab );
  mapTool.setActiveVertex( Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[1], Vertex::Existing ) );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 3 );
  QVERIFY( mapTool.state() == RecordingMapTool::Grab );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveVertex( Vertex() );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[3] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's try to add point from beginning
  //
  mapTool.setInsertPolicy( RecordingMapTool::Start );

  mapTool.addPoint( pointsToAdd[4] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[4] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's add point to the middle of line
  //
  mapTool.setInsertPolicy( RecordingMapTool::End );

  Vertex addPosition = Vertex( QgsVertexId( 0, 0, 3 ), pointsToAdd[3], Vertex::Existing );
  mapTool.addPointAtPosition( addPosition, pointsToAdd[5] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 6 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[5] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // TODO: add to this test adding point to other parts

  delete project;
  delete multiLineLayer;
}

void TestMapTools::testAddVertexPolygonLayer()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *polygonLayer = new QgsVectorLayer( "Polygon?crs=epsg:4326", "polygonlayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( QgsFeature() );

  //
  // ----------- Polygon layer ----------
  //
  QVector<QgsPoint> pointsToAdd =
  {
    { -95.5, 22.0 },
    { -97.5, 22.0 },
    { -97.5, 26.0 },
    { -95.5, 26.0 },
    { -96.5, 22.0 }, // add between first two
    { -97.5, 24.0 }, // add between second & third
    { -95.5, 24.0 }, // add between third & last
  };

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[0] );

  QVERIFY( !mapTool.hasValidGeometry() );
  qDebug() << "mapTool.recordedGeometry().constGet()->nCoordinates()" << mapTool.recordedGeometry().constGet()->nCoordinates();
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().constGet()->nCoordinates(), 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[1] );

  QVERIFY( !mapTool.hasValidGeometry() );
  // ring will be closed, hance 3 points
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 3 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == Qgis::WkbType::Polygon );

  mapTool.addPoint( pointsToAdd[2] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 2 ), pointsToAdd[2] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // nothing should be added in VIEW (neither in GRAB) state
  mapTool.setState( RecordingMapTool::View );
  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::View );

  // neither add anything in GRAB state with active vertex
  mapTool.setState( RecordingMapTool::Grab );
  mapTool.setActiveVertex( Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[1], Vertex::Existing ) );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveVertex( Vertex() );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[3] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's insert points to the middle
  //
  Vertex addVertexPos = Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[4], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[4] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 6 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[4] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 3 ), pointsToAdd[5], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[5] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 7 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[5] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 6 ), pointsToAdd[6], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[6] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 8 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 6 ), pointsToAdd[6] );

  delete project;
  delete polygonLayer;
}

void TestMapTools::testAddVertexMultiPolygonLayer()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *polygonLayer = new QgsVectorLayer( "Polygon?crs=epsg:4326", "polygonlayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( QgsFeature() );

  //
  // ----------- Polygon layer ----------
  //

  QVector<QgsPoint> pointsToAdd =
  {
    { -95.5, 22.0 },
    { -97.5, 22.0 },
    { -97.5, 26.0 },
    { -95.5, 26.0 },
    { -96.5, 22.0 }, // add between first two
    { -97.5, 24.0 }, // add between second & third
    { -95.5, 24.0 }, // add between third & last
  };

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[0] );

  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[1] );

  QVERIFY( !mapTool.hasValidGeometry() );
  // ring will be closed, so 3 points
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 3 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == Qgis::WkbType::Polygon );

  mapTool.addPoint( pointsToAdd[2] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 2 ), pointsToAdd[2] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // nothing should be added in VIEW (neither in GRAB) state
  mapTool.setState( RecordingMapTool::View );
  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::View );

  // neither add anything in GRAB state with active vertex
  mapTool.setState( RecordingMapTool::Grab );
  mapTool.setActiveVertex( Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[1], Vertex::Existing ) );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveVertex( Vertex() );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[3] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's insert points to the middle
  //
  Vertex addVertexPos = Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[4], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[4] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 6 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[4] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 3 ), pointsToAdd[5], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[5] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 7 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[5] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 6 ), pointsToAdd[6], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[6] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 8 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 6 ), pointsToAdd[6] );

  // TODO: Add parts checks

  delete project;
  delete polygonLayer;
}

void TestMapTools::testUpdateVertex()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *lineLayer = new QgsVectorLayer( "LineString?crs=epsg:4326", "linelayer", "memory" );

  QgsGeometry line = QgsGeometry::fromPolyline(
  {
    QgsPoint( 10, 20 ),
    QgsPoint( 20, 30 ),
    QgsPoint( 30, 40 ),
  } );

  QgsFeature feature;
  feature.setGeometry( line );
  QVERIFY( feature.isValid() );

  lineLayer->dataProvider()->addFeatures( QList<QgsFeature>() << feature );
  QCOMPARE( lineLayer->featureCount(), 1 );

  mapTool.setState( RecordingMapTool::Grab );
  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( feature );

  Vertex updateVertexId = Vertex( QgsVertexId( 0, 0, 1 ), QgsPoint( 20, 30 ), Vertex::Existing );

  mapTool.setActiveVertex( updateVertexId );

  mapTool.updateVertex( updateVertexId, QgsPoint( 50, 50 ) );

  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), QgsPoint( 50, 50 ) );

  delete project;
  delete lineLayer;
}

void TestMapTools::testRemoveVertex()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Point layer
  QgsVectorLayer *pointLayer = new QgsVectorLayer( "Point?crs=epsg:4326", "pointlayer", "memory" );

  QgsFeature feature;
  feature.setGeometry( QgsGeometry::fromPointXY( QgsPointXY( 10, 10 ) ) );
  QVERIFY( feature.isValid() );

  pointLayer->dataProvider()->addFeatures( QList<QgsFeature>() << feature );
  QCOMPARE( pointLayer->featureCount(), 1 );

  mapTool.setState( RecordingMapTool::Grab );
  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( feature );

  Vertex v = Vertex( QgsVertexId( 0, 0, 1 ), QgsPoint( 10, 10 ), Vertex::Existing );

  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QVERIFY( mapTool.recordedGeometry().isEmpty() );

  // MultiPoint layer
  QgsVectorLayer *multiPointLayer = new QgsVectorLayer( "MultiPoint?crs=epsg:4326", "pointlayer", "memory" );

  QgsGeometry multiPoint = QgsGeometry::fromMultiPointXY(
  {
    QgsPointXY( 10, 20 ),
    QgsPointXY( 20, 30 ),
    QgsPointXY( 30, 40 ),
    QgsPointXY( 40, 50 ),
  } );

  feature = QgsFeature();
  feature.setGeometry( multiPoint );
  QVERIFY( feature.isValid() );

  multiPointLayer->dataProvider()->addFeatures( QList<QgsFeature>() << feature );
  QCOMPARE( multiPointLayer->featureCount(), 1 );

  mapTool.setActiveLayer( multiPointLayer );
  mapTool.setActiveFeature( feature );
  mapTool.setState( RecordingMapTool::Grab );

  v = Vertex( QgsVertexId( 1, 0, 1 ), QgsPoint( 20, 30 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();

  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 3 );
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 30, 40 ) );

  // Line layer
  QgsVectorLayer *lineLayer = new QgsVectorLayer( "LineString?crs=epsg:4326", "linelayer", "memory" );

  QgsGeometry line = QgsGeometry::fromPolyline(
  {
    QgsPoint( 10, 20 ),
    QgsPoint( 20, 30 ),
    QgsPoint( 30, 40 ),
    QgsPoint( 40, 50 ),
  } );

  feature = QgsFeature();
  feature.setGeometry( line );
  QVERIFY( feature.isValid() );

  lineLayer->dataProvider()->addFeatures( QList<QgsFeature>() << feature );
  QCOMPARE( lineLayer->featureCount(), 1 );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( feature );
  mapTool.setState( RecordingMapTool::Grab );

  v = Vertex( QgsVertexId( 0, 0, 1 ), QgsPoint( 20, 30 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), QgsPoint( 30, 40 ) );
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 10, 20 ) );

  // MultiLine layer
  QgsVectorLayer *multiLineLayer = new QgsVectorLayer( "MultiLineString?crs=epsg:4326", "multilinelayer", "memory" );
  QgsGeometry multiLine = QgsGeometry::fromMultiPolylineXY(
  {
    {
      QgsPointXY( 10, 20 ),
      QgsPointXY( 20, 30 ),
      QgsPointXY( 30, 40 ),
      QgsPointXY( 40, 50 ),
    },
    {
      QgsPointXY( 60, 70 ),
      QgsPointXY( 70, 80 ),
      QgsPointXY( 80, 90 ),
    }
  } );

  feature = QgsFeature();
  feature.setGeometry( multiLine );
  QVERIFY( feature.isValid() );

  multiLineLayer->dataProvider()->addFeatures( QList<QgsFeature>() << feature );
  QCOMPARE( multiLineLayer->featureCount(), 1 );

  mapTool.setActiveLayer( multiLineLayer );
  mapTool.setActiveFeature( feature );
  mapTool.setState( RecordingMapTool::Grab );

  v = Vertex( QgsVertexId( 0, 0, 3 ), QgsPoint( 40, 50 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 2 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 30, 40 ) );

  // delete from the 2nd part
  v = Vertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 60, 70 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 70, 80 ) );

  // remove part completely
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 80, 90 ) );

  // jump to 1st part
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 10, 20 ) );

  // Polygon layer
  QgsVectorLayer *polygonLayer = new QgsVectorLayer( "Polygon?crs=epsg:4326", "polygonlayer", "memory" );
  QgsGeometry polygon = QgsGeometry::fromPolygonXY(
  {
    {
      // exterior ring
      QgsPointXY( 0, 0 ),
      QgsPointXY( 0, 20 ),
      QgsPointXY( 20, 20 ),
      QgsPointXY( 20, 0 ),
    },
    {
      // interior ring - hole
      QgsPointXY( 5, 5 ),
      QgsPointXY( 5, 10 ),
      QgsPointXY( 10, 10 ),
      QgsPointXY( 10, 5 ),
    }
  } );

  feature = QgsFeature();
  feature.setGeometry( polygon );
  QVERIFY( feature.isValid() );

  polygonLayer->dataProvider()->addFeatures( QList<QgsFeature>() << feature );
  QCOMPARE( polygonLayer->featureCount(), 1 );

  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( feature );
  mapTool.setState( RecordingMapTool::Grab );

  // delete from exterior ring
  v = Vertex( QgsVertexId( 0, 0, 2 ), QgsPoint( 20, 20 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 1 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 0, 20 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount(), 2 );

  // delete from interior ring
  v = Vertex( QgsVertexId( 0, 1, 3 ), QgsPoint( 10, 5 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 2 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 10, 10 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount(), 2 );

  // delete interior ring completely
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 1 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 5, 10 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount(), 2 );

  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 5, 5 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount(), 2 );

  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 0, 0 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount(), 1 );


  // MultiPolygon layer
  QgsVectorLayer *multiPolygonLayer = new QgsVectorLayer( "MultiPolygon?crs=epsg:4326", "multipolygonlayer", "memory" );
  QgsGeometry multiPolygon = QgsGeometry::fromMultiPolygonXY(
  {
    {
      // part 1
      {
        QgsPointXY( 50, 50 ),
        QgsPointXY( 50, 70 ),
        QgsPointXY( 70, 70 ),
        QgsPointXY( 70, 50 ),
      }
    },
    {
      // part 2
      {
        // exterior ring
        QgsPointXY( 0, 0 ),
        QgsPointXY( 0, 20 ),
        QgsPointXY( 20, 20 ),
        QgsPointXY( 20, 0 ),
      },
      {
        // interior ring - hole
        QgsPointXY( 5, 5 ),
        QgsPointXY( 5, 10 ),
        QgsPointXY( 10, 10 ),
        QgsPointXY( 10, 5 ),
      }
    },
  } );

  feature = QgsFeature();
  feature.setGeometry( multiPolygon );
  QVERIFY( feature.isValid() );

  multiPolygonLayer->dataProvider()->addFeatures( QList<QgsFeature>() << feature );
  QCOMPARE( multiPolygonLayer->featureCount(), 1 );

  mapTool.setActiveLayer( multiPolygonLayer );
  mapTool.setActiveFeature( feature );
  mapTool.setState( RecordingMapTool::Grab );

  // delete vertex from the 1st part
  v = Vertex( QgsVertexId( 0, 0, 0 ), QgsPoint( 50, 50 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 50, 70 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount( 1 ), 2 );

  // delete vertex from the 2nd part
  v = Vertex( QgsVertexId( 1, 0, 0 ), QgsPoint( 0, 0 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 0, 20 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount( 1 ), 2 );

  // delete from the part ring
  v = Vertex( QgsVertexId( 1, 1, 2 ), QgsPoint( 10, 10 ), Vertex::Existing );
  mapTool.setActiveVertex( v );
  mapTool.removePoint();
  QCOMPARE( mapTool.state(), RecordingMapTool::Grab );
  QCOMPARE( mapTool.activeVertex().vertexId().part, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 1 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 5, 10 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount( 1 ), 2 );

  // delete ring completely
  mapTool.removePoint();
  QCOMPARE( mapTool.activeVertex().vertexId().part, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 5, 5 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount( 1 ), 2 );

  mapTool.removePoint();
  QCOMPARE( mapTool.activeVertex().vertexId().part, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 1 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 10, 5 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount( 1 ), 2 );

  // we should jump to the 1st part
  mapTool.removePoint();
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 50, 70 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );
  QCOMPARE( mapTool.recordedGeometry().constGet()->ringCount( 1 ), 1 );

  // delete 1st part completely
  mapTool.removePoint();
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 70, 70 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );

  mapTool.removePoint();
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 70, 50 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 2 );

  mapTool.removePoint();
  QCOMPARE( mapTool.activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool.activeVertex().vertexId().vertex, 0 );
  QCOMPARE( mapTool.activeVertex().coordinates(), QgsPoint( 0, 20 ) );
  QCOMPARE( mapTool.recordedGeometry().constGet()->partCount(), 1 );

  delete project;
  delete pointLayer;
  delete multiPointLayer;
  delete lineLayer;
  delete multiLineLayer;
  delete polygonLayer;
  delete multiPolygonLayer;
}

void TestMapTools::testVerticesStructure()
{
  //
  // mVertices array inside RecordingMapTool should have structure depending on
  // a type of geometry, as follows:
  //
  //
  // For Points:
  //
  // point: A
  // structure: vA
  //
  // where vA is an existing vertex A
  //
  // For MultiPoints:
  //
  // point: part1: A | part2: B | part3: C
  // structure: vA, vB, vC
  //
  //
  // For LineStrings:
  //
  // linestring: A -> B -> C
  // structure: hS, vA, mA, vB, mB, vC, hE
  //
  // where:
  //    - hS is handle start,
  //    - hE is handle end,
  //    - mA is midpoint between two existing vertices A and B
  //
  // For LineString with parts:
  //
  // linestring: part1: A -> B -> C | part 2: D->E | part 3: X
  // structure: hS1, vA, mA, vB, mB, vC, hE1, hS2, vD, mD, vE, hE2, vX
  //
  //
  // For Polygons:
  //
  // polygon: A -> B -> C (->A)
  // structure: vA, mA, vB, mB, vC, mC
  //
  // For MultiPolygons with rings:
  //
  // polygon: part1: A -> B -> C (->A), ring1: D->E->F(->D) | part2: X -> Y -> Z (-> X) | part3: G -> H | part 4: J
  // structure: vA, mA, vB, mB, vC, mC, vD, mD, vE, mE, vF, mF, vX, mX, vY, mY, vZ, mZ, vG, mG, vH, vJ
  //

  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  //
  // point
  //

  QgsVectorLayer *pointLayer = new QgsVectorLayer( QStringLiteral( "Point?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  project->addMapLayer( pointLayer );

  QgsPointXY pointdata = QgsPointXY( 10, 20 );
  QgsGeometry pointdataGEO = QgsGeometry::fromPointXY( pointdata );
  QVERIFY( pointdataGEO.wkbType() == Qgis::WkbType::Point );

  QgsFeature pointFeature;
  pointFeature.setGeometry( pointdataGEO );
  pointLayer->dataProvider()->addFeature( pointFeature );

  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( pointFeature );

  QCOMPARE( mapTool.collectedVertices().length(), 1 );
  QVERIFY( mapTool.collectedVertices().at( 0 ).type() == Vertex::Existing );
  QCOMPARE( mapTool.collectedVertices().at( 0 ).coordinates(), QgsPoint( pointdata ) );

  //
  // multipoint
  //
  QgsVectorLayer *multiPointLayer = new QgsVectorLayer( QStringLiteral( "MultiPoint?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  project->addMapLayer( multiPointLayer );

  QgsMultiPointXY multipointdata =
  {
    QgsPointXY( 10, 20 ),
    QgsPointXY( 20, 30 ),
    QgsPointXY( 30, 40 )
  };
  QgsGeometry multipointdataGEO = QgsGeometry::fromMultiPointXY( multipointdata );
  QVERIFY( multipointdataGEO.wkbType() == Qgis::WkbType::MultiPoint );

  QgsFeature multiPointFeature;
  multiPointFeature.setGeometry( multipointdataGEO );
  multiPointLayer->dataProvider()->addFeature( multiPointFeature );

  mapTool.setActiveLayer( multiPointLayer );
  mapTool.setActiveFeature( multiPointFeature );

  const QVector<Vertex> verticesmp = mapTool.collectedVertices();

  QCOMPARE( verticesmp.length(), 3 );

  QVERIFY( verticesmp.at( 0 ).type() == Vertex::Existing );
  QVERIFY( verticesmp.at( 1 ).type() == Vertex::Existing );
  QVERIFY( verticesmp.at( 2 ).type() == Vertex::Existing );

  QCOMPARE( verticesmp.at( 0 ).coordinates(), QgsPoint( multipointdata.at( 0 ) ) );
  QCOMPARE( verticesmp.at( 1 ).coordinates(), QgsPoint( multipointdata.at( 1 ) ) );
  QCOMPARE( verticesmp.at( 2 ).coordinates(), QgsPoint( multipointdata.at( 2 ) ) );

  //
  // linestring
  //
  QgsVectorLayer *lineLayer = new QgsVectorLayer( QStringLiteral( "LineString?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  project->addMapLayer( lineLayer );

  QgsPolylineXY linestringdata =
  {
    QgsPointXY( -10, -20 ),
    QgsPointXY( -20, 30 ),
    QgsPointXY( 30, 40 )
  };
  QgsGeometry linestrindataGEO = QgsGeometry::fromPolylineXY( linestringdata );
  QVERIFY( linestrindataGEO.wkbType() == Qgis::WkbType::LineString );

  QgsFeature lineFeature;
  lineFeature.setGeometry( linestrindataGEO );
  lineLayer->dataProvider()->addFeature( lineFeature );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( lineFeature );

  const QVector<Vertex> verticesl = mapTool.collectedVertices();

  // structure should have 7 points: handle start, v1, m1, v2, m2, v3, end handle
  QCOMPARE( verticesl.length(), 7 );

  QVERIFY( verticesl.at( 0 ).type() == Vertex::HandleStart );
  QVERIFY( verticesl.at( 1 ).type() == Vertex::Existing );
  QVERIFY( verticesl.at( 2 ).type() == Vertex::MidPoint );
  QVERIFY( verticesl.at( 3 ).type() == Vertex::Existing );
  QVERIFY( verticesl.at( 4 ).type() == Vertex::MidPoint );
  QVERIFY( verticesl.at( 5 ).type() == Vertex::Existing );
  QVERIFY( verticesl.at( 6 ).type() == Vertex::HandleEnd );

  QCOMPARE( verticesl.at( 1 ).coordinates(), QgsPoint( linestringdata.at( 0 ) ) );
  QCOMPARE( verticesl.at( 3 ).coordinates(), QgsPoint( linestringdata.at( 1 ) ) );
  QCOMPARE( verticesl.at( 5 ).coordinates(), QgsPoint( linestringdata.at( 2 ) ) );

  // single point line
  QgsLineString *r = qgsgeometry_cast<QgsLineString *>( linestrindataGEO.get() );
  QgsPointSequence ps;
  r->points( ps );
  ps.removeAt( 2 );
  ps.removeAt( 1 );
  r->setPoints( ps );

  lineFeature.setGeometry( linestrindataGEO );
  lineLayer->dataProvider()->addFeature( lineFeature );

  mapTool.setActiveLayer( lineLayer );
  mapTool.setActiveFeature( lineFeature );

  QCOMPARE( mapTool.collectedVertices().length(), 1 );
  QVERIFY( mapTool.collectedVertices().at( 0 ).type() == Vertex::Existing );
  QCOMPARE( mapTool.collectedVertices().at( 0 ).coordinates(), QgsPoint( linestringdata.at( 0 ) ) );

  //
  // multilinestring
  //
  QgsVectorLayer *multiLineLayer = new QgsVectorLayer( QStringLiteral( "MultiLineString?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  project->addMapLayer( multiLineLayer );

  QgsMultiPolylineXY multilinestringdata =
  {
    {
      // part 1
      QgsPointXY( -10, -20 ),
      QgsPointXY( -20, 30 ),
      QgsPointXY( 30, 40 )
    },
    {
      // part 2
      QgsPointXY( -10, -20 ),
      QgsPointXY( -15, 10 )
    },
    {
      // part 3 - only one point so far
      QgsPointXY( 10, 20 )
    }
  };

  QgsGeometry multilinestringdataGEO = QgsGeometry::fromMultiPolylineXY( multilinestringdata );
  QVERIFY( multilinestringdataGEO.wkbType() == Qgis::WkbType::MultiLineString );

  QgsFeature multiLineFeature;
  multiLineFeature.setGeometry( multilinestringdataGEO );
  multiLineLayer->dataProvider()->addFeature( multiLineFeature );

  mapTool.setActiveLayer( multiLineLayer );
  mapTool.setActiveFeature( multiLineFeature );

  const QVector<Vertex> verticesml = mapTool.collectedVertices();

  // should have 13 points: hs, v1, m1, v2, m2, v3, he, |<new part>| hs, v4, m4, v5, he, |<new part>| v6
  QCOMPARE( verticesml.length(), 13 );

  QVERIFY( verticesml.at( 0 ).type() == Vertex::HandleStart );
  QVERIFY( verticesml.at( 1 ).type() == Vertex::Existing );
  QVERIFY( verticesml.at( 2 ).type() == Vertex::MidPoint );
  QVERIFY( verticesml.at( 3 ).type() == Vertex::Existing );
  QVERIFY( verticesml.at( 4 ).type() == Vertex::MidPoint );
  QVERIFY( verticesml.at( 5 ).type() == Vertex::Existing );
  QVERIFY( verticesml.at( 6 ).type() == Vertex::HandleEnd );
  QVERIFY( verticesml.at( 7 ).type() == Vertex::HandleStart );
  QVERIFY( verticesml.at( 8 ).type() == Vertex::Existing );
  QVERIFY( verticesml.at( 9 ).type() == Vertex::MidPoint );
  QVERIFY( verticesml.at( 10 ).type() == Vertex::Existing );
  QVERIFY( verticesml.at( 11 ).type() == Vertex::HandleEnd );
  QVERIFY( verticesml.at( 12 ).type() == Vertex::Existing );

  QCOMPARE( verticesml.at( 1 ).coordinates(), QgsPoint( multilinestringdata.at( 0 ).at( 0 ) ) );
  QCOMPARE( verticesml.at( 3 ).coordinates(), QgsPoint( multilinestringdata.at( 0 ).at( 1 ) ) );
  QCOMPARE( verticesml.at( 5 ).coordinates(), QgsPoint( multilinestringdata.at( 0 ).at( 2 ) ) );
  QCOMPARE( verticesml.at( 8 ).coordinates(), QgsPoint( multilinestringdata.at( 1 ).at( 0 ) ) );
  QCOMPARE( verticesml.at( 10 ).coordinates(), QgsPoint( multilinestringdata.at( 1 ).at( 1 ) ) );
  QCOMPARE( verticesml.at( 12 ).coordinates(), QgsPoint( multilinestringdata.at( 2 ).at( 0 ) ) );

  QCOMPARE( verticesml.at( 0 ).vertexId().part, 0 );
  QCOMPARE( verticesml.at( 7 ).vertexId().part, 1 );
  QCOMPARE( verticesml.at( 12 ).vertexId().part, 2 );

  //
  // polygon
  //
  QgsVectorLayer *polygonLayer = new QgsVectorLayer( QStringLiteral( "Polygon?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  project->addMapLayer( polygonLayer );

  QgsPolygonXY simplepolygondata =
  {
    {
      // exterior ring
      QgsPointXY( -10, -20 ),
      QgsPointXY( 0, -20 ),
      QgsPointXY( 0, 0 ),
      QgsPointXY( -10, 0 )
    }
  };
  QgsGeometry simplepolygondataGEO = QgsGeometry::fromPolygonXY( simplepolygondata );
  QVERIFY( simplepolygondataGEO.wkbType() == Qgis::WkbType::Polygon );

  QgsFeature simplepolygonFeature;
  simplepolygonFeature.setGeometry( simplepolygondataGEO );
  polygonLayer->dataProvider()->addFeature( simplepolygonFeature );

  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( simplepolygonFeature );

  const QVector<Vertex> verticesspn = mapTool.collectedVertices();

  // should have 8 points: v1, m1, v2, m2, v3, m3, v4, m4
  QCOMPARE( verticesspn.length(), 8 );

  QVERIFY( verticesspn.at( 0 ).type() == Vertex::Existing );
  QVERIFY( verticesspn.at( 1 ).type() == Vertex::MidPoint );
  QVERIFY( verticesspn.at( 2 ).type() == Vertex::Existing );
  QVERIFY( verticesspn.at( 3 ).type() == Vertex::MidPoint );
  QVERIFY( verticesspn.at( 4 ).type() == Vertex::Existing );
  QVERIFY( verticesspn.at( 5 ).type() == Vertex::MidPoint );
  QVERIFY( verticesspn.at( 6 ).type() == Vertex::Existing );
  QVERIFY( verticesspn.at( 7 ).type() == Vertex::MidPoint );

  QCOMPARE( verticesspn.at( 0 ).coordinates(), QgsPoint( simplepolygondata.at( 0 ).at( 0 ) ) );
  QCOMPARE( verticesspn.at( 2 ).coordinates(), QgsPoint( simplepolygondata.at( 0 ).at( 1 ) ) );
  QCOMPARE( verticesspn.at( 4 ).coordinates(), QgsPoint( simplepolygondata.at( 0 ).at( 2 ) ) );
  QCOMPARE( verticesspn.at( 6 ).coordinates(), QgsPoint( simplepolygondata.at( 0 ).at( 3 ) ) );

  //
  // polygon (with hole)
  //

  QgsPolygonXY polygondata =
  {
    {
      // exterior ring
      QgsPointXY( -10, -20 ),
      QgsPointXY( 0, -20 ),
      QgsPointXY( 0, 0 ),
      QgsPointXY( -10, 0 )
    },
    {
      // interior ring - hole
      QgsPointXY( -8, -10 ),
      QgsPointXY( -4, -10 ),
      QgsPointXY( -4, -5 )
    }
  };
  QgsGeometry polygondataGEO = QgsGeometry::fromPolygonXY( polygondata );
  QVERIFY( polygondataGEO.wkbType() == Qgis::WkbType::Polygon );

  QgsFeature polygonFeature;
  polygonFeature.setGeometry( polygondataGEO );
  polygonLayer->dataProvider()->addFeature( polygonFeature );

  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( polygonFeature );

  const QVector<Vertex> verticespn = mapTool.collectedVertices();

  // should have 14 points: v1, m1, v2, m2, v3, m3, v4, m4, |<new ring>| v5, m5, v6, m6, v7, m7
  QCOMPARE( verticespn.length(), 14 );

  QVERIFY( verticespn.at( 0 ).type() == Vertex::Existing );
  QVERIFY( verticespn.at( 1 ).type() == Vertex::MidPoint );
  QVERIFY( verticespn.at( 2 ).type() == Vertex::Existing );
  QVERIFY( verticespn.at( 3 ).type() == Vertex::MidPoint );
  QVERIFY( verticespn.at( 4 ).type() == Vertex::Existing );
  QVERIFY( verticespn.at( 5 ).type() == Vertex::MidPoint );
  QVERIFY( verticespn.at( 6 ).type() == Vertex::Existing );
  QVERIFY( verticespn.at( 7 ).type() == Vertex::MidPoint );
  QVERIFY( verticespn.at( 8 ).type() == Vertex::Existing );
  QVERIFY( verticespn.at( 9 ).type() == Vertex::MidPoint );
  QVERIFY( verticespn.at( 10 ).type() == Vertex::Existing );
  QVERIFY( verticespn.at( 11 ).type() == Vertex::MidPoint );
  QVERIFY( verticespn.at( 12 ).type() == Vertex::Existing );
  QVERIFY( verticespn.at( 13 ).type() == Vertex::MidPoint );

  QCOMPARE( verticespn.at( 0 ).coordinates(), QgsPoint( polygondata.at( 0 ).at( 0 ) ) );
  QCOMPARE( verticespn.at( 2 ).coordinates(), QgsPoint( polygondata.at( 0 ).at( 1 ) ) );
  QCOMPARE( verticespn.at( 4 ).coordinates(), QgsPoint( polygondata.at( 0 ).at( 2 ) ) );
  QCOMPARE( verticespn.at( 6 ).coordinates(), QgsPoint( polygondata.at( 0 ).at( 3 ) ) );
  QCOMPARE( verticespn.at( 8 ).coordinates(), QgsPoint( polygondata.at( 1 ).at( 0 ) ) );
  QCOMPARE( verticespn.at( 10 ).coordinates(), QgsPoint( polygondata.at( 1 ).at( 1 ) ) );
  QCOMPARE( verticespn.at( 12 ).coordinates(), QgsPoint( polygondata.at( 1 ).at( 2 ) ) );

  QCOMPARE( verticespn.at( 0 ).vertexId().part, 0 );
  QCOMPARE( verticespn.at( 7 ).vertexId().part, 0 );

  QCOMPARE( verticespn.at( 0 ).vertexId().ring, 0 );
  QCOMPARE( verticespn.at( 7 ).vertexId().ring, 0 );
  QCOMPARE( verticespn.at( 8 ).vertexId().ring, 1 );
  QCOMPARE( verticespn.at( 10 ).vertexId().ring, 1 );
  QCOMPARE( verticespn.at( 13 ).vertexId().ring, 1 );

  //
  // multipolygon with holes
  //
  QgsVectorLayer *multiPolygonLayer = new QgsVectorLayer( QStringLiteral( "MultiPolygon?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  project->addMapLayer( multiPolygonLayer );

  QgsMultiPolygonXY multipolygonringsdata =
  {
    {
      // part 1
      {
        // exterior ring
        QgsPointXY( -10, -20 ),
        QgsPointXY( 0, -20 ),
        QgsPointXY( 0, 0 ),
        QgsPointXY( -10, 0 )
      }
    },
    {
      // part 2
      {
        // exterior ring
        QgsPointXY( 40, 40 ),
        QgsPointXY( 50, 40 ),
        QgsPointXY( 50, 50 ),
        QgsPointXY( 40, 50 )
      },
      {
        // hole (interior ring)
        QgsPointXY( 42, 42 ),
        QgsPointXY( 48, 43 ),
        QgsPointXY( 45, 48 )
      }
    },
    {
      // part 3
      {
        // exterior ring
        QgsPointXY( 10, 20 ),
        QgsPointXY( 0, 20 )
      }
    }
  };
  QgsGeometry multipolygonringsdataGEO = QgsGeometry::fromMultiPolygonXY( multipolygonringsdata );
  // We want to test case when last part is a line, not polygon.
  // As QGIS automatically close unclosed rings we need to alter geometry
  // manually and remove one point from the exterior ring of the part 3.
  QgsMultiPolygon *multiPolygon = qgsgeometry_cast<QgsMultiPolygon *>( multipolygonringsdataGEO.constGet() );
  QgsPolygon *poly = multiPolygon->polygonN( 2 );
  QgsLineString *ring = qgsgeometry_cast<QgsLineString *>( poly->exteriorRing() );
  QgsPointSequence points;
  ring->points( points );
  points.removeLast();
  ring->setPoints( points );

  QVERIFY( multipolygonringsdataGEO.wkbType() == Qgis::WkbType::MultiPolygon );

  QgsFeature multipolygonringsFeature;
  multipolygonringsFeature.setGeometry( multipolygonringsdataGEO );
  multiPolygonLayer->dataProvider()->addFeature( multipolygonringsFeature );

  mapTool.setActiveLayer( multiPolygonLayer );
  mapTool.setActiveFeature( multipolygonringsFeature );

  const QVector<Vertex> verticesmpr = mapTool.collectedVertices();

  // should have 25 points:
  // v1, m1, ... v4, m4,
  // |<new part>| v5, m5, ... v8, m8, |<new ring>| v9, m9, ... v11, m11,
  // |<new part>| v12, m12, v13

  QCOMPARE( verticesmpr.length(), 25 );

  // part 1
  QVERIFY( verticesmpr.at( 0 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 1 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 2 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 3 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 4 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 5 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 6 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 7 ).type() == Vertex::MidPoint );
  // part 2 - ext
  QVERIFY( verticesmpr.at( 8 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 9 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 10 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 11 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 12 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 13 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 14 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 15 ).type() == Vertex::MidPoint );
  // part 2 - hole
  QVERIFY( verticesmpr.at( 16 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 17 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 18 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 19 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 20 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 21 ).type() == Vertex::MidPoint );
  // part 3
  QVERIFY( verticesmpr.at( 22 ).type() == Vertex::Existing );
  QVERIFY( verticesmpr.at( 23 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpr.at( 24 ).type() == Vertex::Existing );

  // part 1
  QCOMPARE( verticesmpr.at( 0 ).coordinates(), QgsPoint( multipolygonringsdata.at( 0 ).at( 0 ).at( 0 ) ) );
  QCOMPARE( verticesmpr.at( 2 ).coordinates(), QgsPoint( multipolygonringsdata.at( 0 ).at( 0 ).at( 1 ) ) );
  QCOMPARE( verticesmpr.at( 4 ).coordinates(), QgsPoint( multipolygonringsdata.at( 0 ).at( 0 ).at( 2 ) ) );
  QCOMPARE( verticesmpr.at( 6 ).coordinates(), QgsPoint( multipolygonringsdata.at( 0 ).at( 0 ).at( 3 ) ) );
  // part 2 - ext
  QCOMPARE( verticesmpr.at( 8 ).coordinates(), QgsPoint( multipolygonringsdata.at( 1 ).at( 0 ).at( 0 ) ) );
  QCOMPARE( verticesmpr.at( 10 ).coordinates(), QgsPoint( multipolygonringsdata.at( 1 ).at( 0 ).at( 1 ) ) );
  QCOMPARE( verticesmpr.at( 12 ).coordinates(), QgsPoint( multipolygonringsdata.at( 1 ).at( 0 ).at( 2 ) ) );
  QCOMPARE( verticesmpr.at( 14 ).coordinates(), QgsPoint( multipolygonringsdata.at( 1 ).at( 0 ).at( 3 ) ) );
  // part 2 - hole
  QCOMPARE( verticesmpr.at( 16 ).coordinates(), QgsPoint( multipolygonringsdata.at( 1 ).at( 1 ).at( 0 ) ) );
  QCOMPARE( verticesmpr.at( 18 ).coordinates(), QgsPoint( multipolygonringsdata.at( 1 ).at( 1 ).at( 1 ) ) );
  QCOMPARE( verticesmpr.at( 20 ).coordinates(), QgsPoint( multipolygonringsdata.at( 1 ).at( 1 ).at( 2 ) ) );
  // part 3
  QCOMPARE( verticesmpr.at( 22 ).coordinates(), QgsPoint( multipolygonringsdata.at( 2 ).at( 0 ).at( 0 ) ) );
  QCOMPARE( verticesmpr.at( 24 ).coordinates(), QgsPoint( multipolygonringsdata.at( 2 ).at( 0 ).at( 1 ) ) );

  QCOMPARE( verticesmpr.at( 1 ).vertexId().part, 0 );
  QCOMPARE( verticesmpr.at( 3 ).vertexId().part, 0 );
  QCOMPARE( verticesmpr.at( 5 ).vertexId().part, 0 );
  QCOMPARE( verticesmpr.at( 9 ).vertexId().part, 1 );
  QCOMPARE( verticesmpr.at( 11 ).vertexId().part, 1 );
  QCOMPARE( verticesmpr.at( 13 ).vertexId().part, 1 );
  QCOMPARE( verticesmpr.at( 17 ).vertexId().part, 1 );
  QCOMPARE( verticesmpr.at( 19 ).vertexId().part, 1 );
  QCOMPARE( verticesmpr.at( 21 ).vertexId().part, 1 );
  QCOMPARE( verticesmpr.at( 22 ).vertexId().part, 2 );
  QCOMPARE( verticesmpr.at( 23 ).vertexId().part, 2 );
  QCOMPARE( verticesmpr.at( 24 ).vertexId().part, 2 );

  QCOMPARE( verticesmpr.at( 0 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpr.at( 7 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpr.at( 8 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpr.at( 10 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpr.at( 13 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpr.at( 15 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpr.at( 16 ).vertexId().ring, 1 );
  QCOMPARE( verticesmpr.at( 19 ).vertexId().ring, 1 );
  QCOMPARE( verticesmpr.at( 21 ).vertexId().ring, 1 );
  QCOMPARE( verticesmpr.at( 22 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpr.at( 24 ).vertexId().ring, 0 );

  //
  // multipolygon with empty and invalid parts
  //

  QgsMultiPolygonXY multipolygoninvaliddata =
  {
    {
      // part 1
      {
        // exterior ring
        QgsPointXY( -10, -20 ),
        QgsPointXY( 0, -20 ),
        QgsPointXY( 0, 0 )
      }
    },
    {
      // part 2 - invalid with hole
      {
        // exterior ring
        QgsPointXY( 40, 40 ),
        QgsPointXY( 50, 40 )
      },
      {
        // hole (interior ring)
        QgsPointXY( 42, 42 ),
        QgsPointXY( 48, 43 ),
        QgsPointXY( 45, 48 )
      }
    },
    {
      // part 3 - invalid
      {
        // exterior ring
        QgsPointXY( 10, 20 )
      }
    },
    {
      // part 4 - empty
    }
  };
  QgsGeometry multipolygoninvaliddataGEO = QgsGeometry::fromMultiPolygonXY( multipolygoninvaliddata );
  // We want to test case when 2nd part is a line, not polygon.
  // As QGIS automatically close unclosed rings we need to alter geometry
  multiPolygon = qgsgeometry_cast<QgsMultiPolygon *>( multipolygoninvaliddataGEO.constGet() );
  poly = multiPolygon->polygonN( 1 );
  ring = qgsgeometry_cast<QgsLineString *>( poly->exteriorRing() );
  ring->points( points );
  points.removeLast();
  ring->setPoints( points );

  QVERIFY( multipolygoninvaliddataGEO.wkbType() == Qgis::WkbType::MultiPolygon );

  QgsFeature multipolygoninvalidFeature;
  multipolygoninvalidFeature.setGeometry( multipolygoninvaliddataGEO );
  multiPolygonLayer->dataProvider()->addFeature( multipolygoninvalidFeature );

  mapTool.setActiveLayer( multiPolygonLayer );
  mapTool.setActiveFeature( multipolygoninvalidFeature );

  const QVector<Vertex> verticesmpi = mapTool.collectedVertices();

  // should have 16 points:
  // v1, m1, v2, m2, v3, m3,
  // |<new part>| v4, m4, v5, |<new ring>| v6, m6, v7, m7, v8, m8,
  // |<new part>| v9

  QCOMPARE( verticesmpi.length(), 16 );

  // part 1
  QVERIFY( verticesmpi.at( 0 ).type() == Vertex::Existing );
  QVERIFY( verticesmpi.at( 1 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpi.at( 2 ).type() == Vertex::Existing );
  QVERIFY( verticesmpi.at( 3 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpi.at( 4 ).type() == Vertex::Existing );
  QVERIFY( verticesmpi.at( 5 ).type() == Vertex::MidPoint );
  // part 2 - ext
  QVERIFY( verticesmpi.at( 6 ).type() == Vertex::Existing );
  QVERIFY( verticesmpi.at( 7 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpi.at( 8 ).type() == Vertex::Existing );
  // part 2 - hole
  QVERIFY( verticesmpi.at( 9 ).type() == Vertex::Existing );
  QVERIFY( verticesmpi.at( 10 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpi.at( 11 ).type() == Vertex::Existing );
  QVERIFY( verticesmpi.at( 12 ).type() == Vertex::MidPoint );
  QVERIFY( verticesmpi.at( 13 ).type() == Vertex::Existing );
  QVERIFY( verticesmpi.at( 14 ).type() == Vertex::MidPoint );
  // part 3
  QVERIFY( verticesmpi.at( 15 ).type() == Vertex::Existing );
  // part 4 - empty, no element

  // part 1
  QCOMPARE( verticesmpi.at( 0 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 0 ).at( 0 ).at( 0 ) ) );
  QCOMPARE( verticesmpi.at( 2 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 0 ).at( 0 ).at( 1 ) ) );
  QCOMPARE( verticesmpi.at( 4 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 0 ).at( 0 ).at( 2 ) ) );
  // part 2 - ext
  QCOMPARE( verticesmpi.at( 6 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 1 ).at( 0 ).at( 0 ) ) );
  QCOMPARE( verticesmpi.at( 8 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 1 ).at( 0 ).at( 1 ) ) );
  // part 2 - hole
  QCOMPARE( verticesmpi.at( 9 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 1 ).at( 1 ).at( 0 ) ) );
  QCOMPARE( verticesmpi.at( 11 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 1 ).at( 1 ).at( 1 ) ) );
  QCOMPARE( verticesmpi.at( 13 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 1 ).at( 1 ).at( 2 ) ) );
  // part 3
  QCOMPARE( verticesmpi.at( 15 ).coordinates(), QgsPoint( multipolygoninvaliddata.at( 2 ).at( 0 ).at( 0 ) ) );

  QCOMPARE( verticesmpi.at( 1 ).vertexId().part, 0 );
  QCOMPARE( verticesmpi.at( 3 ).vertexId().part, 0 );
  QCOMPARE( verticesmpi.at( 5 ).vertexId().part, 0 );
  QCOMPARE( verticesmpi.at( 6 ).vertexId().part, 1 );
  QCOMPARE( verticesmpi.at( 9 ).vertexId().part, 1 );
  QCOMPARE( verticesmpi.at( 11 ).vertexId().part, 1 );
  QCOMPARE( verticesmpi.at( 13 ).vertexId().part, 1 );
  QCOMPARE( verticesmpi.at( 14 ).vertexId().part, 1 );
  QCOMPARE( verticesmpi.at( 15 ).vertexId().part, 2 );

  QCOMPARE( verticesmpi.at( 0 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpi.at( 3 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpi.at( 6 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpi.at( 8 ).vertexId().ring, 0 );
  QCOMPARE( verticesmpi.at( 9 ).vertexId().ring, 1 );
  QCOMPARE( verticesmpi.at( 14 ).vertexId().ring, 1 );
  QCOMPARE( verticesmpi.at( 15 ).vertexId().ring, 0 );
}

void TestMapTools::testZMRecording()
{
  QgsVectorLayer *pointZLayer = new QgsVectorLayer( QStringLiteral( "PointZ?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsVectorLayer *lineZMLayer = new QgsVectorLayer( QStringLiteral( "LineStringZM?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  QgsVectorLayer *polygonMLayer = new QgsVectorLayer( QStringLiteral( "PolygonM?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );

  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  mapTool.setState( RecordingMapTool::Record );

  mapTool.setActiveLayer( pointZLayer );
  mapTool.setActiveFeature( QgsFeature() );

  QgsPoint pointToAdd( -97.129, 22.602, 10 );

  //
  // PointZ geo
  //
  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointToAdd );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointToAdd );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // LineStringZM geo

  mapTool.setActiveLayer( nullptr );
  mapTool.setActiveLayer( lineZMLayer );

  pointToAdd = QgsPoint( -97.129, 22.602, 10, 100 );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointToAdd );

  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointToAdd );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  // PolygoneM geo

  mapTool.setActiveLayer( nullptr );
  mapTool.setActiveLayer( polygonMLayer );

  pointToAdd = QgsPoint( -97.129, 22.602 );
  pointToAdd.addMValue();
  pointToAdd.setZ( 100 );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointToAdd );

  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointToAdd );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  delete project;
  delete pointZLayer;
  delete lineZMLayer;
  delete polygonMLayer;
}

void TestMapTools::testAntennaHeight()
{
  QgsVectorLayer *pointLayer = new QgsVectorLayer( QStringLiteral( "PointZ?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );

  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );
  mapTool.setPositionKit( mPositionKit );

  mAppSettings->setGpsAntennaHeight( 0 );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  mapTool.setState( RecordingMapTool::Record );

  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( QgsFeature() );

  QgsPoint pointToAdd( -97.129, 22.602, 10 );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointToAdd );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointToAdd );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.setActiveFeature( QgsFeature() );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  double antennaHeight = 2.0;
  mAppSettings->setGpsAntennaHeight( antennaHeight );

  mapTool.addPoint( pointToAdd );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QgsPoint p = mapTool.recordedGeometry().vertexAt( 0 );
  QCOMPARE( p.z(), pointToAdd.z() - antennaHeight );
}

void TestMapTools::testSmallTracking()
{
#ifdef HAVE_BLUETOOTH
  QgsVectorLayer *pointLayer = new QgsVectorLayer( QStringLiteral( "PointZ?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );

  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mAppSettings->setGpsAntennaHeight( 0 );

  mapTool.setMapSettings( ms );
  mapTool.setPositionKit( mPositionKit );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setRecordingType( RecordingMapTool::StreamMode );
  mapTool.setRecordingInterval( 10 );
  mapTool.setRecordingIntervalType( StreamingIntervalType::IntervalType::Distance );

  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( QgsFeature() );

  BluetoothPositionProvider *btProvider = new BluetoothPositionProvider( "AA:AA:FF:AA:00:10", "testBluetoothProvider" );
  mPositionKit->setPositionProvider( btProvider );

  NmeaParser parser;
  QString nmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile nmeaFile( nmeaPositionFilePath );
  nmeaFile.open( QFile::ReadOnly );
  QVERIFY( nmeaFile.isOpen() );
  QgsGpsInformation position = parser.parseNmeaString( nmeaFile.readAll() );
  GeoPosition pos = GeoPosition::fromQgsGpsInformation( position );
  emit btProvider->positionChanged( pos );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );

  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), mPositionKit->positionCoordinate() );

  pos.latitude = 48.10319550872501;
  pos.longitude = 17.105920116666667;
  emit btProvider->positionChanged( pos );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), mPositionKit->positionCoordinate() );
#endif
}

void TestMapTools::testAvoidIntersections()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );
  project->setAvoidIntersectionsMode( Qgis::AvoidIntersectionsMode::AvoidIntersectionsCurrentLayer );
  QCOMPARE( project->avoidIntersectionsMode(), Qgis::AvoidIntersectionsMode::AvoidIntersectionsCurrentLayer );

  InputMapCanvasMap canvas;
  InputMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  QCOMPARE( mapTool.recordingType(), RecordingMapTool::Manual );

  // Create memory layer to work with
  QgsVectorLayer *polygonLayer = new QgsVectorLayer( "Polygon?crs=epsg:4326", "polygonlayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveLayer( polygonLayer );
  mapTool.setActiveFeature( QgsFeature() );

  // create first polygon
  mapTool.addPoint( { -95.5, 22.0 } );
  mapTool.addPoint( { -97.5, 22.0 } );
  mapTool.addPoint( { -97.5, 26.0 } );
  mapTool.addPoint( { -95.5, 26.0 } );
  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  // finish creating the feature
  FeatureLayerPair pair = mapTool.getFeatureLayerPair();
  QVERIFY( pair.layer()->isValid() );
  QVERIFY( pair.feature().isValid() );

  // create 2nd polygon intersecting first
  mapTool.setActiveFeature( QgsFeature() );
  mapTool.addPoint( { -96.5, 23.0 } );
  mapTool.addPoint( { -96.5, 25.0 } );
  mapTool.addPoint( { -90.5, 25.0 } );
  mapTool.addPoint( { -90.5, 23.0 } );
  QVERIFY( mapTool.hasValidGeometry() );
  QCOMPARE( mapTool.recordedGeometry().constGet()->area(), 12 );
  // finish creating the feature
  pair = mapTool.getFeatureLayerPair();
  QVERIFY( pair.layer()->isValid() );
  QVERIFY( pair.feature().isValid() );
  QCOMPARE( pair.feature().geometry().constGet()->area(), 10 );

  // now edit 2nd polygon
  // first we move one vertex to intersect the other polygon, then we move
  // another vertex somewhere else not intersecting anything
  mapTool.setState( RecordingMapTool::Grab );
  mapTool.setActiveVertex( Vertex( QgsVertexId( 0, 0, 2 ), { -95.5, 23.0 }, Vertex::Existing ) );
  QVERIFY( mapTool.activeVertex().isValid() );
  mapTool.releaseVertex( { -96.5, 23 } );
  mapTool.getFeatureLayerPair();
  QCOMPARE( mapTool.recordedGeometry().constGet()->area(), 10 );

  mapTool.setActiveVertex( Vertex( QgsVertexId( 0, 0, 3 ), { -90.5, 25.0 }, Vertex::Existing ) );
  QVERIFY( mapTool.activeVertex().isValid() );
  mapTool.releaseVertex( { -88.5, 25.0 } );
  mapTool.getFeatureLayerPair();
  QCOMPARE( mapTool.recordedGeometry().constGet()->area(), 12 );

  delete project;
  delete polygonLayer;
}
