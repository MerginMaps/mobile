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

#include "qobject.h"

#include "position/geoposition.h"

#include "qgspoint.h"


class AbstractPositionProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString stateMessage READ stateMessage NOTIFY stateMessageChanged )
    Q_PROPERTY( State state READ state NOTIFY stateChanged )

  public:

    enum State
    {
      NoConnection = 0,
      WaitingToReconnect,
      Connecting,
      Connected
    };
    Q_ENUM( State )

    AbstractPositionProvider( const QString &id, const QString &type, const QString &name, QObject *object = nullptr );
    virtual ~AbstractPositionProvider();

    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;
    virtual void closeProvider() = 0;

    virtual void setUpdateInterval( double msecs ) {}

    virtual void setPosition( QgsPoint position );

    QString stateMessage() const;
    State state() const;
    Q_INVOKABLE QString id() const;
    Q_INVOKABLE QString name() const;
    Q_INVOKABLE QString type() const;

  signals:
    void positionChanged( const GeoPosition &position );

    void stateMessageChanged( const QString &message );
    void stateChanged( AbstractPositionProvider::State state );

  protected:
    void setState( const QString &message ); // keeps state enum the same and only changes the message
    void setState( const QString &message, State state );

    // ProviderId - unique id of this provider.
    // For external receiver it holds mac address of a bluetooth device.
    // Internal providers (internal gps and simulated provider) has constant values of "devicegps" and "simulated"
    QString mProviderId;

    // ProviderType - whether it is an "internal" or "external" provider
    QString mProviderType;

    // ProviderName - name of the provider.
    // External receiver - name of a bluetooth device
    // Internal providers has constant values of "Internal" and "Simulated provider"
    QString mProviderName;

    // State of this provider, see State enum. Message bears human readable explanation of the state
    QString mStateMessage;
    State mState = State::NoConnection;
};

Q_DECLARE_METATYPE( AbstractPositionProvider::State );

#endif // ABSTRACTPOSITIONPROVIDER_H
