/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "networkpositionprovider.h"

#include "coreutils.h"

static int ONE_SECOND_MS = 1000;

NetworkPositionProvider::NetworkPositionProvider( const QString &addr, const QString &name, QObject *parent )
  : AbstractPositionProvider( addr, QStringLiteral( "external_ip" ), name, parent )
{
  const QStringList targetAddress = addr.split( ":" );
  mTargetAddress = targetAddress.at( 0 );
  mTargetPort = targetAddress.at( 1 ).toInt();

  mTcpSocket = std::make_unique<QTcpSocket>();
  mUdpSocket = std::make_unique<QUdpSocket>();

// only one socket will be active depending on, which will find host, the other will be closed
// connect(mTcpSocket.get(), &QAbstractSocket::hostFound, this, [this]
// {
//  CoreUtils::log("NetworkPositionProvider", "TCP socket found host, aborting UDP socket");
//  mUdpSocket->abort();
// });
// connect(mUdpSocket.get(), &QAbstractSocket::hostFound, this, [this]
// {
//  CoreUtils::log("NetworkPositionProvider", "UDP socket found host, aborting TCP socket");
//  mTcpSocket->abort();
// });

  connect( mTcpSocket.get(), &QAbstractSocket::readyRead, this, &NetworkPositionProvider::positionUpdateReceived );
  connect( mUdpSocket.get(), &QAbstractSocket::readyRead, this, &NetworkPositionProvider::positionUpdateReceived );

  connect( mTcpSocket.get(), &QAbstractSocket::stateChanged, this, &NetworkPositionProvider::socketStateChanged );
  connect( mUdpSocket.get(), &QAbstractSocket::stateChanged, this, &NetworkPositionProvider::socketStateChanged );

  mReconnectTimer.setSingleShot( false );
  mReconnectTimer.setInterval( ONE_SECOND_MS );
  connect( &mReconnectTimer, &QTimer::timeout, this, &NetworkPositionProvider::reconnectTimeout );

  NetworkPositionProvider::startUpdates();
}

void NetworkPositionProvider::startUpdates()
{
  CoreUtils::log( "NetworkPositionProvider", "Connecting to host..." );
  mTcpSocket->connectToHost( mTargetAddress, mTargetPort );
  mUdpSocket->connectToHost( mTargetAddress, mTargetPort );
}

void NetworkPositionProvider::stopUpdates()
{
  if ( mTcpSocket->state() != QAbstractSocket::UnconnectedState && mTcpSocket->state() != QAbstractSocket::ClosingState )
  {
    mTcpSocket->disconnectFromHost();
  }
  if ( mUdpSocket->state() != QAbstractSocket::UnconnectedState && mUdpSocket->state() != QAbstractSocket::ClosingState )
  {
    mUdpSocket->disconnectFromHost();
  }
}

void NetworkPositionProvider::closeProvider()
{
  mTcpSocket->close();
  mUdpSocket->close();
}

void NetworkPositionProvider::positionUpdateReceived()
{
  const QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>( sender() );
  const QString socketTypeToString = QMetaEnum::fromType<QAbstractSocket::SocketType>().valueToKey( socket->socketType() );
  CoreUtils::log( "NetworkPositionProvider", QStringLiteral( "%1 socket has received new data!" ).arg( socketTypeToString ) );
  if ( mTcpSocket->isValid() || mUdpSocket->isValid() )
  {
    const QByteArray rawNmeaData = activeSocket()->readAll();
    const QString nmeaData( rawNmeaData );
    const QgsGpsInformation gpsInfo = mNmeaParser.parseNmeaString( nmeaData );

    emit positionChanged( GeoPosition::fromQgsGpsInformation( gpsInfo ) );
  }
}

void NetworkPositionProvider::socketStateChanged( const QAbstractSocket::SocketState state )
{
  if ( state == QAbstractSocket::SocketState::ConnectingState || state == QAbstractSocket::SocketState::HostLookupState )
  {
    setState( tr( "Connecting to %1" ).arg( mProviderName ), State::Connecting );
  }
  else if ( state == QAbstractSocket::SocketState::ConnectedState )
  {
    setState( tr( "Connected" ), State::Connected );
  }
  else if ( state == QAbstractSocket::SocketState::UnconnectedState )
  {
    setState( tr( "No connection" ), State::NoConnection );
    startReconnectTimer();
    // let's also invalidate current position since we no longer have connection
    emit positionChanged( GeoPosition() );
  }
  const QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>( sender() );
  const QString socketTypeToString = QMetaEnum::fromType<QAbstractSocket::SocketType>().valueToKey( socket->socketType() );
  const QString stateToString = QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey( state );
  CoreUtils::log( QStringLiteral( "NetworkPositionProvider" ), QStringLiteral( "%1 Socket changed state, code: %2" ).arg( socketTypeToString, stateToString ) );
}

void NetworkPositionProvider::reconnectTimeout()
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

QAbstractSocket *NetworkPositionProvider::activeSocket() const
{
  if ( mTcpSocket->state() >= QAbstractSocket::ConnectingState && mTcpSocket->state() <= QAbstractSocket::BoundState )
  {
    return mTcpSocket.get();
  }

  if ( mUdpSocket->state() >= QAbstractSocket::ConnectingState && mUdpSocket->state() <= QAbstractSocket::BoundState )
  {
    return mUdpSocket.get();
  }

//TODO: wouldn't it be better to guard against nullptr in caller and return nullptr here?
  return new QAbstractSocket( QAbstractSocket::UnknownSocketType, nullptr );
}

void NetworkPositionProvider::reconnect()
{
  mReconnectTimer.stop();

  setState( tr( "Reconnecting" ), State::Connecting );

  CoreUtils::log( QStringLiteral( "NetworkPositionProvider" ), QStringLiteral( "Reconnecting to %1" ).arg( mProviderName ) );

  stopUpdates();
  startUpdates();
}

void NetworkPositionProvider::startReconnectTimer()
{
  mSecondsLeftToReconnect = mReconnectDelay / ONE_SECOND_MS;
  setState( tr( "No connection, reconnecting in (%1)" ).arg( mSecondsLeftToReconnect ), State::WaitingToReconnect );

  mReconnectTimer.start();

// first time do reconnect in short time, then each other in long time
  if ( mReconnectDelay == NetworkPositionProvider::ShortDelay )
  {
    mReconnectDelay = NetworkPositionProvider::LongDelay;
  }
}
