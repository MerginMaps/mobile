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

#include "qgspoint.h"
#include "qgslinestring.h"
#include "qgspolygon.h"
#include "qgsmultipolygon.h"
#include "qgslinestring.h"
#include "qgsgeometry.h"

#include "qgsquickmapcanvasmap.h"
#include "qgsquickmapsettings.h"

#include "testutils.h"

#include "inpututils.h"
#include "snaputils.h"
#include "maptools/splittingmaptool.h"
#include "maptools/recordingmaptool.h"

#include "featurelayerpair.h"

void setupMapSettings( QgsQuickMapSettings *settings, QgsProject *project, QgsRectangle extent, QSize outputsize );

void setupMapSettings( QgsQuickMapSettings *settings, QgsProject *project, QgsRectangle extent, QSize outputsize )
{
  if ( !project || !settings )
    return;

  settings->setProject( project );
  settings->setDestinationCrs( project->crs() );
  settings->setLayers( project->layers<QgsMapLayer *>().toList() );
  settings->setOutputSize( outputsize );
  settings->setExtent( extent );
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

  QgsQuickMapCanvasMap canvas;

  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QgsQuickMapCanvasMap canvas;

  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QgsQuickMapCanvasMap canvas;

  QgsQuickMapSettings *ms = canvas.mapSettings();
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

  QCOMPARE( geometryChangedSpy.count(), 2 );

  recordTool->addPoint( pointsToAdd[0] );
  recordTool->addPoint( pointsToAdd[1] );

  QVERIFY( !recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[2] );

  QVERIFY( recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[3] );

  QCOMPARE( geometryChangedSpy.count(), 6 );

  delete project;
  delete recordTool;
}

void TestMapTools::testExistingVertices()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
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
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
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
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
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

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
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
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, -0.5 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 0.5 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 0.5, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 1.5, 1 ) );

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

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QCOMPARE( handles.wkbType(), QgsWkbTypes::MultiLineString );
  QCOMPARE( handles.constGet()->partCount(), 2 );

  // handle segments first point is a handle point and second point is
  // an existing vertex (start/end of the line)
  QVector<QgsGeometry> expected =
  {
    QgsGeometry::fromWkt( "LINESTRING(0 -0.5, 0 0)" ),
    QgsGeometry::fromWkt( "LINESTRING(1 1, 1.5 1)" ),
  };

  const QVector<QgsGeometry> parts = handles.asGeometryCollection();
  for ( int i = 0; i < parts.count(); i++ )
  {
    QVERIFY( parts.at( i ).equals( expected.at( i ) ) );
  }

  delete lineLayer;
}

void TestMapTools::testLookForVertex()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QPointF screenPoint = ms->coordinateToScreen( QgsPoint( -0.05, -0.53 ) );
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

  delete lineLayer;
}

void TestMapTools::testAddVertexPointLayer()
{
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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

  QVERIFY( mapTool.recordedGeometry().wkbType() == QgsWkbTypes::Point );

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
  QSKIP( "testAddVertexMultiPointLayer will be enabled later; ENABLE WHEN PUSHING TO MASTER" );
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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

  // nothing should happen really
  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == QgsWkbTypes::MultiPoint );

  // if maptool is in GRAB and VIEW state, no point should be added

  // clear recorded geometry
  mapTool.setActiveLayer( nullptr );
  mapTool.setActiveLayer( multiPointLayer );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.setState( RecordingMapTool::Grab );

  mapTool.addPoint( pointsToAdd[0] );

  // no point should be added
  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 0 );

  mapTool.setState( RecordingMapTool::View );

  mapTool.addPoint( pointsToAdd[0] );

  // no point should be added
  QVERIFY( !mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 0 );

  delete project;
  delete ms;
  delete multiPointLayer;
}

void TestMapTools::testAddVertexLineLayer()
{
  QSKIP( "testAddVertexLineLayer will be enabled later; ENABLE WHEN PUSHING TO MASTER" );
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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

  QVERIFY( mapTool.recordedGeometry().wkbType() == QgsWkbTypes::LineString );

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

  delete project;
  delete ms;
  delete lineLayer;
}

