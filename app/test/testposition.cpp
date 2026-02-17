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
#include <QScreen>
#include <QSignalSpy>

#include "qgsapplication.h"
#include "appsettings.h"
#include "coreutils.h"
#include "position/positionkit.h"
#include "position/providers/simulatedpositionprovider.h"

#ifdef HAVE_BLUETOOTH
#include "position/providers/bluetoothpositionprovider.h"
#endif

#include "position/providers/internalpositionprovider.h"
#include "position/providers/positionprovidersmodel.h"
#include "position/mapposition.h"

#include "position/tracking/positiontrackingmanager.h"
#include "position/tracking/internaltrackingbackend.h"
#include "position/tracking/positiontrackinghighlight.h"

#include "testutils.h"

#include "inputmapcanvasmap.h"
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

  SimulatedPositionProvider *simulatedProvider = new SimulatedPositionProvider( *positionKit->mPositionTransformer, -92.36, 38.93, 0 );

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

  SimulatedPositionProvider *simulatedProvider2 = new SimulatedPositionProvider( *positionKit->mPositionTransformer, 90.36, 33.93, 0 );

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

#ifdef HAVE_BLUETOOTH

void TestPosition::testBluetoothProviderConnection()
{
  BluetoothPositionProvider *btProvider = new BluetoothPositionProvider( "AA:AA:AA:AA:00:00", "testBluetoothProvider", *positionKit->mPositionTransformer );

  positionKit->setPositionProvider( btProvider ); // positionKit takes ownership of this provider

  AbstractPositionProvider *pkProvider = positionKit->positionProvider();

  // let's make sure that we have correct provider assigned
  QCOMPARE( "testBluetoothProvider", btProvider->name() );
  QCOMPARE( "testBluetoothProvider", pkProvider->name() );
  QCOMPARE( "AA:AA:AA:AA:00:00", btProvider->id() );
  QCOMPARE( "AA:AA:AA:AA:00:00", pkProvider->id() );
  QCOMPARE( "external_bt", btProvider->type() );
  QCOMPARE( "external_bt", pkProvider->type() );

  //
  // let's continue with BT instance,
  // grab it's socket
  //

  QBluetoothSocket *socket = btProvider->socket();

  // emit different signals on the socket and test if the internal state of out provider changes accordingly

  QSignalSpy providerSpy( btProvider, &AbstractPositionProvider::stateChanged );

  emit socket->errorOccurred( QBluetoothSocket::SocketError::NetworkError );
  QCOMPARE( AbstractPositionProvider::NoConnection, btProvider->state() );

  emit socket->stateChanged( QBluetoothSocket::SocketState::ConnectingState );
  QCOMPARE( AbstractPositionProvider::Connecting, btProvider->state() );
  QVERIFY( providerSpy.count() > 0 );

  emit socket->stateChanged( QBluetoothSocket::SocketState::ConnectedState );
  QCOMPARE( AbstractPositionProvider::Connected, btProvider->state() );
  QVERIFY( providerSpy.count() > 1 );

  emit socket->stateChanged( QBluetoothSocket::SocketState::UnconnectedState );
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

  BluetoothPositionProvider *btProvider = new BluetoothPositionProvider( "AA:AA:FF:AA:00:10", "testBluetoothProvider", *positionKit->mPositionTransformer );

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
  QCOMPARE( positionKit->altitude() + positionKit->antennaHeight(), 171.3 );
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
  QCOMPARE( positionKit->altitude() + positionKit->antennaHeight(), 153.026 );
  QCOMPARE( positionKit->speed(), 0.05 );
  QCOMPARE( positionKit->hdop(), 3.2 );
  QVERIFY( positionKit->satellitesUsed() > 3 );
  QCOMPARE( positionKit->fix(), "RTK float" );
  QCOMPARE( positionKit->lastRead(), QDateTime().fromString( "2022-01-31T12:17:17Z", Qt::ISODate ) );
}

#endif

