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

#ifdef ANDROID
#include <QPermissions>
#endif

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
  connect( mSocket.get(), &QBluetoothSocket::errorOccurred, this, [ = ]( QBluetoothSocket::SocketError error )
  {
    QString errorToString = QMetaEnum::fromType<QBluetoothSocket::SocketError>().valueToKey( int( error ) );
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
    mSocket->connectToService( mTargetAddress, QBluetoothUuid( QBluetoothUuid::ServiceClassUuid::SerialPort ), QIODevice::ReadOnly );
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
  // first, check the permission status (Android-only)
  Qt::PermissionStatus permissionStatus = Qt::PermissionStatus::Granted;

#ifdef ANDROID
  QBluetoothPermission btPermission;
  btPermission.setCommunicationModes( QBluetoothPermission::Access );

  if ( qApp && ( permissionStatus = qApp->checkPermission( btPermission ) ) != Qt::PermissionStatus::Granted )
  {
    if ( permissionStatus == Qt::PermissionStatus::Undetermined )
    {
      qApp->requestPermission( btPermission, []() {} );
      startReconnectionTime();
    }
    else
    {
      setState( tr( "Bluetooth permission disabled" ), State::NoConnection ); // permanent error
    }
  }

#endif

  if ( permissionStatus == Qt::PermissionStatus::Granted )
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
  }

  // let's also invalidate current position since we no longer have connection
  emit positionChanged( GeoPosition() );
}

void BluetoothPositionProvider::socketStateChanged( QBluetoothSocket::SocketState state )
{
  if ( state == QBluetoothSocket::SocketState::ConnectingState || state == QBluetoothSocket::SocketState::ServiceLookupState )
  {
    setState( tr( "Connecting to %1" ).arg( mProviderName ), State::Connecting );
  }
  else if ( state == QBluetoothSocket::SocketState::ConnectedState )
  {
    setState( tr( "Connected" ), State::Connected );
  }
  else if ( state == QBluetoothSocket::SocketState::UnconnectedState )
  {
    setState( tr( "No connection" ), State::NoConnection );
    handleLostConnection();
  }

  QString stateToString = QMetaEnum::fromType<QBluetoothSocket::SocketState>().valueToKey( int( state ) );
  CoreUtils::log( QStringLiteral( "BluetoothPositionProvider" ), QStringLiteral( "Socket changed state, code: %1" ).arg( stateToString ) );
}

void BluetoothPositionProvider::positionUpdateReceived()
{
  if ( mSocket->state() != QBluetoothSocket::SocketState::UnconnectedState )
  {
    // if by any chance we showed wrong message in the status like "no connection", fix it here
    // we know the connection is working because we just received data from the device
    setState( tr( "Connected" ), State::Connected );

    const QByteArray rawNmea = mSocket->readAll();
    const QString nmea( rawNmea );

    const QgsGpsInformation data = mNmeaParser.parseNmeaString( nmea );
    GeoPosition positionData = GeoPosition::fromQgsGpsInformation( data );

    // The geoid models used in GNSS devices can be often times unreliable, thus we apply the transformations ourselves
    // GNSS supplied orthometric elevation -> ellipsoid elevation -> orthometric elevation based on our model
    const double ellipsoidElevation = positionData.elevation + positionData.elevation_diff;
    const QgsPoint geoidPosition = InputUtils::transformPoint(
                                     PositionKit::positionCrs3DEllipsoidHeight(),
                                     PositionKit::positionCrs3D(),
                                     QgsProject::instance()->transformContext(),
    {positionData.longitude, positionData.latitude, ellipsoidElevation} );
    positionData.elevation = geoidPosition.z();
    positionData.elevation_diff = ellipsoidElevation - geoidPosition.z();

    emit positionChanged( positionData );
  }
}
