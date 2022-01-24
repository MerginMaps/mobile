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

#include <QTimer>

NmeaParser::NmeaParser() : QgsNmeaConnection( new QBluetoothSocket() )
{
}

QgsGpsInformation NmeaParser::parseNmeaString( const QString &nmeastring )
{
  mStringBuffer = nmeastring;
  processStringBuffer();
  return mLastGPSInformation;
}

BluetoothPositionProvider::BluetoothPositionProvider( const QString &addr, const QString &name, QObject *parent )
  : AbstractPositionProvider( addr, QStringLiteral( "external" ), name, parent )
  , mTargetAddress( addr )
{
  mSocket = std::unique_ptr<QBluetoothSocket>( new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol ) );
  mReceiverDevice = std::make_unique<QBluetoothLocalDevice>();

  connect( mSocket.get(), &QBluetoothSocket::stateChanged, this, &BluetoothPositionProvider::socketStateChanged );
  connect( mSocket.get(), QOverload<QBluetoothSocket::SocketError>::of( &QBluetoothSocket::error ), this,
           [ = ]( QBluetoothSocket::SocketError error )
  {
    QString errorToString = QMetaEnum::fromType<QBluetoothSocket::SocketError>().valueToKey( error );
    CoreUtils::log(
      QStringLiteral( "BluetoothPositionProvider" ),
      QStringLiteral( "Occured connection error: %1, text: %2" ).arg( errorToString ).arg( mSocket->errorString() )
    );

    // let's try to reconnect in few seconds in case of network error
    if ( error == QBluetoothSocket::NetworkError &&
         mReceiverDevice->pairingStatus( mTargetAddress ) == QBluetoothLocalDevice::Paired &&
         !mAlreadyTriedToRepairConnection )
    {
      mRepairingConnection = true;
      mAlreadyTriedToRepairConnection = true; // we try to reconnect only once

      QTimer::singleShot( 3000, this, [ = ]()
      {
        reconnect();
        mRepairingConnection = false;
      } );

      setHasError( false );
      setStatusString( QStringLiteral( "Reconnecting the receiver" ) );
    }
    else
    {
      setHasError( true );
      setStatusString( QStringLiteral( "Lost connection, click to reconnect" ) );
      emit lostConnection();
    }
  } );

  connect( mSocket.get(), &QBluetoothSocket::readyRead, this, &BluetoothPositionProvider::positionUpdateReceived );

  BluetoothPositionProvider::startUpdates();
}

BluetoothPositionProvider::~BluetoothPositionProvider()
{
  mSocket->close();
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

void BluetoothPositionProvider::reconnect()
{
  setStatusString( QStringLiteral( "Reconnecting" ) );
  AbstractPositionProvider::reconnect();
}

void BluetoothPositionProvider::socketStateChanged( QBluetoothSocket::SocketState state )
{
  if ( state == QBluetoothSocket::ConnectingState || state == QBluetoothSocket::ServiceLookupState )
  {
    setHasError( false );
    setStatusString( QStringLiteral( "Connecting to the receiver" ) );
    emit providerConnecting();
  }
  else if ( state == QBluetoothSocket::ConnectedState )
  {
    setHasError( false );
    setStatusString( QStringLiteral( "Connected" ) );
    emit providerConnected();
  }
  else if ( state == QBluetoothSocket::UnconnectedState )
  {
    if ( !mRepairingConnection )
    {
      setHasError( true );
      setStatusString( QStringLiteral( "Lost connection, click to reconnect" ) );
      emit lostConnection();
    }
  }

  QString stateToString = QMetaEnum::fromType<QBluetoothSocket::SocketState>().valueToKey( state );
  CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Socket changed state, code: %1" ).arg( stateToString ) );
}

void BluetoothPositionProvider::positionUpdateReceived()
{
  if ( mSocket->state() != QBluetoothSocket::UnconnectedState )
  {
    // if by any chance we showed wrong message in the status like "lost connection", fix it here
    // we know the connection is working because we just received data from the device
    setStatusString( QStringLiteral( "Connected" ) );
    setHasError( false );

    QByteArray rawNmea = mSocket->readAll();
    QString nmea( rawNmea );

    QgsGpsInformation data = mNmeaParser.parseNmeaString( nmea );

    emit positionChanged( GeoPosition::fromQgsGpsInformation( data ) );
  }
}
