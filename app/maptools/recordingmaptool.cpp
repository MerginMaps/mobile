/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "recordingmaptool.h"


#include "qgspolygon.h"
#include "qgsvectorlayerutils.h"
#include "qgsmultipoint.h"
#include "qgsmultilinestring.h"
#include "qgspolygon.h"
#include "qgsmultipolygon.h"
#include "qgsrendercontext.h"
#include "qgsvectorlayereditbuffer.h"

#include "position/positionkit.h"
#include "coreutils.h"

#include <QUndoStack>
#include <QUndoCommand>

RecordingMapTool::RecordingMapTool( QObject *parent )
  : AbstractMapTool{parent}
{
  connect( this, &RecordingMapTool::activeFeatureChanged, this, &RecordingMapTool::prepareEditing );
  connect( this, &RecordingMapTool::recordedGeometryChanged, this, &RecordingMapTool::completeEditOperation );
  connect( this, &RecordingMapTool::recordedGeometryChanged, this, &RecordingMapTool::collectVertices );
  connect( this, &RecordingMapTool::activeVertexChanged, this, &RecordingMapTool::updateVisibleItems );
  connect( this, &RecordingMapTool::activeVertexChanged, this, &RecordingMapTool::updateActiveVertexGeometry );
  connect( this, &RecordingMapTool::stateChanged, this, &RecordingMapTool::updateVisibleItems );
}

RecordingMapTool::~RecordingMapTool() = default;

void RecordingMapTool::addPoint( const QgsPoint &point )
{
  if ( !mActiveLayer )
  {
    return;
  }

  // if maptool is in GRAB and VIEW state, no point should be added
  if ( mState == RecordingMapTool::View || mState == RecordingMapTool::Grab )
  {
    return;
  }

  QgsPoint pointToAdd( point );

  if ( mPositionKit && ( mCenteredToGPS || mRecordingType == StreamMode ) )
  {
    // we want to use GPS point here instead of the point from map
    pointToAdd = mPositionKit->positionCoordinate();

    QgsPoint transformed = InputUtils::transformPoint(
                             mPositionKit->positionCrs3D(),
                             mActiveLayer->crs(),
                             mActiveLayer->transformContext(),
                             pointToAdd
                           );

    pointToAdd.setX( transformed.x() );
    pointToAdd.setY( transformed.y() );
  }

  if ( mLastRecordedPoint == pointToAdd )
  {
    // Avoid inserting duplicated vertex
    return;
  }

  mLastRecordedPoint = pointToAdd;

  fixZM( pointToAdd );

  // apply gps antenna height
  if ( QgsWkbTypes::hasZ( pointToAdd.wkbType() ) && mPositionKit && mPositionKit->antennaHeight() > 0 )
  {
    pointToAdd.setZ( pointToAdd.z() - mPositionKit->antennaHeight() );
  }

  QgsVertexId id( mActivePart, mActiveRing, 0 );

  if ( !mActiveFeature.isValid() )
  {
    QgsGeometry geometry = InputUtils::createGeometryForLayer( mActiveLayer );
    mActiveFeature = QgsFeature();
    mActiveFeature.setFields( mActiveLayer->fields(), true );
    mRecordedGeometry = mActiveFeature.geometry();

    mActiveLayer->startEditing();
    mActiveLayer->beginEditCommand( QStringLiteral( "Add new feature" ) );
    mActiveLayer->addFeature( mActiveFeature );
    mActiveLayer->endEditCommand();
  }

  if ( mRecordedGeometry.isEmpty() )
  {
    mRecordedGeometry = InputUtils::createGeometryForLayer( mActiveLayer );
  }
  else
  {
    if ( mInsertPolicy == InsertPolicy::End )
    {
      id.vertex = mRecordedGeometry.constGet()->vertexCount( mActivePart, mActiveRing );
    }
  }

  if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
  {
    // if it is a polygon and ring is not correctly defined yet (e.g. only
    // contains 1 point or not closed) we add point directly to the ring
    // and close it

    QgsLineString *r;
    const QgsPolygon *poly;

    if ( mRecordedGeometry.isMultipart() )
    {
      poly = qgsgeometry_cast<const QgsMultiPolygon *>( mRecordedGeometry.constGet() )->polygonN( mActivePart );
    }
    else
    {
      poly = qgsgeometry_cast<const QgsPolygon *>( mRecordedGeometry.constGet() );
    }

    if ( !poly )
    {
      return;
    }

    if ( mActiveRing == 0 )
    {
      r = qgsgeometry_cast<QgsLineString *>( poly->exteriorRing() );
    }
    else
    {
      // interior rings starts indexing from 0
      r = qgsgeometry_cast<QgsLineString *>( poly->interiorRing( mActiveRing - 1 ) );
    }

    if ( !r )
    {
      return;
    }

    // create part if all were removed and this is multipolygon geometry
    if ( mRecordedGeometry.isMultipart() && id.part >= mRecordedGeometry.constGet()->partCount() )
    {
      QgsLineString ring;
      ring.addVertex( pointToAdd );
      QgsPolygon poly( &ring );
      // ring will be closed automatically, bur we need to keep only one point,
      // so we remove end point
      QgsLineString *r = qgsgeometry_cast<QgsLineString *>( poly.exteriorRing() );
      if ( !r )
      {
        return;
      }
      QgsPointSequence points;
      r->points( points );
      points.removeLast();
      r->setPoints( points );
      mRecordedGeometry.addPart( poly.clone(), Qgis::GeometryType::Polygon );
    }

    if ( r->nCoordinates() < 2 )
    {
      r->addVertex( pointToAdd );
      r->close();

      mActiveLayer->beginEditCommand( QStringLiteral( "Add point" ) );
      emit recordedGeometryChanged( mRecordedGeometry );
      return;
    }
    else
    {
      // as rings are closed, we need to insert before last vertex
      id.vertex = mRecordedGeometry.constGet()->vertexCount( mActivePart, mActiveRing ) - 1;
    }
  }

  if ( QgsWkbTypes::flatType( mRecordedGeometry.wkbType() ) == Qgis::WkbType::Point )
  {
    mRecordedGeometry.set( pointToAdd.clone() );
  }
  else if ( QgsWkbTypes::flatType( mRecordedGeometry.wkbType() ) == Qgis::WkbType::MultiPoint )
  {
    mRecordedGeometry.addPart( pointToAdd.clone() );
  }
  else
  {
    // create part if it does not exist
    if ( mRecordedGeometry.isMultipart() && id.part >= mRecordedGeometry.constGet()->partCount() )
    {
      QgsLineString line;
      line.addVertex( pointToAdd );
      mRecordedGeometry.addPart( line.clone(), Qgis::GeometryType::Line );
    }
    else
    {
      mRecordedGeometry.get()->insertVertex( id, pointToAdd );
    }
  }


  mActiveLayer->beginEditCommand( QStringLiteral( "Add point" ) );
  emit recordedGeometryChanged( mRecordedGeometry );
}

