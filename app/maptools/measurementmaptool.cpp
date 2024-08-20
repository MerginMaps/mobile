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
    rebuildGeometry();
  }
}

bool MeasurementMapTool::hasValidGeometry() const
{
  return mPoints.count() >= 2;
}

void MeasurementMapTool::rebuildGeometry()
{
  QgsGeometry geometry;

  if ( mPoints.count() > 0 )
    geometry = QgsGeometry::fromPolyline( mPoints );

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

  QgsPoint lastPoint = mPoints.last();

  double distance = QgsDistanceArea().measureLine( crosshairPoint, lastPoint );

  return distance;
}

