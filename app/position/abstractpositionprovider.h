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
    static GeoPosition fromQgsGpsInformation( const QgsGpsInformation &other );

    bool hasValidPosition() const;
};

class AbstractPositionProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString statusString READ statusString NOTIFY statusStringChanged )
    Q_PROPERTY( bool hasError READ hasError NOTIFY hasErrorChanged )

  public:
    AbstractPositionProvider( const QString &id, const QString &type, const QString &name, QObject *object = nullptr );
    virtual ~AbstractPositionProvider();

    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;
    virtual void closeProvider() = 0;
    Q_INVOKABLE virtual void reconnect();

    QString statusString() const;
    bool hasError() const;
    Q_INVOKABLE QString name() const;
    Q_INVOKABLE QString providerId() const;
    Q_INVOKABLE QString providerType() const;

  signals:
    void positionChanged( const GeoPosition &position );
    void providerConnecting();
    void providerConnected();
    void lostConnection();

    void statusStringChanged( const QString &status );
    void hasErrorChanged( bool err );

  protected:
    void setStatusString( const QString &newStatus );
    void setHasError( bool newError );

    QString mProviderId;
    QString mProviderType;
    QString mProviderName;
    QString mStatusString;
    bool mHasError = false;
};


#endif // ABSTRACTPOSITIONPROVIDER_H
