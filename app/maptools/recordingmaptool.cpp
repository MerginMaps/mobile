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
  connect( this, &RecordingMapTool::activeVertexChanged, this, &RecordingMapTool::createNodesAndHandles );
  connect( this, &RecordingMapTool::activeVertexChanged, this, []( Vertex newV )
  {
    qDebug() << "New active vertex:" << newV.vertexId().ring << newV.vertexId().vertex << newV.coordinates().asWkt( 6 ) << newV.type();
  } );
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
    if ( mActiveVertex.isValid() )
    {
      id = mActiveVertex.vertexId();
    }

    if ( mNewVertexOrder == NewVertexOrder::End )
    {
      id.vertex = mRecordedGeometry.constGet()->vertexCount();
    }
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

  if ( mRecordedGeometry.wkbType() == QgsWkbTypes::Point )
  {
    mRecordedGeometry.set( pointToAdd.clone() );
  }
  else if ( mRecordedGeometry.wkbType() == QgsWkbTypes::MultiPoint )
  {
    mRecordedGeometry.addPart( pointToAdd.clone() );
  }
  else
  {
    mRecordedGeometry.get()->insertVertex( id, pointToAdd );
  }

  emit recordedGeometryChanged( mRecordedGeometry );
}

void RecordingMapTool::addPointAtPosition( Vertex vertex, const QgsPoint &point )
{
  if ( vertex.isValid() )
  {
    if ( mRecordedGeometry.get()->insertVertex( vertex.vertexId(), point ) )
    {
      emit recordedGeometryChanged( mRecordedGeometry );
    }
  }
}