void TestMapTools::testAddVertexMultiLineLayer()
{
  QSKIP( "testAddVertexMultiLineLayer will be enabled later; ENABLE WHEN PUSHING TO MASTER" );
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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

  // TODO: add to this test adding point to other parts

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

  QVERIFY( mapTool.recordedGeometry().wkbType() == QgsWkbTypes::LineString );

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

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::View );

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

  delete project;
  delete ms;
  delete multiLineLayer;
}

void TestMapTools::testAddVertexPolygonLayer()
{
  QSKIP( "testAddVertexPolygonLayer will be enabled later; ENABLE WHEN PUSHING TO MASTER" );
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 2 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == QgsWkbTypes::Polygon );

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

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveVertex( Vertex() );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[3] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's insert points to the middle
  //

  Vertex addVertexPos = Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[4], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[4] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[4] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 3 ), pointsToAdd[5], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[5] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 6 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[5] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 6 ), pointsToAdd[6], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[6] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 7 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[6] );

  delete project;
  delete ms;
  delete polygonLayer;
}

void TestMapTools::testAddVertexMultiPolygonLayer()
{
  QSKIP( "testAddVertexMultiPolygonLayer will be enabled later; ENABLE WHEN PUSHING TO MASTER" );
  RecordingMapTool mapTool;

  QgsProject *project = TestUtils::loadPlanesTestProject();
  QVERIFY( project && !project->homePath().isEmpty() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
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
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 2 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[1] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == QgsWkbTypes::Polygon );

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

  mapTool.setState( RecordingMapTool::Record );
  mapTool.setActiveVertex( Vertex() );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  mapTool.addPoint( pointsToAdd[3] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 4 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[3] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's insert points to the middle
  //

  Vertex addVertexPos = Vertex( QgsVertexId( 0, 0, 1 ), pointsToAdd[4], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[4] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[4] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 3 ), pointsToAdd[5], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[5] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 6 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 3 ), pointsToAdd[5] );

  addVertexPos = Vertex( QgsVertexId( 0, 0, 6 ), pointsToAdd[6], Vertex::Existing );
  mapTool.addPointAtPosition( addVertexPos, pointsToAdd[6] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 7 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), pointsToAdd[6] );

  // TODO: Add parts checks

  delete project;
  delete ms;
  delete polygonLayer;
}

void TestMapTools::testUpdateVertex()
{
  /*
    //
    // Take some initial geometry and update existing vertices position.
    // It should work only when tool is in GRAB state
    //

    RecordingMapTool mapTool;

    QgsGeometry line = QgsGeometry::fromPolyline(
    {
      QgsPoint( 10, 20 ),
      QgsPoint( 20, 30 ),
      QgsPoint( 30, 40 ),
    } );

    mapTool.setInitialGeometry( line );

    Vertex updateVertexId = Vertex( QgsVertexId( 0, 0, 1 ), QgsPoint( 20, 30 ), Vertex::Existing );

    mapTool.updateVertex( updateVertexId, QgsPoint( 50, 50 ) );

    QCOMPARE( mapTool.recordedGeometry().vertexAt( 1 ), QgsPoint( 50, 50 ) );
  */
}

void TestMapTools::testRemoveVertex()
{
  // remove point to:
  // - point geo
  // - multipoint geo
  // - line geo
  // - multiline geo
  // - polygon geo
  // - multipolygon geo
}

