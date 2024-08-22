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

void MeasurementMapTool::setRecordedGeometry( const QgsGeometry &newRecordedGeometry )
{
  if ( mRecordedGeometry.equals( newRecordedGeometry ) )
    return;
  mRecordedGeometry = newRecordedGeometry;
  emit recordedGeometryChanged( mRecordedGeometry );
}

double MeasurementMapTool::updateDistance( const QgsPoint &crosshairPoint )
{
  if ( mPoints.isEmpty() )
    return 0.0;

  if ( mPoints.count() >= 3 )
  {
    QgsPoint firstPoint = mPoints.first();
    double distanceToFirstPoint = QgsDistanceArea().measureLine( crosshairPoint, firstPoint );

    if ( distanceToFirstPoint <= CLOSE_THRESHOLD )
      emit canCloseShape( true );
    else
      emit canCloseShape( false );
  }

  QgsPoint lastPoint = mPoints.last();

  double distance = QgsDistanceArea().measureLine( crosshairPoint, lastPoint );

  return distance;
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

  QgsDistanceArea distanceArea;
  double area = distanceArea.measureArea( polygonGeometry );
  double perimeter = distanceArea.measureLength( polygonGeometry );

  emit shapeArea( area );
  emit shapePerimeter( perimeter );
  emit canCloseShape( false );
}

void MeasurementMapTool::repeat()
{
  mPoints.clear();

  emit canCloseShape( false );

  rebuildGeometry();
}