void TestPosition::testPositionProviderKeysInSettings()
{
  //
  // Look to QSettings to see if the keys are being correctly saved and read
  //

  QSettings rawSettings;
  rawSettings.remove( AppSettings::POSITION_PROVIDERS_GROUP ); // make sure nothing is there from previous tests

#ifdef HAVE_BLUETOOTH
  positionKit->setPositionProvider( positionKit->constructProvider( "external_bt", "AA:BB:CC:DD:EE:FF", "testProviderA" ) );

  QCOMPARE( positionKit->positionProvider()->id(), "AA:BB:CC:DD:EE:FF" );
  QCOMPARE( positionKit->positionProvider()->name(), "testProviderA" );
  QCOMPARE( positionKit->positionProvider()->type(), "external_bt" );

  QCOMPARE( rawSettings.value( CoreUtils::QSETTINGS_APP_GROUP_NAME + "/activePositionProviderId" ).toString(), "AA:BB:CC:DD:EE:FF" );
#endif

  positionKit->setPositionProvider( positionKit->constructProvider( "internal", "devicegps" ) );

  QCOMPARE( rawSettings.value( CoreUtils::QSETTINGS_APP_GROUP_NAME + "/activePositionProviderId" ).toString(), "devicegps" );

  // even without appSettings provider model should have two items in desktop build: simulated and internal provider
  PositionProvidersModel providersModel;
  AppSettings appSettings;

  QCOMPARE( providersModel.rowCount(), 2 );
  QCOMPARE( providersModel.data( providersModel.index( 0 ), PositionProvidersModel::ProviderId ), "devicegps" );
  QCOMPARE( providersModel.data( providersModel.index( 1 ), PositionProvidersModel::ProviderId ), "simulated" );

  providersModel.setAppSettings( &appSettings );
  providersModel.addProvider( "testProviderB", "AA:00:11:22:23:44", "external_bt" );
  providersModel.addProvider( "testProviderC", "localhost:9000", "external_ip" );

  // app settings should have two saved providers - testProviderB & testProviderC
  QVariantList providers = appSettings.savedPositionProviders();

  QCOMPARE( providers.count(), 2 ); // we have two (external) providers
  QCOMPARE( providers.at( 0 ).toList().count(), 3 ); // the provider has two properties

  QVariantList providerData = providers.at( 0 ).toList();
  QCOMPARE( providerData.at( 0 ).toString(), "testProviderB" );
  QCOMPARE( providerData.at( 1 ).toString(), "AA:00:11:22:23:44" );
  QCOMPARE( providerData.at( 2 ).toString(), "external_bt" );

  providerData = providers.at( 1 ).toList();
  QCOMPARE( providerData.at( 0 ).toString(), "testProviderC" );
  QCOMPARE( providerData.at( 1 ).toString(), "localhost:9000" );
  QCOMPARE( providerData.at( 2 ).toString(), "external_ip" );

  // remove that provider
  providersModel.removeProvider( "AA:00:11:22:23:44" );
  providersModel.removeProvider( "localhost:9000" );

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
  InputMapCanvasMap canvas;

  QgsVectorLayer *tempLayer = new QgsVectorLayer( QStringLiteral( "Point?crs=epsg:5514" ), QStringLiteral( "vl" ), QStringLiteral( "memory" ) );
  QVERIFY( tempLayer->isValid() );

  InputMapSettings *ms = canvas.mapSettings();
  ms->setDestinationCrs( crsGPS );
  ms->setExtent( extent );
  ms->setOutputSize( QSize( 1000, 500 ) );
  ms->setLayers( QList<QgsMapLayer *>() << tempLayer );

  // Create position kit provider
  SimulatedPositionProvider *provider = new SimulatedPositionProvider( *positionKit->mPositionTransformer, 17.1, 48.1, 0 );
  positionKit->setPositionProvider( provider );

  // Create MapPosition
  MapPosition mapPosition;

  QVERIFY( mapPosition.mapPosition().isEmpty() );

  mapPosition.setMapSettings( ms );
  mapPosition.setPositionKit( positionKit );

  // Let's give simulated provider some time to generate first location
  QSignalSpy positionUpdateSpy( provider, &AbstractPositionProvider::positionChanged );
  positionUpdateSpy.wait( 5000 );

  QVERIFY( !mapPosition.mapPosition().isEmpty() );

  QgsPoint oldmappos = mapPosition.mapPosition();

  QSignalSpy mpSpy( &mapPosition, &MapPosition::mapPositionChanged );

  // Change extent and see if map position changes
  ms->setExtent( QgsRectangle( -120, 23, -82, 40 ) );

  QVERIFY( mpSpy.isEmpty() ); // changing extent does not change position printed on map
  QVERIFY( mapPosition.mapPosition() == oldmappos );

  // Now let's assign a not stationary provider
  positionKit->setPositionProvider( nullptr );
  SimulatedPositionProvider *provider2 = new SimulatedPositionProvider( *positionKit->mPositionTransformer, 15.1, 48.1, 1, 500 );
  positionKit->setPositionProvider( provider2 );

  QSignalSpy positionUpdateSpy2( provider2, &AbstractPositionProvider::positionChanged );
  positionUpdateSpy2.wait( 5000 );

  QVERIFY( !positionUpdateSpy2.isEmpty() );
  QVERIFY( mapPosition.mapPosition() != oldmappos );
}

