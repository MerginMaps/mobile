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
#include "qgsrendercontext.h"

#include "position/positionkit.h"
#include "variablesmanager.h"
#include "inpututils.h"

RecordingMapTool::RecordingMapTool( QObject *parent )
  : AbstractMapTool{parent}
{
  connect( this, &RecordingMapTool::initialGeometryChanged, this, &RecordingMapTool::prepareEditing );
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

  if ( mActiveVertex.isValid() ) // if Grab
  {
    if ( mRecordedGeometry.get()->insertVertex( mActiveVertex.vertexId(), pointToAdd ) )
      emit recordedGeometryChanged( mRecordedGeometry );
  }
  else if ( mState == MapToolState::Record )
  {
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

      if ( !r )
      {
        return;
      }

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
}

void RecordingMapTool::removePoint()
{
  if ( mActiveVertex.isValid() ) // if Grab
  {
    if ( mRecordedGeometry.get()->deleteVertex( mActiveVertex.vertexId() ) )
      emit recordedGeometryChanged( mRecordedGeometry );

    // TODO: grab previous vertex
  }
  else if ( mState == MapToolState::Record )
  {
    if ( mRecordedGeometry.constGet()->vertexCount() > 0 )
    {
      QgsVertexId id( 0, 0, mRecordedGeometry.constGet()->vertexCount() - 1 );
      if ( mRecordedGeometry.type() == QgsWkbTypes::PolygonGeometry )
      {
        QgsLineString *r = qgsgeometry_cast<QgsLineString *>( qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() )->exteriorRing() );
        if ( !r )
        {
          return;
        }

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

void RecordingMapTool::prepareEditing()
{
  if ( !mInitialGeometry.isEmpty() )
  {
    setState( MapToolState::View );
    setRecordedGeometry( mInitialGeometry );
  }
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
    mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );

    // for lines and polygons create midpoints
    if ( mRecordedGeometry.type() != QgsWkbTypes::PointGeometry && vertexId.vertex < geom->vertexCount( vertexId.part, vertexId.ring ) - 1 )
    {
      QgsVertexId id( vertexId.part, vertexId.ring, vertexId.vertex + 1 );
      QgsPoint midPoint = QgsGeometryUtils::midpoint( geom->vertexAt( vertexId ), geom->vertexAt( id ) );
      midPoints->addGeometry( midPoint.clone() );
      mVertices.push_back( Vertex( id, midPoint, Vertex::MidPoint ) );
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
        mVertices.push_back( Vertex( startId, point, Vertex::Handle ) );

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
        mVertices.push_back( Vertex( endId, point, Vertex::Handle ) );
      }
      currentPart = vertexId.part;
      currentRing = vertexId.ring;
    }
  }

  emit existingVerticesChanged( mExistingVertices );
  emit midPointsChanged( mMidPoints );
  emit handlesChanged( mHandles );
}

void RecordingMapTool::lookForVertex( const QPointF &clickedPoint, double searchRadius )
{
  double minDistance = std::numeric_limits<double>::max();
  double currentDistance = 0;
  QgsRenderContext context = QgsRenderContext::fromMapSettings( mapSettings()->mapSettings() );
  double searchDistance = pixelsToMapUnits( searchRadius );

  QgsPoint pnt = mapSettings()->screenToCoordinate( clickedPoint );

  if ( mRecordedGeometry.isEmpty() )
  {
    setState( MapToolState::View );
    setActiveVertex( Vertex() );
    return;
  }

  int idx = -1;
  for ( int i = 0; i < mVertices.count(); i++ )
  {
    QgsPoint vertex( mVertices.at( i ).coordinates() );
    vertex.transform( mapSettings()->mapSettings().layerTransform( mLayer ) );
    currentDistance = pnt.distance( vertex );
    if ( currentDistance < minDistance && currentDistance <= searchDistance )
    {
      minDistance = currentDistance;
      idx = i;
    }
  }

  if ( idx >= 0 )
  {
    toggleSelectedVertexVisibility( idx );

    if ( mActiveVertex.type() == Vertex::Existing )
    {
      setState( MapToolState::Grab );
    }
    else if ( mActiveVertex.type() == Vertex::MidPoint )
    {
      addPoint( mActiveVertex.coordinates() );
      setState( MapToolState::Grab );
    }
    else if ( mActiveVertex.type() == Vertex::Handle )
    {
      mActiveVertex = Vertex();
      setState( MapToolState::Record );
    }

    emit activeVertexChanged( mActiveVertex );
  }
  else
  {
    // We did not select any point, but if some point was selected previously
    // we need to update its position first
    if ( mState == MapToolState::Grab )
    {
      updateVertex( mRecordPoint );
    }

    toggleSelectedVertexVisibility( -1 );
    setActiveVertex( Vertex() );
    setState( MapToolState::View );
  }
}

void RecordingMapTool::updateVertex( const QgsPoint &point )
{
  if ( !mActiveVertex.isValid() )
  {
    return;
  }

  if ( mRecordedGeometry.get()->moveVertex( mActiveVertex.vertexId(), point ) )
    emit recordedGeometryChanged( mRecordedGeometry );

  setState( MapToolState::View );
  setActiveVertex( Vertex() );
}

QgsPoint RecordingMapTool::vertexMapCoors( const Vertex &vertex ) const
{
  if ( vertex.isValid() && mLayer && mapSettings() )
  {
    return InputUtils::transformPoint( mLayer->crs(), mapSettings()->destinationCrs(), mLayer->transformContext(), vertex.coordinates() );
  }

  return QgsPoint();
}

void RecordingMapTool::cancelGrab()
{
  QgsPoint activeVertexPosition = vertexMapCoors( mActiveVertex );
  if ( !activeVertexPosition.isEmpty() )
  {
    mapSettings()->setCenter( activeVertexPosition );
  }

  setState( MapToolState::View );
  toggleSelectedVertexVisibility( -1 );
  setActiveVertex( Vertex() );
}

void RecordingMapTool::toggleSelectedVertexVisibility( int vertexIndex )
{
  if ( !mExistingVertices.isEmpty() )
  {
    // add currently selected vertex back to the mExistingVertices in order
    // to make it visible again
    if ( mActiveVertex.isValid() )
    {
      QgsMultiPoint *points = qgsgeometry_cast<QgsMultiPoint *>( mExistingVertices.get() );
      points->addGeometry( mActiveVertex.coordinates().clone() );
    }

    if ( vertexIndex != -1 )
    {
      mActiveVertex = mVertices.at( vertexIndex );

      // remove newly selected vertex from the mExistingVertices
      QgsPoint p = mActiveVertex.coordinates();
      mExistingVertices.filterVertices( [p]( const QgsPoint & point )->bool
      {
        return !InputUtils::equals( point, p, 1e-16 );
      } );
    }
    emit existingVerticesChanged( mExistingVertices );
  }
}

void RecordingMapTool::toggleHandleVisibility()
{
  double minDistance = std::numeric_limits<double>::max();
  double currentDistance = 0;

  if ( !mHandles.isEmpty() && mActiveVertex.isValid() )
  {
    // add previously hidden handle back
    if ( !mHiddenHandle.isEmpty() )
    {
      QgsMultiLineString *lines = qgsgeometry_cast<QgsMultiLineString *>( mHandles.get() );
      lines->addGeometry( mHiddenHandle.clone() );
      mHiddenHandle = QgsGeometry();
    }

    // find part containing active vertex and hide it
    QgsVertexId id;
    QgsPoint point;
    int part = -1;
    const QgsAbstractGeometry *geom = mHandles.constGet();
    while ( geom->nextVertex( id, point ) )
    {
      currentDistance = point.distance( mActiveVertex.coordinates() );
      if ( currentDistance < minDistance )
      {
        minDistance = currentDistance;
        part = id.part;
      }
    }

    if ( part != -1 )
    {
      mmHiddenHandle.set( geom->lineStringN( part ).clone() );
      mHandles.deletePart( part );
    }
  }

  emit handlesChanged( mHandles );
}

double RecordingMapTool::pixelsToMapUnits( double numPixels )
{
  QgsRenderContext context = QgsRenderContext::fromMapSettings( mapSettings()->mapSettings() );
  return numPixels * context.scaleFactor() * context.mapToPixel().mapUnitsPerPixel();
}

Vertex::Vertex()
{

}

Vertex::Vertex( QgsVertexId id, QgsPoint coordinates, VertexType type )
  : mVertexId( id )
  , mCoordinates( coordinates )
  , mType( type )
{

}

Vertex::~Vertex()
{

}

bool Vertex::isValid() const
{
  return mVertexId.isValid();
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

  emit initialGeometryChanged( mInitialGeometry );
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

RecordingMapTool::MapToolState RecordingMapTool::state() const
{
  return mState;
}

void RecordingMapTool::setState( const MapToolState &newState )
{
  if ( mState == newState )
    return;
  mState = newState;
  emit stateChanged( mState );
}

QgsPoint RecordingMapTool::recordPoint() const
{
  return mRecordPoint;
}

void RecordingMapTool::setRecordPoint( QgsPoint newRecordPoint )
{
  if ( mRecordPoint == newRecordPoint )
    return;
  mRecordPoint = newRecordPoint;
  emit recordPointChanged( mRecordPoint );
}

const Vertex &RecordingMapTool::activeVertex() const
{
  return mActiveVertex;
}

void RecordingMapTool::setActiveVertex( const Vertex &newActiveVertex )
{
  if ( mActiveVertex == newActiveVertex )
    return;
  mActiveVertex = newActiveVertex;
  emit activeVertexChanged( mActiveVertex );
}


const QgsVertexId &Vertex::vertexId() const
{
  return mVertexId;
}

void Vertex::setVertexId( const QgsVertexId &newVertexId )
{
  if ( mVertexId == newVertexId )
    return;
  mVertexId = newVertexId;
}

QgsPoint Vertex::coordinates() const
{
  return mCoordinates;
}

void Vertex::setCoordinates( QgsPoint newCoordinates )
{
  if ( mCoordinates == newCoordinates )
    return;
  mCoordinates = newCoordinates;
}

const Vertex::VertexType &Vertex::type() const
{
  return mType;
}

void Vertex::setType( const VertexType &newType )
{
  if ( mType == newType )
    return;
  mType = newType;
}
