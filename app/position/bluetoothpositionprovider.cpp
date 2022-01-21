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

NmeaParser::NmeaParser() : QgsNmeaConnection( new QBluetoothSocket() )
{
}

QgsGpsInformation NmeaParser::parseNmeaString( const QString &nmeastring )
{
  mStringBuffer = nmeastring;
  processStringBuffer();
  return mLastGPSInformation;
}

BluetoothPositionProvider::BluetoothPositionProvider( const QString &addr, QObject *parent )
  : AbstractPositionProvider( addr, parent )
  , mTargetAddress( addr )
{
  mSocket = std::unique_ptr<QBluetoothSocket>( new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol ) );
  mReceiverDevice = std::make_unique<QBluetoothLocalDevice>();

  connect( mSocket.get(), &QBluetoothSocket::stateChanged, this, &BluetoothPositionProvider::socketStateChanged );
  connect( mSocket.get(), QOverload<QBluetoothSocket::SocketError>::of( &QBluetoothSocket::error ), this,
           [ = ]( QBluetoothSocket::SocketError error )
  {
    QString errorToString = QMetaEnum::fromType<QBluetoothSocket::SocketError>().valueToKey( error );
    CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Occured connection error: %1" ).arg( errorToString ) );

    qDebug() << "BT!: PAIRING STATE:" << mReceiverDevice->pairingStatus( mTargetAddress );
    qDebug() << "BT!: SOCKET ERROR:" << error; // TODO: remove

    emit lostConnection();
  } );

  connect( mSocket.get(), &QBluetoothSocket::readyRead, this, &BluetoothPositionProvider::positionUpdateReceived );

  BluetoothPositionProvider::startUpdates();
}

BluetoothPositionProvider::~BluetoothPositionProvider()
{
  mSocket->close();
  qDebug() << "Destroyed provider" << this;
}

void BluetoothPositionProvider::startUpdates()
{
  mSocket->connectToService( mTargetAddress, QBluetoothUuid( QBluetoothUuid::SerialPort ), QIODevice::ReadOnly );
}

void BluetoothPositionProvider::stopUpdates()
{
  mSocket->close();
}

void BluetoothPositionProvider::closeProvider()
{
  mSocket->close();
}

void BluetoothPositionProvider::socketStateChanged( QBluetoothSocket::SocketState state )
{
  if ( state == QBluetoothSocket::ConnectingState || state == QBluetoothSocket::ServiceLookupState )
  {
    emit providerConnecting();
  }
  else if ( state == QBluetoothSocket::ConnectedState )
  {
    emit providerConnected();
  }
  else if ( state == QBluetoothSocket::UnconnectedState )
  {
    emit lostConnection();
  }

  qDebug() << "BT!: SOCKET STATE:" << state;
  qDebug() << "BT!: PAIRING STATE:" << mReceiverDevice->pairingStatus( mTargetAddress );

//  if ( mSocket->error() == QBluetoothSocket::NetworkError
//       && mReceiverDevice->pairingStatus( mTargetAddress ) == QBluetoothLocalDevice::Paired
//       && state == QBluetoothSocket::UnconnectedState )
//  {
    // try to fix connection
//    qDebug() << "BT!: HERE I WOULD FIX THE CONNECTION!";
//    if ( !mRepairingConnection )
//    {
//      mRepairingConnection = true;
//      startUpdates();
//    }
//  }

  QString stateToString = QMetaEnum::fromType<QBluetoothSocket::SocketState>().valueToKey( state );
  CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Socket changed state, code: %1" ).arg( stateToString ) );
}

void BluetoothPositionProvider::positionUpdateReceived()
{
  if ( mSocket->state() != QBluetoothSocket::UnconnectedState )
  {
    QByteArray rawNmea = mSocket->readAll();
    QString nmea( rawNmea );

    qDebug() << "NMEA:" << nmea; // TODO: remove

    CoreUtils::log( "NMEA STRING", nmea );

    QgsGpsInformation data = mNmeaParser.parseNmeaString( nmea );

    QString parsed = QStringLiteral( "Parsed position: la: %1 lo: %2 alt: %3 h/v acc: %4/%5 speed: %6 hdop: %7" ).arg( data.latitude ).arg( data.longitude ).arg( data.elevation ).arg( data.hacc ).arg( data.vacc ).arg( data.speed ).arg( data.hdop );

    CoreUtils::log( "NMEA PARSED:", parsed );

    GeoPosition out = GeoPosition::fromQgsGpsInformation( data );

    emit positionChanged( out );
  }
}

