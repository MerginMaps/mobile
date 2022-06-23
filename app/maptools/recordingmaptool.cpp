/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "recordingmaptool.h"

#include "qgsvectorlayer.h"
#include "qgspolygon.h"
#include "qgsvectorlayerutils.h"

#include "position/positionkit.h"
#include "variablesmanager.h"
#include "inpututils.h"

RecordingMapTool::RecordingMapTool( QObject *parent )
  : AbstractMapTool{parent}
{
}

RecordingMapTool::~RecordingMapTool() = default;

void RecordingMapTool::addPoint( const QPointF &point )
{
  QgsPoint addPoint( point );

  if ( mPositionKit && ( mCenteredToGPS || mRecordingType == StreamMode ) )
  {
    // we want to use GPS point here instead of the point from map
    addPoint = mPositionKit->positionCoordinate();

    QgsPointXY transformed = InputUtils::transformPoint(
                               PositionKit::positionCRS(),
                               mLayer->sourceCrs(),
                               mLayer->transformContext(),
                               addPoint
                             );

    addPoint.setX( transformed.x() );
    addPoint.setY( transformed.y() );
  }
  else
  {
    // convert from device x/y screen pixels -> map CRS -> layer's CRS
    addPoint = mapSettings()->screenToCoordinate( addPoint.toQPointF() );
    addPoint = mapSettings()->mapSettings().mapToLayerCoordinates( mLayer, addPoint );
  }

  fixZ( addPoint );

  mPoints.push_back( addPoint );
  rebuildGeometry();
}

void RecordingMapTool::removePoint()
{
  if ( !mPoints.isEmpty() )
  {
    mPoints.pop_back();
    rebuildGeometry();
  }
}

bool RecordingMapTool::hasValidGeometry() const
{
  if ( mLayer )
  {
    if ( mLayer->geometryType() == QgsWkbTypes::PointGeometry )
    {
      return mPoints.count() == 1;
    }
    else if ( mLayer->geometryType() == QgsWkbTypes::LineGeometry )
    {
      return mPoints.count() >= 2;
    }
    else if ( mLayer->geometryType() == QgsWkbTypes::PolygonGeometry )
    {
      return mPoints.count() >= 3;
    }
  }
  return false;
}

void RecordingMapTool::rebuildGeometry()
{
  if ( !mLayer )
    return;

  QgsGeometry geometry;

  if ( mPoints.count() < 1 )
  {
    // pass
  }
  else if ( mLayer->geometryType() == QgsWkbTypes::PointGeometry )
  {
    geometry = QgsGeometry( mPoints[0].clone() );
  }
  else if ( mLayer->geometryType() == QgsWkbTypes::LineGeometry )
  {
    geometry = QgsGeometry::fromPolyline( mPoints );
  }
  else if ( mLayer->geometryType() == QgsWkbTypes::PolygonGeometry )
  {
    // TODO: possible place for crashes!

    QgsLineString *linestring = new QgsLineString;
    Q_FOREACH ( const QgsPoint &pt, mPoints )
      linestring->addVertex( pt );

    QgsPolygon *polygon = new QgsPolygon();
    polygon->setExteriorRing( linestring );
    geometry = QgsGeometry( polygon );
  }

  setRecordedGeometry( geometry );
}

void RecordingMapTool::fixZ( QgsPoint &point ) const
{
  if ( !mLayer )
    return;

  bool layerIs3D = QgsWkbTypes::hasZ( mLayer->wkbType() );
  bool pointIs3D = QgsWkbTypes::hasZ( point.wkbType() );

  if ( layerIs3D )
  {
    if ( !pointIs3D )
    {
      point.addZValue();
    }
  }
  else /* !layerIs3D */
  {
    if ( pointIs3D )
    {
      point.dropZValue();
    }
  }
}

void RecordingMapTool::onPositionChanged()
{
  if ( mRecordingType != StreamMode )
    return;

  if ( !mPositionKit || !mPositionKit->hasPosition() )
    return;

  if ( mLastTimeRecorded.addSecs( mRecordingInterval ) <= QDateTime::currentDateTime() )
  {
    addPoint( QPointF() ); // addPoint will take point from GPS
  }
  else
  {
    // TODO: DigitizingController previously updated last point's position
    // when RecordingInterval has not yet timeouted.. do we need it?
  }
}

// Getters / setters
bool RecordingMapTool::centeredToGPS() const
{
  return mCenteredToGPS;
}

void RecordingMapTool::setCenteredToGPS( bool newCenteredToGPS )
{
  if ( mCenteredToGPS == newCenteredToGPS )
    return;
  mCenteredToGPS = newCenteredToGPS;
  emit centeredToGPSChanged( mCenteredToGPS );
}

const RecordingMapTool::RecordingType &RecordingMapTool::recordingType() const
{
  return mRecordingType;
}

void RecordingMapTool::setRecordingType( const RecordingType &newRecordingType )
{
  if ( mRecordingType == newRecordingType )
    return;
  mRecordingType = newRecordingType;
  emit recordingTypeChanged( mRecordingType );
}

int RecordingMapTool::recordingInterval() const
{
  return mRecordingInterval;
}

void RecordingMapTool::setRecordingInterval( int newRecordingInterval )
{
  if ( mRecordingInterval == newRecordingInterval )
    return;
  mRecordingInterval = newRecordingInterval;
  emit recordingIntervalChanged( mRecordingInterval );
}

PositionKit *RecordingMapTool::positionKit() const
{
  return mPositionKit;
}

void RecordingMapTool::setPositionKit( PositionKit *newPositionKit )
{
  if ( mPositionKit == newPositionKit )
    return;

  if ( mPositionKit )
    disconnect( mPositionKit, nullptr, this, nullptr );

  mPositionKit = newPositionKit;

  if ( mPositionKit )
    connect( mPositionKit, &PositionKit::positionChanged, this, &RecordingMapTool::onPositionChanged );

  emit positionKitChanged( mPositionKit );
}

QgsVectorLayer *RecordingMapTool::layer() const
{
  return mLayer;
}

void RecordingMapTool::setLayer( QgsVectorLayer *newLayer )
{
  if ( mLayer == newLayer )
    return;
  mLayer = newLayer;
  emit layerChanged( mLayer );
}

const QgsGeometry &RecordingMapTool::recordedGeometry() const
{
  return mRecordedGeometry;
}

void RecordingMapTool::setRecordedGeometry( const QgsGeometry &newRecordedGeometry )
{
  if ( mRecordedGeometry.equals( newRecordedGeometry ) )
    return;
  mRecordedGeometry = newRecordedGeometry;
  emit recordedGeometryChanged( mRecordedGeometry );
}

const QgsGeometry &RecordingMapTool::initialGeometry() const
{
  return mInitialGeometry;
}

void RecordingMapTool::setInitialGeometry( const QgsGeometry &newInitialGeometry )
{
  if ( mInitialGeometry.equals( newInitialGeometry ) )
    return;

  // We currently only support editing of points
  if ( mLayer->geometryType() == QgsWkbTypes::PointGeometry )
  {
    QgsPoint point = QgsPoint( mInitialGeometry.asPoint() );
    fixZ( point );

    mPoints.clear();
    mPoints.push_back( point );
  }

  emit initialGeometryChanged( mInitialGeometry );
}
