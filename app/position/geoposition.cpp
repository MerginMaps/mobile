/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "geoposition.h"

GeoPosition::GeoPosition() : QgsGpsInformation()
{
  // set attribute from base type to invalid value
  latitude = std::numeric_limits<double>::quiet_NaN();
  longitude = std::numeric_limits<double>::quiet_NaN();
  elevation = std::numeric_limits<double>::quiet_NaN();
  elevation_diff = std::numeric_limits<double>::quiet_NaN();
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

  if ( !qgsDoubleNear( other.elevation_diff, 0 ) )
  {
    out.elevation_diff = other.elevation_diff;
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
  out.fixStatusString = out.parseFixStatus();

  return out;
}

bool GeoPosition::hasValidPosition() const
{
  return !( std::isnan( latitude ) || std::isnan( longitude ) );
}

QString GeoPosition::parseFixStatus() const
{
  //
  // parsed based on http://lefebure.com/articles/nmea-gga/:
  //
  //  0 = Invalid, no position available.
  //  1 = Autonomous GPS fix, no correction data used.
  //  2 = DGPS fix, using a local DGPS base station or correction service such as WAAS or EGNOS.
  //  3 = PPS fix, I’ve never seen this used.
  //  4 = RTK fix, high accuracy Real Time Kinematic.
  //  5 = RTK Float, better than DGPS, but not quite RTK.
  //  6 = Estimated fix (dead reckoning).
  //  7 = Manual input mode.
  //  8 = Simulation mode.
  //  9 = WAAS fix (not NMEA standard, but NovAtel receivers report this instead of a 2).
  //

  switch ( quality )
  {
    case -1:
      return QObject::tr( "No data" );

    case 0:
      return QObject::tr( "No fix" );

    case 1:
      return QObject::tr( "GPS fix, no correction data" );

    case 2:
    // fall through
    case 9:
      return QObject::tr( "DGPS fix" );

    case 3:
      return QObject::tr( "PPS fix" );

    case 4:
      return QObject::tr( "RTK fix" );

    case 5:
      return QObject::tr( "RTK float" );

    case 6:
      return QObject::tr( "Estimated fix (dead reckoning)" );

    default:
      return QObject::tr( "Unknown fix" );
  }
}
