/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "networkpositionprovider.h"

#include <QApplication>
#include <QNetworkDatagram>

static int ONE_SECOND_MS = 1000;

NetworkPositionProvider::NetworkPositionProvider( const QString &addr, const QString &name, PositionTransformer &positionTransformer, QObject *parent )
  : AbstractPositionProvider( addr, QStringLiteral( "external_ip" ), name, positionTransformer, parent ),
    mSecondsLeftToReconnect( ReconnectDelay::ShortDelay / ONE_SECOND_MS )
{
  const QStringList targetAddress = addr.split( ":" );
  mTargetAddress = targetAddress.at( 0 );
  mTargetPort = targetAddress.at( 1 ).toInt();

  mTcpSocket = std::make_unique<QTcpSocket>();
  mUdpSocket = std::make_unique<QUdpSocket>();

  connect( mTcpSocket.get(), &QTcpSocket::readyRead, this, &NetworkPositionProvider::positionUpdateReceived );
  connect( mUdpSocket.get(), &QUdpSocket::readyRead, this, &NetworkPositionProvider::positionUpdateReceived );

  connect( mTcpSocket.get(), &QTcpSocket::stateChanged, this, &NetworkPositionProvider::socketStateChanged );
  connect( mUdpSocket.get(), &QUdpSocket::stateChanged, this, &NetworkPositionProvider::socketStateChanged );

  mReconnectTimer.setSingleShot( false );
  mReconnectTimer.setInterval( ONE_SECOND_MS );
  connect( &mReconnectTimer, &QTimer::timeout, this, &NetworkPositionProvider::reconnectTimeout );
  mUdpReconnectTimer.setSingleShot( true );
  connect( &mUdpReconnectTimer, &QTimer::timeout, this, [this]
  {
    if ( mTcpSocket->state() != QAbstractSocket::ConnectedState )
    {
      setState( tr( "No connection" ), State::NoConnection );
      startReconnectTimer();
      // let's also invalidate current position since we no longer have connection
      emit positionChanged( GeoPosition() );
    }
  } );

  NetworkPositionProvider::startUpdates();
}

void NetworkPositionProvider::startUpdates()
{
  // TODO: QHostAddress doesn't support hostname lookup (QHostInfo does)
  mTcpSocket->connectToHost( mTargetAddress, mTargetPort );
  mUdpSocket->bind( QHostAddress::LocalHost, mTargetPort );
  mUdpReconnectTimer.start( ReconnectDelay::ExtraLongDelay );
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
  QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>( sender() );
  const QString socketTypeToString = QMetaEnum::fromType<QAbstractSocket::SocketType>().valueToKey( socket->socketType() );

  // if udp is not connected to the host yet, connect
  // this approach will let us use QIODevice functions for both sockets
  if ( socket->socketType() == QAbstractSocket::UdpSocket && mUdpSocket->state() != QAbstractSocket::ConnectedState )
  {
    mUdpReconnectTimer.stop();

    // if by any chance we showed wrong message in the status like "no connection", fix it here
    // we know the connection is working because we just received data from the device
    setState( tr( "Connected" ), State::Connected );

    QHostAddress peerAddress;
    int peerPort;
    // process the incoming data as it will break the signal emitting if unprocessed
    while ( mUdpSocket->hasPendingDatagrams() )
    {
      QNetworkDatagram datagram = mUdpSocket->receiveDatagram();
      peerAddress = datagram.senderAddress();
      peerPort = datagram.senderPort();
      const QByteArray rawNmeaData = datagram.data();
      const QString nmeaData( rawNmeaData );
      const QgsGpsInformation gpsInfo = mNmeaParser.parseNmeaString( nmeaData );
      emit positionChanged( GeoPosition::fromQgsGpsInformation( gpsInfo ) );
    }

    // "connect" to peer if we are not already connecting
    if ( mUdpSocket->state() != QAbstractSocket::ConnectedState && mUdpSocket->state() != QAbstractSocket::ConnectingState )
    {
      mUdpSocket->connectToHost( peerAddress.toString(), peerPort );
    }
    return;
  }

  // stop the UDP silence timer, we just received data
  // kills the timer when the app was minimized, and we were able to reconnect in the meantime
  if ( socket->socketType() == QAbstractSocket::UdpSocket )
  {
    mUdpReconnectTimer.stop();
  }

  // if by any chance we showed wrong message in the status like "no connection", fix it here
  // we know the connection is working because we just received data from the device
  setState( tr( "Connected" ), State::Connected );

  const QByteArray rawNmeaData = socket->readAll();
  const QString nmeaData( rawNmeaData );
  const QgsGpsInformation gpsInfo = mNmeaParser.parseNmeaString( nmeaData );

  emit positionChanged( GeoPosition::fromQgsGpsInformation( gpsInfo ) );
}

void NetworkPositionProvider::socketStateChanged( const QAbstractSocket::SocketState state )
{
  const QAbstractSocket *socket = dynamic_cast<QAbstractSocket *>( sender() );

  if ( state == QAbstractSocket::ConnectingState || state == QAbstractSocket::HostLookupState )
  {
    setState( tr( "Connecting to %1" ).arg( mProviderName ), State::Connecting );
  }
  // Only with TCP we can be sure in ConnectedState that we are connected, with UDP we wait until the first datagram arrives
  else if ( state == QAbstractSocket::ConnectedState  && socket->socketType() == QAbstractSocket::TcpSocket )
  {
    setState( tr( "Connected" ), State::Connected );
  }
  else if ( state == QAbstractSocket::UnconnectedState )
  {
    const bool isUdpSocketListening = mUdpSocket->state() == QAbstractSocket::ConnectedState || mUdpSocket->state() == QAbstractSocket::BoundState || mUdpReconnectTimer.isActive();
    if ( socket->socketType() == QAbstractSocket::TcpSocket && !isUdpSocketListening && QApplication::applicationState() == Qt::ApplicationActive )
    {
      setState( tr( "No connection" ), State::NoConnection );
      startReconnectTimer();
      // let's also invalidate current position since we no longer have connection
      emit positionChanged( GeoPosition() );
    }
    else if ( socket->socketType() == QAbstractSocket::UdpSocket && QApplication::applicationState() == Qt::ApplicationActive )
    {
      setState( tr( "No connection" ), State::NoConnection );
      startReconnectTimer();
      // let's also invalidate current position since we no longer have connection
      emit positionChanged( GeoPosition() );
    }
  }

  const QString socketTypeToString = QMetaEnum::fromType<QAbstractSocket::SocketType>().valueToKey( socket->socketType() );
  const QString stateToString = QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey( state );
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

void NetworkPositionProvider::reconnect()
{
  mReconnectTimer.stop();

  setState( tr( "Reconnecting" ), State::Connecting );

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
