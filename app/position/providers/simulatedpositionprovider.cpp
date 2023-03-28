/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "simulatedpositionprovider.h"
#include "qgspoint.h"

SimulatedPositionProvider::SimulatedPositionProvider( double longitude, double latitude, double flightRadius, double timerTimeout, QObject *parent )
  : AbstractPositionProvider( QStringLiteral( "simulated" ), QStringLiteral( "internal" ), QStringLiteral( "Simulated provider" ), parent )
  , mTimer( new QTimer() )
  , mLongitude( longitude )
  , mLatitude( latitude )
  , mFlightRadius( flightRadius )
  , mTimerTimeout( timerTimeout )
{
  std::random_device seed;
  mGenerator = std::unique_ptr<std::mt19937>( new std::mt19937( seed() ) );

  connect( mTimer.get(), &QTimer::timeout, this, &SimulatedPositionProvider::generateNextPosition );

  SimulatedPositionProvider::startUpdates();
}

SimulatedPositionProvider::~SimulatedPositionProvider() = default;

void SimulatedPositionProvider::startUpdates()
{
  mTimer->start( mTimerTimeout );
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

void SimulatedPositionProvider::setPosition( QgsPoint position )
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

  double altitude = ( *mGenerator )() % 40 + 20; // rand altitude <20,55>m and lost (0)
  if ( altitude <= 55 )
  {
    position.elevation = altitude;
  }

  QDateTime timestamp = QDateTime::currentDateTime();
  position.utcDateTime = timestamp;

  position.direction = 360 - int( mAngle ) % 360;

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
  position.elevation = 20;
  position.utcDateTime = QDateTime::currentDateTime();
  position.direction = 360 - int( mAngle ) % 360;
  position.hacc = ( *mGenerator )() % 20;
  position.satellitesUsed = ( *mGenerator )() % 30;
  position.satellitesVisible = ( *mGenerator )() % 30;
  position.speed = ( *mGenerator )() % 50 - ( ( ( *mGenerator )() % 10 ) / 10. );

  emit positionChanged( position );
}
