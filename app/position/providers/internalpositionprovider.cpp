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

//   bool positionOutsideGeoidModelArea = false;
// #ifdef Q_OS_IOS
//   // on ios we can get both ellipsoid and geoid altitude, depending on what is available we transform the altitude or not
//   // we also check if the user set vertical CRS pass through in plugin, which prohibits any transformation
//   bool valueRead = false;
//   const bool isVerticalCRSPassedThrough = QgsProject::instance()->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "VerticalCRSPassThrough" ), true, &valueRead );
//   const bool isEllipsoidalAltitude = localPosition.attribute( QGeoPositionInfo::VerticalSpeed );
//   localPosition.removeAttribute( QGeoPositionInfo::VerticalSpeed );
//   const bool isMockedLocation = localPosition.attribute( QGeoPositionInfo::MagneticVariation );
//   mLastPosition.isMock = isMockedLocation;
//   localPosition.removeAttribute( QGeoPositionInfo::MagneticVariation );
//
//   QgsPoint geoidPosition;
//
//   // transform the altitude from EPSG:4979 (WGS84 (EPSG:4326) + ellipsoidal height) to specified geoid model
//   // (by default EPSG:9707 (WGS84 + EGM96))
//   // we do the transformation only in case the position is not mocked, and it's ellipsoidal altitude
//   // the second variant is when the position is mocked, the altitude is ellipsoidal plus pass through is not enabled
//   const bool isInternalProviderEllipsoidAltitude = !isMockedLocation && isEllipsoidalAltitude;
//   const bool isMockedProviderEllipsoidAltitude = isMockedLocation && isEllipsoidalAltitude && valueRead;
//
//   if ( isInternalProviderEllipsoidAltitude || ( isMockedProviderEllipsoidAltitude && !isVerticalCRSPassedThrough ) )
//   {
//     geoidPosition = InputUtils::transformPoint(
//                       PositionKit::positionCrs3DEllipsoidHeight(),
//                       PositionKit::positionCrs3D(),
//                       QgsProject::instance()->transformContext(),
//     {localPosition.coordinate().longitude(), localPosition.coordinate().latitude(), localPosition.coordinate().altitude()},
//     positionOutsideGeoidModelArea );
//   }
//   // everything else gets propagated as received
//   else
//   {
//     geoidPosition = {localPosition.coordinate().longitude(), localPosition.coordinate().latitude(), localPosition.coordinate().altitude()};
//   }
// #elif defined (ANDROID)
//   // transform the altitude from EPSG:4979 (WGS84 (EPSG:4326) + ellipsoidal height) to specified geoid model
//   // (by default EPSG:9707 (WGS84 + EGM96))
//   const QgsPoint geoidPosition = InputUtils::transformPoint(
//                                    PositionKit::positionCrs3DEllipsoidHeight(),
//                                    PositionKit::positionCrs3D(),
//                                    QgsProject::instance()->transformContext(),
//   {localPosition.coordinate().longitude(), localPosition.coordinate().latitude(), localPosition.coordinate().altitude()},
//   positionOutsideGeoidModelArea );
// #else
//   const QgsPoint geoidPosition = {localPosition.coordinate().longitude(), localPosition.coordinate().latitude(), localPosition.coordinate().altitude()};
// #endif
//   if ( !positionOutsideGeoidModelArea )
//   {
//     if ( !qgsDoubleNear( geoidPosition.z(), mLastPosition.elevation ) )
//     {
//       mLastPosition.elevation = geoidPosition.z();
//       positionDataHasChanged = true;
//     }
//
//     // QGeoCoordinate::altitude() docs claim that it is above the sea level (i.e. geoid) altitude,
//     // but that's not really true in our case:
//     // - on Android - it is MSL altitude only if "useMslAltitude" parameter is passed to the Android
//     //   Qt positioning plugin, which we don't do - see https://doc.qt.io/qt-6/position-plugin-android.html
//     // - on iOS - it would return MSL altitude, but we have a custom patch in vcpkg to return
//     //   ellipsoid altitude, if it's available (so we do not rely on geoid model of unknown quality/resolution),
//     //   or we get orthometric altitude from mocked location, but the altitude separation is unknown
//     // - on Windows - it returns MSL altitude, which we pass along, but the altitude separation is unknown
// #ifdef Q_OS_IOS
//     if ( isEllipsoidalAltitude && !isVerticalCRSPassedThrough )
//     {
// #endif
//       const double ellipsoidAltitude = localPosition.coordinate().altitude();
//       const double geoidSeparation = ellipsoidAltitude - geoidPosition.z();
//       if ( !qgsDoubleNear( geoidSeparation, mLastPosition.elevation_diff ) )
//       {
//         mLastPosition.elevation_diff = geoidSeparation;
//         positionDataHasChanged = true;
//       }
// #ifdef Q_OS_IOS
//     }
// #endif
//   }

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
