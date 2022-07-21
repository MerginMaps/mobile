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
#include "qgsmultipoint.h"
#include "qgsmultilinestring.h"

#include "position/positionkit.h"
#include "variablesmanager.h"
#include "inpututils.h"

RecordingMapTool::RecordingMapTool( QObject *parent )
  : AbstractMapTool{parent}
{
}

RecordingMapTool::~RecordingMapTool() = default;

void RecordingMapTool::addPoint( const QgsPoint &point )
{
  QgsPoint pointToAdd( point );

  if ( mPositionKit && ( mCenteredToGPS || mRecordingType == StreamMode ) )
  {
    // we want to use GPS point here instead of the point from map
    pointToAdd = mPositionKit->positionCoordinate();

    QgsPointXY transformed = InputUtils::transformPoint(
                               PositionKit::positionCRS(),
                               mLayer->sourceCrs(),
                               mLayer->transformContext(),
                               pointToAdd
                             );

    pointToAdd.setX( transformed.x() );
    pointToAdd.setY( transformed.y() );
  }

  fixZ( pointToAdd );

  mPoints.push_back( pointToAdd );
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
    addPoint( QgsPoint() ); // addPoint will take point from GPS
    mLastTimeRecorded = QDateTime::currentDateTime();
  }
  else
  {
    if ( !mPoints.isEmpty() )
    {
      // update the last point of the geometry
      // so that it is placed on user's current position
      QgsPoint position = mPositionKit->positionCoordinate();

      QgsPointXY transformed = InputUtils::transformPoint(
                                 PositionKit::positionCRS(),
                                 mLayer->sourceCrs(),
                                 mLayer->transformContext(),
                                 position
                               );

      mPoints.last().setX( transformed.x() );
      mPoints.last().setY( transformed.y() );
      mPoints.last().setZ( position.z() );

      rebuildGeometry();
    }
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

  // we need to clear all recorded points and recalculate the geometry
  mPoints.clear();
  rebuildGeometry();
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

  mInitialGeometry = newInitialGeometry;

  mPoints.clear();
  for ( auto pointIt = mInitialGeometry.vertices_begin(); pointIt != mInitialGeometry.vertices_end(); ++pointIt )
  {
    mPoints.push_back( QgsPoint( *pointIt ) );
  }

  createNodesAndHandles();

  emit initialGeometryChanged( mInitialGeometry );
}

void RecordingMapTool::createNodesAndHandles()
{
  mVertexIds.clear();

  QgsPoint vertex;
  QgsVertexId vertexId;
  const QgsAbstractGeometry *geom = mInitialGeometry.constGet();

  QgsMultiPoint *existingVertices = new QgsMultiPoint();
  mExistingVertices.set( existingVertices );

  QgsMultiPoint *midPoints = new QgsMultiPoint();
  mMidPoints.set( midPoints );

  QgsMultiLineString *handles = new QgsMultiLineString();
  mHandles.set( handles );

  int currentPart = -1;
  int currentRing = -1;

  while ( geom->nextVertex( vertexId, vertex ) )
  {
    existingVertices->addGeometry( vertex.clone() );
    mVertexIds.push_back( qMakePair( vertexId, vertex ) );

    // for lines and polygons create midpoints
    if ( mInitialGeometry.type() != QgsWkbTypes::PointGeometry && vertexId.vertex < geom->vertexCount( vertexId.part, vertexId.ring ) - 1 )
    {
      QgsVertexId id( vertexId.part, vertexId.ring, vertexId.vertex + 1 );
      QgsPoint midPoint = QgsGeometryUtils::midpoint( geom->vertexAt( vertexId ), geom->vertexAt( id ) );
      midPoints->addGeometry( midPoint.clone() );
      mVertexIds.push_back( qMakePair( id, midPoint ) );
    }

    // for lines also create start/end points and handles
    if ( mInitialGeometry.type() == QgsWkbTypes::LineGeometry && ( vertexId.part != currentPart && vertexId.ring != currentRing ) )
    {
      int vertexCount = geom->vertexCount( vertexId.part, vertexId.ring );
      if ( vertexCount >= 2 )
      {
        // start point and handle
        QgsVertexId startId( vertexId.part, vertexId.ring, 0 );
        QgsVertexId endId( vertexId.part, vertexId.ring, 1 );
        QgsPoint point = QgsGeometryUtils::interpolatePointOnLine( geom->vertexAt( startId ), geom->vertexAt( endId ), -0.1 );

        midPoints->addGeometry( point.clone() );
        mVertexIds.push_back( qMakePair( startId, point ) );

        QgsLineString handle( point, geom->vertexAt( startId ) );
        handles->addGeometry( handle.clone() );

        // end point and handle
        startId.vertex = vertexCount - 2;
        endId.vertex = vertexCount - 1;
        point = QgsGeometryUtils::interpolatePointOnLine( geom->vertexAt( startId ), geom->vertexAt( endId ), 1.1 );

        handle = QgsLineString( geom->vertexAt( endId ), point );
        handles->addGeometry( handle.clone() );

        midPoints->addGeometry( point.clone() );
        endId.vertex = vertexCount;
        mVertexIds.push_back( qMakePair( endId, point ) );
      }
      currentPart = vertexId.part;
      currentRing = vertexId.ring;
    }
  }

  emit existingVerticesChanged( mExistingVertices );
  emit midPointsChanged( mMidPoints );
  emit handlesChanged( mHandles );
}

