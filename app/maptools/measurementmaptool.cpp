/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "measurementmaptool.h"

MeasurementMapTool::MeasurementMapTool( QObject *parent )
  : AbstractMapTool{ parent }
{
  if ( !mapSettings() )
  {
    return;
  }

  mDistanceArea.setEllipsoid( QStringLiteral( "WGS84" ) );
  mDistanceArea.setSourceCrs( mapSettings()->destinationCrs(), mapSettings()->transformContext() );
}

MeasurementMapTool::~MeasurementMapTool() = default;

void MeasurementMapTool::addPoint( const QPointF &point )
{
  QgsPoint transformedPoint = mapSettings()->screenToCoordinate( point );
  mPoints.push_back( transformedPoint );
  rebuildGeometry();
}

void MeasurementMapTool::removePoint()
{
  if ( !mPoints.isEmpty() )
  {
    mPoints.pop_back();

    if ( mPoints.count() < 3 )
      setCanCloseShape( false );

    rebuildGeometry();
  }
}

void MeasurementMapTool::updateDistance( const QPointF &crosshairPoint )
{
  if ( mPoints.isEmpty() )
  {
    setLengthWithGuideline( 0.0 );
    return;
  }

  checkCanCloseShape( crosshairPoint ) ;

  QgsPoint lastPoint = mPoints.last();
  QgsPoint transformedCrosshairPoint = mapSettings()->screenToCoordinate( crosshairPoint );

  double calculatedLength = mDistanceArea.measureLength( mRecordedGeometry ) + mDistanceArea.measureLine( transformedCrosshairPoint, lastPoint );
  setLengthWithGuideline( calculatedLength );
}

void MeasurementMapTool::checkCanCloseShape( const QPointF &crosshairPoint )
{
  if ( mRecordedGeometry.isEmpty() || mPoints.count() < 3 )
  {
    setCanCloseShape( false );
    return;
  }

  if ( mPoints.count() >= 3 )
  {
    QgsPoint firstPoint = mPoints.first();
    QPointF firstPointScreen = mapSettings()->coordinateToScreen( firstPoint );
    double distanceToFirstPoint = std::hypot( crosshairPoint.x() - firstPointScreen.x(), crosshairPoint.y() - firstPointScreen.y() );
    setCanCloseShape( distanceToFirstPoint <= CLOSE_THRESHOLD );
  }
}

void MeasurementMapTool::closeShape()
{
  if ( mPoints.count() < 3 )
    return;

  QList<QgsPointXY> pointList;
  for ( const QgsPoint &point : mPoints )
    pointList.append( QgsPointXY( point.x(), point.y() ) );

  QgsGeometry polygonGeometry = QgsGeometry::fromPolygonXY( QList<QList<QgsPointXY>>() << pointList );
  setRecordedGeometry( polygonGeometry );

  double area = mDistanceArea.measureArea( polygonGeometry );
  setArea( area );

  double perimeter = mDistanceArea.measurePerimeter( polygonGeometry );
  setPerimeter( perimeter );

  setCanCloseShape( false );
  setCloseShapeDone( true );
}

void MeasurementMapTool::reset()
{
  mPoints.clear();

  setPerimeter( 0.0 );
  setLengthWithGuideline( 0.0 );
  setCanCloseShape( false );
  setCloseShapeDone( false );

  rebuildGeometry();
}

void MeasurementMapTool::rebuildGeometry()
{
  QgsGeometry geometry;

  QgsMultiPoint *existingVertices = new QgsMultiPoint();
  mExistingVertices.set( existingVertices );

  if ( mPoints.count() > 0 )
  {
    geometry = QgsGeometry::fromPolyline( mPoints );

    for ( const QgsPoint &point : mPoints )
    {
      existingVertices->addGeometry( point.clone() );
    }

    double perimeter = mDistanceArea.measureLength( geometry );
    setPerimeter( perimeter );
    setCanUndo( true );
  }
  else
  {
    setCanUndo( false );
  }

  emit existingVerticesChanged( mExistingVertices );
  setRecordedGeometry( geometry );
}

const QgsGeometry &MeasurementMapTool::recordedGeometry() const
{
  return mRecordedGeometry;
}

QgsGeometry MeasurementMapTool::existingVertices() const
{
  return mExistingVertices;
}

void MeasurementMapTool::setExistingVertices( const QgsGeometry &vertices )
{
  if ( mExistingVertices.equals( vertices ) )
    return;

  mExistingVertices = vertices;
  emit existingVerticesChanged( mExistingVertices );
}

double MeasurementMapTool::area() const
{
  return mArea;
}

double MeasurementMapTool::perimeter() const
{
  return mPerimeter;
}

double MeasurementMapTool::lengthWithGuideline() const
{
  return mLengthWithGuideline;
}

bool MeasurementMapTool::canUndo() const
{
  return mCanUndo;
}

void MeasurementMapTool::setCanUndo( bool newCanUndo )
{
  if ( mCanUndo == newCanUndo )
    return;

  mCanUndo = newCanUndo;
  emit canUndoChanged( mCanUndo );
}

bool MeasurementMapTool::canCloseShape() const
{
  return mCanCloseShape;
}

void MeasurementMapTool::setCanCloseShape( bool newCanCloseShape )
{
  if ( mCanCloseShape == newCanCloseShape )
    return;

  mCanCloseShape = newCanCloseShape;
  emit canCloseShapeChanged( mCanCloseShape );
}

bool MeasurementMapTool::closeShapeDone() const
{
  return mCloseShapeDone;
}

void MeasurementMapTool::setCloseShapeDone( bool newCloseShapeDone )
{
  if ( mCloseShapeDone == newCloseShapeDone )
    return;

  mCloseShapeDone = newCloseShapeDone;
  emit closeShapeDoneChanged( mCloseShapeDone );
}

void MeasurementMapTool::setRecordedGeometry( const QgsGeometry &newRecordedGeometry )
{
  if ( mRecordedGeometry.equals( newRecordedGeometry ) )
    return;

  mRecordedGeometry = newRecordedGeometry;
  emit recordedGeometryChanged( mRecordedGeometry );
}

void MeasurementMapTool::setLengthWithGuideline( const double &lengthWithGuideline )
{
  if ( mLengthWithGuideline == lengthWithGuideline )
    return;

  mLengthWithGuideline = lengthWithGuideline;
  emit lengthWithGuidelineChanged( lengthWithGuideline );
}

void MeasurementMapTool::setArea( const double &area )
{
  if ( mArea == area )
    return;

  mArea = area;
  emit areaChanged( area );
}

void MeasurementMapTool::setPerimeter( const double &perimeter )
{
  if ( mPerimeter == perimeter )
    return;

  mPerimeter = perimeter;
  emit perimeterChanged( perimeter );
}
