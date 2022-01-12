/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstractpositionprovider.h"

AbstractPositionProvider::AbstractPositionProvider( QObject *object ) : QObject( object )
{

}

AbstractPositionProvider::~AbstractPositionProvider() = default;

GeoPosition::GeoPosition()
{
  // set attribute from base type to invalid value
  latitude = -1;
  longitude = -1;
  elevation = -1;
  direction = -1;
  speed = -1;
  pdop = -1;
  hdop = -1;
  vdop = -1;
  hacc = -1;
  vacc = -1;
  satellitesUsed = -1;
}

GeoPosition GeoPosition::from( const QgsGpsInformation &other )
{
  GeoPosition out;
  out.latitude = other.latitude;
  out.longitude = other.longitude;
  out.elevation = other.elevation;
  out.elevation_diff = other.elevation_diff;
  out.speed = other.speed;
  out.direction = other.direction;
  out.satellitesVisible = other.satellitesInView.count();
  out.satellitesInView = other.satellitesInView;
  out.satellitesUsed = other.satellitesUsed;
  out.pdop = other.pdop;
  out.hdop = other.hdop;
  out.vdop = other.vdop;
  out.hacc = other.hacc;
  out.vacc = other.vacc;
  out.hvacc = other.hvacc;
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
  return !std::isnan( latitude ) && !std::isnan( longitude );
}
