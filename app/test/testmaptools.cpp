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
  recordTool->setLayer( flySectorLayer );

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

  QVERIFY( recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[2] );

  QVERIFY( recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[3] );

  QCOMPARE( geometryChangedSpy.count(), 6 );

  delete project;
  delete recordTool;
}

void TestMapTools::testExistingVertices()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";
  QgsProject *project = new QgsProject();
  QVERIFY( project->read( projectDir + "/" + projectName ) );
  QgsMapLayer *polyL = project->mapLayersByName( QStringLiteral( "FlySector" ) ).at( 0 );
  QgsVectorLayer *polygonLayer = static_cast<QgsVectorLayer *>( polyL );
  QVERIFY( polygonLayer && polygonLayer->isValid() );
  QgsMapLayer *lineL = project->mapLayersByName( QStringLiteral( "Roads" ) ).at( 0 );
  QgsVectorLayer *lineLayer = static_cast<QgsVectorLayer *>( lineL );
  QVERIFY( lineLayer && lineLayer->isValid() );
  QgsMapLayer *pointL = project->mapLayersByName( QStringLiteral( "Planes" ) ).at( 0 );
  QgsVectorLayer *pointLayer = static_cast<QgsVectorLayer *>( pointL );
  QVERIFY( pointLayer && pointLayer->isValid() );

  RecordingMapTool *mapTool = new RecordingMapTool();

  QgsGeometry geometry;

  QgsPolygon *polygon = new QgsPolygon( new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) << QgsPoint( 0, 0 ) ) );
  geometry.set( polygon );
  mapTool->setLayer( polygonLayer );
  mapTool->setInitialGeometry( geometry );
  QgsGeometry vertices = mapTool->existingVertices();

  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 3 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 1, 1 ) );

  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) << QgsPoint( 2, 2 ) );
  geometry.set( line );
  mapTool->setLayer( lineLayer );
  mapTool->setInitialGeometry( geometry );
  vertices = mapTool->existingVertices();
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 1, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 2, 2 ) );

  geometry = QgsGeometry::fromWkt( "MultiPoint( 0 0, 1 1, 2 2)" );
  mapTool->setLayer( pointLayer );
  mapTool->setInitialGeometry( geometry );
  vertices = mapTool->existingVertices();
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 3 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 1, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 2, 2 ) );

  delete mapTool;
}

void TestMapTools::testMidSegmentVertices()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";
  QgsProject *project = new QgsProject();
  QVERIFY( project->read( projectDir + "/" + projectName ) );
  QgsMapLayer *polyL = project->mapLayersByName( QStringLiteral( "FlySector" ) ).at( 0 );
  QgsVectorLayer *polygonLayer = static_cast<QgsVectorLayer *>( polyL );
  QVERIFY( polygonLayer && polygonLayer->isValid() );
  QgsMapLayer *lineL = project->mapLayersByName( QStringLiteral( "Roads" ) ).at( 0 );
  QgsVectorLayer *lineLayer = static_cast<QgsVectorLayer *>( lineL );
  QVERIFY( lineLayer && lineLayer->isValid() );
  QgsMapLayer *pointL = project->mapLayersByName( QStringLiteral( "Planes" ) ).at( 0 );
  QgsVectorLayer *pointLayer = static_cast<QgsVectorLayer *>( pointL );
  QVERIFY( pointLayer && pointLayer->isValid() );

  RecordingMapTool *mapTool = new RecordingMapTool();

  QgsGeometry geometry;

  QgsPolygon *polygon = new QgsPolygon( new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 2 ) << QgsPoint( 2, 2 ) << QgsPoint( 2, 0 ) << QgsPoint( 0, 0 ) ) );
  geometry.set( polygon );
  mapTool->setLayer( polygonLayer );
  mapTool->setInitialGeometry( geometry );
  QgsGeometry vertices = mapTool->midPoints();
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 1, 2 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 2, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 1, 0 ) );

  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );
  mapTool->setLayer( lineLayer );
  mapTool->setInitialGeometry( geometry );
  vertices = mapTool->midPoints();
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, -0.5 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 0.5 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 0.5, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 1.5, 1 ) );

  geometry = QgsGeometry::fromWkt( "MultiPoint( 0 0, 1 1, 2 2)" );
  mapTool->setLayer( pointLayer );
  mapTool->setInitialGeometry( geometry );
  vertices = mapTool->midPoints();
  QVERIFY( vertices.constGet()->vertexCount() == 0 );

  delete mapTool;
}

void TestMapTools::testHandles()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";
  QgsProject *project = new QgsProject();
  QVERIFY( project->read( projectDir + "/" + projectName ) );
  QgsMapLayer *lineL = project->mapLayersByName( QStringLiteral( "Roads" ) ).at( 0 );
  QgsVectorLayer *lineLayer = static_cast<QgsVectorLayer *>( lineL );
  QVERIFY( lineLayer && lineLayer->isValid() );

  RecordingMapTool *mapTool = new RecordingMapTool();

  QgsGeometry geometry;

  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );
  mapTool->setLayer( lineLayer );
  mapTool->setInitialGeometry( geometry );
  QgsGeometry handles = mapTool->handles();
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

  delete mapTool;
}

