/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "internalpositionprovider.h"
#include "coreutils.h"

#include "qgis.h"

#include "qdebug.h" //TODO: remove

InternalPositionProvider::InternalPositionProvider( QObject *parent ) : AbstractPositionProvider( parent )
{
  mGpsPositionSource = std::unique_ptr<QGeoPositionInfoSource>( QGeoPositionInfoSource::createDefaultSource( nullptr ) );
  if ( !mGpsPositionSource.get() || mGpsPositionSource->error() != QGeoPositionInfoSource::NoError )
  {
    CoreUtils::log(
      QStringLiteral( "Internal GPS provider" ),
      QStringLiteral( "Unable to create default GPS position source, error: %1" ).arg( mGpsPositionSource->error() )
    );

    mIsValid = false;
  }

  mGpsSatellitesSource = std::unique_ptr<QGeoSatelliteInfoSource>( QGeoSatelliteInfoSource::createDefaultSource( nullptr ) );
  if ( !mGpsSatellitesSource.get() || mGpsSatellitesSource->error() != QGeoSatelliteInfoSource::NoError )
  {
    CoreUtils::log(
      QStringLiteral( "Internal GPS provider" ),
      QStringLiteral( "Unable to create default GPS satellite source, error: %1" ).arg( mGpsSatellitesSource->error() )
    );

    mIsValid = false;
  }

  if ( mIsValid )
  {
    //TODO: maybe set a minimal timeout (e.g. 500 ms)?

    connect( mGpsPositionSource.get(), &QGeoPositionInfoSource::positionUpdated, this, &InternalPositionProvider::parsePositionUpdate );
    connect( mGpsPositionSource.get(), QOverload<QGeoPositionInfoSource::Error>::of( &QGeoPositionInfoSource::error ),
             [ = ]( QGeoPositionInfoSource::Error positioningError )
    {
      CoreUtils::log( QStringLiteral( "Internal GPS provider" ), QStringLiteral( "Error occured (position source), code: %1" ).arg( positioningError ) );
      qDebug() << positioningError << " <- has occured during initialization of internal GPS position provider!"; // TODO: remove
      emit lostConnection();
    } );
    connect( mGpsPositionSource.get(), &QGeoPositionInfoSource::updateTimeout, [ = ]()
    {
      CoreUtils::log( QStringLiteral( "Internal GPS provider" ), QStringLiteral( "Stopped receiving position data" ) );
      qDebug() << " Internal GPS (position) stopped receiving data!"; // TODO: remove
      emit lostConnection();
    } );

    connect( mGpsSatellitesSource.get(), &QGeoSatelliteInfoSource::satellitesInViewUpdated, this, &InternalPositionProvider::parseVisibleSatellitesUpdate );
    connect( mGpsSatellitesSource.get(), &QGeoSatelliteInfoSource::satellitesInUseUpdated, this, &InternalPositionProvider::parseUsedSatellitesUpdate );
    connect( mGpsSatellitesSource.get(), QOverload<QGeoSatelliteInfoSource::Error>::of( &QGeoSatelliteInfoSource::error ),
             [ = ]( QGeoSatelliteInfoSource::Error satelliteError )
    {
      CoreUtils::log( QStringLiteral( "Internal GPS provider" ), QStringLiteral( "Error occured (satellites source), code: %1" ).arg( satelliteError ) );
      qDebug() << satelliteError << " <- has occured during initialization of internal GPS satellites provider!"; // TODO: remove
      emit lostConnection();
    } );
    connect( mGpsSatellitesSource.get(), &QGeoSatelliteInfoSource::requestTimeout, [ = ]()
    {
      CoreUtils::log( QStringLiteral( "Internal GPS provider" ), QStringLiteral( "Stopped receiving satellites data" ) );
      qDebug() << " Internal GPS (satellite) stopped receiving data!"; // TODO: remove
      emit lostConnection();
    } );
  }
}

InternalPositionProvider::~InternalPositionProvider() = default;

