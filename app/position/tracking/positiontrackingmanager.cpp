/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positiontrackingmanager.h"

#include "position/tracking/simulatedtrackingbackend.h"
#include "position/positionkit.h"
#include "inputmapsettings.h"

#include "qgslinestring.h"

#include "inpututils.h"

PositionTrackingManager::PositionTrackingManager( QObject *parent )
  : QObject{parent}
{
  // build track line
  QgsLineString *line = new QgsLineString();
  mTrackedGeometry.set( line );
}

void PositionTrackingManager::addPoint( GeoPosition position )
{
  if ( !mMapSettings )
    return;

  QgsPoint toAdd( position.longitude, position.latitude, position.elevation, QDateTime::currentDateTime().toSecsSinceEpoch() );
  toAdd = InputUtils::transformPoint( QgsCoordinateReferenceSystem::fromEpsgId( 4326 ), mMapSettings->destinationCrs(), QgsCoordinateTransformContext(), toAdd );

  if ( toAdd.isEmpty() )
    return;

  int pointsCount = mTrackedGeometry.constGet()->vertexCount();

  mTrackedGeometry.get()->insertVertex( QgsVertexId( 0, 0, pointsCount ), toAdd );

  emit trackedGeometryChanged( mTrackedGeometry );
}

void PositionTrackingManager::setup()
{
  if ( !mMapSettings || !mPositionKit )
  {
    return;
  }

  mTrackingBackend = std::unique_ptr<AbstractTrackingBackend>( constructTrackingBackend( mPositionKit ) );
  connect( mTrackingBackend.get(), &AbstractTrackingBackend::positionChanged, this, &PositionTrackingManager::addPoint );
}

AbstractTrackingBackend *PositionTrackingManager::constructTrackingBackend( PositionKit *positionKit )
{
  AbstractTrackingBackend *positionBackend = nullptr;

  if ( positionKit && positionKit->positionProvider() )
  {
    positionBackend = new SimulatedTrackingBackend( positionKit->positionProvider(), AbstractTrackingBackend::Often );
  }
  else
  {
    positionBackend = new SimulatedTrackingBackend( AbstractTrackingBackend::Often );
  }

  QQmlEngine::setObjectOwnership( positionBackend, QQmlEngine::CppOwnership );

  return positionBackend;
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

  setup();
}

QgsGeometry PositionTrackingManager::trackedGeometry() const
{
  return mTrackedGeometry;
}

PositionKit *PositionTrackingManager::positionKit() const
{
  return mPositionKit;
}

void PositionTrackingManager::setPositionKit( PositionKit *newPositionKit )
{
  if ( mPositionKit == newPositionKit )
    return;
  mPositionKit = newPositionKit;
  emit positionKitChanged( mPositionKit );

  setup();
}
