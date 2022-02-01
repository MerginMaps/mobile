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
#include "androidutils.h"
#include "inpututils.h"

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
      QStringLiteral( "Occured connection error: %1, text: %2" ).arg( errorToString, mSocket->errorString() )
    );

    setState( tr( "No connection" ), State::NoConnection );
    handleLostConnection();
  } );

  connect( mSocket.get(), &QBluetoothSocket::readyRead, this, &BluetoothPositionProvider::positionUpdateReceived );

  mReconnectTimer.setSingleShot( false );
  mReconnectTimer.setInterval( 1000 );

  connect( &mReconnectTimer, &QTimer::timeout, this, &BluetoothPositionProvider::reconnectTimeout );

  BluetoothPositionProvider::startUpdates();
}

BluetoothPositionProvider::~BluetoothPositionProvider()
{
  mSocket->close();
}

void BluetoothPositionProvider::startUpdates()
{
  if ( mSocket->state() == QBluetoothSocket::SocketState::UnconnectedState )
  {
    mSocket->connectToService( mTargetAddress, QBluetoothUuid( QBluetoothUuid::SerialPort ), QIODevice::ReadOnly );
  }
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
  mReconnectTimer.stop();

  setState( tr( "Reconnecting" ), State::Connecting );

  CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Reconnecting to %1" ).arg( mProviderName ) );

  if ( mReceiverDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff )
  {
    // bluetooth is powered off.. ask user once to turn it on
    if ( InputUtils::appPlatform() == QStringLiteral( "android" ) )
    {
      AndroidUtils android;
      android.turnBluetoothOn();
    }
  }

  stopUpdates();
  startUpdates();
}

QBluetoothSocket *BluetoothPositionProvider::socket() const
{
  return mSocket.get();
}

void BluetoothPositionProvider::reconnectTimeout()
{
  if ( mSecondsLeftToReconnect <= 1 )
  {
    reconnect();
  }
  else
  {
    mSecondsLeftToReconnect--;
    setState( tr( "No connection, reconnecting in (%1)" ).arg( mSecondsLeftToReconnect ), State::WaitingToReconnect );
    mReconnectTimer.start();
  }
}

void BluetoothPositionProvider::startReconnectionTime()
{
  mSecondsLeftToReconnect = mReconnectDelay / 1000;
  setState( tr( "No connection, reconnecting in (%1)" ).arg( mSecondsLeftToReconnect ), State::WaitingToReconnect );

  mReconnectTimer.start();

  // first time do reconnect in short time, then each other in long time
  if ( mReconnectDelay == BluetoothPositionProvider::ShortDelay )
  {
    mReconnectDelay = BluetoothPositionProvider::LongDelay;
  }
}

void BluetoothPositionProvider::handleLostConnection()
{
  // we want to reconnect, but only to devices that are paired

  if ( mReceiverDevice->pairingStatus( mTargetAddress ) == QBluetoothLocalDevice::Unpaired )
  {
    setState( tr( "Could not connect to device, not paired" ), State::NoConnection );
  }
  else if ( mState != WaitingToReconnect && mState != Connecting )
  {
    startReconnectionTime();
  }

  // let's also invalidate current position since we no longer have connection
  emit positionChanged( GeoPosition() );
}

void BluetoothPositionProvider::socketStateChanged( QBluetoothSocket::SocketState state )
{
  if ( state == QBluetoothSocket::ConnectingState || state == QBluetoothSocket::ServiceLookupState )
  {
    setState( tr( "Connecting to %1" ).arg( mProviderName ), State::Connecting );
  }
  else if ( state == QBluetoothSocket::ConnectedState )
  {
    setState( tr( "Connected" ), State::Connected );
  }
  else if ( state == QBluetoothSocket::UnconnectedState )
  {
    setState( tr( "No connection" ), State::NoConnection );
    handleLostConnection();
  }

  QString stateToString = QMetaEnum::fromType<QBluetoothSocket::SocketState>().valueToKey( state );
  CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Socket changed state, code: %1" ).arg( stateToString ) );
}

void BluetoothPositionProvider::positionUpdateReceived()
{
  if ( mSocket->state() != QBluetoothSocket::UnconnectedState )
  {
    // if by any chance we showed wrong message in the status like "no connection", fix it here
    // we know the connection is working because we just received data from the device
    setState( tr( "Connected" ), State::Connected );

    QByteArray rawNmea = mSocket->readAll();
    QString nmea( rawNmea );

    QgsGpsInformation data = mNmeaParser.parseNmeaString( nmea );

    emit positionChanged( GeoPosition::fromQgsGpsInformation( data ) );
  }
}
