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
  : AbstractMapTool{parent}
{
  qDebug() << "MeasurementMapTool constructor called";
}

MeasurementMapTool::~MeasurementMapTool()
{
  qDebug() << "MeasurementMapTool destructor called";
}

void MeasurementMapTool::addPoint( const QgsPoint &point )
{
  mPoints.push_back( point );
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

void MeasurementMapTool::rebuildGeometry()
{
  QgsGeometry geometry;

  if ( mPoints.count() > 0 )
  {
    geometry = QgsGeometry::fromPolyline( mPoints );
    setCanUndo( true );
  }
  else
    setCanUndo( false );

  setRecordedGeometry( geometry );

}

const QgsGeometry &MeasurementMapTool::recordedGeometry() const
{
  return mRecordedGeometry;
}

double MeasurementMapTool::area() const
{
  return mArea;
}

double MeasurementMapTool::perimeter() const
{
  return mPerimeter;
}

double MeasurementMapTool::length() const
{
  return mLength;
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

void MeasurementMapTool::updateDistance( const QgsPoint &crosshairPoint )
{
  qDebug() << "Entered updateDistance function";
  qDebug() << "Crosshair point:";

  if ( mPoints.isEmpty() )
  {
    qDebug() << "mPoints is empty, setting length to 0.0";
    setLength( 0.0 );
    return;
  }

  qDebug() << "Number of points:" << mPoints.count();

  if ( mPoints.count() >= 3 )
  {
    QgsPoint firstPoint = mPoints.first();
    qDebug() << "First point:";

    QPointF firstPointScreen = mapSettings()->coordinateToScreen( firstPoint );
    qDebug() << "First point screen coordinates:" << firstPointScreen;

    QPointF crosshairScreen = mapSettings()->coordinateToScreen( crosshairPoint );
    qDebug() << "Crosshair screen coordinates:" << crosshairScreen;

    double distanceToFirstPoint = std::hypot( crosshairScreen.x() - firstPointScreen.x(), crosshairScreen.y() - firstPointScreen.y() );
    qDebug() << "Distance to first point:" << distanceToFirstPoint;

    if ( distanceToFirstPoint <= CLOSE_THRESHOLD )
    {
      qDebug() << "Distance to first point is within threshold, setting canCloseShape to true";
      setCanCloseShape( true );
    }
    else
    {
      qDebug() << "Distance to first point exceeds threshold, setting canCloseShape to false";
      setCanCloseShape( false );
    }
  }

  QgsPoint lastPoint = mPoints.last();
  qDebug() << "Last point:";

  QgsDistanceArea mDistanceArea;
  qDebug() << "Created QgsDistanceArea object";

  mDistanceArea.setEllipsoid( QStringLiteral( "WGS84" ) );
  qDebug() << "Ellipsoid set to WGS84";

  mDistanceArea.setSourceCrs( mActiveLayer->crs(), QgsCoordinateTransformContext() );
  qDebug() << "Source CRS set with active layer CRS";

  //mDistanceArea.setSourceCrs(mapSettings()->destinationCrs(), QgsCoordinateTransformContext() );
  //qDebug() << "Source CRS set with destination CRS";

  double calculatedLength = mDistanceArea.measureLength( mRecordedGeometry ) + mDistanceArea.measureLine( crosshairPoint, lastPoint );
  qDebug() << "Calculated length:" << calculatedLength;

  setLength( calculatedLength );
  qDebug() << "Length set to calculated value";
}


void MeasurementMapTool::closeShape()
{
  if ( mPoints.count() < 3 )
    return;

  QList<QgsPointXY> pointList;
  for ( const QgsPoint &point : mPoints )
  {
    pointList.append( QgsPointXY( point.x(), point.y() ) );
  }

  QgsGeometry polygonGeometry = QgsGeometry::fromPolygonXY( QList<QList<QgsPointXY>>() << pointList );

  setRecordedGeometry( polygonGeometry );

  QgsDistanceArea mDistanceArea;
  mDistanceArea.setEllipsoid( QStringLiteral( "WGS84" ) );
  mDistanceArea.setSourceCrs( mActiveLayer->crs(), QgsCoordinateTransformContext() );

  double area = mDistanceArea.measureArea( polygonGeometry );
  double perimeter = mDistanceArea.measurePerimeter( polygonGeometry );

  setArea( area );
  setPerimeter( perimeter );
  setCanCloseShape( false );
  setCloseShapeDone( true );
}

void MeasurementMapTool::repeat()
{
  mPoints.clear();

  setCanCloseShape( false );
  setCloseShapeDone( false );

  rebuildGeometry();
}

void MeasurementMapTool::setActiveLayer( QgsVectorLayer *newActiveLayer )
{
  if ( mActiveLayer == newActiveLayer )
    return;

  if ( mActiveLayer && mActiveLayer->isEditable() )
  {
    mActiveLayer->rollBack();
    mActiveLayer->triggerRepaint();
  }

  mActiveLayer = newActiveLayer;
  emit activeLayerChanged( mActiveLayer );
}

void MeasurementMapTool::setLength( const double &length )
{
  if ( mLength == length )
    return;

  mLength = length;
  emit lengthChanged( length );
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


QgsVectorLayer *MeasurementMapTool::activeLayer() const
{
  return mActiveLayer;
}

