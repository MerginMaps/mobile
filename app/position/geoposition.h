/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GEOPOSITION_H
#define GEOPOSITION_H

#include <qglobal.h>
#include "qobject.h"

#include "qgsgpsconnection.h"

class GeoPosition : public QgsGpsInformation
{
  public:
    GeoPosition();

    // add information to QgsGpsInformation class to bear simple int for satellites in view
    int satellitesVisible = -1;

    double magneticVariation = -1;

    double verticalSpeed = -1;

    QString fixStatusString;

    bool isMock = false;

    // Set by providers that already account for antenna height (e.g. Trimble via TMM).
    // antennaHeight >= 0 means the provider reported it; -1 = not provided.
    // antennaHeightApplied = true means the provider already reduced to the ground point
    // and the app must NOT subtract it again.
    double antennaHeight = -1;
    bool antennaHeightApplied = false;

    // copies all data from QgsGpsInformation other and updates satellitesVisible
    static GeoPosition fromQgsGpsInformation( const QgsGpsInformation &other );

    bool hasValidPosition() const;

    QString parseFixStatus() const;
};
#endif // GEOPOSITION_H
