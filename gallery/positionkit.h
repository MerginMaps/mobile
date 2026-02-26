/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONKIT_H
#define POSITIONKIT_H

#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

class  PositionKit : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY( double latitude READ latitude CONSTANT )
    Q_PROPERTY( double longitude READ longitude CONSTANT )
    Q_PROPERTY( double verticalAccuracy READ verticalAccuracy CONSTANT )
    Q_PROPERTY( double horizontalAccuracy READ horizontalAccuracy CONSTANT )
    Q_PROPERTY( double altitude READ altitude CONSTANT )
    Q_PROPERTY( double speed READ speed CONSTANT )
    Q_PROPERTY( double x READ x CONSTANT )
    Q_PROPERTY( double y READ y CONSTANT )
    Q_PROPERTY( int satellitesUsed READ satellitesUsed CONSTANT )
    Q_PROPERTY( int satellitesVisible READ satellitesVisible CONSTANT )
    Q_PROPERTY( bool hasPosition READ hasPosition CONSTANT )
    Q_PROPERTY( bool positionProvider READ positionProvider CONSTANT )
    Q_PROPERTY( QString providerName READ providerName CONSTANT )
    Q_PROPERTY( QString providerType READ providerType CONSTANT )
    Q_PROPERTY( QString providerMessage READ providerMessage CONSTANT )
    Q_PROPERTY( QString stateMessage READ stateMessage CONSTANT )
    Q_PROPERTY( QString lastRead READ lastRead CONSTANT )

  public:
    explicit PositionKit( QObject *parent = nullptr ) {};

    double latitude() const { return hLatitude; }
    double longitude() const { return hLongitude; }
    double verticalAccuracy() const { return pVerticalAccuracy; }
    double horizontalAccuracy() const { return pHorizontalAccuracy; }
    double altitude() const { return pAltitude; }
    double speed() const { return pSpeed; }
    double x() const { return pX; }
    double y() const { return pY; }
    int satellitesUsed() const { return pSatellitesUsed; }
    int satellitesVisible() const { return pSatellitesVisible; }
    bool positionProvider() const { return pPositionProvider; }
    bool hasPosition() const { return true; }
    QString providerName() const { return pProviderName; }
    QString providerMessage() const {   return pProviderMessage; }
    QString providerType() const { return pProviderType; }
    QString stateMessage() const { return pStateMessage; }
    QString lastRead() const { return pLastRead; }

  private:
    QString pProviderName = "Gps Source is ok!";
    QString pProviderType = "external_bt";
    QString pProviderMessage = "Connected";
    QString pStateMessage = "Message";
    QString pLastRead = "17:19:08 CEST";
    bool pPositionProvider = true;
    int pSatellitesVisible = 40;
    int pSatellitesUsed = 1;
    double pY = 20.00;
    double pX = 20.00;
    double hLatitude = -22.906;
    double pVerticalAccuracy = 20;
    double pHorizontalAccuracy = 20;
    double hLongitude = -43.1729;
    double pAltitude = 199.85;
    double pSpeed = 27;
    double pGpsAntennaHeight = 0;

};

#endif // POSITIONKIT_H