const QgsGeometry &RecordingMapTool::existingVertices() const
{
  return mExistingVertices;
}

void RecordingMapTool::setExistingVertices( const QgsGeometry &newExistingVertices )
{
  if ( mExistingVertices.equals( newExistingVertices ) )
    return;
  mExistingVertices = newExistingVertices;
  emit existingVerticesChanged( mExistingVertices );
}

const QgsGeometry &RecordingMapTool::midPoints() const
{
  return mMidPoints;
}

void RecordingMapTool::setMidPoints( const QgsGeometry &newMidPoints )
{
  if ( mMidPoints.equals( newMidPoints ) )
    return;
  mMidPoints = newMidPoints;
  emit midPointsChanged( mMidPoints );
}

const QgsGeometry &RecordingMapTool::handles() const
{
  return mHandles;
}

void RecordingMapTool::setHandles( const QgsGeometry &newHandles )
{
  if ( mHandles.equals( newHandles ) )
    return;
  mHandles = newHandles;
  emit handlesChanged( mHandles );
}

const QString &RecordingMapTool::state() const
{
  return mState;
}

void RecordingMapTool::setState( const QString &newState )
{
  if ( mState == newState )
    return;
  mState = newState;
  emit stateChanged( mState );
}

QgsVertexId &RecordingMapTool::clickedVertexId()
{
  return mClickedVertexId;
}

void RecordingMapTool::setClickedVertexId( QgsVertexId newId )
{
  if ( mClickedVertexId == newId )
    return;
  mClickedVertexId.part = newId.part;
  mClickedVertexId.ring = newId.ring;
  mClickedVertexId.vertex = newId.vertex;
  emit clickedVertexIdChanged( mClickedVertexId );
}

void RecordingMapTool::lookForVertex( const QPointF &clickedPoint, double searchRadius )
{
  double minDistance = std::numeric_limits<double>::max();
  double currentDistance = 0;
  QgsPoint point;
  QgsVertexId vertexId;

  QgsPoint pnt = mapSettings()->screenToCoordinate( clickedPoint );

  if ( mInitialGeometry.isEmpty() )
  {
    setClickedVertexId( vertexId );
    return;
  }

  for ( QPair<QgsVertexId, QgsPoint> pair : mVertexIds )
  {
    currentDistance = QgsGeometryUtils::sqrDistance2D( pnt, pair.second );
    if ( currentDistance <= minDistance && currentDistance <= searchRadius )
    {
      minDistance = currentDistance;
      vertexId.part = pair.first.part;
      vertexId.ring = pair.first.ring;
      vertexId.vertex = pair.first.vertex;
    }
  }

  setClickedVertexId( vertexId );
}
