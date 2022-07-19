/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testrecordingtool.h"
#include "testutils.h"
#include "maptools/recordingmaptool.h"

#include "qgspoint.h"
#include "qgslinestring.h"
#include "qgspolygon.h"
#include "qgsgeometry.h"

#include <QtTest/QtTest>

void TestRecordingTool::init()
{
}

void TestRecordingTool::cleanup()
{
}


void TestRecordingTool::testExtractVertices()
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
}

void TestRecordingTool::testExtractMidSegmentVertices()
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
}

void TestRecordingTool::testCreateHandles()
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
}
