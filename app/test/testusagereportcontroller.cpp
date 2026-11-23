/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testusagereportcontroller.h"
#include "usagereportcontroller.h"
#include "appsettings.h"
#include "coreutils.h"

#include <QtTest/QtTest>
#include <QJsonDocument>
#include <QSettings>
#include <QUuid>

static QString usageReportKey( const QString &key )
{
  return AppSettings::USAGE_REPORT_GROUP + QStringLiteral( "/" ) + key;
}

static QString usageDataKey( const QString &key )
{
  return usageReportKey( AppSettings::USAGE_REPORT_DATA_KEY + QStringLiteral( "/" ) + key );
}

TestUsageReportController::TestUsageReportController( UsageReportController *controller, AppSettings *appSettings )
  : mController( controller )
  , mAppSettings( appSettings )
{
}

void TestUsageReportController::initTestCase()
{
  QVERIFY( mController );
  QVERIFY( mAppSettings );

  // the tests run with the settings of this device, keep them as they were
  mOriginalEnabled = mAppSettings->usageReportEnabled();
  QSettings settings;
  settings.beginGroup( AppSettings::USAGE_REPORT_GROUP );
  const QStringList keys = settings.allKeys();
  for ( const QString &key : keys )
    mOriginalSettings.insert( key, settings.value( key ) );
  settings.endGroup();
}

void TestUsageReportController::cleanupTestCase()
{
  mController->mSnapshotInFlight = false;
  mController->mSentData.clear();
  mAppSettings->setUsageReportEnabled( mOriginalEnabled );

  QSettings settings;
  settings.remove( AppSettings::USAGE_REPORT_GROUP );
  settings.beginGroup( AppSettings::USAGE_REPORT_GROUP );
  for ( auto it = mOriginalSettings.cbegin(); it != mOriginalSettings.cend(); ++it )
    settings.setValue( it.key(), it.value() );
  settings.endGroup();
}

void TestUsageReportController::init()
{
  mController->mSnapshotInFlight = false;
  mController->mSentData.clear();

  // opting out clears the data and the telemetry id
  mAppSettings->setUsageReportEnabled( true );
  mAppSettings->setUsageReportEnabled( false );
  mAppSettings->setUsageReportEnabled( true );
}

void TestUsageReportController::testSnapshotHasOnlyExpectedKeys()
{
  // record every kind of data the app collects
  mController->trackFeature( QStringLiteral( "filtering" ) );
  mController->incrementCounter( QStringLiteral( "captured_images" ) );
  mController->trackExternalProvider( QStringLiteral( "AA:BB:CC:DD:EE:FF" ), QStringLiteral( "external_bt" ), QStringLiteral( "Leica GS18" ) );
  mController->trackProjectRole( QStringLiteral( "writer" ) );
  mController->startLoadTimer();
  mController->recordLoadTime();

  QVariantMap sentData;
  const QJsonObject body = mController->buildSnapshot( sentData );

  const QStringList bodyKeys = body.keys();
  QCOMPARE( QSet<QString>( bodyKeys.cbegin(), bodyKeys.cend() ), QSet<QString>( { "device_id", "timestamp", "properties" } ) );

  // any new key has to be added here deliberately
  const QSet<QString> expectedProperties =
  {
    // static data
    "app_language", "system_language", "device_manufacturer", "device_model", "app_version", "platform", "os_version",
    "project_count", "num_external_providers", "server_type", "server_version", "plan_name", "plan", "last_reported_at",
    // feature flags
    "filtering", "map_sketching", "map_measuring", "external_gps", "autosync", "reuse_last_value", "bulk_editing",
    "photo_sketching", "created_project", "stakeout", "layers_search", "features_search",
    // counters
    "captured_images", "attached_images", "workspace_switches", "ping_success_count", "ping_fail_count", "ping_total_count",
    "avg_project_load_time_ms",
    // strings
    "external_connection_type", "external_brand", "highest_role", "industry"
  };

  const QStringList propertyKeys = body.value( QStringLiteral( "properties" ) ).toObject().keys();
  QCOMPARE( QSet<QString>( propertyKeys.cbegin(), propertyKeys.cend() ), expectedProperties );

  const QStringList planKeys = body.value( QStringLiteral( "properties" ) ).toObject().value( QStringLiteral( "plan" ) ).toObject().keys();
  QCOMPARE( QSet<QString>( planKeys.cbegin(), planKeys.cend() ), QSet<QString>( { "name", "alias" } ) );
}