void TestPosition::testPositionTracking()
{
  // test adding points to tracking geometry with simulated provider

  QString projectDir = TestUtils::testDataDir() + "/tracking";
  QString projectName = "tracking-project.qgz";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QVERIFY( !PositionTrackingManager::constructTrackingBackend( QgsProject::instance(), nullptr ) ); // should return null without pk

  SimulatedPositionProvider *simulatedProvider = new SimulatedPositionProvider( *positionKit->mPositionTransformer, -92.36, 38.93, 0 );
  positionKit->setPositionProvider( simulatedProvider ); // ownership of the provider is passed to pk
  simulatedProvider = nullptr;

  QVERIFY( positionKit->positionProvider() );

  PositionTrackingManager manager;

  QSignalSpy isTrackingSpy( &manager, &PositionTrackingManager::isTrackingPositionChanged );

  QVERIFY( manager.trackedGeometry().isEmpty() );

  manager.setQgsProject( QgsProject::instance() );
  manager.setTrackingBackend( PositionTrackingManager::constructTrackingBackend( QgsProject::instance(), positionKit ) );

  QVERIFY( manager.trackingBackend() );

  QCOMPARE( isTrackingSpy.count(), 1 );
  QCOMPARE( isTrackingSpy.takeFirst().at( 0 ), true );
  QCOMPARE( manager.isTrackingPosition(), true );

  QSignalSpy trackingSpy( &manager, &PositionTrackingManager::trackedGeometryChanged );

  trackingSpy.wait( 4000 ); // new position should be emited in 2k ms
  QVERIFY( manager.trackedGeometry().asWkt( 3 ).startsWith( QStringLiteral( "LineString ZM (-92.36 38.93 133.331" ) ) );

  // store the geometry
  QgsVectorLayer *trackingLayer = QgsProject::instance()->mapLayer<QgsVectorLayer *>( "tracking_layer_aad89df7_21db_466e_b5c1_a80160f74c01" );
  QVERIFY( trackingLayer );

  QSignalSpy addedSpy( trackingLayer, &QgsVectorLayer::featureAdded );

  manager.commitTrackedPath();

  QCOMPARE( addedSpy.count(), 2 ); // called twice, once with FID_NEW and second time after commit, with fid>0

  int addedFid = addedSpy.at( 1 ).at( 0 ).toInt();
  QgsFeature f = trackingLayer->getFeature( addedFid );
  QVERIFY( f.geometry().asWkt( 3 ).startsWith( QStringLiteral( "LineString ZM (-92.36 38.93 133.331" ) ) );

  QString datetimeFormat = QStringLiteral( "dd.MM.yyyy hh:mm:ss" );
  QString dateTrackingStartedFromManager = manager.startTime().toString( datetimeFormat );
  QString dateTrackingStartedInFeature = f.attribute( QStringLiteral( "tracking_start_time" ) ).toDateTime().toString( datetimeFormat );

  QCOMPARE( dateTrackingStartedFromManager, dateTrackingStartedInFeature );
}

