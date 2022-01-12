/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bluetoothpositionprovider.h"
#include "coreutils.h"

NmeaParser::NmeaParser() : QgsNmeaConnection( nullptr )
{
}

QgsGpsInformation NmeaParser::parseNmeaString( const QString &nmeastring )
{
  mStringBuffer = nmeastring;
  processStringBuffer();
  return mLastGPSInformation;
}

BluetoothPositionProvider::BluetoothPositionProvider( const QString &addr, QObject *parent )
  : AbstractPositionProvider( parent )
  , mTargetAddress( addr )
{
  mSocket = std::unique_ptr<QBluetoothSocket>( new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol ) );

  connect( mSocket.get(), &QBluetoothSocket::stateChanged, this, &BluetoothPositionProvider::socketStateChanged );
  connect( mSocket.get(), QOverload<QBluetoothSocket::SocketError>::of( &QBluetoothSocket::error ),
           [ = ]( QBluetoothSocket::SocketError error )
  {
    CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Occured error code: %1" ).arg( error ) );
    qDebug() << error << "<- occured bluetooth socket error!!"; // TODO: remove
  } );

  connect( mSocket.get(), &QBluetoothSocket::readyRead, this, &BluetoothPositionProvider::positionUpdateReceived );
}

BluetoothPositionProvider::~BluetoothPositionProvider()
{
  mSocket->disconnectFromService();
  mSocket->close();
}

void BluetoothPositionProvider::startUpdates()
{
  mSocket->connectToService( mTargetAddress, QBluetoothUuid( QBluetoothUuid::SerialPort ), QIODevice::ReadOnly );
}

void BluetoothPositionProvider::stopUpdates()
{
  mSocket->disconnectFromService();
}

void BluetoothPositionProvider::closeProvider()
{
  mSocket->disconnectFromService();
  mSocket->close();
}

void BluetoothPositionProvider::socketStateChanged( QBluetoothSocket::SocketState state )
{
  if ( state == QBluetoothSocket::ConnectingState )
    emit providerConnecting();
  else if ( state == QBluetoothSocket::ConnectedState )
    emit providerConnected();
  else if ( state == QBluetoothSocket::UnconnectedState )
    emit lostConnection();

  CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Socket changed state, code: " ).arg( state ) );
}

void BluetoothPositionProvider::positionUpdateReceived()
{
  if ( mSocket->state() != QBluetoothSocket::UnconnectedState )
  {
    QByteArray rawNmea = mSocket->readAll();
    QString nmea( rawNmea );

    qDebug() << "NMEA:" << nmea;

    QgsGpsInformation data = mNmeaParser.parseNmeaString( nmea );

    qDebug() << "Parsed position: " << data.latitude << data.longitude;

    GeoPosition out = GeoPosition::from( data );

    qDebug() << "After copy parsed position: " << out.latitude << out.longitude;

    emit positionChanged( out );
  }
}