void TestUsageReportController::testSnapshotHasNoPersonalData()
{
  // the provider name and address are set by the user and can contain their name
  const QString providerName = QStringLiteral( "John Smith's Leica GS18" );
  const QString providerAddress = QStringLiteral( "AA:BB:CC:DD:EE:FF" );
  mController->trackExternalProvider( providerAddress, QStringLiteral( "external_bt" ), providerName );

  QVariantMap sentData;
  const QJsonObject body = mController->buildSnapshot( sentData );
  const QString json = QString::fromUtf8( QJsonDocument( body ).toJson( QJsonDocument::Compact ) );

  QCOMPARE( body.value( QStringLiteral( "properties" ) ).toObject().value( QStringLiteral( "external_brand" ) ).toString(), QStringLiteral( "Leica" ) );
  QVERIFY( !json.contains( QStringLiteral( "John" ), Qt::CaseInsensitive ) );
  QVERIFY( !json.contains( QStringLiteral( "Smith" ), Qt::CaseInsensitive ) );
  QVERIFY( !json.contains( providerAddress, Qt::CaseInsensitive ) );

  // the snapshot is identified by its own random id, never by the device id
  const QString telemetryId = body.value( QStringLiteral( "device_id" ) ).toString();
  QVERIFY( !QUuid::fromString( telemetryId ).isNull() );
  QVERIFY( telemetryId != CoreUtils::deviceUuid() );
  QVERIFY( !json.contains( CoreUtils::deviceUuid(), Qt::CaseInsensitive ) );

  // the telemetry id stays the same until the user opts out
  QCOMPARE( mController->buildSnapshot( sentData ).value( QStringLiteral( "device_id" ) ).toString(), telemetryId );
}

void TestUsageReportController::testSnapshotValues()
{
  mController->trackFeature( QStringLiteral( "filtering" ) );
  mController->incrementCounter( QStringLiteral( "captured_images" ), 2 );
  mController->trackProjectRole( QStringLiteral( "writer" ) );
  mController->trackProjectRole( QStringLiteral( "reader" ) ); // lower role does not replace the higher one

  QSettings().setValue( usageDataKey( QStringLiteral( "total_load_time_ms" ) ), 300 );
  QSettings().setValue( usageDataKey( QStringLiteral( "load_count" ) ), 3 );

  QVariantMap sentData;
  const QJsonObject properties = mController->buildSnapshot( sentData ).value( QStringLiteral( "properties" ) ).toObject();

  QCOMPARE( properties.value( QStringLiteral( "filtering" ) ).toBool(), true );
  QCOMPARE( properties.value( QStringLiteral( "map_sketching" ) ).toBool(), false );
  QCOMPARE( properties.value( QStringLiteral( "captured_images" ) ).toInt(), 2 );
  QCOMPARE( properties.value( QStringLiteral( "attached_images" ) ).toInt(), 0 );
  QCOMPARE( properties.value( QStringLiteral( "highest_role" ) ).toString(), QStringLiteral( "writer" ) );
  QCOMPARE( properties.value( QStringLiteral( "avg_project_load_time_ms" ) ).toInt(), 100 );
  QCOMPARE( properties.value( QStringLiteral( "last_reported_at" ) ).toString(), QString() );

  // every stored value is reported, so it can be reset once sent
  QCOMPARE( sentData.value( QStringLiteral( "captured_images" ) ).toInt(), 2 );
  QVERIFY( sentData.contains( QStringLiteral( "filtering" ) ) );
  QVERIFY( sentData.contains( QStringLiteral( "total_load_time_ms" ) ) );
}

