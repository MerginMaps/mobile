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
  connect( this, &RecordingMapTool::recordedGeometryChanged, this, &RecordingMapTool::createNodesAndHandles );
}

RecordingMapTool::~RecordingMapTool() = default;

void RecordingMapTool::addPoint( const QgsPoint &point )
{
  QgsPoint pointToAdd( point );

  if ( mPositionKit && ( mCenteredToGPS || mRecordingType == StreamMode ) )
  {
    // we want to use GPS point here instead of the point from map
    pointToAdd = mPositionKit->positionCoordinate();

    QgsPoint transformed = InputUtils::transformPoint(
                             PositionKit::positionCRS(),
                             mLayer->sourceCrs(),
                             mLayer->transformContext(),
                             pointToAdd
                           );

    pointToAdd.setX( transformed.x() );
    pointToAdd.setY( transformed.y() );
  }

  fixZ( pointToAdd );

  QgsVertexId id( 0, 0, 0 );
  if ( mRecordedGeometry.isEmpty() )
  {
    mRecordedGeometry = InputUtils::createGeometryForLayer( mLayer );
  }
  else
  {
    id.vertex = mRecordedGeometry.constGet()->vertexCount();
  }

  if ( mRecordedGeometry.type() == QgsWkbTypes::PolygonGeometry )
  {
    // if it is a polygon and ring is not correctly defined yet (e.g. only
    // contains 1 point or not closed) we add point directly to the ring
    // and close it
    QgsLineString *r = qgsgeometry_cast<QgsLineString *>( qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() )->exteriorRing() );

    if ( r->nCoordinates() < 2 )
    {
      r->addVertex( pointToAdd );
      r->close();
      emit recordedGeometryChanged( mRecordedGeometry );
      return;
    }
    else
    {
      // as rings are closed, we need to insert before last vertex
      id.vertex = mRecordedGeometry.constGet()->vertexCount() - 1;
    }
  }

  mRecordedGeometry.get()->insertVertex( id, pointToAdd );
  emit recordedGeometryChanged( mRecordedGeometry );
}

void RecordingMapTool::removePoint()
{
  if ( mRecordedGeometry.constGet()->vertexCount() > 0 )
  {
    QgsVertexId id( 0, 0, mRecordedGeometry.constGet()->vertexCount() - 1 );
    if ( mRecordedGeometry.type() == QgsWkbTypes::PolygonGeometry )
    {
      QgsLineString *r = qgsgeometry_cast<QgsLineString *>( qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() )->exteriorRing() );
      if ( r->nCoordinates() == 4 )
      {
        // this is the smallest possible closed ring (first and last vertex are equal),
        // we need to remove two last vertices in order to get correct linestring
        r->deleteVertex( QgsVertexId( 0, 0, r->nCoordinates() - 1 ) );
        r->deleteVertex( QgsVertexId( 0, 0, r->nCoordinates() - 1 ) );
        emit recordedGeometryChanged( mRecordedGeometry );
        return;
      }
      else if ( r->nCoordinates() <= 2 )
      {
        // if we remove last vertex directly the geometry will be cleared
        // but we want to keep start point, so instead of removing vertex
        // from the linestring we remove item from the QgsPointSequence.
        QgsPointSequence points;
        r->points( points );
        points.takeLast();
        r->setPoints( points );
        emit recordedGeometryChanged( mRecordedGeometry );
        return;
      }
      else
      {
        id.vertex = mRecordedGeometry.constGet()->vertexCount() - 2;
      }
    }

    mRecordedGeometry.get()->deleteVertex( id );
    emit recordedGeometryChanged( mRecordedGeometry );
  }
}

bool RecordingMapTool::hasValidGeometry() const
{
  if ( mLayer )
  {
    if ( mLayer->geometryType() == QgsWkbTypes::PointGeometry )
    {
      return mRecordedGeometry.constGet()->nCoordinates() == 1;
    }
    else if ( mLayer->geometryType() == QgsWkbTypes::LineGeometry )
    {
      return mRecordedGeometry.constGet()->nCoordinates() >= 2;
    }
    else if ( mLayer->geometryType() == QgsWkbTypes::PolygonGeometry )
    {
      return mRecordedGeometry.constGet()->nCoordinates() >= 3;
    }
  }
  return false;
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
    if ( !mRecordedGeometry.isEmpty() )
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
      QgsPoint p( transformed.x(), transformed.y(), position.z() );
      QgsVertexId id( 0, 0, mRecordedGeometry.constGet()->vertexCount() - 1 );
      mRecordedGeometry.get()->moveVertex( id, p );
      emit recordedGeometryChanged( mRecordedGeometry );
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
  setRecordedGeometry( QgsGeometry() );
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

  setRecordedGeometry( newInitialGeometry );

  emit initialGeometryChanged( mInitialGeometry );
}

