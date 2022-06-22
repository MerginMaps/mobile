/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "snaputils.h"

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
  if ( mMapSettings == newMapSettings )
    return;

  if ( mMapSettings )
  {
    disconnect( mMapSettings, nullptr, this, nullptr );
  }

  mMapSettings = newMapSettings;

  connect( mMapSettings, &QgsQuickMapSettings::extentChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::mapUnitsPerPixelChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::outputSizeChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::outputDpiChanged, this, &SnapUtils::setup );

  mSnappingUtils.setMapSettings( mMapSettings->mapSettings() );
  emit mapSettingsChanged( mMapSettings );
}

void SnapUtils::getsnap( QPointF mapPoint )
{
  QgsPoint mapCoords = mMapSettings->screenToCoordinate( mapPoint );
  QPointF snappoint = mCenterPosition; // by default show crosshair in center, no snap

  // do no snap in the streaming mode
  if ( !mUseSnapping )
  {
    setSnappedPosition( mCenterPosition );
    setSnapped( false );
    return;
  }

  QgsPointLocator::Match snap = mSnappingUtils.snapToMap( QgsPointXY( mapCoords.x(), mapCoords.y() ) );
  if ( snap.isValid() )
  {
    snappoint = mMapSettings->coordinateToScreen( QgsPoint( snap.point().x(), snap.point().y() ) );
    setSnappedPosition( snappoint );

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

void SnapUtils::setup()
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

QPointF SnapUtils::snappedPosition() const
{
  return mSnappedPosition;
}

void SnapUtils::setSnappedPosition( QPointF newSnappedPosition )
{
  if ( mSnappedPosition == newSnappedPosition )
    return;
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
      config.setTolerance( 20.0 );
      config.setTolerance( QgsTolerance::Pixels );
      config.setEnabled( true );

      mSnappingUtils.setConfig( config );
      mSnappingUtils.setEnableSnappingForInvisibleFeature( false );
      break;
    }
    case 2:
    {
      mSnappingUtils.setConfig( mQgsProject->snappingConfig() );
      break;
    }
  }
  mSnappingUtils.setIndexingStrategy( QgsSnappingUtils::IndexExtent );
}