void InternalPositionProvider::startUpdates()
{
  if ( mIsValid )
  {
    mGpsPositionSource->startUpdates();
    mGpsSatellitesSource->startUpdates();
  }
}

void InternalPositionProvider::stopUpdates()
{
  if ( mIsValid )
  {
    mGpsPositionSource->stopUpdates();
    mGpsSatellitesSource->stopUpdates();
  }
}

void InternalPositionProvider::parsePositionUpdate( const QGeoPositionInfo &position )
{
  bool hasPosition = position.coordinate().isValid();
  if ( !hasPosition )
  {
    emit lostConnection();
    return;
  }

  // go over attributes and find if there are any changes from previous position, emit position update if so
  bool positionDataHasChanged = false;

  if ( !qgsDoubleNear( position.coordinate().latitude(), mLastPosition.latitude ) )
  {
    mLastPosition.latitude = position.coordinate().latitude();
    positionDataHasChanged = true;
  }

  if ( !qgsDoubleNear( position.coordinate().longitude(), mLastPosition.longitude ) )
  {
    mLastPosition.longitude = position.coordinate().longitude();
    positionDataHasChanged = true;
  }

  if ( !qgsDoubleNear( position.coordinate().altitude(), mLastPosition.elevation ) )
  {
    mLastPosition.elevation = position.coordinate().altitude();
    positionDataHasChanged = true;
  }

  bool hasSpeedInfo = position.hasAttribute( QGeoPositionInfo::GroundSpeed );
  if ( hasSpeedInfo && !qgsDoubleNear( position.attribute( QGeoPositionInfo::GroundSpeed ), mLastPosition.speed ) )
  {
    mLastPosition.speed = position.attribute( QGeoPositionInfo::GroundSpeed ) * 3.6; // convert from m/s to km/h
    positionDataHasChanged = true;
  }

  bool hasDirectionInfo = position.hasAttribute( QGeoPositionInfo::Direction );
  if ( hasDirectionInfo && !qgsDoubleNear( position.attribute( QGeoPositionInfo::Direction ), mLastPosition.direction ) )
  {
    mLastPosition.direction = position.attribute( QGeoPositionInfo::Direction );
    positionDataHasChanged = true;
  }

  bool hasHacc = position.hasAttribute( QGeoPositionInfo::HorizontalAccuracy );
  if ( hasHacc && !qgsDoubleNear( position.attribute( QGeoPositionInfo::HorizontalAccuracy ), mLastPosition.hacc ) )
  {
    mLastPosition.hacc = position.attribute( QGeoPositionInfo::HorizontalAccuracy );
    positionDataHasChanged = true;
  }

  bool hasVacc = position.hasAttribute( QGeoPositionInfo::VerticalAccuracy );
  if ( hasVacc && !qgsDoubleNear( position.attribute( QGeoPositionInfo::VerticalAccuracy ), mLastPosition.vacc ) )
  {
    mLastPosition.vacc = position.attribute( QGeoPositionInfo::VerticalAccuracy );
    positionDataHasChanged = true;
  }

  if ( position.timestamp() != mLastPosition.utcDateTime )
  {
    mLastPosition.utcDateTime = position.timestamp();
    positionDataHasChanged = true;
  }

  if ( positionDataHasChanged )
    emit positionChanged( mLastPosition );
}

void InternalPositionProvider::parseVisibleSatellitesUpdate( const QList<QGeoSatelliteInfo> &satellites )
{
  int visibleSatellites = satellites.count();

  if ( mLastPosition.satellitesVisible != visibleSatellites )
  {
    mLastPosition.satellitesVisible = visibleSatellites;
    emit positionChanged( mLastPosition );
  }
}

void InternalPositionProvider::parseUsedSatellitesUpdate( const QList<QGeoSatelliteInfo> &satellites )
{
  int usedSatellites = satellites.count();

  if ( mLastPosition.satellitesUsed != usedSatellites )
  {
    mLastPosition.satellitesUsed = usedSatellites;
    emit positionChanged( mLastPosition );
  }
}
