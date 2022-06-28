/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "snaputils.h"
#include "inpututils.h"

SnapUtils::SnapUtils( QObject *parent )
  : QObject{parent}
{
}


QgsProject *SnapUtils::qgsProject() const
{
  return mQgsProject;
}

void SnapUtils::setQgsProject( QgsProject *newQgsProject )
{
  if ( !newQgsProject )
  {
    return;
  }

  if ( newQgsProject->homePath().isEmpty() || mQgsProject == newQgsProject )
  {
    return;
  }

  mQgsProject = newQgsProject;
  emit qgsProjectChanged( mQgsProject );

  setupSnapping();
}

QgsQuickMapSettings *SnapUtils::mapSettings() const
{
  return mMapSettings;
}

void SnapUtils::setMapSettings( QgsQuickMapSettings *newMapSettings )
{
  if ( !newMapSettings || mMapSettings == newMapSettings )
    return;

  if ( mMapSettings )
  {
    disconnect( mMapSettings, nullptr, this, nullptr );
  }

  mMapSettings = newMapSettings;

  connect( mMapSettings, &QgsQuickMapSettings::extentChanged, this, &SnapUtils::onMapSettingsUpdated );
  connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, &SnapUtils::onMapSettingsUpdated );
  connect( mMapSettings, &QgsQuickMapSettings::mapUnitsPerPixelChanged, this, &SnapUtils::onMapSettingsUpdated );
  connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, &SnapUtils::onMapSettingsUpdated );
  connect( mMapSettings, &QgsQuickMapSettings::outputSizeChanged, this, &SnapUtils::onMapSettingsUpdated );
  connect( mMapSettings, &QgsQuickMapSettings::outputDpiChanged, this, &SnapUtils::onMapSettingsUpdated );

  mSnappingUtils.setMapSettings( mMapSettings->mapSettings() );
  emit mapSettingsChanged( mMapSettings );
}

void SnapUtils::getsnap( QPointF mapPoint )
{
  QgsPoint mapCoords = mMapSettings->screenToCoordinate( mapPoint );
  QgsPoint snappoint = mMapSettings->screenToCoordinate( mCenterPosition ); // by default show crosshair in center, no snap

  // do no snap in the streaming mode
  if ( !mUseSnapping )
  {
    setSnappedPosition( snappoint );
    setSnapped( false );
    return;
  }

  QgsPointLocator::Match snap = mSnappingUtils.snapToMap( QgsPointXY( mapCoords.x(), mapCoords.y() ) );
  if ( snap.isValid() )
  {
    QgsPoint layerPoint;

    // if snapped to vertex or line start/end point we get point coordinates from the
    // geometry of the feature to which we have snapped. Otherwise get point found by
    // the QgsPointLocator
    if ( snap.hasVertex() || snap.hasLineEndpoint() )
    {
      QgsFeature f;
      QgsFeatureRequest request;
      request.setFilterFid( snap.featureId() );
      const bool fetched = snap.layer()->getFeatures( request ).nextFeature( f );
      if ( fetched )
      {
        QgsVertexId vId;
        if ( !f.geometry().vertexIdFromVertexNr( snap.vertexIndex(), vId ) )
        {
          setSnappedPosition( snappoint );
          setSnapped( false );
          return;
        }
        const QgsGeometry geom( f.geometry() );
        layerPoint = InputUtils::transformPoint( snap.layer()->crs(), mDestinationCrs, mQgsProject->transformContext(), geom.constGet()->vertexAt( vId ) );
      }
    }
    else
    {
      layerPoint = InputUtils::transformPoint( mMapSettings->destinationCrs(), mDestinationCrs, mQgsProject->transformContext(), QgsPoint( snap.point() ) );
    }

    setSnappedPosition( layerPoint );

    if ( snap.hasVertex() )
    {
      setSnapType( SnapUtils::Vertex );
    }
    else if ( snap.hasArea() || snap.hasCentroid() || snap.hasMiddleSegment() )
    {
      setSnapType( SnapUtils::Other );
    }
    else
    {
      setSnapType( SnapUtils::Segment );
    }

    setSnapped( true );
  }
  else
  {
    setSnappedPosition( snappoint );
    setSnapped( false );
  }
}

void SnapUtils::onMapSettingsUpdated()
{
  mSnappingUtils.setMapSettings( mMapSettings->mapSettings() );

  getsnap( mCenterPosition );
}

QPointF SnapUtils::centerPosition() const
{
  return mCenterPosition;
}

void SnapUtils::setCenterPosition( QPointF newCenterPosition )
{
  if ( mCenterPosition == newCenterPosition )
    return;
  mCenterPosition = newCenterPosition;
  emit centerPositionChanged( mCenterPosition );
}

QgsPoint SnapUtils::snappedPosition() const
{
  return mSnappedPosition;
}

void SnapUtils::setSnappedPosition( QgsPoint newSnappedPosition )
{
  mSnappedPosition = newSnappedPosition;
  emit snappedPositionChanged( mSnappedPosition );
}

bool SnapUtils::snapped() const
{
  return mSnapped;
}

void SnapUtils::setSnapped( bool newSnapped )
{
  if ( mSnapped == newSnapped )
    return;
  mSnapped = newSnapped;
  emit snappedChanged( mSnapped );
}

const SnapUtils::SnapType &SnapUtils::snapType() const
{
  return mSnapType;
}

void SnapUtils::setSnapType( const SnapUtils::SnapType &newSnapType )
{
  if ( mSnapType == newSnapType )
    return;
  mSnapType = newSnapType;
  emit snapTypeChanged( mSnapType );
}

bool SnapUtils::useSnapping() const
{
  return mUseSnapping;
}

void SnapUtils::setUseSnapping( bool useSnapping )
{
  mUseSnapping = useSnapping;
}

QgsCoordinateReferenceSystem SnapUtils::destinationCrs() const
{
  return mDestinationCrs;
}

void SnapUtils::setDestinationCrs( QgsCoordinateReferenceSystem crs )
{
  if ( mDestinationCrs == crs )
    return;

  mDestinationCrs = crs;
}

void SnapUtils::setupSnapping()
{
  int mode = mQgsProject->readNumEntry( QStringLiteral( "Mergin" ), QStringLiteral( "Snapping" ), 0 );
  switch ( mode )
  {
    case 0:
    {
      mUseSnapping = false;
      break;
    }
    case 1:
    {
      QgsSnappingConfig config;
      config.setMode( QgsSnappingConfig::AllLayers );
      config.setTypeFlag( QgsSnappingConfig::VertexFlag | QgsSnappingConfig::SegmentFlag );
      config.setTolerance( 20.0 * InputUtils::calculateDpRatio() );
      config.setUnits( QgsTolerance::Pixels );
      config.setEnabled( true );

      mSnappingUtils.setConfig( config );
      mSnappingUtils.setEnableSnappingForInvisibleFeature( false );
      break;
    }
    case 2:
    {
      QgsSnappingConfig config = mQgsProject->snappingConfig();
      if ( config.units() == QgsTolerance::Pixels )
      {
        config.setTolerance( config.tolerance() * InputUtils::calculateDpRatio() );
      }
      mSnappingUtils.setConfig( config );
      break;
    }
  }
  mSnappingUtils.setIndexingStrategy( QgsSnappingUtils::IndexExtent );
}