void TestPosition::testPositionTrackingHighlight()
{
  // simulate some tracking path and check if the map position is added correctly to the geometry
  PositionTrackingHighlight trackingHighlight;

  // nothing tracked so far
  QgsGeometry g;
  QgsPoint p = QgsPoint( 10, 10, 10 );

  trackingHighlight.setTrackedGeometry( g );
  trackingHighlight.setMapPosition( p );

  QVERIFY( trackingHighlight.highlightGeometry().isEmpty() );

  // empty linestring
  g = QgsGeometry::fromPolyline( {} );
  p = QgsPoint( 10, 10, 10 );

  trackingHighlight.setTrackedGeometry( g );
  trackingHighlight.setMapPosition( p );

  QVERIFY( trackingHighlight.highlightGeometry().isEmpty() );

  // one point in the tracked geo
  g = QgsGeometry::fromPolyline( { QgsPoint( 5, 5, 5, 5 ) } );
  p = QgsPoint( 10, 10, 10 );

  trackingHighlight.setTrackedGeometry( g );
  trackingHighlight.setMapPosition( p );

  QString result = QStringLiteral( "LineString ZM (5 5 5 5, 10 10 10 nan)" );

  QCOMPARE( trackingHighlight.highlightGeometry().asWkt( 1 ), result );

  // two points in the tracked geo
  g = QgsGeometry::fromPolyline( { QgsPoint( 5, 5, 5, 5 ), QgsPoint( 6, 6, 5, 5 ) } );
  p = QgsPoint( 10, 10, 10 );

  trackingHighlight.setTrackedGeometry( g );
  trackingHighlight.setMapPosition( p );

  result = QStringLiteral( "LineString ZM (5 5 5 5, 6 6 5 5, 10 10 10 nan)" );

  QCOMPARE( trackingHighlight.highlightGeometry().asWkt( 1 ), result );

  // three points in the tracked geo
  g = QgsGeometry::fromPolyline( { QgsPoint( 5, 5, 5, 5 ), QgsPoint( 6, 6, 5, 5 ), QgsPoint( 7, 7, 5, 5 ) } );
  p = QgsPoint( 10, 10, 10 );

  trackingHighlight.setTrackedGeometry( g );
  trackingHighlight.setMapPosition( p );

  result = QStringLiteral( "LineString ZM (5 5 5 5, 6 6 5 5, 7 7 5 5, 10 10 10 nan)" );

  QCOMPARE( trackingHighlight.highlightGeometry().asWkt( 1 ), result );

  // change map position
  p = QgsPoint( 20, 20, 20 );
  trackingHighlight.setMapPosition( p );

  result = QStringLiteral( "LineString ZM (5 5 5 5, 6 6 5 5, 7 7 5 5, 20 20 20 nan)" );
  QCOMPARE( trackingHighlight.highlightGeometry().asWkt( 1 ), result );

  // lost map position
  p = QgsPoint();
  trackingHighlight.setMapPosition( p );

  QVERIFY( trackingHighlight.highlightGeometry().isEmpty() );
}

void TestPosition::testPositionTransformerAndroidPosition()
{
  // prepare position transformers
  // WGS84 + ellipsoid
  QgsCoordinateReferenceSystem ellipsoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 4979 );
  // WGS84 + EGM96
  QgsCoordinateReferenceSystem geoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 9707 );
  PositionTransformer passThroughTransformer( ellipsoidHeightCrs, geoidHeightCrs, true, QgsCoordinateTransformContext() );
  PositionTransformer positionTransformer( ellipsoidHeightCrs, geoidHeightCrs, false, QgsCoordinateTransformContext() );

#ifdef HAVE_BLUETOOTH
  // mini file contains only minimal info like position and date
  QString miniNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile miniNmeaFile( miniNmeaPositionFilePath );
  miniNmeaFile.open( QFile::ReadOnly );

  QVERIFY( miniNmeaFile.isOpen() );

  NmeaParser parser;
  QgsGpsInformation position = parser.parseNmeaString( miniNmeaFile.readAll() );
  GeoPosition geoPosition = GeoPosition::fromQgsGpsInformation( position );

  QVERIFY( qgsDoubleNear( geoPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( geoPosition.longitude, 17.1064 ) );
  QCOMPARE( geoPosition.elevation, 171.3 );
  QCOMPARE( geoPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );
#else
  GeoPosition geoPosition;
  geoPosition.latitude = 48.10305;
  geoPosition.longitude = 17.1064;
  geoPosition.elevation = 171.3;
#endif

  // transform with pass through enabled, but position is not mocked
  GeoPosition newPosition = passThroughTransformer.processAndroidPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( newPosition.longitude, 17.1064 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation, 127.53574931171875 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation_diff, 43.764250688281265 ) );

  // transform with pass through enabled, position is mocked
  geoPosition.isMock = true;
  newPosition = passThroughTransformer.processAndroidPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( newPosition.longitude, 17.1064 ) );
  QCOMPARE( newPosition.elevation, 171.3 );
  QCOMPARE( newPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );

  // transform with pass through disabled, position is mocked
  newPosition = positionTransformer.processAndroidPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( newPosition.longitude, 17.1064 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation, 127.53574931171875 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation_diff, 43.764250688281265 ) );
}

