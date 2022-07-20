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
  if ( !mMapSettings || mCrosshairPosition.isNull() || mRealGeometry.isNull() || mRealGeometry.isEmpty() )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  if ( mRealGeometry.type() == QgsWkbTypes::PointGeometry )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  if ( !mActiveVertexId.isValid() )
  {
    // we add current crosshair to the end of geometry - creating new point

    QgsPoint crosshair = mMapSettings->screenToCoordinate( mCrosshairPosition );

    if ( mRealGeometry.type() == QgsWkbTypes::LineGeometry )
    {
      QgsPolylineXY points = mRealGeometry.asPolyline();
      points.append( crosshair );

      setGuidelineGeometry( QgsGeometry::fromPolylineXY( points ) );
    }
    else if ( mRealGeometry.type() == QgsWkbTypes::PolygonGeometry )
    {
      QgsPolygonXY poly = mRealGeometry.asPolygon();

      if ( poly[0].count() < 2 )
      {
        // if it is not yet a polygon, create line guideline
        poly[0].append( crosshair );
        setGuidelineGeometry( QgsGeometry::fromPolylineXY( poly[0] ) );
      }
      else
      {
        // let's add the crosshair as one-before-last vertex
        poly[0].insert( poly[0].count() - 1, crosshair );
        setGuidelineGeometry( QgsGeometry::fromPolygonXY( poly ) );
      }
    }
  }
  else
  {
    // we add current crosshair in place of active vertex id

    QgsPoint crosshair = mMapSettings->screenToCoordinate( mCrosshairPosition );

    QgsGeometry g( mRealGeometry );

    g.moveVertex( crosshair, g.vertexNrFromVertexId( mActiveVertexId ) );

    setGuidelineGeometry( g );
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

QPointF GuidelineController::crosshairPosition() const
{
  return mCrosshairPosition;
}

void GuidelineController::setCrosshairPosition( QPointF newCrosshairPosition )
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

QgsQuickMapSettings *GuidelineController::mapSettings() const
{
  return mMapSettings;
}

void GuidelineController::setMapSettings( QgsQuickMapSettings *newMapSettings )
{
  if ( !newMapSettings || mMapSettings == newMapSettings )
    return;

  if ( mMapSettings )
  {
    disconnect( mMapSettings, nullptr, this, nullptr );
  }

  mMapSettings = newMapSettings;

  connect( mMapSettings, &QgsQuickMapSettings::extentChanged, this, &GuidelineController::buildGuideline );
  connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, &GuidelineController::buildGuideline );
  connect( mMapSettings, &QgsQuickMapSettings::mapUnitsPerPixelChanged, this, &GuidelineController::buildGuideline );
  connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, &GuidelineController::buildGuideline );
  connect( mMapSettings, &QgsQuickMapSettings::outputSizeChanged, this, &GuidelineController::buildGuideline );
  connect( mMapSettings, &QgsQuickMapSettings::outputDpiChanged, this, &GuidelineController::buildGuideline );

  emit mapSettingsChanged( mMapSettings );
}

const QgsVertexId &GuidelineController::activeVertexId() const
{
  return mActiveVertexId;
}

void GuidelineController::setActiveVertexId( const QgsVertexId &newActiveVertexId )
{
  if ( mActiveVertexId == newActiveVertexId )
    return;
  mActiveVertexId = newActiveVertexId;
  emit activeVertexIdChanged( mActiveVertexId );

  buildGuideline();
}