void RecordingMapTool::removePoint()
{
  if ( mRecordedGeometry.isEmpty() )
  {
    return;
  }

  if ( mActiveVertex.isValid() && mState == MapToolState::Grab ) // if Grab
  {
    int removedVertexId = mActiveVertex.vertexId().vertex;

    if ( mRecordedGeometry.constGet()->vertexCount( mActiveVertex.vertexId().part,  mActiveVertex.vertexId().ring ) > 0 )
    {
      QgsVertexId id = mActiveVertex.vertexId();
      if ( mRecordedGeometry.type() == QgsWkbTypes::PolygonGeometry )
      {
        QgsLineString *r;

        if ( mActiveVertex.vertexId().ring == 0 )
        {
          r = qgsgeometry_cast<QgsLineString *>( qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() )->exteriorRing() );
        }
        else
        {
          r = qgsgeometry_cast<QgsLineString *>( qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() )->interiorRing( mActiveVertex.vertexId().ring ) );
        }

        if ( !r )
        {
          return;
        }

        if ( r->nCoordinates() == 4 )
        {
          // this is the smallest possible closed ring (first and last vertex are equal),
          // we need to remove two last vertices in order to get correct linestring
          if ( removedVertexId == 0 || removedVertexId == r->nCoordinates() - 1 )
          {
            r->deleteVertex( QgsVertexId( 0, 0, 0 ) );
            r->deleteVertex( QgsVertexId( 0, 0, r->nCoordinates() - 1 ) );
          }
          else
          {
            r->deleteVertex( QgsVertexId( 0, 0, removedVertexId ) );
            r->deleteVertex( QgsVertexId( 0, 0, r->nCoordinates() - 1 ) );
          }

          emit recordedGeometryChanged( mRecordedGeometry );

          // Grab previous vertex if there is any, otherwise grab next one if there is any, otherwise go to record
          grabNextVertex( removedVertexId );
          return;
        }
        else if ( r->nCoordinates() <= 2 )
        {
          // if we remove last vertex directly the geometry will be cleared
          // but we want to keep start point, so instead of removing vertex
          // from the linestring we remove item from the QgsPointSequence.
          QgsPointSequence points;
          r->points( points );

          points.removeAt( removedVertexId );

          r->setPoints( points );
          emit recordedGeometryChanged( mRecordedGeometry );

          // Grab previous vertex if there is any, otherwise grab next one if there is any, otherwise go to record
          grabNextVertex( removedVertexId );
          return;
        }
        else
        {
          id.vertex = mActiveVertex.vertexId().vertex;
        }
      }
      else if ( mRecordedGeometry.type() == QgsWkbTypes::LineGeometry )
      {
        QgsLineString *r;
        if ( mRecordedGeometry.constGet()->partCount() > 1 )
        {
          QgsMultiLineString *ml = qgsgeometry_cast<QgsMultiLineString *>( mRecordedGeometry.get() );
          r = ml->lineStringN( mActiveVertex.vertexId().part );
        }
        else
        {
          r = qgsgeometry_cast<QgsLineString *>( mRecordedGeometry.get() );
        }

        if ( !r )
        {
          return;
        }

        if ( mRecordedGeometry.constGet()->vertexCount() == 2 )
        {
          // if we remove second vertex directly the geometry will be cleared
          // but we want to keep start point, so instead of removing vertex
          // from the linestring we remove item from the QgsPointSequence.
          QgsPointSequence points;
          r->points( points );
          points.removeAt( removedVertexId );
          r->setPoints( points );
          emit recordedGeometryChanged( mRecordedGeometry );

          // Grab previous vertex if there is any, otherwise grab next one if there is any, otherwise go to record
          grabNextVertex( removedVertexId );
          return;
        }
        else
        {
          id.vertex = mActiveVertex.vertexId().vertex;
        }
      }

      if ( mNewVertexOrder == Start )
      {
        id.vertex = 0;
      }

      mRecordedGeometry.get()->deleteVertex( id );

      emit recordedGeometryChanged( mRecordedGeometry );
    }

    // Grab previous vertex if there is any, otherwise grab next one if there is any, otherwise go to record
    grabNextVertex( removedVertexId );
  }
  else if ( mState == MapToolState::Record )
  {
    // select first/last existing vertex as active and change state to GRAB
    if ( mNewVertexOrder == NewVertexOrder::End )
    {
      mActiveVertex.setVertexId( QgsVertexId( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring, mRecordedGeometry.constGet()->vertexCount() - 1 ) );

      if ( InputUtils::isPolygonLayer( mLayer ) )
      {
        // if the ring is closed, we need to jump -2 vertices
        if ( mRecordedGeometry.constGet()->nCoordinates() >= 4 )
        {
          mActiveVertex.setVertexId( QgsVertexId( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring, mRecordedGeometry.constGet()->vertexCount() - 2 ) );
        }
      }
    }
    else if ( mNewVertexOrder == NewVertexOrder::Start )
    {
      mActiveVertex.setVertexId( QgsVertexId( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring, 0 ) );
    }

    if ( mActiveVertex.vertexId().part < 0 )
    {
      mActiveVertex.setVertexId( QgsVertexId( 0, mActiveVertex.vertexId().ring, mActiveVertex.vertexId().vertex ) );
    }

    if ( mActiveVertex.vertexId().ring < 0 )
    {
      mActiveVertex.setVertexId( QgsVertexId( mActiveVertex.vertexId().part, 0, mActiveVertex.vertexId().vertex ) );
    }

    mActiveVertex.setCoordinates( mRecordedGeometry.constGet()->vertexAt( mActiveVertex.vertexId() ) );
    mActiveVertex.setType( Vertex::Existing );

    emit activeVertexChanged( mActiveVertex );
    setState( MapToolState::Grab );
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
    mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );

    if ( shouldBeVisible( vertex ) )
    {
      existingVertices->addGeometry( vertex.clone() );
    }

    // for lines and polygons create midpoints
    if ( mRecordedGeometry.type() != QgsWkbTypes::PointGeometry && vertexId.vertex < geom->vertexCount( vertexId.part, vertexId.ring ) - 1 )
    {
      QgsVertexId id( vertexId.part, vertexId.ring, vertexId.vertex + 1 );

      QgsPoint midPoint = QgsGeometryUtils::midpoint( geom->vertexAt( vertexId ), geom->vertexAt( id ) );
      mVertices.push_back( Vertex( id, midPoint, Vertex::MidPoint ) );

      // hide midpoints on the left and right side of the selected node
      if ( shouldBeVisible( geom->vertexAt( vertexId ) ) && shouldBeVisible( geom->vertexAt( id ) ) )
      {
        midPoints->addGeometry( midPoint.clone() );
      }
    }

    //
    // We want to hide handles (both points and lines)
    //
    // It is visible when
    //  - no point is selected
    //  - existing vertex is selected, but not the first or last one
    //
    // Invisible when
    //  - handle is selected
    //  - first or last vertex is selected
    //
    // When you select handle
    //  - center to handle
    //  - go to record mode
    //  - add points either to start or end of the line based on the handle
    //

    // for lines also create start/end points and handles
    if ( mRecordedGeometry.type() == QgsWkbTypes::LineGeometry && ( vertexId.part != currentPart && vertexId.ring != currentRing ) )
    {
      int vertexCount = geom->vertexCount( vertexId.part, vertexId.ring );
      if ( vertexCount >= 2 )
      {
        // start point and handle
        QgsVertexId startId( vertexId.part, vertexId.ring, 0 );
        QgsVertexId endId( vertexId.part, vertexId.ring, 1 );

        QgsPoint handlePoint = QgsGeometryUtils::interpolatePointOnLine( geom->vertexAt( startId ), geom->vertexAt( endId ), -0.5 );

        mVertices.push_back( Vertex( startId, handlePoint, Vertex::HandleStart ) );

        if ( !( mState == Record && mNewVertexOrder == Start ) )
        {

          if ( shouldBeVisible( geom->vertexAt( startId ) ) && shouldBeVisible( handlePoint ) )
          {
            // Add handle start point to midPoints
            midPoints->addGeometry( handlePoint.clone() );

            // Add line to start point handle
            QgsLineString handle( handlePoint, geom->vertexAt( startId ) );
            handles->addGeometry( handle.clone() );
          }
        }

        // end point and handle
        startId = QgsVertexId( vertexId.part, vertexId.ring, vertexCount - 2 );
        endId = QgsVertexId( vertexId.part, vertexId.ring, vertexCount - 1 );

        handlePoint = QgsGeometryUtils::interpolatePointOnLine( geom->vertexAt( startId ), geom->vertexAt( endId ), 1.5 );

        mVertices.push_back( Vertex( endId, handlePoint, Vertex::HandleEnd ) );

        if ( !( mState == Record && mNewVertexOrder == End ) )
        {
          if ( shouldBeVisible( geom->vertexAt( endId ) ) && shouldBeVisible( handlePoint ) )
          {
            // Add handle end point to midPoints
            midPoints->addGeometry( handlePoint.clone() );

            // Add line to end point handle
            QgsLineString handle = QgsLineString( geom->vertexAt( endId ), handlePoint );
            handles->addGeometry( handle.clone() );
          }
        }
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
  double searchDistance = pixelsToMapUnits( searchRadius );

  QgsPoint pnt = mapSettings()->screenToCoordinate( clickedPoint );

  if ( mRecordedGeometry.isEmpty() )
  {
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

  // Update the previously grabbed point's position
  if ( mState == MapToolState::Grab )
  {
    updateVertex( mActiveVertex, mRecordPoint );
  }

  if ( idx >= 0 )
  {
    // we found a point
    Vertex lastActiveVertex = mActiveVertex;
    mActiveVertex = Vertex();

    if ( mActiveVertex.type() == Vertex::Existing )
    {
      mActiveVertex = mVertices.at( idx );
      setState( MapToolState::Grab );
    }
    else if ( mActiveVertex.type() == Vertex::MidPoint )
    {
      // We need to invalidate activeVertex so that
      // next construction of midpoints and handles contains all points
      addPointAtPosition( lastActiveVertex, lastActiveVertex.coordinates() );

      // After adding new point to the position, we need to
      // search again, because mVertices now includes more points.
      // Search should find the created vertex as Existing.
      return lookForVertex( clickedPoint, searchRadius );
    }
    else if ( mActiveVertex.type() == Vertex::HandleStart )
    {
      setNewVertexOrder( NewVertexOrder::Start );
      setState( MapToolState::Record );
    }
    else if ( mActiveVertex.type() == Vertex::HandleEnd )
    {
      setNewVertexOrder( NewVertexOrder::End );
      setState( MapToolState::Record );
    }

    emit activeVertexChanged( mActiveVertex );
  }
  else
  {
    // nothing found
    setActiveVertex( Vertex() );
    setState( MapToolState::View );
  }
}

void RecordingMapTool::releaseVertex( const QgsPoint &point )
{
  if ( !mActiveVertex.isValid() )
  {
    return;
  }

  int vertexCount = mRecordedGeometry.constGet()->vertexCount( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );

  if ( mRecordedGeometry.type() == QgsWkbTypes::PolygonGeometry )
  {
    QgsLineString *r;

    if ( mActiveVertex.vertexId().ring == 0 )
    {
      r = qgsgeometry_cast<QgsLineString *>( qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() )->exteriorRing() );
    }
    else
    {
      r = qgsgeometry_cast<QgsLineString *>( qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() )->interiorRing( mActiveVertex.vertexId().ring ) );
    }

    if ( !r )
    {
      return;
    }

    if ( vertexCount == 2 )
    {
      r->close();
      emit recordedGeometryChanged( mRecordedGeometry );

      setState( MapToolState::Record );
      setActiveVertex( Vertex() );
      return;
    }
    else if ( vertexCount == 1 )
    {
      updateVertex( mActiveVertex, point );
      setState( MapToolState::Record );
      setActiveVertex( Vertex() );
      return;
    }
  }

  updateVertex( mActiveVertex, point );

  // if it is a first or last vertex of the line we go to the recording mode
  if ( mRecordedGeometry.type() == QgsWkbTypes::LineGeometry )
  {
    if ( mActiveVertex.type() == Vertex::Existing && mActiveVertex.vertexId().vertex == 0 )
    {
      // Note: Order matters - we rebuild visible geometry when active vertex is changed
      setNewVertexOrder( NewVertexOrder::Start );
      setState( MapToolState::Record );
      setActiveVertex( Vertex() );
      return;
    }
    else if ( mActiveVertex.type() == Vertex::Existing && mActiveVertex.vertexId().vertex == vertexCount - 1 )
    {
      // Note: Order matters - we rebuild visible geometry when active vertex is changed
      setNewVertexOrder( NewVertexOrder::End );
      setState( MapToolState::Record );
      setActiveVertex( Vertex() );
      return;
    }
  }

  setState( MapToolState::View );
  setActiveVertex( Vertex() );
}