void RecordingMapTool::addPointAtPosition( Vertex vertex, const QgsPoint &point )
{
  if ( !mActiveLayer )
  {
    return;
  }

  if ( vertex.isValid() )
  {
    mActiveLayer->beginEditCommand( QStringLiteral( "Add point at position" ) );
    if ( mRecordedGeometry.get()->insertVertex( vertex.vertexId(), point ) )
    {
      emit recordedGeometryChanged( mRecordedGeometry );
    }
  }
}

void RecordingMapTool::removePoint()
{
  if ( !mActiveLayer )
  {
    return;
  }

  if ( mRecordedGeometry.isEmpty() )
  {
    return;
  }

  if ( mState == MapToolState::Grab )
  {
    // we are removing existing vertex selected by ActiveVertex
    if ( !mActiveVertex.isValid() )
    {
      return;
    }

    QgsVertexId current = mActiveVertex.vertexId();

    if ( mRecordedGeometry.constGet()->vertexCount( current.part,  current.ring ) < 1 )
    {
      return;
    }

    if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
    {
      QgsLineString *r;
      QgsPolygon *poly;

      if ( mRecordedGeometry.isMultipart() )
      {
        poly = qgsgeometry_cast<QgsMultiPolygon *>( mRecordedGeometry.get() )->polygonN( current.part );
      }
      else
      {
        poly = qgsgeometry_cast<QgsPolygon *>( mRecordedGeometry.get() );
      }

      if ( !poly )
      {
        return;
      }

      if ( current.ring == 0 )
      {
        r = qgsgeometry_cast<QgsLineString *>( poly->exteriorRing() );
      }
      else
      {
        // interior rings starts indexing from 0
        r = qgsgeometry_cast<QgsLineString *>( poly->interiorRing( current.ring - 1 ) );
      }

      if ( !r )
      {
        return;
      }

      if ( r->nCoordinates() == 4 )
      {
        // this is the smallest possible closed ring (first and last vertex are equal),
        // we need to remove two last vertices in order to get correct linestring
        if ( current.vertex == 0 || current.vertex == r->nCoordinates() - 1 )
        {
          r->deleteVertex( QgsVertexId( 0, 0, 0 ) );
          r->deleteVertex( QgsVertexId( 0, 0, r->nCoordinates() - 1 ) );
        }
        else
        {
          r->deleteVertex( QgsVertexId( 0, 0, current.vertex ) );
          r->deleteVertex( QgsVertexId( 0, 0, r->nCoordinates() - 1 ) );
        }
      }
      else if ( r->nCoordinates() <= 2 )
      {
        // if we remove last vertex directly the geometry will be cleared
        // but we want to keep start point, so instead of removing vertex
        // from the linestring we remove item from the QgsPointSequence.
        QgsPointSequence points;
        r->points( points );

        points.removeAt( current.vertex );

        r->setPoints( points );
      }
      else
      {
        mRecordedGeometry.get()->deleteVertex( current );
      }

      // if this was the last point in the ring
      // and ring is interior we remove that ring completely
      //
      // WARNING:
      // "r" pointer may be invalid for exterior rings at this point!
      // so dereference it only for interior rings
      // see https://github.com/MerginMaps/mobile/issues/2875
      if ( current.ring > 0 && r->isEmpty() )
      {
        QgsCurvePolygon *p = qgsgeometry_cast<QgsCurvePolygon *>( poly );
        // rings numerarion starts with 0
        p->removeInteriorRing( current.ring - 1 );
      }

      // if this was the last point in the part of the multipart geometry
      // we remove that part completely
      if ( mRecordedGeometry.isMultipart() && poly->isEmpty() )
      {
        mRecordedGeometry.deletePart( current.part );
      }
    }
    else if ( mRecordedGeometry.type() == Qgis::GeometryType::Line )
    {
      QgsLineString *r;

      if ( mRecordedGeometry.isMultipart() )
      {
        QgsMultiLineString *ml = qgsgeometry_cast<QgsMultiLineString *>( mRecordedGeometry.get() );
        r = ml->lineStringN( current.part );
      }
      else
      {
        r = qgsgeometry_cast<QgsLineString *>( mRecordedGeometry.get() );
      }

      if ( !r )
      {
        return;
      }

      if ( r->nCoordinates() == 2 )
      {
        // if we remove second vertex directly the geometry will be cleared
        // but we want to keep start point, so instead of removing vertex
        // from the linestring we remove item from the QgsPointSequence.
        QgsPointSequence points;
        r->points( points );

        points.removeAt( current.vertex );
        r->setPoints( points );
      }
      else
      {
        // if this is the last point of the part in multipart geometry
        // let's remove that part completely
        if ( mRecordedGeometry.isMultipart() && r->nCoordinates() == 1 )
        {
          mRecordedGeometry.deletePart( current.part );
        }
        else
        {
          mRecordedGeometry.get()->deleteVertex( current );
        }
      }
    }
    else
    {
      // points / multipoints
      if ( mRecordedGeometry.isMultipart() )
      {
        mRecordedGeometry.deletePart( current.part );
      }
      else
      {
        mRecordedGeometry = QgsGeometry();
      }
    }

    mActiveLayer->beginEditCommand( QStringLiteral( "Delete vertex" ) );
    emit recordedGeometryChanged( mRecordedGeometry );

    grabNextVertex();
  }
  else if ( mState == MapToolState::Record )
  {
    // select first/last existing vertex as active and change state to GRAB
    int nVertices = mRecordedGeometry.constGet()->vertexCount( mActivePart, mActiveRing );
    if ( nVertices < 1 )
    {
      return;
    }

    int vertexToGrab = nVertices - 1;

    if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
    {
      if ( nVertices >= 4 )
      {
        // skip ring close vertex
        vertexToGrab = nVertices - 2;
      }
    }
    else if ( mInsertPolicy == InsertPolicy::Start )
    {
      vertexToGrab = 0;
    }

    QgsVertexId target( mActivePart, mActiveRing, vertexToGrab );
    QgsPoint targetPosition = mRecordedGeometry.constGet()->vertexAt( target );

    setActiveVertex( Vertex( target, targetPosition, Vertex::Existing ) );
    setState( MapToolState::Grab );
  }
}

