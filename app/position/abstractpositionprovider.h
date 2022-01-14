/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACTPOSITIONPROVIDER_H
#define ABSTRACTPOSITIONPROVIDER_H

#include "qgsgpsconnection.h"

#include "qobject.h"

class GeoPosition : public QgsGpsInformation
{
  public:
    GeoPosition();

    // add information to QgsGpsInformation class to bear simple int for satellites in view
    int satellitesVisible = -1;

    double magneticVariation = -1;

    double verticalSpeed = -1;

    // copies all data from QgsGpsInformation other and updates satellitesVisible
    static GeoPosition from( const QgsGpsInformation &other );

    bool hasValidPosition() const;
};

class AbstractPositionProvider : public QObject
{
    Q_OBJECT

  public:
    AbstractPositionProvider( const QString &id, QObject *object = nullptr );
    virtual ~AbstractPositionProvider();

    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;
    virtual void closeProvider() = 0;

    Q_INVOKABLE QString providerId() const;

  signals:
    void positionChanged( const GeoPosition &position );
    void providerConnecting();
    void providerConnected();
    void lostConnection();

  protected:
    QString mProviderId;
};


#endif // ABSTRACTPOSITIONPROVIDER_H
