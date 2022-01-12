/***************************************************************************
  positionkit.cpp
  --------------------------------------
  Date                 : Dec. 2017
  Copyright            : (C) 2017 Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positionkit.h"
#include "coreutils.h"

#include "qgis.h"

PositionKit::PositionKit( QObject *parent )
  : QObject( parent )
{
  // TODO: try to load last used provider
}

QgsCoordinateReferenceSystem PositionKit::positionCRS() const
{
  return QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
}

void PositionKit::startUpdates()
{
  if ( mPositionProvider )
    mPositionProvider->startUpdates();
}

void PositionKit::stopUpdates()
{
  if ( mPositionProvider )
    mPositionProvider->stopUpdates();
}

void PositionKit::setPositionProvider( AbstractPositionProvider *provider )
{
  if ( mPositionProvider.get() == provider )
    return;

  if ( mPositionProvider )
    mPositionProvider->disconnect();

  mPositionProvider.reset( provider );

  if ( mPositionProvider )
  {
    connect( mPositionProvider.get(), &AbstractPositionProvider::positionChanged, this, &PositionKit::parsePositionUpdate );
    connect( mPositionProvider.get(), &AbstractPositionProvider::lostConnection, this, &PositionKit::lostConnection );
  }
}

void PositionKit::parsePositionUpdate( const GeoPosition &newPosition )
{
  bool hasAnythingChanged = false;

  if ( !qgsDoubleNear( newPosition.latitude, mPosition.latitude ) )
  {
    mPosition.latitude = newPosition.latitude;
    emit latitudeChanged( mPosition.latitude );
    emit positionCoordinateChanged( positionCoordinate() );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.longitude, mPosition.longitude ) )
  {
    mPosition.longitude = newPosition.longitude;
    emit longitudeChanged( mPosition.longitude );
    emit positionCoordinateChanged( positionCoordinate() );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.elevation, mPosition.elevation ) )
  {
    mPosition.elevation = newPosition.elevation;
    emit altitudeChanged( mPosition.elevation );
    hasAnythingChanged = true;
  }

  if ( newPosition.hasValidPosition() != mHasPosition )
  {
    mHasPosition = newPosition.hasValidPosition();
    emit hasPositionChanged( mHasPosition );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.hacc, mPosition.hacc ) )
  {
    mPosition.hacc = newPosition.hacc;
    emit horizontalAccuracyChanged( mPosition.hacc );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.vacc, mPosition.vacc ) )
  {
    mPosition.vacc = newPosition.vacc;
    emit verticalAccuracyChanged( mPosition.vacc );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.speed, mPosition.speed ) )
  {
    mPosition.speed = newPosition.speed;
    emit speedChanged( mPosition.speed );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.verticalSpeed, mPosition.verticalSpeed ) )
  {
    mPosition.verticalSpeed = newPosition.verticalSpeed;
    emit verticalSpeedChanged( mPosition.verticalSpeed );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.direction, mPosition.direction ) )
  {
    mPosition.direction = newPosition.direction;
    emit directionChanged( mPosition.direction );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.magneticVariation, mPosition.magneticVariation ) )
  {
    mPosition.magneticVariation = newPosition.magneticVariation;
    emit magneticVariationChanged( mPosition.magneticVariation );
    hasAnythingChanged = true;
  }

  if ( newPosition.satellitesVisible != mPosition.satellitesVisible )
  {
    mPosition.satellitesVisible = newPosition.satellitesVisible;
    emit satellitesVisibleChanged( mPosition.satellitesVisible );
    hasAnythingChanged = true;
  }

  if ( newPosition.satellitesUsed != mPosition.satellitesUsed )
  {
    mPosition.satellitesUsed = newPosition.satellitesUsed;
    emit satellitesUsedChanged( mPosition.satellitesUsed );
    hasAnythingChanged = true;
  }

  if ( !qgsDoubleNear( newPosition.hdop, mPosition.hdop ) )
  {
    mPosition.hdop = newPosition.hdop;
    emit hdopChanged( mPosition.hdop );
    hasAnythingChanged = true;
  }

  if ( newPosition.utcDateTime != mPosition.utcDateTime )
  {
    mPosition.utcDateTime = newPosition.utcDateTime;
    emit lastReadChanged( mPosition.utcDateTime );
    hasAnythingChanged = true;
  }

  if ( hasAnythingChanged )
  {
    emit positionChanged( mPosition );
  }
}

double PositionKit::latitude() const
{
  return mPosition.latitude;
}

double PositionKit::longitude() const
{
  return mPosition.longitude;
}

double PositionKit::altitude() const
{
  return mPosition.elevation;
}

QgsPoint PositionKit::positionCoordinate() const
{
  if ( mPosition.latitude < 0 || mPosition.longitude < 0 )
    return QgsPoint();

  return QgsPoint( mPosition.latitude, mPosition.longitude, mPosition.elevation );
}

bool PositionKit::hasPosition() const
{
  return mHasPosition;
}

double PositionKit::horizontalAccuracy() const
{
  return mPosition.hacc;
}

double PositionKit::verticalAccuracy() const
{
  return mPosition.vacc;
}

double PositionKit::direction() const
{
  return mPosition.direction;
}

double PositionKit::magneticVariation() const
{
  return mPosition.magneticVariation;
}

double PositionKit::speed() const
{
  return mPosition.speed;
}

double PositionKit::verticalSpeed() const
{
  return mPosition.verticalSpeed;
}

const QDateTime &PositionKit::lastRead() const
{
  return mPosition.utcDateTime;
}

int PositionKit::satellitesUsed() const
{
  return mPosition.satellitesUsed;
}

int PositionKit::satellitesVisible() const
{
  return mPosition.satellitesVisible;
}

double PositionKit::hdop() const
{
  return mPosition.hdop;
}

AbstractPositionProvider *PositionKit::positionProvider() const
{
  return mPositionProvider.get();
}

const GeoPosition &PositionKit::position() const
{
  return mPosition;
}
