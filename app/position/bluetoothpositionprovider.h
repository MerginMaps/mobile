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

#include "abstractpositionprovider.h"

#include "qgsnmeaconnection.h"

#include <QBluetoothSocket>
#include <QBluetoothLocalDevice>

/**
 * NmeaParser is a big hack how to reuse QGIS NmeaConnection function in order to (a) keep ownership of bluetooth
 * socket, (b) do not have multiple unique_ptrs holding the same pointer and to avoid some possible crashes.
 *
 * Note: This way of reusing makes the parser highly dependent on QgsNmeaConnection class and any change inside the class
 * can lead to misbehaviors. See implementation of QgsNmeaConnection and QgsGpsConnection for more details.
 */
class NmeaParser : public QgsNmeaConnection
{
  public:
    NmeaParser();

    // Takes nmea string and returns gps position
    QgsGpsInformation parseNmeaString( const QString &nmeastring );
};

/**
 * BluetoothPositionProvider initiates connection to bluetooth device
 * at provided address and consumes NMEA strings from the stream.
 * Should be used as a position provider in PositionKit.
 */
class BluetoothPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

  public:
    BluetoothPositionProvider( const QString &addr, QObject *parent = nullptr );
    virtual ~BluetoothPositionProvider();

    virtual void startUpdates() override;
    virtual void stopUpdates() override;
    virtual void closeProvider() override;

  public slots:
    void positionUpdateReceived();
    void socketStateChanged( QBluetoothSocket::SocketState );

  private:
    bool mRepairingConnection = false;

    QBluetoothAddress mTargetAddress;
    std::unique_ptr<QBluetoothSocket> mSocket;
    std::unique_ptr<QBluetoothLocalDevice> mReceiverDevice;

    NmeaParser mNmeaParser;
};

#endif // BLUETOOTHPOSITIONPROVIDER_H