void TestPosition::testPositionTransformerBluetoothPosition()
{
  // prepare position transformers
  // WGS84 + ellipsoid
  QgsCoordinateReferenceSystem ellipsoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 4979 );
  // WGS84 + EGM96
  QgsCoordinateReferenceSystem geoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 9707 );
  PositionTransformer passThroughTransformer( ellipsoidHeightCrs, geoidHeightCrs, true, QgsCoordinateTransformContext() );
  PositionTransformer positionTransformer( ellipsoidHeightCrs, geoidHeightCrs, false, QgsCoordinateTransformContext() );

#ifdef  HAVE_BLUETOOTH
  // mini file contains only minimal info like position and date
  QString miniNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile miniNmeaFile( miniNmeaPositionFilePath );
  miniNmeaFile.open( QFile::ReadOnly );

  QVERIFY( miniNmeaFile.isOpen() );

  NmeaParser parser;
  QgsGpsInformation position = parser.parseNmeaString( miniNmeaFile.readAll() );
  GeoPosition geoPosition = GeoPosition::fromQgsGpsInformation( position );

  QVERIFY( qgsDoubleNear( geoPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( geoPosition.longitude, 17.1064 ) );
  QCOMPARE( geoPosition.elevation, 171.3 );
  QCOMPARE( geoPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );
#else
  GeoPosition geoPosition;
  geoPosition.latitude = 48.10305;
  geoPosition.longitude = 17.1064;
  geoPosition.elevation = 171.3;
#endif

  // transform with pass through disabled and missing elevation separation
  GeoPosition newPosition = positionTransformer.processBluetoothPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( newPosition.longitude, 17.1064 ) );
  QCOMPARE( newPosition.elevation, 171.3 );
  QCOMPARE( newPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );

  // transform with pass through enabled and missing elevation separation
  newPosition = passThroughTransformer.processBluetoothPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( newPosition.longitude, 17.1064 ) );
  QCOMPARE( newPosition.elevation, 171.3 );
  QCOMPARE( newPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );

  // transform with pass through enabled and elevation separation
  geoPosition.elevation_diff = 40;
  newPosition = passThroughTransformer.processBluetoothPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( newPosition.longitude, 17.1064 ) );
  QCOMPARE( newPosition.elevation, 171.3 );
  QCOMPARE( newPosition.elevation_diff, 40 );

  // transform with pass through disabled and elevation separation
  newPosition = positionTransformer.processBluetoothPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( newPosition.longitude, 17.1064 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation, 167.53574931171875 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation_diff, 43.764250688281265 ) );
}

void TestPosition::testPositionTransformerInternalAndroidPosition()
{
  // prepare position transformers
  // WGS84 + ellipsoid
  QgsCoordinateReferenceSystem ellipsoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 4979 );
  // WGS84 + EGM96
  QgsCoordinateReferenceSystem geoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 9707 );
  PositionTransformer positionTransformer( ellipsoidHeightCrs, geoidHeightCrs, false, QgsCoordinateTransformContext() );

#ifdef HAVE_BLUETOOTH
  // mini file contains only minimal info like position and date
  QString miniNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile miniNmeaFile( miniNmeaPositionFilePath );
  miniNmeaFile.open( QFile::ReadOnly );

  QVERIFY( miniNmeaFile.isOpen() );

  NmeaParser parser;
  QgsGpsInformation qgsPosition = parser.parseNmeaString( miniNmeaFile.readAll() );
  QGeoCoordinate positionCoordinate( qgsPosition.latitude, qgsPosition.longitude, qgsPosition.elevation );
  QGeoPositionInfo geoPosition( positionCoordinate, QDateTime::currentDateTimeUtc() );

  QVERIFY( qgsDoubleNear( geoPosition.coordinate().latitude(), 48.10305 ) );
  QVERIFY( qgsDoubleNear( geoPosition.coordinate().longitude(), 17.1064 ) );
  QCOMPARE( geoPosition.coordinate().altitude(), 171.3 );
