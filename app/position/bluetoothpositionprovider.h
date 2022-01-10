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

class BluetoothPositionProvider : public AbstractPositionProvider
{
    Q_OBJECT

  public:
    BluetoothPositionProvider( const QString &addr, QIODevice::OpenMode mode = QIODevice::ReadOnly );
    virtual ~BluetoothPositionProvider();

  public slots:
    void connected();
    void disconnected();
    void stateChanged( QBluetoothSocket::SocketState );
    void positionUpdateReceived();
    void positionChanged( QgsGpsInformation position );

  private:
    std::unique_ptr<QBluetoothSocket> mSocket;
    std::unique_ptr<QgsNmeaConnection> mNmeaConnection;
};

#endif // BLUETOOTHPOSITIONPROVIDER_H
