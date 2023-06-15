/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positiontrackinghighlight.h"

PositionTrackingHighlight::PositionTrackingHighlight( QObject *parent )
  : QObject{parent}
{
  connect( this, &PositionTrackingHighlight::trackedGeometryChanged, this, &PositionTrackingHighlight::recalculate );
  connect( this, &PositionTrackingHighlight::mapPositionChanged, this, &PositionTrackingHighlight::recalculate );
}

void PositionTrackingHighlight::recalculate()
{
  if ( mMapPosition.isEmpty() )
  {
    setHighlightGeometry( QgsGeometry() );
    return;
  }

  if ( mTrackedGeometry.isEmpty() )
  {
    setHighlightGeometry( QgsGeometry() );
    return;
  }

  // add map position to the end of the tracked geometry
  // note - map position must be in the same CRS as the tracked geometry

  QgsGeometry highlightGeometry( mTrackedGeometry );

  QgsVertexId lastVertex( 0, 0, highlightGeometry.constGet()->vertexCount() );
  highlightGeometry.get()->insertVertex( lastVertex, mMapPosition );

  setHighlightGeometry( highlightGeometry );
}

QgsGeometry PositionTrackingHighlight::trackedGeometry() const
{
  return mTrackedGeometry;
}

void PositionTrackingHighlight::setTrackedGeometry( const QgsGeometry &newTrackedGeometry )
{
  if ( mTrackedGeometry.equals( newTrackedGeometry ) )
    return;

  mTrackedGeometry = newTrackedGeometry;
  emit trackedGeometryChanged( mTrackedGeometry );
}

QgsGeometry PositionTrackingHighlight::highlightGeometry() const
{
  return mHighlightGeometry;
}

void PositionTrackingHighlight::setHighlightGeometry( const QgsGeometry &newHighlightGeometry )
{
  if ( mHighlightGeometry.equals( newHighlightGeometry ) )
    return;

  mHighlightGeometry = newHighlightGeometry;
  emit highlightGeometryChanged( mHighlightGeometry );
}

QgsPoint PositionTrackingHighlight::mapPosition() const
{
  return mMapPosition;
}

void PositionTrackingHighlight::setMapPosition( QgsPoint newMapPosition )
{
  if ( mMapPosition == newMapPosition )
    return;

  mMapPosition = newMapPosition;
  emit mapPositionChanged( mMapPosition );
}