bool RecordingMapTool::hasValidGeometry() const
{
  if ( mActiveLayer )
  {
    if ( mRecordedGeometry.isEmpty() )
    {
      return false;
    }

    if ( mActiveLayer->geometryType() == Qgis::GeometryType::Point )
    {
      if ( mRecordedGeometry.isMultipart() )
      {
        const QgsAbstractGeometry *geom = mRecordedGeometry.constGet();
        for ( auto it = geom->const_parts_begin(); it != geom->const_parts_end(); ++it )
        {
          if ( ( *it )->nCoordinates() != 1 )
          {
            return false;
          }
        }
        return true;
      }
      else
      {
        return mRecordedGeometry.constGet()->nCoordinates() == 1;
      }
    }
    else if ( mActiveLayer->geometryType() == Qgis::GeometryType::Line )
    {
      if ( mRecordedGeometry.isMultipart() )
      {
        const QgsAbstractGeometry *geom = mRecordedGeometry.constGet();
        for ( auto it = geom->const_parts_begin(); it != geom->const_parts_end(); ++it )
        {
          if ( ( *it )->nCoordinates() < 2 )
          {
            return false;
          }
        }
        return true;
      }
      else
      {
        return mRecordedGeometry.constGet()->nCoordinates() >= 2;
      }
    }
    else if ( mActiveLayer->geometryType() == Qgis::GeometryType::Polygon )
    {
      if ( mRecordedGeometry.isMultipart() )
      {
        const QgsAbstractGeometry *geom = mRecordedGeometry.constGet();
        for ( auto it = geom->const_parts_begin(); it != geom->const_parts_end(); ++it )
        {
          if ( ( *it )->nCoordinates() < 4 )
          {
            return false;
          }
        }
        return true;
      }
      else
      {
        return mRecordedGeometry.constGet()->nCoordinates() >= 4;
      }
    }
  }
  return false;
}

void RecordingMapTool::fixZM( QgsPoint &point ) const
{
  if ( !mActiveLayer )
    return;

  bool layerIs3D = QgsWkbTypes::hasZ( mActiveLayer->wkbType() );
  bool pointIs3D = QgsWkbTypes::hasZ( point.wkbType() );

  bool layerIsM = QgsWkbTypes::hasM( mActiveLayer->wkbType() );
  bool pointIsM = QgsWkbTypes::hasM( point.wkbType() );

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

  if ( layerIsM )
  {
    if ( !pointIsM )
    {
      point.addMValue();
    }
  }
  else /* !layerIsM */
  {
    if ( pointIsM )
    {
      point.dropMValue();
    }
  }
}

void RecordingMapTool::onPositionChanged()
{
  if ( !mActiveLayer )
  {
    return;
  }

  if ( mRecordingType != StreamMode )
  {
    return;
  }

  if ( !mPositionKit || !mPositionKit->hasPosition() )
  {
    return;
  }

  if ( mRecordingIntervalType == StreamingIntervalType::IntervalType::Time )
  {
    if ( mLastTimeRecorded.addSecs( mRecordingInterval ) <= QDateTime::currentDateTime() )
    {
      addPoint( QgsPoint() ); // addPoint will take point from GPS
      mLastTimeRecorded = QDateTime::currentDateTime();
    }
  }
  else if ( mRecordingIntervalType == StreamingIntervalType::IntervalType::Distance )
  {
    double factor = QgsUnitTypes::fromUnitToUnitFactor( Qgis::DistanceUnit::Degrees, Qgis::DistanceUnit::Meters );
    double distance = mLastRecordedPoint.distance( mPositionKit->positionCoordinate() );
    if ( mLastRecordedPoint.isEmpty() || factor * distance >= mRecordingInterval )
    {
      addPoint( QgsPoint() ); // addPoint will take point from GPS
    }
  }
  else
  {
    if ( !mRecordedGeometry.isEmpty() )
    {
      // update the last point of the geometry
      // so that it is placed on user's current position
      QgsPoint position = mPositionKit->positionCoordinate();

      QgsPointXY transformed = InputUtils::transformPoint(
                                 mPositionKit->positionCrs3D(),
                                 mActiveLayer->sourceCrs(),
                                 mActiveLayer->transformContext(),
                                 position
                               );
      QgsPoint p( transformed.x(), transformed.y(), position.z() );
      QgsVertexId id( mActivePart, mActiveRing, mRecordedGeometry.constGet()->vertexCount() - 1 );
      mRecordedGeometry.get()->moveVertex( id, p );
      emit recordedGeometryChanged( mRecordedGeometry );
    }
  }
}

