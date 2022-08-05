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
#include "qgslinestring.h"
#include "qgsmultilinestring.h"
#include "qgspolygon.h"
#include "qgsmultipolygon.h"

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
  if ( !mAllowed || !mMapSettings || mCrosshairPosition.isNull() )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  if ( mRealGeometry.isEmpty() )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  QgsPoint crosshair = mMapSettings->screenToCoordinate( mCrosshairPosition );
  if ( crosshair.isEmpty() )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  /**
   * How guideline is built
   *
   * Point / MultiPoint
   * No guideline
   *
   * LineString / MultiLineString
   * Differentiate between recording and grabbing state.
   * If recording - create simple line from the beggining/end of the active part's point to crosshair position
   * If grab - create line consisting of three points (one before active*, crosshair, one after active*)
   *    * ~ if such vertex does not exist, skip it - we move first or last point
   *
   * Polygon / MultiPolygon
   * Differentiate between recording and grabbing state.
   * If grab - if we hold the only existing vertex - do not draw guideline
   *         - line consisting of three points (one before active*, crosshair, one after active*)
   *    * ~ if such vertex does not exist, skip it - we move first or last point
   *
   * If recording - create line if there is only one vertex - between vertex and crosshair
   *              - create polygon between first point, crosshair and last added point (make sure it is not the first one)
   */

  QgsWkbTypes::GeometryType geotype = mRealGeometry.type();

  if ( geotype == QgsWkbTypes::PointGeometry )
  {
    setGuidelineGeometry( QgsGeometry() );
    return;
  }

  if ( mActiveVertex.isValid() )
  {
    // we grab an existing point

    int nVertices = mRealGeometry.constGet()->vertexCount( mActiveVertex.vertexId().part, mActiveVertex.vertexId().ring );
    if ( nVertices <= 1 )
    {
      // we hold the only point or there is no point
      setGuidelineGeometry( QgsGeometry() );
      return;
    }

    QgsPolylineXY guideline;

    QgsVertexId current = mActiveVertex.vertexId();
    QgsPoint previous, next;

    previous = mRealGeometry.constGet()->vertexAt( QgsVertexId( current.part, current.ring, current.vertex - 1 ) );
    next = mRealGeometry.constGet()->vertexAt( QgsVertexId( current.part, current.ring, current.vertex + 1 ) );

    if ( !previous.isEmpty() )
    {
      guideline.push_back( previous );
    }

    guideline.push_back( crosshair );

    if ( !next.isEmpty() )
    {
      guideline.push_back( next );
    }

    setGuidelineGeometry( QgsGeometry::fromPolylineXY( guideline ) );
  }
  else
  {
    // recording new points

    if ( geotype == QgsWkbTypes::LineGeometry )
    {
      // we are adding new point to the end/beginning
      QgsGeometry guideline;
      QgsLineString *line;

      if ( mRealGeometry.isMultipart() )
      {
        QgsMultiLineString *multiLine = qgsgeometry_cast<QgsMultiLineString *>( mRealGeometry.constGet() );
        line = multiLine->lineStringN( mActivePart );
      }
      else
      {
        line = qgsgeometry_cast<QgsLineString *>( mRealGeometry.constGet() );
      }

      if ( mInsertPolicy == RecordingMapTool::Start )
      {
        // add crosshair to the begginning
        QgsPoint firstPoint = line->pointN( 0 );
        guideline = QgsGeometry::fromPolyline( { crosshair, firstPoint } );
      }
      else
      {
        // add crosshair to the end of the geometry
        QgsPoint lastPoint = line->pointN( line->vertexCount() - 1 );
        guideline = QgsGeometry::fromPolyline( { lastPoint, crosshair } );
      }

      setGuidelineGeometry( guideline );
    }
    else if ( geotype == QgsWkbTypes::PolygonGeometry )
    {
      int nVertices = mRealGeometry.constGet()->vertexCount( mActivePart, 0 /*TODO: add active ring*/ );
      if ( nVertices == 0 )
      {
        // we hold the only point
        setGuidelineGeometry( QgsGeometry() );
        return;
      }
      else if ( nVertices == 1 )
      {
        // build line
        QgsPointXY previous = mRealGeometry.constGet()->vertexAt( QgsVertexId( mActivePart, 0 /*TODO: add active ring*/, 0 ) );
        setGuidelineGeometry( QgsGeometry::fromPolylineXY( { previous, crosshair } ) );
      }
      else
      {
        // build a polygon
        QgsPointXY first = mRealGeometry.constGet()->vertexAt( QgsVertexId( mActivePart, 0 /*TODO: add active ring*/, 0 ) );
        QgsPointXY last = mRealGeometry.constGet()->vertexAt( QgsVertexId( mActivePart, 0 /*TODO: add active ring*/, nVertices - 2 ) );
        setGuidelineGeometry( QgsGeometry::fromPolygonXY( { { first, crosshair, last } } ) );
      }
    }
  }