#else
  QGeoCoordinate positionCoordinate( 48.10305, 17.1064, 171.3 );
  QGeoPositionInfo geoPosition( positionCoordinate, QDateTime::currentDateTimeUtc() );
#endif

  GeoPosition newPosition = positionTransformer.processInternalAndroidPosition( geoPosition );
  QVERIFY( qgsDoubleNear( newPosition.elevation, 127.53574931171875 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation_diff, 43.764250688281265 ) );
}

void TestPosition::testPositionTransformerInternalIosPosition()
{
  // prepare position transformers
  // WGS84 + ellipsoid
  QgsCoordinateReferenceSystem ellipsoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 4979 );
  // WGS84 + EGM96
  QgsCoordinateReferenceSystem geoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 9707 );
  PositionTransformer positionTransformer( ellipsoidHeightCrs, geoidHeightCrs, false, QgsCoordinateTransformContext() );
  PositionTransformer passThroughTransformer( ellipsoidHeightCrs, geoidHeightCrs, true, QgsCoordinateTransformContext() );

#ifdef HAVE_BLUETOOTH
  // mini file contains only minimal info like position and date
  QString miniNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile miniNmeaFile( miniNmeaPositionFilePath );
  miniNmeaFile.open( QFile::ReadOnly );

  QVERIFY( miniNmeaFile.isOpen() );

  NmeaParser parser;
  QgsGpsInformation qgsPosition = parser.parseNmeaString( miniNmeaFile.readAll() );
  QGeoCoordinate positionCoordinate( qgsPosition.latitude, qgsPosition.longitude, qgsPosition.elevation );
  QGeoPositionInfo geoPosition( positionCoordinate, QDateTime::currentDateTimeUtc() );

  QVERIFY( qgsDoubleNear( geoPosition.coordinate().latitude(), 48.10305 ) );
  QVERIFY( qgsDoubleNear( geoPosition.coordinate().longitude(), 17.1064 ) );
  QCOMPARE( geoPosition.coordinate().altitude(), 171.3 );
#else
  QGeoCoordinate positionCoordinate( 48.10305, 17.1064, 171.3 );
  QGeoPositionInfo geoPosition( positionCoordinate, QDateTime::currentDateTimeUtc() );
#endif

  // ellipsoid elevation
  geoPosition.setAttribute( QGeoPositionInfo::VerticalSpeed, 1 );
  // position is not mocked and ellipsoid
  GeoPosition newPosition = positionTransformer.processInternalIosPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.elevation, 127.53574931171875 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation_diff, 43.764250688281265 ) );
  QVERIFY( !newPosition.isMock );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::VerticalSpeed ) );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::MagneticVariation ) );


  // position is not mocked and not ellipsoid
  newPosition = positionTransformer.processInternalIosPosition( geoPosition );

  QCOMPARE( newPosition.elevation, 171.3 );
  QCOMPARE( newPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );
  QVERIFY( !newPosition.isMock );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::VerticalSpeed ) );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::MagneticVariation ) );


  // ellipsoid elevation
  geoPosition.setAttribute( QGeoPositionInfo::VerticalSpeed, 1 );
  // mocked
  geoPosition.setAttribute( QGeoPositionInfo::MagneticVariation, 1 );
  // transform with pass through disabled, position is mocked and ellipsoid
  newPosition = positionTransformer.processInternalIosPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.elevation, 127.53574931171875 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation_diff, 43.764250688281265 ) );
  QVERIFY( newPosition.isMock );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::VerticalSpeed ) );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::MagneticVariation ) );


  // mocked
  geoPosition.setAttribute( QGeoPositionInfo::MagneticVariation, 1 );
  // transform with pass through disabled, position is mocked and orthometric
  newPosition = positionTransformer.processInternalIosPosition( geoPosition );

  QCOMPARE( newPosition.elevation, 171.3 );
  QCOMPARE( newPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );
  QVERIFY( newPosition.isMock );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::VerticalSpeed ) );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::MagneticVariation ) );


  // ellipsoid elevation
  geoPosition.setAttribute( QGeoPositionInfo::VerticalSpeed, 1 );
  // mocked
  geoPosition.setAttribute( QGeoPositionInfo::MagneticVariation, 1 );
  // transform with pass through enabled, position is mocked and ellipsoid
  newPosition = passThroughTransformer.processInternalIosPosition( geoPosition );

  QCOMPARE( newPosition.elevation, 171.3 );
  QCOMPARE( newPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );
  QVERIFY( newPosition.isMock );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::VerticalSpeed ) );
  QVERIFY( !geoPosition.hasAttribute( QGeoPositionInfo::MagneticVariation ) );

}

