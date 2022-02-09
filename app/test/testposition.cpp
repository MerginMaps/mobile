/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "testposition.h"

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
#include "position/positionprovidersmodel.h"

#include "testutils.h"

#include "qgsquickmapcanvasmap.h"
#include "qgsvectorlayer.h"

TestPosition::TestPosition( PositionKit *kit, QObject *parent ) : QObject( parent )
{
  positionKit = kit;
}

void TestPosition::init()
{
  qRegisterMetaType< GeoPosition >( "GeoPosition" );
  qRegisterMetaType< AbstractPositionProvider::State >( "State" );
  qRegisterMetaType< QgsPoint >( "QgsPoint" );

  positionKit->setPositionProvider( nullptr );
}

void TestPosition::simulatedPosition()
{
  QVERIFY( !positionKit->positionProvider() );

  SimulatedPositionProvider *simulatedProvider = new SimulatedPositionProvider( -92.36, 38.93, 0 );

  positionKit->setPositionProvider( simulatedProvider ); // ownership of provider is passed to positionkit

  QVERIFY( positionKit->positionProvider() );
  simulatedProvider = nullptr;

  QSignalSpy positionKitSpy( positionKit, &PositionKit::positionChanged );
  bool hasPositionChanged = positionKitSpy.wait( 2000 );

  QVERIFY( hasPositionChanged );
  QVERIFY( positionKit->hasPosition() );

  COMPARENEAR( positionKit->positionCoordinate().y(), 38.93, 1e-4 );
  QVERIFY( positionKit->horizontalAccuracy() >= 0 );
  QVERIFY( positionKit->satellitesVisible() >= 0 );
  QVERIFY( positionKit->satellitesUsed() >= 0 );

  SimulatedPositionProvider *simulatedProvider2 = new SimulatedPositionProvider( 90.36, 33.93, 0 );

  // position kit ignores new provider if it is the same type and id, so delete the previous one first
  positionKit->setPositionProvider( nullptr ); // deletes the first provider
  positionKit->setPositionProvider( simulatedProvider2 );
  simulatedProvider2 = nullptr;

  hasPositionChanged = positionKitSpy.wait( 2000 );

  QVERIFY( hasPositionChanged );
  QVERIFY( positionKit->hasPosition() );
  COMPARENEAR( positionKit->positionCoordinate().y(), 33.93, 1e-4 );

  positionKit->stopUpdates();

  positionKit->setPositionProvider( nullptr );
  QVERIFY( !positionKit->positionProvider() );
}