void TestMapTools::testLookForVertex()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";
  QgsProject *project = new QgsProject();
  QVERIFY( project->read( projectDir + "/" + projectName ) );
  QgsMapLayer *lineL = project->mapLayersByName( QStringLiteral( "Roads" ) ).at( 0 );
  QgsVectorLayer *lineLayer = static_cast<QgsVectorLayer *>( lineL );
  QVERIFY( lineLayer && lineLayer->isValid() );

  QgsQuickMapCanvasMap canvas;
  QgsQuickMapSettings *ms = canvas.mapSettings();
  ms->setProject( project );
  ms->setDestinationCrs( project->crs() );

  RecordingMapTool *mapTool = new RecordingMapTool();
  mapTool->setMapSettings( ms );

  QgsGeometry geometry;
  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );
  mapTool->setLayer( lineLayer );
  mapTool->setInitialGeometry( geometry );

  // when initial geometry set we start in View state
  QCOMPARE( mapTool->state(), RecordingMapTool::MapToolState::View );

  // Start handle point. Active vertex is invalid, state changes to Record
  QPointF screenPoint = ms->coordinateToScreen( QgsPoint( -0.05, -0.53 ) );
  mapTool->lookForVertex( screenPoint );
  QVERIFY( !mapTool->activeVertex().isValid() );
  QCOMPARE( mapTool->state(), RecordingMapTool::MapToolState::Record );

  // reset state to View
  mapTool->setState( RecordingMapTool::MapToolState::View );

  // Existing geometry vertex (first point). Active vertex is valid, state changes to Grab
  screenPoint = ms->coordinateToScreen( QgsPoint( -0.01, 0.1 ) );
  mapTool->lookForVertex( screenPoint );
  QVERIFY( mapTool->activeVertex().isValid() );
  QCOMPARE( mapTool->activeVertex().type(), Vertex::Existing );
  QCOMPARE( mapTool->activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool->activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool->activeVertex().vertexId().vertex, 0 );

  // reset state to View
  mapTool->setState( RecordingMapTool::MapToolState::View );

  // Midpoint between 2nd and 3rd existing vertices (0.5, 1).
  // This creates new point with the midpoint coordinates and triggers
  // rebuild of the vertex cache (new exsting vertex will be added).
  // Active vertex is valid, state changes to Grab.
  screenPoint = ms->coordinateToScreen( QgsPoint( 0.6, 1.2 ) );
  mapTool->lookForVertex( screenPoint );
  QVERIFY( mapTool->activeVertex().isValid() );
  QCOMPARE( mapTool->activeVertex().type(), Vertex::Existing );
  QCOMPARE( mapTool->activeVertex().vertexId().part, 0 );
  QCOMPARE( mapTool->activeVertex().vertexId().ring, 0 );
  QCOMPARE( mapTool->activeVertex().vertexId().vertex, 2 );
  QCOMPARE( mapTool->activeVertex().coordinates(), QgsPoint( 0.5, 1 ) );

  // reset state to View
  mapTool->setState( RecordingMapTool::MapToolState::View );

  // Distant point. Active vertex is invalid (nothing found), View state
  screenPoint = ms->coordinateToScreen( QgsPoint( 15, 13 ) );
  mapTool->lookForVertex( screenPoint );
  QVERIFY( !mapTool->activeVertex().isValid() );
  QCOMPARE( mapTool->state(), RecordingMapTool::MapToolState::View );
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

  mapTool.setLayer( pointLayer );

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

  // nothing should happen really
  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 1 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[0] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  QVERIFY( mapTool.recordedGeometry().wkbType() == QgsWkbTypes::Point );

  // if maptool is in GRAB and VIEW state, no point should be added
  mapTool.setLayer( nullptr );
  mapTool.setLayer( pointLayer );

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
  delete pointLayer;
}

void TestMapTools::testAddVertexMultiPointLayer()
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
  QgsVectorLayer *multiPointLayer = new QgsVectorLayer( "MultiPoint?crs=epsg:4326", "mpointlayer", "memory" );

  mapTool.setState( RecordingMapTool::Record );

  mapTool.setLayer( multiPointLayer );

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
  mapTool.setLayer( nullptr );
  mapTool.setLayer( multiPointLayer );

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

  mapTool.setLayer( lineLayer );

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

  mapTool.setNewVertexOrder( RecordingMapTool::Start );

  mapTool.addPoint( pointsToAdd[4] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[4] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's add point to the middle of line
  //

  mapTool.setNewVertexOrder( RecordingMapTool::End );

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
  mapTool.setLayer( multiLineLayer );

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

  mapTool.setNewVertexOrder( RecordingMapTool::Start );

  mapTool.addPoint( pointsToAdd[4] );

  QVERIFY( mapTool.hasValidGeometry() );
  QVERIFY( mapTool.recordedGeometry().constGet()->nCoordinates() == 5 );
  QCOMPARE( mapTool.recordedGeometry().vertexAt( 0 ), pointsToAdd[4] );

  QVERIFY( !mapTool.activeVertex().isValid() );
  QVERIFY( mapTool.state() == RecordingMapTool::Record );

  //
  // Let's add point to the middle of line
  //

  mapTool.setNewVertexOrder( RecordingMapTool::End );

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
  mapTool.setLayer( polygonLayer );

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
  mapTool.setLayer( polygonLayer );

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