void TestPosition::testPositionTransformerInternalDesktopPosition()
{
  // prepare position transformers
  // WGS84 + ellipsoid
  QgsCoordinateReferenceSystem ellipsoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 4979 );
  // WGS84 + EGM96
  QgsCoordinateReferenceSystem geoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 9707 );
  PositionTransformer positionTransformer( ellipsoidHeightCrs, geoidHeightCrs, false, QgsCoordinateTransformContext() );

#ifdef  HAVE_BLUETOOTH
  // mini file contains only minimal info like position and date
  QString miniNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile miniNmeaFile( miniNmeaPositionFilePath );
  miniNmeaFile.open( QFile::ReadOnly );

  QVERIFY( miniNmeaFile.isOpen() );

  NmeaParser parser;
  QgsGpsInformation qgsPosition = parser.parseNmeaString( miniNmeaFile.readAll() );
  QGeoCoordinate positionCoordinate( qgsPosition.latitude, qgsPosition.longitude, qgsPosition.elevation );
  QGeoPositionInfo geoPosition( positionCoordinate, QDateTime::currentDateTimeUtc() );

  QVERIFY( qgsDoubleNear( geoPosition.coordinate().latitude(), 48.10305 ) );
  QVERIFY( qgsDoubleNear( geoPosition.coordinate().longitude(), 17.1064 ) );
  QCOMPARE( geoPosition.coordinate().altitude(), 171.3 );
#else
  QGeoCoordinate positionCoordinate( 48.10305, 17.1064, 171.3 );
  QGeoPositionInfo geoPosition( positionCoordinate, QDateTime::currentDateTimeUtc() );
#endif

  GeoPosition newPosition = positionTransformer.processInternalDesktopPosition( geoPosition );
  QVERIFY( qgsDoubleNear( newPosition.elevation, 171.3 ) );
}

void TestPosition::testPositionTransformerSimulatedPosition()
{
  // prepare position transformers
  // WGS84 + ellipsoid
  QgsCoordinateReferenceSystem ellipsoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 4979 );
  // WGS84 + EGM96
  QgsCoordinateReferenceSystem geoidHeightCrs = QgsCoordinateReferenceSystem::fromEpsgId( 9707 );
  PositionTransformer positionTransformer( ellipsoidHeightCrs, geoidHeightCrs, false, QgsCoordinateTransformContext() );

#ifdef HAVE_BLUETOOTH
  // mini file contains only minimal info like position and date
  QString miniNmeaPositionFilePath = TestUtils::testDataDir() + "/position/nmea_petrzalka_mini.txt";
  QFile miniNmeaFile( miniNmeaPositionFilePath );
  miniNmeaFile.open( QFile::ReadOnly );

  QVERIFY( miniNmeaFile.isOpen() );

  NmeaParser parser;
  QgsGpsInformation position = parser.parseNmeaString( miniNmeaFile.readAll() );
  GeoPosition geoPosition = GeoPosition::fromQgsGpsInformation( position );

  QVERIFY( qgsDoubleNear( geoPosition.latitude, 48.10305 ) );
  QVERIFY( qgsDoubleNear( geoPosition.longitude, 17.1064 ) );
  QCOMPARE( geoPosition.elevation, 171.3 );
  QCOMPARE( geoPosition.elevation_diff, std::numeric_limits<double>::quiet_NaN() );
#else
  GeoPosition geoPosition;
  geoPosition.latitude = 48.10305;
  geoPosition.longitude = 17.1064;
  geoPosition.elevation = 171.3;
#endif

  GeoPosition newPosition = positionTransformer.processSimulatedPosition( geoPosition );

  QVERIFY( qgsDoubleNear( newPosition.elevation, 127.53574931171875 ) );
  QVERIFY( qgsDoubleNear( newPosition.elevation_diff, 43.764250688281265 ) );
}
