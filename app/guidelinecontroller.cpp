/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "guidelinecontroller.h"
#include "inpututils.h"
#include "qgspolygon.h"

GuidelineController::GuidelineController( QObject *parent )
  : QObject{parent}
{

}

void GuidelineController::buildGuideline()
{
  // take the existing geometry and add crosshair position to it
  if ( mCrosshairPosition.isEmpty() || mRealGeometry.isNull() || mRealGeometry.isEmpty() )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  if ( mRealGeometry.type() == QgsWkbTypes::PointGeometry )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  if ( mRealGeometry.type() == QgsWkbTypes::LineGeometry )
  {
    QgsPolylineXY points = mRealGeometry.asPolyline();
    points.append( mCrosshairPosition );

    setGuidelineGeometry( QgsGeometry::fromPolylineXY( points ) );
  }
  else if ( mRealGeometry.type() == QgsWkbTypes::PolygonGeometry )
  {
    QgsPolygonXY poly = mRealGeometry.asPolygon();

    if ( poly[0].count() < 2 )
    {
      // if it is not yet a polygon, create line guideline
      poly[0].append( mCrosshairPosition );
      setGuidelineGeometry( QgsGeometry::fromPolylineXY( poly[0] ) );
    }
    else
    {
      // let's add the crosshair as one-before-last vertex
      poly[0].insert( poly[0].count() - 1, mCrosshairPosition );
      setGuidelineGeometry( QgsGeometry::fromPolygonXY( poly ) );
    }
  }
}

const QgsGeometry &GuidelineController::guidelineGeometry() const
{
  return mGuidelineGeometry;
}

void GuidelineController::setGuidelineGeometry( const QgsGeometry &newGuidelineGeometry )
{
  if ( mGuidelineGeometry.equals( newGuidelineGeometry ) )
    return;
  mGuidelineGeometry = newGuidelineGeometry;
  emit guidelineGeometryChanged( mGuidelineGeometry );
}

QgsPoint GuidelineController::crosshairPosition() const
{
  return mCrosshairPosition;
}

void GuidelineController::setCrosshairPosition( QgsPoint newCrosshairPosition )
{
  if ( mCrosshairPosition == newCrosshairPosition )
    return;
  mCrosshairPosition = newCrosshairPosition;
  emit crosshairPositionChanged( mCrosshairPosition );

  buildGuideline();
}

const QgsGeometry &GuidelineController::realGeometry() const
{
  return mRealGeometry;
}

void GuidelineController::setRealGeometry( const QgsGeometry &newRealGeometry )
{
  if ( mRealGeometry.equals( newRealGeometry ) )
    return;
  mRealGeometry = newRealGeometry;
  emit realGeometryChanged( mRealGeometry );

  buildGuideline();
}