void RecordingMapTool::updateVertex( const Vertex &vertex, const QgsPoint &point )
{
  if ( vertex.isValid() )
  {
    if ( mRecordedGeometry.get()->moveVertex( vertex.vertexId(), point ) )
    {
      emit recordedGeometryChanged( mRecordedGeometry );
    }
  }
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
  setActiveVertex( Vertex() );
}

double RecordingMapTool::pixelsToMapUnits( double numPixels )
{
  QgsRenderContext context = QgsRenderContext::fromMapSettings( mapSettings()->mapSettings() );
  return numPixels * context.scaleFactor() * context.mapToPixel().mapUnitsPerPixel();
}

bool RecordingMapTool::shouldBeVisible( const QgsPoint point )
{
  return !mActiveVertex.isValid() || ( mActiveVertex.isValid() && !InputUtils::equals( point, mActiveVertex.coordinates(), 1e-16 ) );
}

void RecordingMapTool::grabNextVertex( const int removedVertexId )
{
  // Grab previous vertex if there is any, otherwise grab next one if there is any, otherwise go to record
  if ( removedVertexId - 1 >= 0 )
  {
    mActiveVertex.setVertexId( QgsVertexId( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring, removedVertexId - 1 ) );
    mActiveVertex.setCoordinates( mRecordedGeometry.constGet()->vertexAt( mActiveVertex.vertexId() ) );
    mActiveVertex.setType( Vertex::Existing );
    emit activeVertexChanged( mActiveVertex );
  }
  else if ( removedVertexId < mRecordedGeometry.constGet()->vertexCount( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring ) )
  {
    mActiveVertex.setCoordinates( mRecordedGeometry.constGet()->vertexAt( mActiveVertex.vertexId() ) );
    mActiveVertex.setType( Vertex::Existing );
    emit activeVertexChanged( mActiveVertex );
  }
  else
  {
    // geometry is now empty
    setActiveVertex( Vertex() );
    setState( MapToolState::Record );
  }
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

const RecordingMapTool::NewVertexOrder &RecordingMapTool::newVertexOrder() const
{
  return mNewVertexOrder;
}

void RecordingMapTool::setNewVertexOrder( const NewVertexOrder &newNewVertexOrder )
{
  if ( mNewVertexOrder == newNewVertexOrder )
    return;
  mNewVertexOrder = newNewVertexOrder;
  emit newVertexOrderChanged( mNewVertexOrder );
}