void RecordingMapTool::createNodesAndHandles()
{
  mVertices.clear();

  QgsMultiPoint *existingVertices = new QgsMultiPoint();
  mExistingVertices.set( existingVertices );

  QgsMultiPoint *midPoints = new QgsMultiPoint();
  mMidPoints.set( midPoints );

  QgsMultiLineString *handles = new QgsMultiLineString();
  mHandles.set( handles );

  if ( mRecordedGeometry.isEmpty() )
  {
    emit existingVerticesChanged( mExistingVertices );
    emit midPointsChanged( mMidPoints );
    emit handlesChanged( mHandles );
    return;
  }

  QgsPoint vertex;
  QgsVertexId vertexId;
  const QgsAbstractGeometry *geom = mRecordedGeometry.constGet();

  int currentPart = -1;
  int currentRing = -1;

  while ( geom->nextVertex( vertexId, vertex ) )
  {
    existingVertices->addGeometry( vertex.clone() );
    addVertex( vertexId, vertex, false );

    // for lines and polygons create midpoints
    if ( mRecordedGeometry.type() != QgsWkbTypes::PointGeometry && vertexId.vertex < geom->vertexCount( vertexId.part, vertexId.ring ) - 1 )
    {
      QgsVertexId id( vertexId.part, vertexId.ring, vertexId.vertex + 1 );
      QgsPoint midPoint = QgsGeometryUtils::midpoint( geom->vertexAt( vertexId ), geom->vertexAt( id ) );
      midPoints->addGeometry( midPoint.clone() );
      addVertex( id, midPoint, true );
    }

    // for lines also create start/end points and handles
    if ( mRecordedGeometry.type() == QgsWkbTypes::LineGeometry && ( vertexId.part != currentPart && vertexId.ring != currentRing ) )
    {
      int vertexCount = geom->vertexCount( vertexId.part, vertexId.ring );
      if ( vertexCount >= 2 )
      {
        // start point and handle
        QgsVertexId startId( vertexId.part, vertexId.ring, 0 );
        QgsVertexId endId( vertexId.part, vertexId.ring, 1 );
        QgsPoint point = QgsGeometryUtils::interpolatePointOnLine( geom->vertexAt( startId ), geom->vertexAt( endId ), -0.1 );

        midPoints->addGeometry( point.clone() );
        addVertex( startId, point, true );

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
        addVertex( endId, point, true );
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
  mClickedVertexId.part = newId.part;
  mClickedVertexId.ring = newId.ring;
  mClickedVertexId.vertex = newId.vertex;
  emit clickedVertexIdChanged( mClickedVertexId );
}

QgsPoint &RecordingMapTool::clickedPoint()
{
  return mClickedPoint;
}

void RecordingMapTool::setClickedPoint( QgsPoint newPoint )
{
  mClickedPoint = newPoint;
  emit clickedPointChanged( mClickedPoint );
}

void RecordingMapTool::lookForVertex( const QPointF &clickedPoint, double searchRadius )
{
  double minDistance = std::numeric_limits<double>::max();
  double currentDistance = 0;

  QgsPoint pnt = mapSettings()->screenToCoordinate( clickedPoint );
  pnt = InputUtils::transformPoint( mapSettings()->destinationCrs(), mLayer->crs(), mLayer->transformContext(), pnt );

  if ( mRecordedGeometry.isEmpty() )
  {
    setState( QStringLiteral( "view" ) );
    setClickedVertexId( QgsVertexId() );
    setClickedPoint( QgsPoint() );
    return;
  }

  int idx = -1;
  for ( int i = 0; i < mVertices.count(); i++ )
  {
    currentDistance = QgsGeometryUtils::sqrDistance2D( pnt, mVertices.at( i ).point );
    if ( currentDistance <= minDistance && currentDistance <= searchRadius )
    {
      minDistance = currentDistance;
      idx = i;
    }
  }

  QgsVertexId vertexId;
  if ( idx >= 0 )
  {
    vertexId.part = mVertices.at( idx ).id.part;
    vertexId.ring = mVertices.at( idx ).id.ring;
    vertexId.vertex = mVertices.at( idx ).id.vertex;
    setState( mVertices.at( idx ).isVirtual ? QStringLiteral( "create" ) : QStringLiteral( "update" ) );
    // convert point to map coordinates so we can center map
    setClickedPoint( InputUtils::transformPoint( mLayer->crs(), mapSettings()->destinationCrs(), mLayer->transformContext(), mVertices.at( idx ).point ) );
  }
  else
  {
    setState( QStringLiteral( "view" ) );
  }
  setClickedVertexId( vertexId );
}

void RecordingMapTool::removeVertex()
{
  if ( !mClickedVertexId.isValid() )
  {
    return;
  }

  if ( mRecordedGeometry.get()->deleteVertex( mClickedVertexId ) )
    emit recordedGeometryChanged( mRecordedGeometry );
}

void RecordingMapTool::insertVertex( const QgsPoint &point )
{
  if ( !mClickedVertexId.isValid() )
  {
    return;
  }

  if ( mRecordedGeometry.get()->insertVertex( mClickedVertexId, point ) )
    emit recordedGeometryChanged( mRecordedGeometry );
}

void RecordingMapTool::updateVertex( const QgsPoint &point )
{
  if ( !mClickedVertexId.isValid() )
  {
    return;
  }

  if ( mRecordedGeometry.get()->moveVertex( mClickedVertexId, point ) )
    emit recordedGeometryChanged( mRecordedGeometry );
}

void RecordingMapTool::addVertex( QgsVertexId id, QgsPoint &point, bool isVirtual )
{
  Vertex v;
  v.id = id;
  v.point = point;
  v.isVirtual = isVirtual;
  mVertices.push_back( v );
}