void TestPosition::testBluetoothProviderConnection()
{
  BluetoothPositionProvider *btProvider = new BluetoothPositionProvider( "AA:AA:AA:AA:00:00", "testBluetoothProvider" );

  positionKit->setPositionProvider( btProvider ); // positionKit takes ownership of this provider

  AbstractPositionProvider *pkProvider = positionKit->positionProvider();

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

  emit socket->stateChanged( QBluetoothSocket::ConnectingState );
  QCOMPARE( AbstractPositionProvider::Connecting, btProvider->state() );
  QVERIFY( providerSpy.count() > 0 );

  emit socket->stateChanged( QBluetoothSocket::ConnectedState );
  QCOMPARE( AbstractPositionProvider::Connected, btProvider->state() );
  QVERIFY( providerSpy.count() > 1 );

  emit socket->stateChanged( QBluetoothSocket::UnconnectedState );
  QCOMPARE( AbstractPositionProvider::NoConnection, btProvider->state() );
  QCOMPARE( "Could not connect to device, not paired", btProvider->stateMessage() );
  QVERIFY( providerSpy.count() > 2 );

  // position kit should have its position invalidated
  QVERIFY( !positionKit->hasPosition() );

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

  // non-blocking wait
  QEventLoop loop;
  QTimer t;
  t.connect( &t, &QTimer::timeout, &loop, &QEventLoop::quit );
  t.start( 5000 );
  loop.exec();

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
  //
  // read nmea sentences from test file and make sure that position kit has correct position
  // NOTE: If you want to read NMEA sentences from file, make sure that files has CRLF line endings!
  //

  BluetoothPositionProvider *btProvider = new BluetoothPositionProvider( "AA:AA:FF:AA:00:10", "testBluetoothProvider" );

  positionKit->setPositionProvider( btProvider ); // positionKit takes ownership of this provider

  // mini file contains only minimal info like position and date
  QString miniNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile miniNmeaFile( miniNmeaPositionFilePath );
  miniNmeaFile.open( QFile::ReadOnly );

  QVERIFY( miniNmeaFile.isOpen() );

  NmeaParser parser;
  QgsGpsInformation position = parser.parseNmeaString( miniNmeaFile.readAll() );
  emit btProvider->positionChanged( GeoPosition::fromQgsGpsInformation( position ) );

  // test if position kit has correct information
  QVERIFY( qgsDoubleNear( positionKit->latitude(), 48.10305 ) );
  QVERIFY( qgsDoubleNear( positionKit->longitude(), 17.1064 ) );
  QCOMPARE( positionKit->horizontalAccuracy(), -1 );
  QCOMPARE( positionKit->verticalAccuracy(), -1 );
  QCOMPARE( positionKit->altitude(), 171.3 );
  QCOMPARE( positionKit->speed(), -1 );
  QCOMPARE( positionKit->hdop(), -1 );
  QCOMPARE( positionKit->fix(), "GPS fix, no correction data" );

  // let's invalidate previous data
  emit btProvider->positionChanged( GeoPosition() );

  // full file contains position, accuracy, fix, speed, hdop and similar
  QString fullNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_full.txt";
  QFile fullNmeaFile( fullNmeaPositionFilePath );
  fullNmeaFile.open( QFile::ReadOnly );

  QVERIFY( fullNmeaFile.isOpen() );

  position = parser.parseNmeaString( fullNmeaFile.readAll() );
  emit btProvider->positionChanged( GeoPosition::fromQgsGpsInformation( position ) );

  // test if position kit has correct information
  QCOMPARE( positionKit->latitude(), 48.10313552 );
  QVERIFY( qgsDoubleNear( positionKit->longitude(), 17.1059, 0.0001 ) );
  QVERIFY( qgsDoubleNear( positionKit->horizontalAccuracy(), 0.0257, 0.0001 ) );
  QCOMPARE( positionKit->verticalAccuracy(), 0.041 );
  QCOMPARE( positionKit->altitude(), 153.026 );
  QCOMPARE( positionKit->speed(), 0.05 );
  QCOMPARE( positionKit->hdop(), 3.2 );
  QCOMPARE( positionKit->satellitesUsed(), 9 );
  QCOMPARE( positionKit->fix(), "RTK float" );
  QCOMPARE( positionKit->lastRead(), QDateTime().fromString( "2022-01-31T12:17:17Z", Qt::ISODate ) );
}

