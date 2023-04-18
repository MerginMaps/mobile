/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positiontrackingmanager.h"

#include "internaltrackingbackend.h"
#include "positionkit.h"
#include "inputmapsettings.h"
#include "qgsproject.h"
#include "qgslinestring.h"

#include "inpututils.h"
#include "coreutils.h"

PositionTrackingManager::PositionTrackingManager( QObject *parent )
  : QObject{parent}
{
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
  if ( !mMapSettings || !mTrackingBackend || !mQgsProject )
  {
    return;
  }

  //
  // find the tracking layer
  //

//  QString trackingLayerId = mQgsProject->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), QStringLiteral() );
  QString trackingLayerId = "tracking_layer_0942e7ff_965d_4c4f_bf35_b51048dcd4ca"; // MOCK!

  if ( trackingLayerId.isEmpty() )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Could not find tracking layer for the project" ) );
    return;
  }

  QgsVectorLayer *trackingLayer = mQgsProject->mapLayer<QgsVectorLayer *>( trackingLayerId );

  if ( !trackingLayer || !trackingLayer->isValid() )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Tracking layer not found or invalid" ) );
    return;
  }

  setLayer( trackingLayer );

  //
  // build track line
  //

  QgsLineString *line = new QgsLineString();
  mTrackedGeometry.set( line );

  connect( mTrackingBackend.get(), &AbstractTrackingBackend::positionChanged, this, &PositionTrackingManager::addPoint );
}

AbstractTrackingBackend *PositionTrackingManager::constructTrackingBackend( QgsProject *project, PositionKit *positionKit )
{
  AbstractTrackingBackend *positionBackend = nullptr;
  AbstractTrackingBackend::UpdateFrequency frequency = AbstractTrackingBackend::Normal;

  if ( project )
  {
    int readFrequency = project->readNumEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/UpdateFrequency" ), 0 );

    if ( readFrequency == 0 )
    {
      frequency = AbstractTrackingBackend::Often;
    }
    else if ( readFrequency == 1 )
    {
      frequency = AbstractTrackingBackend::Normal;
    }
    else
    {
      frequency = AbstractTrackingBackend::Occasional;
    }
  }

  QString platform = InputUtils::appPlatform();
  if ( platform == QStringLiteral( "android" ) )
  {
    // TODO: android provider
  }
  else if ( platform == QStringLiteral( "ios" ) )
  {
    // TODO: iOS provider
  }
  else
  {
    // desktop
    if ( positionKit )
    {
      positionBackend = new InternalTrackingBackend( positionKit, frequency );
    }
    else
    {
      CoreUtils::log(
        QStringLiteral( "Position tracking" ),
        QStringLiteral( "Requested tracking on desktop, but no position kit was provided" )
      );
    }
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

AbstractTrackingBackend *PositionTrackingManager::trackingBackend() const
{
  return mTrackingBackend.get();
}

void PositionTrackingManager::setTrackingBackend( AbstractTrackingBackend *newTrackingBackend )
{
  if ( mTrackingBackend )
  {
    disconnect( mTrackingBackend.get() );
  }

  mTrackingBackend.reset( newTrackingBackend );

//  if ( mTrackingBackend )
//  {
//    connect( mTrackingBackend.get(), &AbstractTrackingBackend::positionChanged, this, &PositionTrackingManager::addPoint );
//  }

  emit trackingBackendChanged( mTrackingBackend.get() );

  setup();
}

QgsProject *PositionTrackingManager::qgsProject() const
{
  return mQgsProject;
}

void PositionTrackingManager::setQgsProject( QgsProject *newQgsProject )
{
  if ( mQgsProject == newQgsProject )
    return;
  mQgsProject = newQgsProject;
  emit qgsProjectChanged( mQgsProject );

  setup();
}
