/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "internalpositionprovider.h"

#include <qgsproject.h>

#include "coreutils.h"
#include "inpututils.h"

InternalPositionProvider::InternalPositionProvider( PositionTransformer &positionTransformer, QObject *parent )
  : AbstractPositionProvider( QStringLiteral( "devicegps" ), QStringLiteral( "internal" ), tr( "Internal" ), positionTransformer, parent )
{
  mGpsPositionSource = std::unique_ptr<QGeoPositionInfoSource>( QGeoPositionInfoSource::createDefaultSource( nullptr ) );

  if ( mGpsPositionSource.get() && mGpsPositionSource->error() == QGeoPositionInfoSource::NoError )
  {
    // let's have an update each second
    mGpsPositionSource->setUpdateInterval( 1000 );

    connect( mGpsPositionSource.get(), &QGeoPositionInfoSource::positionUpdated, this, &InternalPositionProvider::parsePositionUpdate );
    connect( mGpsPositionSource.get(), &QGeoPositionInfoSource::errorOccurred, this, [ = ]( QGeoPositionInfoSource::Error positioningError )
    {
      // Since Qt 6.2 we started to see this error type too often. That consumed the entire diagnostic log.
      // Let's skip logging this error, see https://github.com/MerginMaps/mobile/issues/3029
      if ( positioningError != QGeoPositionInfoSource::UpdateTimeoutError )
      {
        CoreUtils::log( QStringLiteral( "Internal GPS provider" ), QStringLiteral( "Error occured (position source), code: %1" ).arg( positioningError ) );
      }
      else
      {
        qDebug() << "Internal GPS provider error" << QGeoPositionInfoSource::UpdateTimeoutError;
      }
    } );

    mPositionSourceValid = true;
    setState( tr( "Connected" ), State::Connected );
  }
  else
  {
    CoreUtils::log(
      QStringLiteral( "Internal GPS provider" ),
      QStringLiteral( "Unable to create default GPS position source" )
    );

    mPositionSourceValid = false;
  }

  // now the same for satellites source
  mGpsSatellitesSource = std::unique_ptr<QGeoSatelliteInfoSource>( QGeoSatelliteInfoSource::createDefaultSource( nullptr ) );

  if ( mGpsSatellitesSource.get() && mGpsSatellitesSource->error() == QGeoSatelliteInfoSource::NoError )
  {
    connect( mGpsSatellitesSource.get(), &QGeoSatelliteInfoSource::satellitesInViewUpdated, this, &InternalPositionProvider::parseVisibleSatellitesUpdate );
    connect( mGpsSatellitesSource.get(), &QGeoSatelliteInfoSource::satellitesInUseUpdated, this, &InternalPositionProvider::parseUsedSatellitesUpdate );
    connect( mGpsSatellitesSource.get(), &QGeoSatelliteInfoSource::errorOccurred, this, [ = ]( QGeoSatelliteInfoSource::Error satelliteError )
    {
      if ( satelliteError == QGeoSatelliteInfoSource::UpdateTimeoutError )
      {
        CoreUtils::log( QStringLiteral( "Internal GPS provider" ), QStringLiteral( "Stopped receiving satellites data" ) );
      }
      else
      {
        CoreUtils::log( QStringLiteral( "Internal GPS provider" ), QStringLiteral( "Error occured (satellites source), code: %1" ).arg( satelliteError ) );
      }
    } );

    mSatelliteSourceValid = true;
  }
  else
  {
    CoreUtils::log(
      QStringLiteral( "Internal GPS provider" ),
      QStringLiteral( "Unable to create default GPS satellite source" )
    );

    mSatelliteSourceValid = false;
  }

  InternalPositionProvider::startUpdates();
}

InternalPositionProvider::~InternalPositionProvider() = default;

void InternalPositionProvider::startUpdates()
{
  if ( mPositionSourceValid )
    mGpsPositionSource->startUpdates();

  if ( mSatelliteSourceValid )
    mGpsSatellitesSource->startUpdates();
}

void InternalPositionProvider::stopUpdates()
{
  if ( mPositionSourceValid )
    mGpsPositionSource->stopUpdates();

  if ( mSatelliteSourceValid )
    mGpsSatellitesSource->stopUpdates();
}

void InternalPositionProvider::closeProvider()
{
  if ( mPositionSourceValid )
    mGpsPositionSource->stopUpdates();

  if ( mSatelliteSourceValid )
    mGpsSatellitesSource->stopUpdates();
}