void RecordingMapTool::prepareEditing()
{
  if ( mActiveLayer && mActiveFeature.isValid() && !mActiveFeature.geometry().isEmpty() )
  {
    mActiveLayer->startEditing();

    // if we are editing point layer we start with the grabbed point
    if ( mActiveFeature.geometry().type() == Qgis::GeometryType::Point && !mActiveFeature.geometry().isMultipart() )
    {
      Vertex v( QgsVertexId( 0, 0, 0 ), QgsPoint( mActiveFeature.geometry().asPoint() ), Vertex::Existing );
      setActiveVertex( v );
      setState( MapToolState::Grab );
    }
    else
    {
      setState( MapToolState::View );
    }
    setRecordedGeometry( mActiveFeature.geometry() );
  }
  else if ( !mActiveFeature.isValid() || mActiveFeature.geometry().isEmpty() )
  {
    setRecordedGeometry( QgsGeometry() );
  }

  mLastRecordedPoint = QgsPoint();
}

void RecordingMapTool::collectVertices()
{
  mVertices.clear();

  if ( mRecordedGeometry.isEmpty() )
  {
    updateVisibleItems();
    return;
  }

  QgsPoint vertex;
  QgsVertexId vertexId;
  const QgsAbstractGeometry *geom = mRecordedGeometry.constGet();

  int startPart = -1;
  int endPart = -1;

  /**
   * Extracts existing geometry vertices and generates virtual vertices representing
   * midpoints (for lines and polygons) and start/end handles (for lines).
   *
   * For lines each part extracted in the following order: start handle, first vertex,
   * midPointN, vertexN, …, last vertex, end handle. So, for simple line containing
   * just 3 points we will get the following sequence of vertices (h — handle,
   * v — exisiting vertex, m — midpoint):
   * LINE: A -> B -> C
   * VERTICES: hS, vA, mA, vB, mB, vC, hE
   *
   * Similarly for multiparts:
   * LINE: part1: A -> B -> C | part 2: D -> E | part 3: X
   * VERTICES: hS1, vA, mA, vB, mB, vC, hE1, hS2, vD, mD, vE, hE2, vX
   *
   * For polygons each part extracted in the following order: first vertex, midPointN,
   * vertexN, …. Last closing vertex (which has the same coordinates as the first one)
   * is ignored. Interior rings (holes) are extracted in the same way and go right
   * after the correspoinding inrerior ring. So for simple polygon containing 4 points
   * (first and last point are the same) we will get following sequence of vertices:
   * POLYGON: A -> B -> C -> A
   * VERTICES vA, mA, vB, mB, vC, mC
   *
   * Similarly for multiparts:
   * POLYGON: part1: A -> B -> C -> A, ring1: D -> E -> F ->D | part2: X -> Y -> Z -> X | part3: G -> H | part 4: J
   * VERTICES: vA, mA, vB, mB, vC, mC, vD, mD, vE, mE, vF, mF, vX, mX, vY, mY, vZ, mZ, vG, mG, vH, vJ
   */
  while ( geom->nextVertex( vertexId, vertex ) )
  {
    int vertexCount = geom->vertexCount( vertexId.part, vertexId.ring );

    if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
    {
      if ( vertexCount == 1 )
      {
        // Edge case! Sometimes we want to have invalid polygon, which is in fact
        // single point, in such case we keep this point
        mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );
        continue;
      }

      if ( vertexId.vertex < vertexCount - 1 )
      {
        // actual vertex
        mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );

        QgsVertexId id( vertexId.part, vertexId.ring, vertexId.vertex + 1 );
        QgsPoint midPoint = QgsGeometryUtils::midpoint( geom->vertexAt( vertexId ), geom->vertexAt( id ) );
        mVertices.push_back( Vertex( id, midPoint, Vertex::MidPoint ) );
      }
      else if ( vertexCount == 2 && vertexId.vertex == vertexCount - 1 )
      {
        // Edge case! Sometimes we want to have invalid polygon, which is in fact
        // just a line segment, in such case last vertex should be kept
        mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );
      }
      // ignore the closing vertex in polygon
      else if ( vertexId.vertex == vertexCount - 1 )
      {
        continue;
      }
    }
    else if ( mRecordedGeometry.type() == Qgis::GeometryType::Line )
    {
      // if this is firt point in line (or part) we add handle start point first
      if ( vertexId.vertex == 0 && vertexId.part != startPart && vertexCount >= 2 )
      {
        // next line point. needed to get calculate handle point coordinates
        QgsVertexId id( vertexId.part, vertexId.ring, 1 );

        // start handle point
        QgsPoint handle = handlePoint( geom->vertexAt( id ), geom->vertexAt( vertexId ) );
        mVertices.push_back( Vertex( vertexId, handle, Vertex::HandleStart ) );
        startPart = vertexId.part;
      }

      // add actual vertex and midpoint if this is not the last vertex of the line
      if ( vertexId.vertex < vertexCount - 1 )
      {
        // actual vertex
        mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );

        // midpoint
        QgsVertexId id( vertexId.part, vertexId.ring, vertexId.vertex + 1 );
        QgsPoint midPoint = QgsGeometryUtils::midpoint( geom->vertexAt( vertexId ), geom->vertexAt( id ) );
        mVertices.push_back( Vertex( id, midPoint, Vertex::MidPoint ) );
      }

      // if this is last point in line (or part) we save actual vertex first
      // and then handle end point
      if ( vertexId.vertex == vertexCount - 1 && vertexId.part != endPart )
      {
        // last vertex of the line
        mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );

        if ( vertexCount >= 2 )
        {
          // previous line point. needed to get calculate handle point coordinates
          QgsVertexId id( vertexId.part, vertexId.ring, vertexCount - 2 );

          // end handle point
          QgsPoint handle = handlePoint( geom->vertexAt( id ), geom->vertexAt( vertexId ) );
          mVertices.push_back( Vertex( vertexId, handle, Vertex::HandleEnd ) );
          endPart = vertexId.part;
        }
      }
    }
    else
    {
      // for points and multipoints we just add existing vertices
      mVertices.push_back( Vertex( vertexId, vertex, Vertex::Existing ) );
    }
  }
  updateVisibleItems();
}

