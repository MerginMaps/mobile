/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INTERNALPOSITIONPROVIDER_H
#define INTERNALPOSITIONPROVIDER_H

#include "abstractpositionprovider.h"

#include <QObject>
#include <qglobal.h>

#include <QGeoPositionInfoSource>
#include <QGeoSatelliteInfoSource>

class InternalPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT
  public:
    explicit InternalPositionProvider( QObject *parent = nullptr );

  private:
    std::unique_ptr<QGeoPositionInfoSource> mGpsLocationSource;
    std::unique_ptr<QGeoSatelliteInfoSource> mGpsSatellitesSource;

};

#endif // INTERNALPOSITIONPROVIDER_H