void InternalPositionProvider::parsePositionUpdate( const QGeoPositionInfo &position )
{
  // if by any chance we are in wrong state (QML thinking that provider is not connected)
  // emit connected signal here to know that the connection is OK
  setState( tr( "Connected" ), State::Connected );

  // we create a local copy of position because on iOS we use QGeoPositionInfo::VerticalSpeed attribute as helper value
  // for transformation, we need to remove it afterwards
  QGeoPositionInfo localPosition( position );

  const bool hasPosition = localPosition.coordinate().isValid();
  if ( !hasPosition )
  {
    return;
  }

  // go over attributes and find if there are any changes from previous position, emit position update if so
  bool positionDataHasChanged = false;

  if ( !qgsDoubleNear( localPosition.coordinate().latitude(), mLastPosition.latitude ) )
  {
    mLastPosition.latitude = localPosition.coordinate().latitude();
    positionDataHasChanged = true;
  }

  if ( !qgsDoubleNear( localPosition.coordinate().longitude(), mLastPosition.longitude ) )
  {
    mLastPosition.longitude = localPosition.coordinate().longitude();
    positionDataHasChanged = true;
  }

  GeoPosition transformedPosition;
#ifdef Q_OS_IOS
  transformedPosition = mPositionTransformer->processInternalIosPosition( localPosition );
#elif defined (ANDROID)
  transformedPosition = mPositionTransformer->processInternalAndroidPosition( localPosition );
#else
  transformedPosition = mPositionTransformer->processInternalDesktopPosition( localPosition );
#endif

  if ( mLastPosition.isMock != transformedPosition.isMock )
  {
    mLastPosition.isMock = transformedPosition.isMock;
    positionDataHasChanged = true;
  }

  if ( !qgsDoubleNear( transformedPosition.elevation, mLastPosition.elevation ) )
  {
    mLastPosition.elevation = transformedPosition.elevation;
    positionDataHasChanged = true;
  }

  if ( !qgsDoubleNear( transformedPosition.elevation_diff, mLastPosition.elevation_diff ) )
  {
    mLastPosition.elevation_diff = transformedPosition.elevation_diff;
    positionDataHasChanged = true;
  }

  const bool hasSpeedInfo = localPosition.hasAttribute( QGeoPositionInfo::GroundSpeed );
  if ( hasSpeedInfo && !qgsDoubleNear( localPosition.attribute( QGeoPositionInfo::GroundSpeed ), mLastPosition.speed ) )
  {
    mLastPosition.speed = localPosition.attribute( QGeoPositionInfo::GroundSpeed ) * 3.6; // convert from m/s to km/h
    positionDataHasChanged = true;
  }

  const bool hasVerticalSpeedInfo = localPosition.hasAttribute( QGeoPositionInfo::VerticalSpeed );
  if ( hasVerticalSpeedInfo && !qgsDoubleNear( localPosition.attribute( QGeoPositionInfo::VerticalSpeed ), mLastPosition.verticalSpeed ) )
  {
    mLastPosition.verticalSpeed = localPosition.attribute( QGeoPositionInfo::VerticalSpeed ) * 3.6; // convert from m/s to km/h
    positionDataHasChanged = true;
  }

  const bool hasDirectionInfo = localPosition.hasAttribute( QGeoPositionInfo::Direction );
  if ( hasDirectionInfo && !qgsDoubleNear( localPosition.attribute( QGeoPositionInfo::Direction ), mLastPosition.direction ) )
  {
    mLastPosition.direction = localPosition.attribute( QGeoPositionInfo::Direction );
    positionDataHasChanged = true;
  }

  const bool hasMagneticVariation = localPosition.hasAttribute( QGeoPositionInfo::MagneticVariation );
  if ( hasMagneticVariation && !qgsDoubleNear( localPosition.attribute( QGeoPositionInfo::MagneticVariation ), mLastPosition.magneticVariation ) )
  {
    mLastPosition.magneticVariation = localPosition.attribute( QGeoPositionInfo::MagneticVariation );
    positionDataHasChanged = true;
  }

  const bool hasHacc = localPosition.hasAttribute( QGeoPositionInfo::HorizontalAccuracy );
  if ( hasHacc && !qgsDoubleNear( localPosition.attribute( QGeoPositionInfo::HorizontalAccuracy ), mLastPosition.hacc ) )
  {
    mLastPosition.hacc = localPosition.attribute( QGeoPositionInfo::HorizontalAccuracy );
    positionDataHasChanged = true;
  }

  const bool hasVacc = localPosition.hasAttribute( QGeoPositionInfo::VerticalAccuracy );
  if ( hasVacc && !qgsDoubleNear( localPosition.attribute( QGeoPositionInfo::VerticalAccuracy ), mLastPosition.vacc ) )
  {
    mLastPosition.vacc = localPosition.attribute( QGeoPositionInfo::VerticalAccuracy );
    positionDataHasChanged = true;
  }

  if ( localPosition.timestamp() != mLastPosition.utcDateTime )
  {
    mLastPosition.utcDateTime = localPosition.timestamp();
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
