/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positiontransformer.h"

#include <coreutils.h>

#include "inpututils.h"

PositionTransformer::PositionTransformer( const QgsCoordinateReferenceSystem &sourceCrs,
    const QgsCoordinateReferenceSystem &destinationCrs, const bool skipElevationTransformation, const QgsCoordinateTransformContext &transformContext, QObject *parent )
  : QObject( parent ),
    mSourceCrs( sourceCrs ),
    mDestinationCrs( destinationCrs ),
    mTransformContext( transformContext ),
    mSkipElevationTransformation( skipElevationTransformation )
{
}

GeoPosition PositionTransformer::processBluetoothPosition( GeoPosition geoPosition )
{
  double incomingHeight = geoPosition.elevation;
  bool willTransform = false;
  bool positionOutsideGeoidModelArea = false;
  if ( !mSkipElevationTransformation && !std::isnan( geoPosition.elevation ) && !std::isnan( geoPosition.elevation_diff ) )
  {
    // The geoid models used in GNSS devices can be often times unreliable, thus we apply the transformations ourselves
    // GNSS supplied orthometric elevation -> ellipsoid elevation -> orthometric elevation based on our model
    const double ellipsoidElevation = geoPosition.elevation + geoPosition.elevation_diff;

    const QgsPoint geoidPosition = InputUtils::transformPoint(
                                     mSourceCrs,
                                     mDestinationCrs,
                                     mTransformContext,
    {geoPosition.longitude, geoPosition.latitude, ellipsoidElevation},
    positionOutsideGeoidModelArea );
    willTransform = true;
    if ( !positionOutsideGeoidModelArea )
    {
      geoPosition.elevation = geoidPosition.z();
      geoPosition.elevation_diff = ellipsoidElevation - geoidPosition.z();
    }
  }

  CoreUtils::log( "Elevation info", QString( "Ellipsoid height: %1, Mocked location: %2, Incoming height: %3, Outgoing height: %4, Separation: %5, Is it transformed: %6, Position outside geoid model: %7" ).arg( std::isnan( geoPosition.elevation_diff ), false, incomingHeight, geoPosition.elevation, geoPosition.elevation_diff, willTransform, positionOutsideGeoidModelArea ) );
  return geoPosition;
}

GeoPosition PositionTransformer::processAndroidPosition( GeoPosition geoPosition )
{
  double incomingElevation = geoPosition.elevation;
  bool willTransform = false;
  if ( geoPosition.elevation != std::numeric_limits<double>::quiet_NaN() )
  {
    bool positionOutsideGeoidModelArea = false;
    if ( !geoPosition.isMock || !mSkipElevationTransformation )
    {
      const QgsPoint geoidPosition = InputUtils::transformPoint(
                                       mSourceCrs,
                                       mDestinationCrs,
                                       mTransformContext,
      {geoPosition.longitude, geoPosition.latitude, geoPosition.elevation},
      positionOutsideGeoidModelArea );
      willTransform = true;
      if ( !positionOutsideGeoidModelArea )
      {
        const double geoidSeparation = geoPosition.elevation - geoidPosition.z();

        geoPosition.elevation = geoidPosition.z();
        geoPosition.elevation_diff = geoidSeparation;
      }
    }
  }

  CoreUtils::log( "Elevation info", QString( "Ellipsoid height: %1, Mocked location: %2, Incoming height: %3, Outgoing height: %4, Separation: %5, Is it transformed: %6, Position outside geoid model: %7" ).arg( true, geoPosition.isMock, incomingElevation, geoPosition.elevation, geoPosition.elevation_diff, willTransform, positionOutsideGeoidModelArea ) );
  return geoPosition;
}

GeoPosition PositionTransformer::processInternalAndroidPosition( const QGeoPositionInfo &geoPosition )
{
  GeoPosition newPosition;
  bool positionOutsideGeoidModelArea = false;
  const QgsPoint geoidPosition = InputUtils::transformPoint(
                                   mSourceCrs,
                                   mDestinationCrs,
                                   mTransformContext,
  {geoPosition.coordinate().longitude(), geoPosition.coordinate().latitude(), geoPosition.coordinate().altitude()},
  positionOutsideGeoidModelArea );

  if ( !positionOutsideGeoidModelArea )
  {
    newPosition.elevation = geoidPosition.z();

    // QGeoCoordinate::altitude() docs claim that it is above the sea level (i.e. geoid) altitude,
    // but that's not really true in our case:
    // - on Android - it is MSL altitude only if "useMslAltitude" parameter is passed to the Android
    //   Qt positioning plugin, which we don't do - see https://doc.qt.io/qt-6/position-plugin-android.html
    const double ellipsoidAltitude = geoPosition.coordinate().altitude();
    const double geoidSeparation = ellipsoidAltitude - geoidPosition.z();
    newPosition.elevation_diff = geoidSeparation;
  }
  CoreUtils::log( "Elevation info", QString( "Ellipsoid height: %1, Mocked location: %2, Incoming height: %3, Outgoing height: %4, Separation: %5, Is it transformed: %6, Position outside geoid model: %7" ).arg( true, QStringLiteral( "¯\_(ツ)_/¯" ), geoPosition.coordinate().altitude(), newPosition.elevation, newPosition.elevation_diff, true, positionOutsideGeoidModelArea ) );
  return newPosition;
}