void TestUsageReportController::testSentDataResetAfterSuccess()
{
  mController->trackFeature( QStringLiteral( "filtering" ) );
  mController->incrementCounter( QStringLiteral( "captured_images" ), 2 );

  QVariantMap sentData;
  mController->buildSnapshot( sentData );
  mController->mSnapshotInFlight = true;
  mController->mSentData = sentData;

  // the user takes another photo while the snapshot is being sent
  mController->incrementCounter( QStringLiteral( "captured_images" ) );

  QSignalSpy spy( mController, &UsageReportController::snapshotSubmitted );
  mController->onUsageReportSent( true );

  QCOMPARE( spy.count(), 1 );
  QVERIFY( !mController->mSnapshotInFlight );

  QSettings settings;
  QVERIFY( !settings.contains( usageDataKey( QStringLiteral( "filtering" ) ) ) );
  QCOMPARE( settings.value( usageDataKey( QStringLiteral( "captured_images" ) ) ).toInt(), 1 ); // only the new photo is left
  QVERIFY( settings.value( usageReportKey( AppSettings::USAGE_REPORT_LAST_REPORTED_KEY ) ).toDateTime().isValid() );
}

void TestUsageReportController::testSentDataKeptAfterFailure()
{
  mController->trackFeature( QStringLiteral( "filtering" ) );
  mController->incrementCounter( QStringLiteral( "captured_images" ), 2 );

  QVariantMap sentData;
  mController->buildSnapshot( sentData );
  mController->mSnapshotInFlight = true;
  mController->mSentData = sentData;

  QSignalSpy spy( mController, &UsageReportController::snapshotSubmitted );
  mController->onUsageReportSent( false );

  QCOMPARE( spy.count(), 0 );
  QVERIFY( !mController->mSnapshotInFlight );

  QSettings settings;
  QVERIFY( settings.value( usageDataKey( QStringLiteral( "filtering" ) ) ).toBool() );
  QCOMPARE( settings.value( usageDataKey( QStringLiteral( "captured_images" ) ) ).toInt(), 2 );
  QVERIFY( !settings.contains( usageReportKey( AppSettings::USAGE_REPORT_LAST_REPORTED_KEY ) ) );
}

void TestUsageReportController::testOptOutClearsData()
{
  mController->trackFeature( QStringLiteral( "filtering" ) );
  QVariantMap sentData;
  mController->buildSnapshot( sentData ); // creates the telemetry id
  QSettings().setValue( usageReportKey( AppSettings::USAGE_REPORT_LAST_REPORTED_KEY ), QDateTime::currentDateTimeUtc() );

  mAppSettings->setUsageReportEnabled( false );

  QSettings settings;
  QVERIFY( !settings.contains( usageDataKey( QStringLiteral( "filtering" ) ) ) );
  QVERIFY( !settings.contains( usageReportKey( AppSettings::USAGE_REPORT_TELEMETRY_ID_KEY ) ) );
  QVERIFY( !settings.contains( usageReportKey( AppSettings::USAGE_REPORT_LAST_REPORTED_KEY ) ) );
  QVERIFY( !settings.contains( usageReportKey( AppSettings::USAGE_REPORT_PERIOD_STARTED_KEY ) ) );
  QCOMPARE( settings.value( usageReportKey( AppSettings::USAGE_REPORT_ENABLED_KEY ) ).toBool(), false );

  // nothing is recorded while opted out
  mController->trackFeature( QStringLiteral( "filtering" ) );
  mController->incrementCounter( QStringLiteral( "captured_images" ) );
  QVERIFY( !settings.contains( usageDataKey( QStringLiteral( "filtering" ) ) ) );
  QVERIFY( !settings.contains( usageDataKey( QStringLiteral( "captured_images" ) ) ) );
}
