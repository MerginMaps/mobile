/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BLUETOOTHLOCATIONPROVIDER_H
#define BLUETOOTHLOCATIONPROVIDER_H

#include "abstractlocationprovider.h"

#include <QBluetoothSocket>

class BluetoothLocationProvider : public AbstractLocationProvider
{
    Q_OBJECT

  public:
    BluetoothLocationProvider( const QString &addr, QIODevice::OpenMode mode = QIODevice::ReadOnly );

//    Q_INVOKABLE static BluetoothLocationProvider *constructProvider( const QString &address );

  public slots:
    void connected();
    void disconnected();
    void stateChanged( QBluetoothSocket::SocketState );
    void positionUpdateReceived();

  private:
    std::unique_ptr<QBluetoothSocket> mBtSocket;
};

#endif // BLUETOOTHLOCATIONPROVIDER_H
