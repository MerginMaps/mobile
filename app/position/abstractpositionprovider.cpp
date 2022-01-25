/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstractpositionprovider.h"
#include "coreutils.h"

AbstractPositionProvider::AbstractPositionProvider( const QString &id, const QString &type, const QString &name, QObject *object )
  : QObject( object )
  , mProviderId( id )
  , mProviderType( type )
  , mProviderName( name )
{
}

void AbstractPositionProvider::reconnect()
{
  CoreUtils::log( QStringLiteral( "PositionProvider" ), QStringLiteral( "Reconnecting provider" ) + mProviderId );
  stopUpdates();
  startUpdates();
}

AbstractPositionProvider::~AbstractPositionProvider() = default;

QString AbstractPositionProvider::name() const
{
  return mProviderName;
}

QString AbstractPositionProvider::statusString() const
{
  return mStatusString;
}

bool AbstractPositionProvider::hasError() const
{
  return mHasError;
}

QString AbstractPositionProvider::id() const
{
  return mProviderId;
}

QString AbstractPositionProvider::type() const
{
  return mProviderType;
}

void AbstractPositionProvider::setStatusString( const QString &newStatus )
{
  if ( mStatusString == newStatus )
  {
    return;
  }

  mStatusString = newStatus;
  emit statusStringChanged( mStatusString );
}

void AbstractPositionProvider::setHasError( bool newError )
{
  if ( mHasError == newError )
  {
    return;
  }

  mHasError = newError;
  emit hasErrorChanged( mHasError );
};

GeoPosition::GeoPosition() : QgsGpsInformation()
{
  // set attribute from base type to invalid value
  latitude = std::numeric_limits<double>::quiet_NaN();
  longitude = std::numeric_limits<double>::quiet_NaN();
  elevation = std::numeric_limits<double>::quiet_NaN();
  direction = -1;
  speed = -1;
  pdop = -1;
  hdop = -1;
  vdop = -1;
  hacc = -1;
  vacc = -1;
  satellitesUsed = -1;
}

GeoPosition GeoPosition::fromQgsGpsInformation( const QgsGpsInformation &other )
{
  GeoPosition out;

  // Copy all data from QgsGpsInformation into GeoPosition.
  // Some members of QgsGpsInformation have default value other than those in our GeoPosition
  // for such

  if ( !qgsDoubleNear( other.latitude, 0 ) )
  {
    out.latitude = other.latitude;
  }

  if ( !qgsDoubleNear( other.longitude, 0 ) )
  {
    out.longitude = other.longitude;
  }

  if ( !qgsDoubleNear( other.elevation, 0 ) )
  {
    out.elevation = other.elevation;
  }

  if ( !std::isnan( other.direction ) )
  {
    out.direction = other.direction;
  }

  if ( !std::isnan( other.hacc ) )
  {
    out.hacc = other.hacc;
  }

  if ( !std::isnan( other.vacc ) )
  {
    out.vacc = other.vacc;
  }

  if ( !std::isnan( other.hvacc ) )
  {
    out.hvacc = other.hvacc;
  }

  if ( !qgsDoubleNear( other.speed, 0 ) )
  {
    out.speed = other.speed;
  }

  if ( !qgsDoubleNear( other.hdop, 0 ) )
  {
    out.hdop = other.hdop;
  }

  if ( !qgsDoubleNear( other.pdop, 0 ) )
  {
    out.pdop = other.pdop;
  }

  if ( !qgsDoubleNear( other.vdop, 0 ) )
  {
    out.vdop = other.vdop;
  }

  out.elevation_diff = other.elevation_diff;
  out.satellitesVisible = other.satellitesInView.count();
  out.satellitesInView = other.satellitesInView;
  out.satellitesUsed = other.satellitesUsed;
  out.utcDateTime = other.utcDateTime;
  out.fixMode = other.fixMode;
  out.fixType = other.fixType;
  out.quality = other.quality;
  out.status = other.status;
  out.satPrn = other.satPrn;
  out.satInfoComplete = other.satInfoComplete;

  return out;
}

bool GeoPosition::hasValidPosition() const
{
  return !( std::isnan( latitude ) || std::isnan( longitude ) );
}