void RecordingMapTool::updateVisibleItems()
{
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

  Vertex v;
  for ( int i = 0; i < mVertices.count(); i++ )
  {
    v = mVertices.at( i );

    if ( v.type() == Vertex::Existing && v != mActiveVertex )
    {
      // show existing vertex if it is not an active one
      existingVertices->addGeometry( v.coordinates().clone() );
    }
    else if ( v.type() == Vertex::MidPoint )
    {
      // for lines show midpoint if previous or next vertex is not active
      if ( mRecordedGeometry.type() == Qgis::GeometryType::Line )
      {
        if ( i > 0 && i < mVertices.count() - 1 )
        {
          Vertex prevVertex = mVertices.at( i - 1 );
          Vertex nextVertex = mVertices.at( i + 1 );
          if ( prevVertex != mActiveVertex && nextVertex != mActiveVertex )
          {
            midPoints->addGeometry( v.coordinates().clone() );
          }
        }
      }

      // for polygons show midpoint if previous or next vertex is not active
      if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
      {
        if ( i > 0 )
        {
          Vertex prevVertex = mVertices.at( i - 1 );

          Vertex nextVertex;

          // next vertex should be the either the next vertex in the sequence
          // if this midpoint is a first or middle midpoint of the ring or
          // it should be the first vertex of the correspoding ring is this
          // midpoint is the last midpoint of the ring
          if ( i == mVertices.count() - 1 )
          {
            for ( int j = 0 ; j < mVertices.count(); j++ )
            {
              nextVertex = mVertices.at( j );
              if ( nextVertex.vertexId().part == v.vertexId().part && nextVertex.vertexId().ring == v.vertexId().ring )
              {
                break;
              }
            }
          }
          else
          {
            nextVertex = mVertices.at( i + 1 );
            if ( nextVertex.vertexId().part != v.vertexId().part || nextVertex.vertexId().ring != v.vertexId().ring )
            {
              for ( int j = 0 ; j < mVertices.count(); j++ )
              {
                nextVertex = mVertices.at( j );
                if ( nextVertex.vertexId().part == v.vertexId().part && nextVertex.vertexId().ring == v.vertexId().ring )
                {
                  break;
                }
              }
            }
          }

          if ( prevVertex != mActiveVertex && nextVertex != mActiveVertex )
          {
            midPoints->addGeometry( v.coordinates().clone() );
          }
        }
      }
    }
    else if ( v.type() == Vertex::HandleStart )
    {
      // start handle is visible if we are not recording from start and first vertex is not active
      Vertex lineStart = mVertices.at( i + 1 );
      if ( !( mState == MapToolState::Record && mInsertPolicy == InsertPolicy::Start ) && mActiveVertex != lineStart )
      {
        // start handle point
        midPoints->addGeometry( v.coordinates().clone() );

        // start handle line
        QgsLineString handle( v.coordinates(), lineStart.coordinates() );
        handles->addGeometry( handle.clone() );
      }
    }
    else if ( v.type() == Vertex::HandleEnd )
    {
      // end handle is visible if we are not recording from end and last vertex is not active
      Vertex lineEnd = mVertices.at( i - 1 );
      if ( !( mState == MapToolState::Record && mInsertPolicy == InsertPolicy::End ) && mActiveVertex != lineEnd )
      {
        // end handle point
        midPoints->addGeometry( v.coordinates().clone() );

        // end handle line
        QgsLineString handle( lineEnd.coordinates(), v.coordinates() );
        handles->addGeometry( handle.clone() );
      }
    }
  }

  emit existingVerticesChanged( mExistingVertices );
  emit midPointsChanged( mMidPoints );
  emit handlesChanged( mHandles );
}

void RecordingMapTool::lookForVertex( const QPointF &clickedPoint, double searchRadius )
{
  if ( !mActiveLayer )
  {
    return;
  }

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
    vertex.transform( mapSettings()->mapSettings().layerTransform( mActiveLayer ) );

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
    Vertex clickedVertex = mVertices.at( idx );

    mActiveVertex = Vertex();

    if ( clickedVertex.type() == Vertex::Existing )
    {
      setActiveVertex( mVertices.at( idx ) );
      setState( MapToolState::Grab );
    }
    else if ( clickedVertex.type() == Vertex::MidPoint )
    {
      // We need to invalidate activeVertex so that
      // next construction of midpoints and handles contains all points
      addPointAtPosition( clickedVertex, clickedVertex.coordinates() );

      // After adding new point to the position, we need to
      // search again, because mVertices now includes more points.
      // Search should find the created vertex as Existing.
      return lookForVertex( clickedPoint, searchRadius );
    }
    else if ( clickedVertex.type() == Vertex::HandleStart )
    {
      setInsertPolicy( InsertPolicy::Start );
      setActivePartAndRing( clickedVertex.vertexId().part, clickedVertex.vertexId().ring );
      setState( MapToolState::Record );
    }
    else if ( clickedVertex.type() == Vertex::HandleEnd )
    {
      setInsertPolicy( InsertPolicy::End );
      setActivePartAndRing( clickedVertex.vertexId().part, clickedVertex.vertexId().ring );
      setState( MapToolState::Record );
    }

    emit activeVertexChanged( mActiveVertex );
  }
  else
  {
    // nothing found
    setState( MapToolState::View );
    setActivePartAndRing( 0, 0 );
    setActiveVertex( Vertex() );
  }
}

