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

  QVERIFY( !recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[2] );

  QVERIFY( recordTool->hasValidGeometry() );

  recordTool->addPoint( pointsToAdd[3] );

  QCOMPARE( geometryChangedSpy.count(), 6 );

  delete project;
  delete recordTool;
}

void TestMapTools::testExtractVertices()
{
  RecordingMapTool *mapTool = new RecordingMapTool();

  QgsGeometry geometry;

  QgsPolygon *polygon = new QgsPolygon( new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) << QgsPoint( 0, 0 ) ) );
  geometry.set( polygon );
  QgsGeometry vertices = mapTool->extractGeometryVertices( geometry );
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 1, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 0, 0 ) );

  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) << QgsPoint( 2, 2 ) );
  geometry.set( line );
  vertices = mapTool->extractGeometryVertices( geometry );
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 1, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 2, 2 ) );

  geometry = QgsGeometry::fromWkt( "MultiPoint( 0 0, 1 1, 2 2)" );
  vertices = mapTool->extractGeometryVertices( geometry );
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 3 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 0 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 1, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 2, 2 ) );

  delete mapTool;
}

void TestMapTools::testExtractMidSegmentVertices()
{
  RecordingMapTool *mapTool = new RecordingMapTool();

  QgsGeometry geometry;

  QgsPolygon *polygon = new QgsPolygon( new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 2 ) << QgsPoint( 2, 2 ) << QgsPoint( 2, 0 ) << QgsPoint( 0, 0 ) ) );
  geometry.set( polygon );

  QgsGeometry vertices = mapTool->extractMidSegmentVertices( geometry );
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 1, 2 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 2, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 1, 0 ) );

  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );
  vertices = mapTool->extractMidSegmentVertices( geometry );
  QCOMPARE( vertices.wkbType(), QgsWkbTypes::MultiPoint );
  QCOMPARE( vertices.constGet()->partCount(), 4 );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 0, 0, 0 ) ), QgsPoint( 0, -0.1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 1, 0, 0 ) ), QgsPoint( 0, 0.5 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 2, 0, 0 ) ), QgsPoint( 0.5, 1 ) );
  QCOMPARE( vertices.constGet()->vertexAt( QgsVertexId( 3, 0, 0 ) ), QgsPoint( 1.1, 1 ) );

  geometry = QgsGeometry::fromWkt( "MultiPoint( 0 0, 1 1, 2 2)" );
  vertices = mapTool->extractMidSegmentVertices( geometry );
  QVERIFY( vertices.isNull() );

  delete mapTool;
}

void TestMapTools::testCreateHandles()
{
  RecordingMapTool *mapTool = new RecordingMapTool();

  QgsGeometry geometry;

  QgsLineString *line = new QgsLineString( QVector< QgsPoint >() << QgsPoint( 0, 0 ) << QgsPoint( 0, 1 ) << QgsPoint( 1, 1 ) );
  geometry.set( line );
  QgsGeometry handles = mapTool->createHandles( geometry );
  QCOMPARE( handles.wkbType(), QgsWkbTypes::MultiLineString );
  QCOMPARE( handles.constGet()->partCount(), 2 );

  QVector<QgsGeometry> expected =
  {
    QgsGeometry::fromWkt( "LINESTRING(0 -0.1, 0 0)" ),
    QgsGeometry::fromWkt( "LINESTRING(1 1, 1.1 1)" ),
  };

  const QVector<QgsGeometry> parts = handles.asGeometryCollection();
  for ( int i = 0; i < parts.count(); i++ )
  {
    QVERIFY( parts.at( i ).equals( expected.at( i ) ) );
  }

  delete mapTool;
}
