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
  if ( mQgsProject == newQgsProject )
    return;
  mQgsProject = newQgsProject;
  emit qgsProjectChanged( mQgsProject );

  mSnappingUtils.setConfig( mQgsProject->snappingConfig() );
}

QgsQuickMapSettings *SnapUtils::mapSettings() const
{
  return mMapSettings;
}

void SnapUtils::setMapSettings( QgsQuickMapSettings *newMapSettings )
{
  if ( mMapSettings == newMapSettings )
    return;

  // TODO: disconnect from previous map settings! (project reload)

  mMapSettings = newMapSettings;
  emit mapSettingsChanged( mMapSettings );

  connect( mMapSettings, &QgsQuickMapSettings::extentChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::mapUnitsPerPixelChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::outputSizeChanged, this, &SnapUtils::setup );
  connect( mMapSettings, &QgsQuickMapSettings::outputDpiChanged, this, &SnapUtils::setup );

  mSnappingUtils.setMapSettings( mMapSettings->mapSettings() );
}

void SnapUtils::getsnap( QPoint mapPoint )
{
  QgsPoint mapCoords = mMapSettings->screenToCoordinate( mapPoint );
  QPoint snappoint = mCenterPosition; // by default show crosshair in center, no snap

  QgsPointLocator::Match snap = mSnappingUtils.snapToMap( QgsPointXY( mapCoords.x(), mapCoords.y() ) );
  if ( snap.isValid() )
  {
    snappoint = mMapSettings->coordinateToScreen( QgsPoint( snap.point().x(), snap.point().y() ) ).toPoint();
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
  mSnappingUtils.setConfig( mQgsProject->snappingConfig() );

  getsnap( mCenterPosition );
}

QPoint SnapUtils::centerPosition() const
{
  return mCenterPosition;
}

void SnapUtils::setCenterPosition( QPoint newCenterPosition )
{
  if ( mCenterPosition == newCenterPosition )
    return;
  mCenterPosition = newCenterPosition;
  emit centerPositionChanged( mCenterPosition );
}

QPoint SnapUtils::snappedPosition() const
{
  return mSnappedPosition;
}

void SnapUtils::setSnappedPosition( QPoint newSnappedPosition )
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