void TestMapTools::testLookForVertexV2()
{
  // try to identify:
  // - no vertex
  // - ring vertex
  // - part vertex
  // -

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

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
  setupMapSettings( ms, project, QgsRectangle( -107.54331499504026226, 21.62302175066136556, -72.73224633912816728, 51.49933451998575151 ), QSize( 600, 1096 ) );

  mapTool.setMapSettings( ms );

  //
  // point
  //

  QgsVectorLayer *pointLayer = new QgsVectorLayer( QStringLiteral( "Point?crs=epsg:4326" ), QString(), QStringLiteral( "memory" ) );
  project->addMapLayer( pointLayer );

  QgsPointXY pointdata = QgsPointXY( 10, 20 );
  QgsGeometry pointdataGEO = QgsGeometry::fromPointXY( pointdata );
  QVERIFY( pointdataGEO.wkbType() == QgsWkbTypes::Point );

  QgsFeature pointFeature;
  pointFeature.setGeometry( pointdataGEO );
  pointLayer->dataProvider()->addFeature( pointFeature );

  mapTool.setActiveLayer( pointLayer );
  mapTool.setActiveFeature( pointFeature );

  QCOMPARE( mapTool.collectedVertices().length(), 1 );
  QVERIFY( mapTool.collectedVertices().at( 0 ).type() == Vertex::Existing );
  QCOMPARE( mapTool.collectedVertices().at( 0 ).coordinates(), pointdata );

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
  QVERIFY( multipointdataGEO.wkbType() == QgsWkbTypes::MultiPoint );

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

  QCOMPARE( verticesmp.at( 0 ).coordinates(), multipointdata.at( 0 ) );
  QCOMPARE( verticesmp.at( 1 ).coordinates(), multipointdata.at( 1 ) );
  QCOMPARE( verticesmp.at( 2 ).coordinates(), multipointdata.at( 2 ) );

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
  QVERIFY( linestrindataGEO.wkbType() == QgsWkbTypes::LineString );

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

  QCOMPARE( verticesl.at( 1 ).coordinates(), linestringdata.at( 0 ) );
  QCOMPARE( verticesl.at( 3 ).coordinates(), linestringdata.at( 1 ) );
  QCOMPARE( verticesl.at( 5 ).coordinates(), linestringdata.at( 2 ) );

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
  QVERIFY( multilinestringdataGEO.wkbType() == QgsWkbTypes::MultiLineString );

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

  QCOMPARE( verticesml.at( 1 ).coordinates(), multilinestringdata.at( 0 ).at( 0 ) );
  QCOMPARE( verticesml.at( 3 ).coordinates(), multilinestringdata.at( 0 ).at( 1 ) );
  QCOMPARE( verticesml.at( 5 ).coordinates(), multilinestringdata.at( 0 ).at( 2 ) );
  QCOMPARE( verticesml.at( 8 ).coordinates(), multilinestringdata.at( 1 ).at( 0 ) );
  QCOMPARE( verticesml.at( 10 ).coordinates(), multilinestringdata.at( 1 ).at( 1 ) );
  QCOMPARE( verticesml.at( 12 ).coordinates(), multilinestringdata.at( 2 ).at( 0 ) );

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
  QVERIFY( simplepolygondataGEO.wkbType() == QgsWkbTypes::Polygon );

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

  QCOMPARE( verticesspn.at( 0 ).coordinates(), simplepolygondata.at( 0 ).at( 0 ) );
  QCOMPARE( verticesspn.at( 2 ).coordinates(), simplepolygondata.at( 0 ).at( 1 ) );
  QCOMPARE( verticesspn.at( 4 ).coordinates(), simplepolygondata.at( 0 ).at( 2 ) );
  QCOMPARE( verticesspn.at( 6 ).coordinates(), simplepolygondata.at( 0 ).at( 3 ) );

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
  QVERIFY( polygondataGEO.wkbType() == QgsWkbTypes::Polygon );

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

  QCOMPARE( verticespn.at( 0 ).coordinates(), polygondata.at( 0 ).at( 0 ) );
  QCOMPARE( verticespn.at( 2 ).coordinates(), polygondata.at( 0 ).at( 1 ) );
  QCOMPARE( verticespn.at( 4 ).coordinates(), polygondata.at( 0 ).at( 2 ) );
  QCOMPARE( verticespn.at( 6 ).coordinates(), polygondata.at( 0 ).at( 3 ) );
  QCOMPARE( verticespn.at( 8 ).coordinates(), polygondata.at( 1 ).at( 0 ) );
  QCOMPARE( verticespn.at( 10 ).coordinates(), polygondata.at( 1 ).at( 1 ) );
  QCOMPARE( verticespn.at( 12 ).coordinates(), polygondata.at( 1 ).at( 2 ) );

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

  QVERIFY( multipolygonringsdataGEO.wkbType() == QgsWkbTypes::MultiPolygon );

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
  QCOMPARE( verticesmpr.at( 0 ).coordinates(), multipolygonringsdata.at( 0 ).at( 0 ).at( 0 ) );
  QCOMPARE( verticesmpr.at( 2 ).coordinates(), multipolygonringsdata.at( 0 ).at( 0 ).at( 1 ) );
  QCOMPARE( verticesmpr.at( 4 ).coordinates(), multipolygonringsdata.at( 0 ).at( 0 ).at( 2 ) );
  QCOMPARE( verticesmpr.at( 6 ).coordinates(), multipolygonringsdata.at( 0 ).at( 0 ).at( 3 ) );
  // part 2 - ext
  QCOMPARE( verticesmpr.at( 8 ).coordinates(), multipolygonringsdata.at( 1 ).at( 0 ).at( 0 ) );
  QCOMPARE( verticesmpr.at( 10 ).coordinates(), multipolygonringsdata.at( 1 ).at( 0 ).at( 1 ) );
  QCOMPARE( verticesmpr.at( 12 ).coordinates(), multipolygonringsdata.at( 1 ).at( 0 ).at( 2 ) );
  QCOMPARE( verticesmpr.at( 14 ).coordinates(), multipolygonringsdata.at( 1 ).at( 0 ).at( 3 ) );
  // part 2 - hole
  QCOMPARE( verticesmpr.at( 16 ).coordinates(), multipolygonringsdata.at( 1 ).at( 1 ).at( 0 ) );
  QCOMPARE( verticesmpr.at( 18 ).coordinates(), multipolygonringsdata.at( 1 ).at( 1 ).at( 1 ) );
  QCOMPARE( verticesmpr.at( 20 ).coordinates(), multipolygonringsdata.at( 1 ).at( 1 ).at( 2 ) );
  // part 3
  QCOMPARE( verticesmpr.at( 22 ).coordinates(), multipolygonringsdata.at( 2 ).at( 0 ).at( 0 ) );
  QCOMPARE( verticesmpr.at( 24 ).coordinates(), multipolygonringsdata.at( 2 ).at( 0 ).at( 1 ) );

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

  QVERIFY( multipolygoninvaliddataGEO.wkbType() == QgsWkbTypes::MultiPolygon );

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
  QCOMPARE( verticesmpi.at( 0 ).coordinates(), multipolygoninvaliddata.at( 0 ).at( 0 ).at( 0 ) );
  QCOMPARE( verticesmpi.at( 2 ).coordinates(), multipolygoninvaliddata.at( 0 ).at( 0 ).at( 1 ) );
  QCOMPARE( verticesmpi.at( 4 ).coordinates(), multipolygoninvaliddata.at( 0 ).at( 0 ).at( 2 ) );
  // part 2 - ext
  QCOMPARE( verticesmpi.at( 6 ).coordinates(), multipolygoninvaliddata.at( 1 ).at( 0 ).at( 0 ) );
  QCOMPARE( verticesmpi.at( 8 ).coordinates(), multipolygoninvaliddata.at( 1 ).at( 0 ).at( 1 ) );
  // part 2 - hole
  QCOMPARE( verticesmpi.at( 9 ).coordinates(), multipolygoninvaliddata.at( 1 ).at( 1 ).at( 0 ) );
  QCOMPARE( verticesmpi.at( 11 ).coordinates(), multipolygoninvaliddata.at( 1 ).at( 1 ).at( 1 ) );
  QCOMPARE( verticesmpi.at( 13 ).coordinates(), multipolygoninvaliddata.at( 1 ).at( 1 ).at( 2 ) );
  // part 3
  QCOMPARE( verticesmpi.at( 15 ).coordinates(), multipolygoninvaliddata.at( 2 ).at( 0 ).at( 0 ) );

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
