/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bluetoothpositionprovider.h"

BluetoothPositionProvider::BluetoothPositionProvider( const QString &addr, QIODevice::OpenMode mode )
{
  mSocket = std::unique_ptr<QBluetoothSocket>( new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol ) );

  connect( mSocket.get(), &QBluetoothSocket::connected, this, &BluetoothPositionProvider::connected );
  connect( mSocket.get(), &QBluetoothSocket::disconnected, this, &BluetoothPositionProvider::disconnected );
  connect( mSocket.get(), &QBluetoothSocket::stateChanged, this, &BluetoothPositionProvider::stateChanged );
  connect( mSocket.get(), QOverload<QBluetoothSocket::SocketError>::of( &QBluetoothSocket::error ),
       [=]( QBluetoothSocket::SocketError error ){
    qDebug() << error << "<- occured bluetooth socket error!!";
  });

  connect( mSocket.get(), &QBluetoothSocket::readyRead, this, &BluetoothPositionProvider::positionUpdateReceived );

  mSocket->connectToService( QBluetoothAddress( addr ), QBluetoothUuid( QBluetoothUuid::SerialPort ), mode );

//  mNmeaConnection = std::unique_ptr<QgsNmeaConnection>( new QgsNmeaConnection( mSocket.get() ) );

//  connect( mNmeaConnection.get(), &QgsNmeaConnection::stateChanged, this, &BluetoothPositionProvider::positionChanged );
}

BluetoothPositionProvider::~BluetoothPositionProvider()
{
  mNmeaConnection->close();
  mSocket->disconnectFromService();
  mSocket->close();
}

void BluetoothPositionProvider::connected()
{
  qDebug() << "SOCKET" << mSocket->peerName() << "CONNECTED!";
//  mNmeaConnection->connect();
}

void BluetoothPositionProvider::disconnected()
{
  qDebug() << "SOCKET" << mSocket->peerName() << "DISCONNECTED!";
  emit lostConnection();
}

void BluetoothPositionProvider::stateChanged( QBluetoothSocket::SocketState state )
{
  qDebug() << "SOCKET" << mSocket->peerName() << "CHANGED STATE TO:" << state << "!";
}

void BluetoothPositionProvider::positionUpdateReceived()
{
  qDebug() << "SOCKET" << mSocket->peerName() << "RECEIVED POSITION UPDATE!";

  if ( mSocket->state() != QBluetoothSocket::UnconnectedState )
  {
    QByteArray rawNmea = mSocket->readAll();
    QString nmea( rawNmea );
    qDebug() << "NMEA:" << nmea;

    emit AbstractPositionProvider::positionChanged();
  }
}

void BluetoothPositionProvider::positionChanged( QgsGpsInformation position )
{
  qDebug() << "Received new position:" << position.latitude << position.longitude << position.hdop << position.satellitesUsed;
}