void RecordingMapTool::releaseVertex( const QgsPoint &point )
{
  if ( !mActiveVertex.isValid() )
  {
    return;
  }

  int vertexCount = mRecordedGeometry.constGet()->vertexCount( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );

  if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
  {
    QgsPolygon *polygon;
    QgsLineString *ring;

    if ( mRecordedGeometry.isMultipart() )
    {
      QgsMultiPolygon *multiPolygon = qgsgeometry_cast<QgsMultiPolygon *>( mRecordedGeometry.get() );
      polygon = multiPolygon->polygonN( mActiveVertex.vertexId().part );
    }
    else
    {
      polygon = qgsgeometry_cast<QgsPolygon *>( mRecordedGeometry.get() );
    }

    if ( mActiveVertex.vertexId().ring == 0 )
    {
      ring = qgsgeometry_cast<QgsLineString *>( polygon->exteriorRing() );
    }
    else
    {
      ring = qgsgeometry_cast<QgsLineString *>( polygon->interiorRing( mActiveVertex.vertexId().ring - 1 ) );
    }

    if ( !ring )
    {
      return;
    }

    if ( vertexCount == 2 )
    {
      ring->close();
      emit recordedGeometryChanged( mRecordedGeometry );

      updateVertex( mActiveVertex, point );
      setState( MapToolState::Record );
      setActivePartAndRing( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );
      setActiveVertex( Vertex() );
      return;
    }
    else if ( vertexCount == 1 )
    {
      updateVertex( mActiveVertex, point );
      setState( MapToolState::Record );
      setActivePartAndRing( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );
      setActiveVertex( Vertex() );
      return;
    }
  }

  updateVertex( mActiveVertex, point );

  // if it is a first or last vertex of the line we go to the recording mode
  if ( mRecordedGeometry.type() == Qgis::GeometryType::Line )
  {
    if ( mActiveVertex.type() == Vertex::Existing && mActiveVertex.vertexId().vertex == 0 )
    {
      // Note: Order matters - we rebuild visible geometry when active vertex is changed
      setInsertPolicy( InsertPolicy::Start );
      setState( MapToolState::Record );
      setActivePartAndRing( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );
      setActiveVertex( Vertex() );
      return;
    }
    else if ( mActiveVertex.type() == Vertex::Existing && mActiveVertex.vertexId().vertex == vertexCount - 1 )
    {
      // Note: Order matters - we rebuild visible geometry when active vertex is changed
      setInsertPolicy( InsertPolicy::End );
      setState( MapToolState::Record );
      setActivePartAndRing( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );
      setActiveVertex( Vertex() );
      return;
    }
  }

  setState( MapToolState::View );
  setActivePartAndRing( 0, 0 );
  setActiveVertex( Vertex() );
}

FeatureLayerPair RecordingMapTool::getFeatureLayerPair()
{
  bool featureIsValid = FID_IS_NEW( mActiveFeature.id() ) || mActiveFeature.isValid();

  if ( mActiveLayer && featureIsValid )
  {
    // Avoid overlaps of features after finishing drawing
    if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
    {
      avoidIntersections();
    }
    mActiveFeature.setGeometry( mRecordedGeometry );
    return FeatureLayerPair( mActiveFeature, mActiveLayer );
  }

  return FeatureLayerPair();
}

void RecordingMapTool::discardChanges()
{
  if ( mActiveLayer && mActiveLayer->isEditable() )
  {
    if ( mActiveLayer->undoStack() )
    {
      //
      // In future, if we want to use REDO, we would probably need to
      // set all changes between mMinUndoStackIndex and current stack index
      // as obsolete.
      //

      mActiveLayer->undoStack()->setIndex( mMinUndoStackIndex );
    }

    mActiveLayer->triggerRepaint();
  }
}

void RecordingMapTool::onFeatureAdded( QgsFeatureId newFeatureId )
{
  setActiveFeature( mActiveLayer->getFeature( newFeatureId ) );
}

void RecordingMapTool::updateVertex( const Vertex &vertex, const QgsPoint &point )
{
  if ( !mActiveLayer )
  {
    return;
  }

  if ( vertex.isValid() && !InputUtils::equals( point, vertex.coordinates(), 1e-8 ) )
  {
    mActiveLayer->beginEditCommand( QStringLiteral( "Move vertex" ) );
    if ( mRecordedGeometry.get()->moveVertex( vertex.vertexId(), point ) )
    {
      emit recordedGeometryChanged( mRecordedGeometry );
    }
  }
}

