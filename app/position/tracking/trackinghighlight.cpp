/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trackinghighlight.h"

TrackingHighlight::TrackingHighlight( QObject *parent )
  : QObject( parent )
{
  connect( this, &TrackingHighlight::trackedGeometryChanged, this, &TrackingHighlight::recalculate );
  connect( this, &TrackingHighlight::mapPositionChanged, this, &TrackingHighlight::recalculate );
}

void TrackingHighlight::recalculate() // TODO: do the recalculations here - note be wise about updates ! when position was updated and when tracked geometry is updated are two different use cases
{
  // onTrackedGeometryUpdated:
  // - transform the whole geometryin
  // - ditch the whole old geometryout
  // - add the new position

  // onPositionUpdated (save yourself from transforming the geometry): -- how do we know the position was there previously though?
  // - pop the last point
  // - add position to the end

  // __inputUtils.transformGeometryToMapWithCRS( trackingManager.trackedGeometry, trackingManager.crs(), mapCanvas.mapSettings )

  if ( mMapPosition.isEmpty() || mTrackedGeometry.isEmpty() )
  {
    setHighlightGeometry( QgsGeometry() );
    return;
  }

  // add map position to the end of the tracked geometry
  // note - map position must be in the same CRS as the tracked geometry


  QgsGeometry highlightGeometry( mTrackedGeometry );

  // mTrackedGeometry.constGet()->addPoint( mMapPosition );

  QgsVertexId lastVertex( 0, 0, highlightGeometry.constGet()->vertexCount() );
  highlightGeometry.get()->insertVertex( lastVertex, mMapPosition );

  setHighlightGeometry( highlightGeometry );
}

QgsGeometry TrackingHighlight::trackedGeometry() const
{
  return mTrackedGeometry;
}

void TrackingHighlight::setTrackedGeometry( const QgsGeometry &newTrackedGeometry )
{
  if ( mTrackedGeometry.equals( newTrackedGeometry ) )
    return;

  mTrackedGeometry = newTrackedGeometry;
  emit trackedGeometryChanged( mTrackedGeometry );
}

QgsGeometry TrackingHighlight::highlightGeometry() const
{
  return mHighlightGeometry;
}

void TrackingHighlight::setHighlightGeometry( const QgsGeometry &newHighlightGeometry )
{
  if ( mHighlightGeometry.equals( newHighlightGeometry ) )
    return;

  mHighlightGeometry = newHighlightGeometry;
  emit highlightGeometryChanged( mHighlightGeometry );
}

QgsPoint TrackingHighlight::mapPosition() const
{
  return mMapPosition;
}

void TrackingHighlight::setMapPosition( QgsPoint newMapPosition )
{
  if ( mMapPosition == newMapPosition )
    return;

  mMapPosition = newMapPosition;
  emit mapPositionChanged( mMapPosition );
}

InputMapSettings *TrackingHighlight::mapSettings() const
{
  return mMapSettings;
}

void TrackingHighlight::setMapSettings( InputMapSettings *newMapSettings )
{
  if ( mMapSettings == newMapSettings )
    return;

  mMapSettings = newMapSettings;
  emit mapSettingsChanged();
}
