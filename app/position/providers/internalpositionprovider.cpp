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

InternalPositionProvider::InternalPositionProvider( QObject *parent )
  : AbstractPositionProvider( QStringLiteral( "devicegps" ), QStringLiteral( "internal" ), tr( "Internal" ), parent )
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

  bool hasPosition = position.coordinate().isValid();
  if ( !hasPosition )
  {
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

  bool positionOutsideGeoidModelArea = false;
#ifdef Q_OS_IOS
  // on ios we can get both ellipsoid and geoid altitude, depending on what is available (e.g. with mocked location we might
  // not be able to get the ellipsoid altitude only the geoid) we transform the altitude or not
  double isTransformRequired = position.attribute( QGeoPositionInfo::VerticalSpeed );
  position.removeAttribute( QGeoPositionInfo::VerticalSpeed );
  QgsPoint geoidPosition;
  if ( isTransformRequired )
  {
    // transform the altitude from EPSG:4979 (WGS84 (EPSG:4326) + ellipsoidal height) to specified geoid model
    geoidPosition = InputUtils::transformPoint(
                      PositionKit::positionCrs3DEllipsoidHeight(),
                      PositionKit::positionCrs3D(),
                      QgsProject::instance()->transformContext(),
    {position.coordinate().longitude(), position.coordinate().latitude(), position.coordinate().altitude()},
    positionOutsideGeoidModelArea );
  }
  else
  {
    geoidPosition = {position.coordinate().longitude(), position.coordinate().latitude(), position.coordinate().altitude()};
  }
#else
  // transform the altitude from EPSG:4979 (WGS84 (EPSG:4326) + ellipsoidal height) to specified geoid model
  const QgsPoint geoidPosition = InputUtils::transformPoint(
                                   PositionKit::positionCrs3DEllipsoidHeight(),
                                   PositionKit::positionCrs3D(),
                                   QgsProject::instance()->transformContext(),
  {position.coordinate().longitude(), position.coordinate().latitude(), position.coordinate().altitude()},
  positionOutsideGeoidModelArea );
#endif
  if ( !positionOutsideGeoidModelArea )
  {
    if ( !qgsDoubleNear( geoidPosition.z(), mLastPosition.elevation ) )
    {
      mLastPosition.elevation = geoidPosition.z();
      positionDataHasChanged = true;
    }

    // QGeoCoordinate::altitude() docs claim that it is above the sea level (i.e. geoid) altitude,
    // but that's not really true in our case:
    // - on Android - it is MSL altitude only if "useMslAltitude" parameter is passed to the Android
    //   Qt positioning plugin, which we don't do - see https://doc.qt.io/qt-6/position-plugin-android.html
    // - on iOS - it would return MSL altitude, but we have a custom patch in vcpkg to return
    //   ellipsoid altitude, if it's available (so we do not rely on geoid model of unknown quality/resolution),
    //   or we get orthometric altitude from mocked location, but the altitude separation is unknown
#ifdef Q_OS_IOS
    if ( isTransformRequired )
    {
#endif
      const double ellipsoidAltitude = position.coordinate().altitude();
      const double geoidSeparation = ellipsoidAltitude - geoidPosition.z();
      if ( !qgsDoubleNear( geoidSeparation, mLastPosition.elevation_diff ) )
      {
        mLastPosition.elevation_diff = geoidSeparation;
        positionDataHasChanged = true;
      }
#ifdef Q_OS_IOS
    }
#endif
  }

  bool hasSpeedInfo = position.hasAttribute( QGeoPositionInfo::GroundSpeed );
  if ( hasSpeedInfo && !qgsDoubleNear( position.attribute( QGeoPositionInfo::GroundSpeed ), mLastPosition.speed ) )
  {
    mLastPosition.speed = position.attribute( QGeoPositionInfo::GroundSpeed ) * 3.6; // convert from m/s to km/h
    positionDataHasChanged = true;
  }

  bool hasVerticalSpeedInfo = position.hasAttribute( QGeoPositionInfo::VerticalSpeed );
  if ( hasVerticalSpeedInfo && !qgsDoubleNear( position.attribute( QGeoPositionInfo::VerticalSpeed ), mLastPosition.verticalSpeed ) )
  {
    mLastPosition.verticalSpeed = position.attribute( QGeoPositionInfo::VerticalSpeed ) * 3.6; // convert from m/s to km/h
    positionDataHasChanged = true;
  }

  bool hasDirectionInfo = position.hasAttribute( QGeoPositionInfo::Direction );
  if ( hasDirectionInfo && !qgsDoubleNear( position.attribute( QGeoPositionInfo::Direction ), mLastPosition.direction ) )
  {
    mLastPosition.direction = position.attribute( QGeoPositionInfo::Direction );
    positionDataHasChanged = true;
  }

  bool hasMagneticVariation = position.hasAttribute( QGeoPositionInfo::MagneticVariation );
  if ( hasMagneticVariation && !qgsDoubleNear( position.attribute( QGeoPositionInfo::MagneticVariation ), mLastPosition.magneticVariation ) )
  {
    mLastPosition.magneticVariation = position.attribute( QGeoPositionInfo::MagneticVariation );
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