GeoPosition PositionTransformer::processInternalIosPosition( QGeoPositionInfo &geoPosition )
{
  GeoPosition newPosition;
  bool positionOutsideGeoidModelArea = false;
  // on ios we can get both ellipsoid and geoid altitude, depending on what is available we transform the altitude or not
  // we also check if the user set vertical CRS pass through in plugin, which prohibits any transformation
  const bool isEllipsoidalAltitude = geoPosition.hasAttribute( QGeoPositionInfo::VerticalSpeed );
  geoPosition.removeAttribute( QGeoPositionInfo::VerticalSpeed );
  const bool isMockedLocation = geoPosition.hasAttribute( QGeoPositionInfo::MagneticVariation );
  newPosition.isMock = isMockedLocation;
  geoPosition.removeAttribute( QGeoPositionInfo::MagneticVariation );

  QgsPoint geoidPosition;

  const bool isInternalProviderEllipsoidAltitude = !isMockedLocation && isEllipsoidalAltitude;
  const bool isMockedProviderEllipsoidAltitude = isMockedLocation && isEllipsoidalAltitude;
  bool willTransform = false;
  if ( isInternalProviderEllipsoidAltitude || ( isMockedProviderEllipsoidAltitude && !mSkipElevationTransformation ) )
  {
    geoidPosition = InputUtils::transformPoint(
                      mSourceCrs,
                      mDestinationCrs,
                      mTransformContext,
    {geoPosition.coordinate().longitude(), geoPosition.coordinate().latitude(), geoPosition.coordinate().altitude()},
    positionOutsideGeoidModelArea );
    willTransform = true;
  }
  // everything else gets propagated as received
  else
  {
    geoidPosition =
    {
      geoPosition.coordinate().longitude(),
      geoPosition.coordinate().latitude(),
      geoPosition.coordinate().altitude()
    };
  }

  if ( !positionOutsideGeoidModelArea )
  {
    newPosition.elevation = geoidPosition.z();

    // QGeoCoordinate::altitude() docs claim that it is above the sea level (i.e. geoid) altitude,
    // but that's not really true in our case:
    // - on iOS - it would return MSL altitude, but we have a custom patch in vcpkg to return
    //   ellipsoid altitude, if it's available (so we do not rely on geoid model of unknown quality/resolution),
    //   or we get orthometric altitude from mocked location, but the altitude separation is unknown
    if ( isEllipsoidalAltitude && !mSkipElevationTransformation )
    {
      const double ellipsoidAltitude = geoPosition.coordinate().altitude();
      const double geoidSeparation = ellipsoidAltitude - geoidPosition.z();
      newPosition.elevation_diff = geoidSeparation;
    }
  }

  CoreUtils::log( "Elevation info", QString( "Ellipsoid height: %1, Mocked location: %2, Incoming height: %3, Outgoing height: %4, Separation: %5, Is it transformed: %6, Position outside geoid model: %7" ).arg( isEllipsoidalAltitude, isMockedLocation, geoPosition.coordinate().altitude(), newPosition.elevation, newPosition.elevation_diff, willTransform, positionOutsideGeoidModelArea ) );
  return newPosition;
}

GeoPosition PositionTransformer::processInternalDesktopPosition( const QGeoPositionInfo &geoPosition )
{
  GeoPosition newPosition;

  // QGeoCoordinate::altitude() docs claim that it is above the sea level (i.e. geoid) altitude,
  // but that's not really true in our case:
  // - on Windows - it returns MSL altitude, which we pass along, but the altitude separation is unknown
  newPosition.elevation = geoPosition.coordinate().altitude();

  return newPosition;
}

GeoPosition PositionTransformer::processSimulatedPosition( const GeoPosition &geoPosition )
{
  GeoPosition newPosition;
  bool positionOutsideGeoidModelArea = false;
  const QgsPoint geoidPosition = InputUtils::transformPoint(
                                   mSourceCrs,
                                   mDestinationCrs,
                                   mTransformContext,
  {geoPosition.longitude, geoPosition.latitude, geoPosition.elevation},
  positionOutsideGeoidModelArea );
  if ( !positionOutsideGeoidModelArea )
  {
    newPosition.elevation = geoidPosition.z();
    newPosition.elevation_diff = geoPosition.elevation - newPosition.elevation;
  }

  return newPosition;
}

void PositionTransformer::setSourceCrs( const QgsCoordinateReferenceSystem &sourceCrs )
{
  mSourceCrs = sourceCrs;
}

void PositionTransformer::setDestinationCrs( const QgsCoordinateReferenceSystem &destinationCrs )
{
  mDestinationCrs = destinationCrs;
}

void PositionTransformer::setTransformContext( const QgsCoordinateTransformContext &transformContext )
{
  mTransformContext = transformContext;
}

void PositionTransformer::setSkipElevationTransformation( const bool skipElevationTransformation )
{
  mSkipElevationTransformation = skipElevationTransformation;
}
