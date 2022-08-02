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
  connect( this, &GuidelineController::allowedChanged, this, &GuidelineController::buildGuideline );
  connect( this, &GuidelineController::activeVertexChanged, this, &GuidelineController::buildGuideline );
  connect( this, &GuidelineController::realGeometryChanged, this, &GuidelineController::buildGuideline );
  connect( this, &GuidelineController::crosshairPositionChanged, this, &GuidelineController::buildGuideline );
}

void GuidelineController::buildGuideline()
{
  // take the existing geometry and add crosshair position to it
  if ( !mAllowed || !mMapSettings || mCrosshairPosition.isNull() || mRealGeometry.isNull() || mRealGeometry.isEmpty() )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  if ( mRealGeometry.type() == QgsWkbTypes::PointGeometry )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  QgsPoint crosshair = mMapSettings->screenToCoordinate( mCrosshairPosition );

  if ( !mActiveVertex.isValid() ) // recording
  {
    // we add current crosshair to the end of geometry - creating new point

    if ( mRealGeometry.type() == QgsWkbTypes::LineGeometry )
    {
      QgsGeometry guideline;

      if ( mNewVertexOrder == RecordingMapTool::Start )
      {
        // add crosshair to the begginning
        QgsPoint firstPoint = mRealGeometry.vertexAt( 0 );
        guideline = QgsGeometry::fromPolyline( { firstPoint, crosshair } );
      }
      else
      {
        // add crosshair to the end of the geometry
        QgsPoint lastPoint = mRealGeometry.vertexAt( mRealGeometry.constGet()->nCoordinates() - 1 );
        guideline = QgsGeometry::fromPolyline( { lastPoint, crosshair } );
      }

      setGuidelineGeometry( guideline );
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
  else // we are in grab state
  {
    QgsGeometry g( mRealGeometry );

//    // for handles we insert new vertex at the beginning or end of the line
//    if ( mActiveVertex.type() == Vertex::VertexType::HandleStart )
//    {
//      qDebug() << "HANDLE START";
//      QgsVertexId id( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring, 0 );
//      g.insertVertex( crosshair, g.vertexNrFromVertexId( id ) );
//    }
//    if ( mActiveVertex.type() == Vertex::VertexType::HandleEnd )
//    {
//      qDebug() << "HANDLE END";
//      int index = g.constGet()->vertexCount( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );
//      QgsVertexId id( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring, index );
//      g.get()->insertVertex( id, crosshair );
//    }
//    else
//    {
    // we add current crosshair in place of active vertex id
//    g.moveVertex( crosshair, g.vertexNrFromVertexId( mActiveVertex.vertexId() ) );
//    }

    g.moveVertex( crosshair, g.vertexNrFromVertexId( mActiveVertex.vertexId() ) );
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

const Vertex &GuidelineController::activeVertex() const
{
  return mActiveVertex;
}

void GuidelineController::setActiveVertex( const Vertex &newActiveVertex )
{
  if ( mActiveVertex == newActiveVertex )
    return;
  mActiveVertex = newActiveVertex;
  emit activeVertexChanged( mActiveVertex );
}

bool GuidelineController::allowed() const
{
  return mAllowed;
}

void GuidelineController::setAllowed( bool newAllowed )
{
  if ( mAllowed == newAllowed )
    return;
  mAllowed = newAllowed;
  emit allowedChanged( mAllowed );
}

const RecordingMapTool::NewVertexOrder &GuidelineController::newVertexOrder() const
{
  return mNewVertexOrder;
}

void GuidelineController::setNewVertexOrder( const RecordingMapTool::NewVertexOrder &newNewVertexOrder )
{
  if ( mNewVertexOrder == newNewVertexOrder )
    return;
  mNewVertexOrder = newNewVertexOrder;
  emit newVertexOrderChanged( mNewVertexOrder );
}
