/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BLUETOOTHPOSITIONPROVIDER_H
#define BLUETOOTHPOSITIONPROVIDER_H

#include <QBluetoothSocket>
#include <QBluetoothLocalDevice>
#include <QTimer>

#include "abstractpositionprovider.h"
#include "nmeaparser.h"

/**
 * BluetoothPositionProvider initiates connection to bluetooth device
 * at provided address and consumes NMEA strings from the stream.
 * Should be used as a position provider in PositionKit.
 */
class BluetoothPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

    // signalizes in how many [ms] we will try to reconnect to GPS again
    enum ReconnectDelay
    {
      ShortDelay = 3000,
      LongDelay = 5000
    };

  public:
    BluetoothPositionProvider( const QString &addr, const QString &name, QObject *parent = nullptr );
    ~BluetoothPositionProvider() override;

    void startUpdates() override;
    void stopUpdates() override;
    void closeProvider() override;

    void handleLostConnection();
    void startReconnectionTime();
    void reconnect();

    QBluetoothSocket *socket() const;

  public slots:
    void positionUpdateReceived();
    void socketStateChanged( QBluetoothSocket::SocketState );
    void reconnectTimeout();

  private:
    QBluetoothAddress mTargetAddress; // BT mac address of the receiver
    std::unique_ptr<QBluetoothSocket> mSocket; // socket used to receive data from GPS
    std::unique_ptr<QBluetoothLocalDevice> mReceiverDevice; // description of this device bluetooth state

    int mReconnectDelay = ReconnectDelay::ShortDelay; // in how many [ms] we will try to reconnect again
    int mSecondsLeftToReconnect = ReconnectDelay::ShortDelay; // how many seconds are left to reconnect. Reconnects if less than or equal to one
    QTimer mReconnectTimer; // timer that times out each second and lowers the mSecondsLeftToReconnect by one

    NmeaParser mNmeaParser;
};

#endif // BLUETOOTHPOSITIONPROVIDER_H
