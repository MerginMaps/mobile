/***************************************************************************
     testqgspositionkit.cpp
     --------------------------------------
  Date                 : May 2018
  Copyright            : (C) 2017 by Viktor Sklencar
  Email                : vsklencar at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "testpositionkit.h"

#include <QObject>
#include <QApplication>
#include <QDesktopWidget>
#include <QSignalSpy>

#include "qgsapplication.h"
#include "appsettings.h"
#include "position/positionkit.h"
#include "position/simulatedpositionprovider.h"
#include "position/bluetoothpositionprovider.h"
#include "position/internalpositionprovider.h"
#include "position/mapposition.h"
#include "position/positiondirection.h"
#include "position/positionprovidersmodel.h"

#include "testutils.h"

void TestPositionKit::init()
{
  qRegisterMetaType< GeoPosition >( "GeoPosition" );
  qRegisterMetaType< AbstractPositionProvider::State >( "State" );
}

void TestPosition::simulatedPosition()
{
  QVERIFY( !positionKit.positionProvider() );

  SimulatedPositionProvider *simulatedProvider = new SimulatedPositionProvider( -92.36, 38.93, 0 );

  positionKit.setPositionProvider( simulatedProvider ); // ownership of provider is passed to positionkit

  QVERIFY( positionKit.positionProvider() );
  simulatedProvider = nullptr;

  QSignalSpy positionKitSpy( &positionKit, &PositionKit::positionChanged );
  bool hasPositionChanged = positionKitSpy.wait( 2000 );

  QVERIFY( hasPositionChanged );
  QVERIFY( positionKit.hasPosition() );

  COMPARENEAR( positionKit.positionCoordinate().y(), 38.93, 1e-4 );
  QVERIFY( positionKit.horizontalAccuracy() > 0 );
  QVERIFY( positionKit.satellitesVisible() >= 0 );
  QVERIFY( positionKit.satellitesUsed() >= 0 );

  SimulatedPositionProvider *simulatedProvider2 = new SimulatedPositionProvider( 90.36, 33.93, 0 );

  // position kit ignores new provider if it is the same type and id, so delete the previous one first
  positionKit.setPositionProvider( nullptr ); // deletes the first provider
  positionKit.setPositionProvider( simulatedProvider2 );
  simulatedProvider2 = nullptr;

  hasPositionChanged = positionKitSpy.wait( 2000 );

  QVERIFY( hasPositionChanged );
  QVERIFY( positionKit.hasPosition() );
  COMPARENEAR( positionKit.positionCoordinate().y(), 33.93, 1e-4 );

  positionKit.stopUpdates();

  positionKit.setPositionProvider( nullptr );
  QVERIFY( !positionKit.positionProvider() );
}

void TestPosition::testBluetoothProviderConnection()
{
  BluetoothPositionProvider *btProvider = new BluetoothPositionProvider( "AA:AA:AA:AA:00:00", "testBluetoothProvider" );

  positionKit.setPositionProvider( btProvider ); // positionKit takes ownership of this provider

  AbstractPositionProvider *pkProvider = positionKit.positionProvider();

  // let's make sure that we have correct provider assigned
  QCOMPARE( "testBluetoothProvider", btProvider->name() );
  QCOMPARE( "testBluetoothProvider", pkProvider->name() );
  QCOMPARE( "AA:AA:AA:AA:00:00", btProvider->id() );
  QCOMPARE( "AA:AA:AA:AA:00:00", pkProvider->id() );
  QCOMPARE( "external", btProvider->type() );
  QCOMPARE( "external", pkProvider->type() );

  //
  // let's continue with BT instance,
  // grab it's socket
  //

  QBluetoothSocket *socket = btProvider->socket();

  // emit different signals on the socket and test if the internal state of out provider changes accordingly

  QSignalSpy providerSpy( btProvider, &AbstractPositionProvider::stateChanged );

  emit socket->error( QBluetoothSocket::NetworkError );
  QCOMPARE( AbstractPositionProvider::NoConnection, btProvider->state() );
  QVERIFY( providerSpy.count() > 1 );

  emit socket->stateChanged( QBluetoothSocket::ConnectingState );
  QCOMPARE( AbstractPositionProvider::Connecting, btProvider->state() );
  QVERIFY( providerSpy.count() > 2 );

  emit socket->stateChanged( QBluetoothSocket::ConnectedState );
  QCOMPARE( AbstractPositionProvider::Connected, btProvider->state() );
  QVERIFY( providerSpy.count() > 3 );

  emit socket->stateChanged( QBluetoothSocket::UnconnectedState );
  QCOMPARE( AbstractPositionProvider::NoConnection, btProvider->state() );
  QCOMPARE( "Could not connect to device, not paired", btProvider->stateMessage() );
  QVERIFY( providerSpy.count() > 4 );

  // position kit should have its position invalidated
  QVERIFY( !positionKit.hasPosition() );

  //
  // test reconnection logic
  //
  // these are more like approximations ..
  // no one knows how long it takes to reconnect (from connecting -> connected or no connection)
  // we will rather focus on if sufficient number of signals were called with correct arguments
  //

  providerSpy.clear();

  // each second a new message should be emitted to indicate timeout
  QSignalSpy providerCountdownSpy( btProvider, &AbstractPositionProvider::stateMessageChanged );

  btProvider->startReconnectionTime();

  // countdown should emit at least 3 message changed signals -> 3,2,1 + message Connecting ... (optionally NoConnection)
  providerCountdownSpy.wait( 5000 );
  QVERIFY( providerCountdownSpy.count() > 3 );

  // state should have changed to WaitingToReconnect and Connecting ... (optionally NoConnection)
  QVERIFY( providerSpy.count() >= 2 );

  // let's inspect if there really were correct states
  bool foundWaitingState = false;
  bool foundConnectingState = false;
  for ( const auto &signal : providerSpy )
  {
    if ( signal.at( 0 ).value<AbstractPositionProvider::State>() == AbstractPositionProvider::WaitingToReconnect )
    {
      foundWaitingState = true;
    }
    if ( signal.at( 0 ).value<AbstractPositionProvider::State>() == AbstractPositionProvider::Connecting )
    {
      foundConnectingState = true;
    }
  }

  QVERIFY( foundWaitingState );
  QVERIFY( foundConnectingState );
}

void TestPosition::testBluetoothProviderPosition()
{
  // read nmea sentences from test file and make sure that position kit has correct position
}

void TestPosition::testPositionProviderKeysInSettings()
{
  // look to qsettings to see if the keys are being correctly saved
  // test position providers model!
}

void TestPositionKit::testMapPosition()
{

}

void TestPositionKit::testPositionDirection()
{

}
