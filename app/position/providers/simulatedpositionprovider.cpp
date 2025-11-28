/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "simulatedpositionprovider.h"

#include <memory>
#include <QtNumeric>

#include "inpututils.h"
#include "qgspoint.h"

SimulatedPositionProvider::SimulatedPositionProvider( const double longitude, const double latitude, const double flightRadius, const double updateTimeout, QObject *parent )
  : AbstractPositionProvider( QStringLiteral( "simulated" ), QStringLiteral( "internal" ), QStringLiteral( "Simulated provider" ), parent )
  , mTimer( new QTimer() )
  , mLongitude( longitude )
  , mLatitude( latitude )
  , mFlightRadius( flightRadius )
  , mTimerTimeout( updateTimeout )
{
  std::random_device seed;
  mGenerator = std::make_unique<std::mt19937>( seed() );

  connect( mTimer.get(), &QTimer::timeout, this, &SimulatedPositionProvider::generateNextPosition );

  SimulatedPositionProvider::startUpdates();
}

void SimulatedPositionProvider::setUpdateInterval( const double msecs )
{
  stopUpdates();
  mTimerTimeout = msecs;
  startUpdates();
}

SimulatedPositionProvider::~SimulatedPositionProvider() = default;

void SimulatedPositionProvider::startUpdates()
{
  mTimer->start( static_cast<int>( mTimerTimeout ) );
  generateNextPosition();
}

void SimulatedPositionProvider::stopUpdates()
{
  mTimer->stop();
}

void SimulatedPositionProvider::closeProvider()
{
  mTimer->stop();
}

void SimulatedPositionProvider::setPosition( const QgsPoint position )
{
  if ( position.isEmpty() )
    return;

  stopUpdates();
  mLatitude = position.y();
  mLongitude = position.x();
  generateConstantPosition();
}

void SimulatedPositionProvider::generateNextPosition()
{
  setState( tr( "Connected" ), State::Connected );

  if ( mFlightRadius <= 0 )
    generateConstantPosition();
  else
    generateRadiusPosition();
}

void SimulatedPositionProvider::generateRadiusPosition()
{
  double latitude = mLatitude, longitude = mLongitude;
  latitude += sin( mAngle * M_PI / 180 ) * mFlightRadius;
  longitude += cos( mAngle * M_PI / 180 ) * mFlightRadius;
  mAngle += 1;

  GeoPosition position;

  position.latitude = latitude;
  position.longitude = longitude;

  double ellipsoidAltitude = ( *mGenerator )() % 40 + 80; // rand altitude <80,115>m and lost (NaN)
  if ( ellipsoidAltitude <= 115 )
  {
    bool positionOutsideGeoidModelArea = false;
    const QgsPoint geoidPosition = InputUtils::transformPoint(
                                     PositionKit::positionCrs3DEllipsoidHeight(),
                                     PositionKit::positionCrs3D(),
                                     QgsCoordinateTransformContext(),
    {longitude, latitude, ellipsoidAltitude},
    positionOutsideGeoidModelArea);
    if ( !positionOutsideGeoidModelArea )
    {
      position.elevation = geoidPosition.z();
      position.elevation_diff = ellipsoidAltitude - position.elevation;
    }
    else
    {
      position.elevation = qQNaN();
      position.elevation_diff = qQNaN();
    }
  }
  else
  {
    position.elevation = qQNaN();
    position.elevation_diff = qQNaN();
  }

  const QDateTime timestamp = QDateTime::currentDateTime();
  position.utcDateTime = timestamp;

  position.direction = 360 - static_cast<int>( mAngle ) % 360;

  int accuracy = ( *mGenerator )() % 40; // rand accuracy <0,35>m and lost (-1)
  if ( accuracy > 35 )
  {
    accuracy = -1;
  }
  position.hacc = accuracy;

  position.satellitesUsed = ( *mGenerator )() % 30;
  position.satellitesVisible = ( *mGenerator )() % 30;

  position.speed = ( *mGenerator )() % 50 - ( ( ( *mGenerator )() % 10 ) / 10. ); // e.g. 45 - 3 / 10 = 44.7 (km/h)

  emit positionChanged( position );
}

void SimulatedPositionProvider::generateConstantPosition()
{
  GeoPosition position;
  position.latitude = mLatitude;
  position.longitude = mLongitude;
  // we take 100 as elevation returned by WGS84 ellipsoid and recalculate it to geoid
  bool positionOutsideGeoidModelArea = false;
  const QgsPoint geoidPosition = InputUtils::transformPoint(
                                   PositionKit::positionCrs3DEllipsoidHeight(),
                                   PositionKit::positionCrs3D(),
                                   QgsCoordinateTransformContext(),
  {mLongitude, mLatitude, 100},
  positionOutsideGeoidModelArea);
  if ( !positionOutsideGeoidModelArea )
  {
    position.elevation = geoidPosition.z();
    position.elevation_diff = 100 - position.elevation;
  }
  else
  {
    position.elevation = qQNaN();
    position.elevation_diff = qQNaN();
  }
  position.utcDateTime = QDateTime::currentDateTime();
  position.direction = 360 - static_cast<int>( mAngle ) % 360;
  position.hacc = ( *mGenerator )() % 20;
  position.satellitesUsed = ( *mGenerator )() % 30;
  position.satellitesVisible = ( *mGenerator )() % 30;
  position.speed = ( *mGenerator )() % 50 - ( ( ( *mGenerator )() % 10 ) / 10. );

  emit positionChanged( position );
}
