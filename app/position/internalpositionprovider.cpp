/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "internalpositionprovider.h"

InternalPositionProvider::InternalPositionProvider( QObject *parent ) : AbstractPositionProvider( parent )
{
  mGpsLocationSource = std::unique_ptr<QGeoPositionInfoSource>( QGeoPositionInfoSource::createDefaultSource( nullptr ) );
  mGpsSatellitesSource = std::unique_ptr<QGeoSatelliteInfoSource>( QGeoSatelliteInfoSource::createDefaultSource( nullptr ) );
}
