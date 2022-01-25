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

    Q_PROPERTY( QString statusMessage READ statusMessage NOTIFY statusMessageChanged )
    Q_PROPERTY( StatusLevel statusLevel READ statusLevel NOTIFY statusLevelChanged )

  public:

    enum StatusLevel
    {
      Info = 0, //!< nothing serious, status like "connected"
      Error     //!< there is a serious error in provider like "lost connection"
    };
    Q_ENUMS( StatusLevel )

    AbstractPositionProvider( const QString &id, const QString &type, const QString &name, QObject *object = nullptr );
    virtual ~AbstractPositionProvider();

    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;
    virtual void closeProvider() = 0;
    Q_INVOKABLE virtual void reconnect();

    QString statusMessage() const;
    StatusLevel statusLevel() const;
    Q_INVOKABLE QString id() const;
    Q_INVOKABLE QString name() const;
    Q_INVOKABLE QString type() const;

  signals:
    void positionChanged( const GeoPosition &position );
    void providerConnecting();
    void providerConnected();
    void lostConnection();

    void statusMessageChanged( const QString &status );
    void statusLevelChanged( StatusLevel level );

  protected:
    void setStatus( const QString &message, StatusLevel level = StatusLevel::Info );

    // ProviderId - unique id of this provider.
    // For external receiver it holds mac address of a bluetooth device.
    // Internal providers (internal gps and simulated provider) has constant values of "devicegps" and "simulated"
    QString mProviderId;

    // ProviderType - whether it is an "internal" or "external" provider
    QString mProviderType;

    // ProviderName - name of the provider.
    // External receiver - name of a bluetooth device
    // Internal providers has constant values of "Internal GPS receiver" and "Simulated provider"
    QString mProviderName;

    // Status of this provider, "connected", "failure",..
    QString mStatusMessage;
    StatusLevel mStatusLevel;
};


#endif // ABSTRACTPOSITIONPROVIDER_H