void TestPosition::testPositionProviderKeysInSettings()
{
  //
  // Look to QSettings to see if the keys are being correctly saved and read
  //

  QSettings rawSettings;
  rawSettings.remove( AppSettings::POSITION_PROVIDERS_GROUP ); // make sure nothing is there from previous tests

  positionKit->setPositionProvider( positionKit->constructProvider( "external", "AA:BB:CC:DD:EE:FF", "testProviderA" ) );

  QCOMPARE( positionKit->positionProvider()->id(), "AA:BB:CC:DD:EE:FF" );
  QCOMPARE( positionKit->positionProvider()->name(), "testProviderA" );
  QCOMPARE( positionKit->positionProvider()->type(), "external" );

  QCOMPARE( rawSettings.value( AppSettings::GROUP_NAME + "/activePositionProviderId" ).toString(), "AA:BB:CC:DD:EE:FF" );

  positionKit->setPositionProvider( positionKit->constructProvider( "internal", "devicegps" ) );

  QCOMPARE( rawSettings.value( AppSettings::GROUP_NAME + "/activePositionProviderId" ).toString(), "devicegps" );

  // even without appSettings provider model should have two items in desktop build: simulated and internal provider
  PositionProvidersModel providersModel;
  AppSettings appSettings;

  QCOMPARE( providersModel.rowCount(), 2 );
  QCOMPARE( providersModel.data( providersModel.index( 0 ), PositionProvidersModel::ProviderId ), "devicegps" );
  QCOMPARE( providersModel.data( providersModel.index( 1 ), PositionProvidersModel::ProviderId ), "simulated" );

  providersModel.setAppSettings( &appSettings );
  providersModel.addProvider( "testProviderB", "AA:00:11:22:23:44" );

  // app settings should have one saved provider - testProviderB
  QVariantList providers = appSettings.savedPositionProviders();

  QCOMPARE( providers.count(), 1 ); // we have one (external) provider
  QCOMPARE( providers.at( 0 ).toList().count(), 2 ); // the provider has two properties

  QVariantList providerData = providers.at( 0 ).toList();
  QCOMPARE( providerData.at( 0 ).toString(), "testProviderB" );
  QCOMPARE( providerData.at( 1 ).toString(), "AA:00:11:22:23:44" );

  // remove that provider
  providersModel.removeProvider( "AA:00:11:22:23:44" );

  providers = appSettings.savedPositionProviders();

  QVERIFY( providers.isEmpty() );
}

void TestPosition::testMapPosition()
{
  //
  // Test if map canvas properties (position on map) change when extent changes
  //

  // Create map settings
  QgsCoordinateReferenceSystem crsGPS = QgsCoordinateReferenceSystem::fromEpsgId( 5514 );
  QVERIFY( crsGPS.authid() == "EPSG:5514" );

  QgsRectangle extent = QgsRectangle( -120, 23, -82, 47 );
  QgsQuickMapCanvasMap canvas;

  QgsVectorLayer *tempLayer = new QgsVectorLayer( QStringLiteral( "Point?crs=epsg:5514" ), QStringLiteral( "vl" ), QStringLiteral( "memory" ) );
  QVERIFY( tempLayer->isValid() );

  QgsQuickMapSettings *ms = canvas.mapSettings();
  ms->setDestinationCrs( crsGPS );
  ms->setExtent( extent );
  ms->setOutputSize( QSize( 1000, 500 ) );
  ms->setLayers( QList<QgsMapLayer *>() << tempLayer );

  // Create position kit provider
  SimulatedPositionProvider *provider = new SimulatedPositionProvider( 17.1, 48.1, 0 );
  positionKit->setPositionProvider( provider );

  // Create MapPosition
  MapPosition mapPosition;

  QVERIFY( mapPosition.mapPosition().isEmpty() );

  mapPosition.setMapSettings( ms );
  mapPosition.setPositionKit( positionKit );

  // Let's give simulated provider some time to generate first location
  QSignalSpy positionUpdateSpy( provider, &AbstractPositionProvider::positionChanged );
  positionUpdateSpy.wait( 3000 );

  QVERIFY( !mapPosition.mapPosition().isEmpty() );

  QgsPoint oldmappos = mapPosition.mapPosition();

  QSignalSpy mpSpy( &mapPosition, &MapPosition::mapPositionChanged );

  // Change extent and see if map position changes
  ms->setExtent( QgsRectangle( -120, 23, -82, 40 ) );

  QVERIFY( mpSpy.isEmpty() ); // changing extent does not change position printed on map
  QVERIFY( mapPosition.mapPosition() == oldmappos );

  // Now let's assign a not stationary provider
  positionKit->setPositionProvider( nullptr );
  SimulatedPositionProvider *provider2 = new SimulatedPositionProvider( 15.1, 48.1, 1, 500 );
  positionKit->setPositionProvider( provider2 );

  QSignalSpy positionUpdateSpy2( provider2, &AbstractPositionProvider::positionChanged );
  positionUpdateSpy2.wait( 1500 );

  QVERIFY( !positionUpdateSpy2.isEmpty() );
  QVERIFY( mapPosition.mapPosition() != oldmappos );
}
