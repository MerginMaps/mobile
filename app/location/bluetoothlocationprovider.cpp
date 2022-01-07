/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bluetoothlocationprovider.h"

BluetoothLocationProvider::BluetoothLocationProvider( const QString &addr, QIODevice::OpenMode mode )
{
  mBtSocket = std::unique_ptr<QBluetoothSocket>( new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol ) );

  connect( mBtSocket.get(), &QBluetoothSocket::connected, this, &BluetoothLocationProvider::connected );
  connect( mBtSocket.get(), &QBluetoothSocket::disconnected, this, &BluetoothLocationProvider::disconnected );
  connect( mBtSocket.get(), &QBluetoothSocket::stateChanged, this, &BluetoothLocationProvider::stateChanged );
  connect( mBtSocket.get(), QOverload<QBluetoothSocket::SocketError>::of( &QBluetoothSocket::error ),
       [=]( QBluetoothSocket::SocketError error ){
    qDebug() << error << "<- occured bluetooth socket error!!";
  });

  connect( mBtSocket.get(), &QBluetoothSocket::readyRead, this, &BluetoothLocationProvider::positionUpdateReceived );

  mBtSocket->connectToService( QBluetoothAddress( addr ), QBluetoothUuid( QBluetoothUuid::SerialPort ), mode );
}

//BluetoothLocationProvider *BluetoothLocationProvider::constructProvider( const QString &bluetoothAddress )
//{
//  BluetoothLocationProvider *provider = new BluetoothLocationProvider();
//  provider->mBtSocket->connectToService( QBluetoothAddress( bluetoothAddress ), QBluetoothUuid( QBluetoothUuid::SerialPort ), QBluetoothSocket::ReadOnly );
//  qDebug() << "Instantiated bt provider!";
//  return provider;
//}

void BluetoothLocationProvider::connected()
{
  qDebug() << "SOCKET" << mBtSocket->peerName() << "CONNECTED!";
}

void BluetoothLocationProvider::disconnected()
{
  qDebug() << "SOCKET" << mBtSocket->peerName() << "DISCONNECTED!";
}

void BluetoothLocationProvider::stateChanged( QBluetoothSocket::SocketState state )
{
  qDebug() << "SOCKET" << mBtSocket->peerName() << "CHANGED STATE TO:" << state << "!";
}

void BluetoothLocationProvider::positionUpdateReceived()
{
  qDebug() << "SOCKET" << mBtSocket->peerName() << "RECEIVED POSITION UPDATE!";

  if ( mBtSocket->state() != QBluetoothSocket::UnconnectedState )
  {
    QByteArray rawNmea = mBtSocket->readAll();
    QString nmea( rawNmea );
    qDebug() << "NMEA:" << nmea;
  }
}