QgsPoint RecordingMapTool::vertexMapCoors( const Vertex &vertex ) const
{
  if ( vertex.isValid() && mActiveLayer && mapSettings() )
  {
    return InputUtils::transformPoint( mActiveLayer->crs(), mapSettings()->destinationCrs(), mActiveLayer->transformContext(), vertex.coordinates() );
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
  setActivePartAndRing( 0, 0 );
  setActiveVertex( Vertex() );
}

double RecordingMapTool::pixelsToMapUnits( double numPixels )
{
  QgsRenderContext context = QgsRenderContext::fromMapSettings( mapSettings()->mapSettings() );
  return numPixels * context.scaleFactor() * context.mapToPixel().mapUnitsPerPixel();
}

bool RecordingMapTool::shouldBeVisible( const QgsPoint point )
{
  return !mActiveVertex.isValid() || !InputUtils::equals( point, mActiveVertex.coordinates(), 1e-16 );
}

void RecordingMapTool::grabNextVertex()
{
  if ( !mActiveVertex.isValid() )
  {
    return;
  }

  if ( mRecordedGeometry.isEmpty() )
  {
    setActivePartAndRing( 0, 0 );
    setState( MapToolState::Record );
    setActiveVertex( Vertex() );
    return;
  }

  // mActiveVertex is pointing to removed vertex
  QgsVertexId current = mActiveVertex.vertexId();

  // check whether we should try to get next vertex in the current part/ring
  // or it was already removed
  bool grabInCurrent = true;
  if ( mRecordedGeometry.isMultipart() )
  {
    if ( mRecordedGeometry.type() == Qgis::GeometryType::Line )
    {
      grabInCurrent = current.part <= mRecordedGeometry.constGet()->partCount();
    }
    else if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
    {
      // excluding exterior ring
      grabInCurrent = current.part <= mRecordedGeometry.constGet()->partCount() && current.ring <= mRecordedGeometry.constGet()->ringCount( current.part ) - 1;
    }
  }
  else
  {
    if ( mRecordedGeometry.type() == Qgis::GeometryType::Polygon )
    {
      // excluding exterior ring
      grabInCurrent = current.ring <= mRecordedGeometry.constGet()->ringCount() - 1;
    }
  }

  // if there are some remaining points in the current ring&part, grab one of them!
  if ( grabInCurrent && mRecordedGeometry.constGet()->vertexCount( current.part, current.ring ) )
  {
    int nextId = 0;

    bool isNotFirst = ( current.vertex > 0 );
    if ( isNotFirst )
    {
      nextId = current.vertex - 1;
    }

    QgsVertexId next( current.part, current.ring, nextId );
    QgsPoint positionOfNext = mRecordedGeometry.constGet()->vertexAt( next );
    setActiveVertex( Vertex( next, positionOfNext, Vertex::Existing ) );
    setState( MapToolState::Grab );
  }
  else
  {
    // jump to other part if there is any
    if ( mRecordedGeometry.constGet()->partCount() >= 1 )
    {
      QgsVertexId nextRingVertex( 0, 0, 0 );
      QgsPoint nextRingVertexPosition = mRecordedGeometry.constGet()->vertexAt( nextRingVertex );
      setActiveVertex( Vertex( nextRingVertex, nextRingVertexPosition, Vertex::Existing ) );
      setState( MapToolState::Grab );
    }
    else
    {
      // no more points in this ring/part, start recording
      setActivePartAndRing( 0, current.ring );
      setState( MapToolState::Record );
      setActiveVertex( Vertex() );
    }
  }
}

void RecordingMapTool::completeEditOperation()
{
  if ( mActiveLayer && mActiveLayer->isEditCommandActive() )
  {
    mActiveLayer->changeGeometry( mActiveFeature.id(), mRecordedGeometry );
    mActiveLayer->endEditCommand();
    mActiveLayer->triggerRepaint();
    setCanUndo( mActiveLayer->undoStack()->index() > mMinUndoStackIndex );
  }
}

void RecordingMapTool::undo()
{
  if ( mActiveLayer && mActiveLayer->undoStack() && mActiveLayer->undoStack()->index() > mMinUndoStackIndex )
  {
    mActiveLayer->undoStack()->undo();

    if ( mActiveFeature.id() < 0 )
    {
      // new feature not commited
      QgsGeometry geom = mActiveLayer->editBuffer()->addedFeatures()[ mActiveFeature.id() ].geometry();
      if ( !geom.isEmpty() ) // && geom.isGeosValid() )
      {
        setRecordedGeometry( geom );
      }
      else
      {
        mActiveLayer->undoStack()->setIndex( mMinUndoStackIndex );
        setActiveFeature( QgsFeature() );
        setState( MapToolState::Record );
        setActivePartAndRing( 0, 0 );
        setActiveVertex( Vertex() );
      }
    }
    else
    {
      QgsGeometry geom = mActiveLayer->editBuffer()->changedGeometries()[ mActiveFeature.id() ];
      if ( !geom.isEmpty() )
      {
        setRecordedGeometry( geom );
      }
      else
      {
        setRecordedGeometry( mActiveFeature.geometry() );
        setState( MapToolState::Record );
        setActivePartAndRing( 0, 0 );
        setActiveVertex( Vertex() );
      }
    }

    mActiveLayer->triggerRepaint();

    setCanUndo( mActiveLayer->undoStack()->index() > mMinUndoStackIndex );
  }
}

void RecordingMapTool::updateActiveVertexGeometry()
{
  if ( mActiveVertex.isValid() )
  {
    setActiveVertexGeometry( QgsGeometry( mActiveVertex.coordinates().clone() ) );
  }
  else
  {
    setActiveVertexGeometry( QgsGeometry() );
  }
}

QgsPoint RecordingMapTool::handlePoint( QgsPoint p1, QgsPoint p2 )
{
  if ( !mActiveLayer )
  {
    return QgsPoint();
  }

  double h = 15 * mapSettings()->mapUnitsPerPixel();
  double factor = QgsUnitTypes::fromUnitToUnitFactor( mapSettings()->destinationCrs().mapUnits(), mActiveLayer->crs().mapUnits() );
  QgsDistanceArea da;
  da.setEllipsoid( QStringLiteral( "WGS84" ) );
  da.setSourceCrs( mActiveLayer->crs(), mapSettings()->transformContext() );
  double d = da.convertLengthMeasurement( da.measureLine( QgsPointXY( p1 ), QgsPointXY( p2 ) ), mActiveLayer->crs().mapUnits() );
  double x = ( ( p2.x() - p1.x() ) * ( d + h * factor ) / d ) + p1.x();
  double y = ( ( p2.y() - p1.y() ) * ( d + h * factor ) / d ) + p1.y();
  return QgsPoint( x, y );
}

bool RecordingMapTool::hasChanges() const
{
  if ( !mActiveLayer )
  {
    return false;
  }

  if ( mActiveLayer->isEditable() && mActiveLayer->undoStack() )
  {
    return mActiveLayer->undoStack()->index() > mMinUndoStackIndex;
  }

  return false;
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

StreamingIntervalType::IntervalType RecordingMapTool::recordingIntervalType() const
{
  return mRecordingIntervalType;
}

void RecordingMapTool::setRecordingIntervalType( StreamingIntervalType::IntervalType intervalType )
{
  if ( mRecordingIntervalType != intervalType )
  {
    mRecordingIntervalType = intervalType;
    emit recordingIntervalTypeChanged();
  }
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

QgsVectorLayer *RecordingMapTool::activeLayer() const
{
  return mActiveLayer;
}

void RecordingMapTool::setActiveLayer( QgsVectorLayer *newActiveLayer )
{
  if ( mActiveLayer == newActiveLayer )
    return;

  if ( mActiveLayer && mActiveLayer->isEditable() )
  {
    mActiveLayer->rollBack();
    mActiveLayer->triggerRepaint();
  }

  mActiveLayer = newActiveLayer;
  emit activeLayerChanged( mActiveLayer );

  // we need to clear all recorded points and recalculate the geometry
  setRecordedGeometry( QgsGeometry() );
  setActiveFeature( QgsFeature() );
  setActiveVertex( Vertex() );
  setActivePartAndRing( 0, 0 );
  setState( MapToolState::Record );
  mMinUndoStackIndex = 0;
  setCanUndo( false );

  if ( mActiveLayer )
  {
    mActiveLayer->startEditing();
    mMinUndoStackIndex = mActiveLayer->undoStack()->index();
  }
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

const QgsGeometry &RecordingMapTool::activeVertexGeometry() const
{
  return mActiveVertexGeometry;
}

void RecordingMapTool::setActiveVertexGeometry( const QgsGeometry &newActiveVertexGeometry )
{
  if ( mActiveVertexGeometry.equals( newActiveVertexGeometry ) )
    return;
  mActiveVertexGeometry = newActiveVertexGeometry;
  emit recordedGeometryChanged( mActiveVertexGeometry );
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

const RecordingMapTool::InsertPolicy &RecordingMapTool::insertPolicy() const
{
  return mInsertPolicy;
}

void RecordingMapTool::setInsertPolicy( const InsertPolicy &insertPolicy )
{
  if ( mInsertPolicy == insertPolicy )
    return;
  mInsertPolicy = insertPolicy;
  emit insertPolicyChanged( mInsertPolicy );
}

int RecordingMapTool::activePart() const
{
  return mActivePart;
}

void RecordingMapTool::setActivePartAndRing( int newActivePart, int newActiveRing )
{
  bool partChanged = false, ringChanged = false;

  if ( mActivePart != newActivePart )
  {
    mActivePart = newActivePart;
    partChanged = true;
  }

  if ( mActiveRing != newActiveRing )
  {
    mActiveRing = newActiveRing;
    ringChanged = true;
  }

  if ( partChanged )
  {
    emit activePartChanged( mActivePart );
  }
  if ( ringChanged )
  {
    emit activeRingChanged( mActiveRing );
  }
}

const QVector< Vertex > &RecordingMapTool::collectedVertices() const
{
  return mVertices;
}

int RecordingMapTool::activeRing() const
{
  return mActiveRing;
}

bool RecordingMapTool::canUndo() const
{
  return mCanUndo;
}

void RecordingMapTool::setCanUndo( bool newCanUndo )
{
  if ( mCanUndo == newCanUndo )
    return;
  mCanUndo = newCanUndo;
  emit canUndoChanged( mCanUndo );
}

const QgsFeature &RecordingMapTool::activeFeature() const
{
  return mActiveFeature;
}

void RecordingMapTool::setActiveFeature( const QgsFeature &newActiveFeature )
{
  if ( mActiveFeature == newActiveFeature )
    return;

  mActiveFeature = newActiveFeature;
  emit activeFeatureChanged( mActiveFeature );
}

void RecordingMapTool::avoidIntersections()
{

  QList<QgsVectorLayer *> avoidIntersectionsLayers;
  switch ( mapSettings()->project()->avoidIntersectionsMode() )
  {
    case Qgis::AvoidIntersectionsMode::AvoidIntersectionsLayers:
      avoidIntersectionsLayers = mapSettings()->project()->avoidIntersectionsLayers();
      break;
    case Qgis::AvoidIntersectionsMode::AvoidIntersectionsCurrentLayer:
      avoidIntersectionsLayers.append( mActiveLayer );
      break;
    case Qgis::AvoidIntersectionsMode::AllowIntersections:
      break;
  }

  // if the list is empty we allow overlaps
  if ( avoidIntersectionsLayers.isEmpty() )
  {
    return;
  }

  // the operation checks the intersection with selected layers and also avoids the active feature that is being edited
  const Qgis::GeometryOperationResult operationResult = mRecordedGeometry.avoidIntersectionsV2( avoidIntersectionsLayers, {{mActiveLayer, {mActiveFeature.id()}}} );

  // the geometry type has changed, we will try to make it compatible with active layer
  if ( operationResult == Qgis::GeometryOperationResult::GeometryTypeHasChanged )
  {
    const QVector<QgsGeometry> newGeoms = mRecordedGeometry.coerceToType( mActiveLayer->wkbType() );
    // if we coerce to just 1 geometry set that geometry as the new geometry
    if ( newGeoms.count() == 1 )
    {
      mRecordedGeometry = newGeoms[0];
    }
    // we coerced to multiple new geometries, pick the biggest and set it as the new geometry
    else
    {
      double largest = 0;
      for ( int i = 0; i < newGeoms.size(); ++i )
      {
        const QgsGeometry &currentPart = newGeoms.at( i );
        const double currentPartSize = mActiveLayer->geometryType() == Qgis::GeometryType::Polygon ? currentPart.area() : currentPart.length();

        if ( currentPartSize > largest )
        {
          mRecordedGeometry = currentPart;
          largest = currentPartSize;
        }
      }
      emit finalSingleGeometry();
    }
  }

  if ( mRecordedGeometry.isEmpty() )
  {
    emit finalEmptyGeometry();
  }

}
