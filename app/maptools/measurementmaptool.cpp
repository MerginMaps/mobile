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
      emit canCloseShape( false );

    rebuildGeometry();
  }
}

void MeasurementMapTool::rebuildGeometry()
{
  QgsGeometry geometry;

  if ( mPoints.count() > 0 )
  {
    geometry = QgsGeometry::fromPolyline( mPoints );
    emit canUndo( true );
  }
  else
    emit canUndo( false );

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

void MeasurementMapTool::setRecordedGeometry( const QgsGeometry &newRecordedGeometry )
{
  if ( mRecordedGeometry.equals( newRecordedGeometry ) )
    return;
  mRecordedGeometry = newRecordedGeometry;
  emit recordedGeometryChanged( mRecordedGeometry );
}

void MeasurementMapTool::updateDistance( const QgsPoint &crosshairPoint )
{
  if ( mPoints.isEmpty() )
    setLength(0.0);

  if ( mPoints.count() >= 3 )
  {
    QgsPoint firstPoint = mPoints.first();
    QPointF firstPointScreen = mapSettings()->coordinateToScreen( firstPoint );
    QPointF crosshairScreen = mapSettings()->coordinateToScreen( crosshairPoint );

    double distanceToFirstPoint = std::hypot( crosshairScreen.x() - firstPointScreen.x(), crosshairScreen.y() - firstPointScreen.y() );

    if ( distanceToFirstPoint <= CLOSE_THRESHOLD )
    {
      emit canCloseShape( true );
    }
    else
    {
      emit canCloseShape( false );
    }
  }

  QgsPoint lastPoint = mPoints.last();

  QgsDistanceArea mDistanceArea;
  mDistanceArea.setEllipsoid( QStringLiteral( "WGS84" ) );
  mDistanceArea.setSourceCrs( mActiveLayer->crs(), QgsCoordinateTransformContext() );
  //mDistanceArea.setSourceCrs(mapSettings()->destinationCrs(), QgsCoordinateTransformContext() );

  double calculatedLength = mDistanceArea.measureLength( mRecordedGeometry ) + mDistanceArea.measureLine( crosshairPoint, lastPoint );

  setLength(calculatedLength);
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

  emit shapeAreaAndPerimeter( area, perimeter );
  emit canCloseShape( false );
}

void MeasurementMapTool::repeat()
{
  mPoints.clear();

  emit canCloseShape( false );

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

void MeasurementMapTool::setLength( const double &length ) {
    if ( mLength == length )
        return;

    mLength = length;
    emit lengthChanged( length );
}

void MeasurementMapTool::setArea( const double &area ) {
    if ( mArea == area )
        return;

    mArea = area;
    emit areaChanged( area );
}

void MeasurementMapTool::setPerimeter( const double &perimeter ) {
    if ( mPerimeter == perimeter )
        return;

    mPerimeter = perimeter;
    emit perimeterChanged( perimeter );
}



QgsVectorLayer *MeasurementMapTool::activeLayer() const
{
  return mActiveLayer;
}

