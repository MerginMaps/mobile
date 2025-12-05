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

/**
 * InternalPositionProvider uses device's GPS receiver (QT implementation) and yields received position updates
 * Should be used as a position provider in PositionKit
 */
class InternalPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

  public:
    explicit InternalPositionProvider( QObject *parent = nullptr );
    virtual ~InternalPositionProvider() override;

    virtual void startUpdates() override;
    virtual void stopUpdates() override;
    virtual void closeProvider() override;

  public slots:
    void parsePositionUpdate( QGeoPositionInfo &position );
    void parseVisibleSatellitesUpdate( const QList<QGeoSatelliteInfo> &satellites );
    void parseUsedSatellitesUpdate( const QList<QGeoSatelliteInfo> &satellites );

  private:
    // determines if sources were successfully created
    bool mPositionSourceValid = false;
    bool mSatelliteSourceValid = false;

    // There are two sources of GPS data, one informs us about position and the other about satellites.
    // Both of them are handled in separate slots and in order to be able to emit merged information
    // as position update, we need to store received data in mLastPosition.
    // Otherwise this provider would emit incomplete GPS info (position without satellites or vice versa).
    GeoPosition mLastPosition;

    std::unique_ptr<QGeoPositionInfoSource> mGpsPositionSource;
    std::unique_ptr<QGeoSatelliteInfoSource> mGpsSatellitesSource;

};

#endif // INTERNALPOSITIONPROVIDER_H
