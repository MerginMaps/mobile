/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positiontrackingmanager.h"

#include "inputmapsettings.h"

PositionTrackingManager::PositionTrackingManager( QObject *parent )
  : QObject{parent}
{

}

QgsVectorLayer *PositionTrackingManager::layer() const
{
  return mLayer;
}

void PositionTrackingManager::setLayer( QgsVectorLayer *newLayer )
{
  if ( mLayer == newLayer )
    return;
  mLayer = newLayer;
  emit layerChanged( mLayer );
}

InputMapSettings *PositionTrackingManager::mapSettings() const
{
  return mMapSettings;
}

void PositionTrackingManager::setMapSettings( InputMapSettings *newMapSettings )
{
  if ( mMapSettings == newMapSettings )
    return;
  mMapSettings = newMapSettings;
  emit mapSettingsChanged( mMapSettings );
}

QgsGeometry PositionTrackingManager::trackedGeometry() const
{
  return mTrackedGeometry;
}

AbstractTrackingBackend *PositionTrackingManager::constructTrackingBackend()
{

}