//  if ( mActiveVertex.isValid() )
//  {
//    // we grab a point - move existing point

//    qDebug() << "R:" << mRealGeometry.asWkt(5);

//    if ( mRealGeometry.type() == QgsWkbTypes::PolygonGeometry )
//    {
//      QgsPolygonXY poly;

//      if ( mRealGeometry.isMultipart() )
//      {
//        QgsMultiPolygon *multiPolygon = qgsgeometry_cast<QgsMultiPolygon *>( mRealGeometry.constGet() );
//        poly = QgsGeometry( multiPolygon->polygonN( mActivePart ) ).asPolygon();
//      }
//      else
//      {
//        poly = mRealGeometry.asPolygon();
//      }

//      if ( poly[0].count() < 2 )
//      {
//        // if it is not yet a polygon, create line guideline
//        poly[0].append( crosshair );
//        setGuidelineGeometry( QgsGeometry::fromPolylineXY( poly[0] ) );
//      }
//      else
//      {
//        // let's add the crosshair as one-before-last vertex
//        poly[0].insert( poly[0].count() - 1, crosshair );
//        setGuidelineGeometry( QgsGeometry::fromPolygonXY( poly ) );
//      }
//    }
//    else
//    {
//      QgsGeometry g( mRealGeometry );
//      g.moveVertex( crosshair, g.vertexNrFromVertexId( mActiveVertex.vertexId() ) );
//      setGuidelineGeometry( g );
//    }
//    qDebug() << "G:" << mGuidelineGeometry.asWkt(5) << mActiveVertex.vertexId().vertex;


//  }
//  else
//  {
//    // we are recording new points - add current crosshair to the end/start of geometry

//    if ( mRealGeometry.type() == QgsWkbTypes::LineGeometry )
//    {
//      QgsGeometry guideline;
//      QgsLineString *line;

//      if ( mRealGeometry.isMultipart() )
//      {
//        QgsMultiLineString *multiLine = qgsgeometry_cast<QgsMultiLineString *>( mRealGeometry.constGet() );
//        line = multiLine->lineStringN( mActivePart );
//      }
//      else
//      {
//        line = qgsgeometry_cast<QgsLineString *>( mRealGeometry.constGet() );
//      }

//      if ( mNewVertexOrder == RecordingMapTool::Start )
//      {
//        // add crosshair to the begginning
//        QgsPoint firstPoint = line->pointN( 0 );
//        guideline = QgsGeometry::fromPolyline( { firstPoint, crosshair } );
//      }
//      else
//      {
//        // add crosshair to the end of the geometry
//        QgsPoint lastPoint = line->pointN( line->vertexCount() - 1 );
//        guideline = QgsGeometry::fromPolyline( { lastPoint, crosshair } );
//      }

//      setGuidelineGeometry( guideline );
//    }
//    else if ( mRealGeometry.type() == QgsWkbTypes::PolygonGeometry )
//    {
//      QgsPolygonXY poly;

//      if ( mRealGeometry.isMultipart() )
//      {
//        QgsMultiPolygon *multiPolygon = qgsgeometry_cast<QgsMultiPolygon *>( mRealGeometry.constGet() );
//        poly = QgsGeometry( multiPolygon->polygonN( mActivePart ) ).asPolygon();
//      }
//      else
//      {
//        poly = mRealGeometry.asPolygon();
//      }

//      if ( poly[0].count() < 2 )
//      {
//        // if it is not yet a polygon, create line guideline
//        poly[0].append( crosshair );
//        setGuidelineGeometry( QgsGeometry::fromPolylineXY( poly[0] ) );
//      }
//      else
//      {
//        // let's add the crosshair as one-before-last vertex
//        poly[0].insert( poly[0].count() - 1, crosshair );
//        setGuidelineGeometry( QgsGeometry::fromPolygonXY( poly ) );
//      }
//    }
//  }
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

const RecordingMapTool::InsertPolicy &GuidelineController::insertPolicy() const
{
  return mInsertPolicy;
}

void GuidelineController::setInsertPolicy( const RecordingMapTool::InsertPolicy &insertPolicy )
{
  if ( mInsertPolicy == insertPolicy )
    return;
  mInsertPolicy = insertPolicy;
  emit insertPolicyChanged( mInsertPolicy );
}

int GuidelineController::activePart() const
{
  return mActivePart;
}

void GuidelineController::setActivePart( int newActivePart )
{
  if ( mActivePart == newActivePart )
    return;
  mActivePart = newActivePart;
  emit activePartChanged( mActivePart );
}
