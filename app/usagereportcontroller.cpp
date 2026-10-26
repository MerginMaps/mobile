/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "usagereportcontroller.h"
#include "appsettings.h"
#include "coreutils.h"
#include "inpututils.h"
#include "merginapi.h"
#include "merginservertype.h"
#include "localprojectsmanager.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QSysInfo>
#include <QUuid>

static const QString USAGE_REPORT_ENDPOINT = QStringLiteral( "https://api.merginmaps.com/mobile/usage-statistics" );
static const QString CONNECTIVITY_CHECK_ENDPOINT = QStringLiteral( "https://isonline.merginmaps.com/generate_204" );
static const int USAGE_REPORT_INTERVAL_SECS = 7 * 24 * 3600; // 1 week
static const int SNAPSHOT_CHECK_INTERVAL_MS = 60 * 60 * 1000; // 1 hour, also the delay before retrying a failed snapshot
static const int SNAPSHOT_FIRST_CHECK_DELAY_MS = 60 * 1000; // give MerginApi time to fetch the server info after start-up

UsageReportController::UsageReportController( AppSettings *appSettings,
    LocalProjectsManager *localProjectsManager,
    MerginApi *merginApi,
    QObject *parent )
  : QObject( parent )
  , mAppSettings( appSettings )
  , mLocalProjectsManager( localProjectsManager )
  , mMerginApi( merginApi )
{
  mPingTimer = new QTimer( this );
  connect( mPingTimer, &QTimer::timeout, this, &UsageReportController::checkConnectivity );

  mSnapshotTimer = new QTimer( this );
  connect( mSnapshotTimer, &QTimer::timeout, this, &UsageReportController::trySubmitSnapshot );

  connect( mAppSettings, &AppSettings::usageReportEnabledChanged, this, [this]( bool enabled )
  {
    if ( !enabled )
    {
      mPingTimer->stop();
      mSnapshotTimer->stop();
    }
    else
    {
      mPingTimer->start( 5 * 60 * 1000 );
      startSnapshotTimer();
    }
  } );
}

bool UsageReportController::isEnabled() const
{
  return mAppSettings && mAppSettings->usageReportEnabled();
}

void UsageReportController::trackFeature( const QString &key )
{
  if ( !isEnabled() ) return;
  QSettings().setValue( QStringLiteral( "usage_report/data/" ) + key, true );
}

void UsageReportController::incrementCounter( const QString &key, int amount )
{
  if ( !isEnabled() ) return;
  QSettings s;
  const QString fullKey = QStringLiteral( "usage_report/data/" ) + key;
  s.setValue( fullKey, s.value( fullKey, 0 ).toInt() + amount );
}

void UsageReportController::setData( const QString &key, const QVariant &value )
{
  if ( !isEnabled() ) return;
  QSettings().setValue( QStringLiteral( "usage_report/data/" ) + key, value );
}

void UsageReportController::trackExternalProvider( const QString &providerId, const QString &providerType, const QString &providerName )
{
  if ( !isEnabled() ) return;

  const QString connectionType = externalConnectionType( providerId, providerType );
  if ( connectionType.isEmpty() )
    return;

  trackFeature( QStringLiteral( "external_gps" ) );
  setData( QStringLiteral( "external_connection_type" ), connectionType );
  setData( QStringLiteral( "external_brand" ), externalProviderBrand( providerType, providerName ) );
}

void UsageReportController::trackProjectRole( const QString &role )
{
  if ( !isEnabled() ) return;

  const QString current = QSettings().value( QStringLiteral( "usage_report/data/highest_role" ) ).toString();
  if ( projectRoleRank( role ) > projectRoleRank( current ) )
    setData( QStringLiteral( "highest_role" ), role );
}

void UsageReportController::startLoadTimer()
{
  if ( !isEnabled() ) return;
  mLoadTimer.start();
}

void UsageReportController::recordLoadTime()
{
  if ( !isEnabled() || !mLoadTimer.isValid() ) return;

  const qint64 elapsed = mLoadTimer.elapsed();
  QSettings s;
  const QString prefix = QStringLiteral( "usage_report/data/" );
  s.setValue( prefix + QStringLiteral( "total_load_time_ms" ), s.value( prefix + QStringLiteral( "total_load_time_ms" ), 0 ).toLongLong() + elapsed );
  s.setValue( prefix + QStringLiteral( "load_count" ), s.value( prefix + QStringLiteral( "load_count" ), 0 ).toInt() + 1 );
  mLoadTimer.invalidate();
}

void UsageReportController::startPingTimer()
{
  if ( !isEnabled() ) return;
  // TODO: check the new endpoint and decide on the interval
  mPingTimer->start( 5 * 60 * 1000 ); // 5 minutes
}

void UsageReportController::startSnapshotTimer()
{
  if ( !isEnabled() ) return;

  // Start the first period right away, the check below is delayed
  QSettings settings;
  if ( !settings.contains( QStringLiteral( "usage_report/last_reported_at" ) ) && !settings.contains( QStringLiteral( "usage_report/period_started_at" ) ) )
    settings.setValue( QStringLiteral( "usage_report/period_started_at" ), QDateTime::currentDateTimeUtc() );

  // The snapshot is checked once shortly after start-up and then every hour. This covers long-running
  // sessions (common on mobile) and retries a failed snapshot, as a failed attempt keeps the snapshot due.
  mSnapshotTimer->start( SNAPSHOT_CHECK_INTERVAL_MS );
  QTimer::singleShot( SNAPSHOT_FIRST_CHECK_DELAY_MS, this, &UsageReportController::trySubmitSnapshot );
}

void UsageReportController::checkConnectivity()
{
  if ( !isEnabled() ) return;

  QUrl url( CONNECTIVITY_CHECK_ENDPOINT );
  QNetworkRequest request( url );
  request.setAttribute( QNetworkRequest::Http2AllowedAttribute, false );

  QNetworkReply *reply = mNam.head( request );
  connect( reply, &QNetworkReply::finished, reply, [this, reply]()
  {
    incrementCounter( QStringLiteral( "ping_total_count" ) );
    // Only 204 means online, captive portals redirect to a login page that returns 200
    const int status = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    if ( reply->error() == QNetworkReply::NoError && status == 204 )
      incrementCounter( QStringLiteral( "ping_success_count" ) );
    else
      incrementCounter( QStringLiteral( "ping_fail_count" ) );
    reply->deleteLater();
  } );
}

QString UsageReportController::externalProviderBrand( const QString &providerType, const QString &providerName )
{
  if ( providerType == QLatin1String( "external_trimble" ) )
    return QStringLiteral( "trimble" );

  // network provider name is an alias typed by the user, it says nothing about the device
  if ( providerType != QLatin1String( "external_bt" ) )
    return QStringLiteral( "unknown" );

  // bluetooth name is broadcast by the device and usually starts with its model name
  // TODO: fill in confirmed broadcast name prefixes per brand (next telemetry version)
  static const QList<QPair<QString, QString>> BRAND_PREFIXES =
  {
  };

  for ( const QPair<QString, QString> &prefix : BRAND_PREFIXES )
  {
    if ( providerName.startsWith( prefix.first, Qt::CaseInsensitive ) )
      return prefix.second;
  }

  return QStringLiteral( "other" );
}

QString UsageReportController::externalConnectionType( const QString &providerId, const QString &providerType )
{
  // simulated provider has type "internal", so check it by id first
  if ( providerId == QLatin1String( "simulated" ) )
    return QStringLiteral( "mock" );
  if ( providerType == QLatin1String( "external_bt" ) )
    return QStringLiteral( "bluetooth" );
  if ( providerType == QLatin1String( "external_ip" ) )
    return QStringLiteral( "network" );
  if ( providerType == QLatin1String( "external_trimble" ) )
    return QStringLiteral( "trimble" );

  return QString(); // internal
}

int UsageReportController::projectRoleRank( const QString &role )
{
  if ( role == QLatin1String( "owner" ) ) return 6;
  if ( role == QLatin1String( "admin" ) ) return 5;
  if ( role == QLatin1String( "writer" ) ) return 4;
  if ( role == QLatin1String( "editor" ) ) return 3;
  if ( role == QLatin1String( "reader" ) ) return 2;
  if ( role == QLatin1String( "guest" ) ) return 1;
  return 0;
}

// Temporary: bypass interval check for testing
void UsageReportController::forceSubmitSnapshot()
{
  submitSnapshot( true );
}

// Temporary: indirection to support forceSubmitSnapshot — collapse back to trySubmitSnapshot() when removing
void UsageReportController::trySubmitSnapshot()
{
  submitSnapshot( false );
}

void UsageReportController::submitSnapshot( bool force )
{
  if ( !isEnabled() || mSnapshotInFlight )
    return;

  QSettings settings;
  const QDateTime lastReported = settings.value( QStringLiteral( "usage_report/last_reported_at" ) ).toDateTime();
  const QDateTime now = QDateTime::currentDateTimeUtc();

  if ( !force )
  {
    // The first period starts when the user opts in, there is no data to report before that
    QDateTime periodStart = lastReported;
    if ( !periodStart.isValid() )
    {
      periodStart = settings.value( QStringLiteral( "usage_report/period_started_at" ) ).toDateTime();
      if ( !periodStart.isValid() )
      {
        settings.setValue( QStringLiteral( "usage_report/period_started_at" ), now );
        return;
      }
    }

    if ( periodStart.secsTo( now ) < USAGE_REPORT_INTERVAL_SECS )
      return;
  }

  // Ensure telemetry UUID exists (separate from the device UUID)
  QString telemetryId = settings.value( QStringLiteral( "usage_report/telemetry_id" ) ).toString();
  if ( telemetryId.isEmpty() )
  {
    telemetryId = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
    settings.setValue( QStringLiteral( "usage_report/telemetry_id" ), telemetryId );
  }

  // Collect static data
  QVariantMap properties;
  properties.insert( QStringLiteral( "app_language" ), QLocale().name() );
  properties.insert( QStringLiteral( "system_language" ), QLocale::system().name() );
  properties.insert( QStringLiteral( "device_manufacturer" ), InputUtils::getManufacturer() );
  properties.insert( QStringLiteral( "device_model" ), InputUtils::getDeviceModel() );
  properties.insert( QStringLiteral( "app_version" ), CoreUtils::appVersion() );
  properties.insert( QStringLiteral( "platform" ), InputUtils::appPlatform() );
  properties.insert( QStringLiteral( "os_version" ), QSysInfo::productVersion() );
  properties.insert( QStringLiteral( "project_count" ), mLocalProjectsManager->projects().count() );

  // External provider count
  int externalProviderCount = 0;
  const QVariantList providers = mAppSettings->savedPositionProviders();
  for ( const QVariant &v : providers )
  {
    const QStringList p = v.toStringList();
    if ( p.size() >= 3 && p[2] != QLatin1String( "internal" ) && p[2] != QLatin1String( "simulated" ) )
      externalProviderCount++;
  }
  properties.insert( QStringLiteral( "num_external_providers" ), externalProviderCount );

  // Server info
  const auto serverType = static_cast<MerginServerType::ServerType>( mMerginApi->serverType() );
  QString serverTypeStr;
  switch ( serverType )
  {
    case MerginServerType::SAAS: serverTypeStr = QStringLiteral( "saas" ); break;
    case MerginServerType::EE:   serverTypeStr = QStringLiteral( "ee" );   break;
    case MerginServerType::CE:   serverTypeStr = QStringLiteral( "ce" );   break;
    default:                     serverTypeStr = QStringLiteral( "old" );  break;
  }
  properties.insert( QStringLiteral( "server_type" ), serverTypeStr );
  properties.insert( QStringLiteral( "server_version" ), mMerginApi->apiVersion() );
  properties.insert( QStringLiteral( "plan_name" ), mMerginApi->workspacePlanName() );
  properties.insert( QStringLiteral( "plan" ), QVariantMap
  {
    { QStringLiteral( "name" ), mMerginApi->workspacePlanName() },
    { QStringLiteral( "alias" ), mMerginApi->workspacePlanAlias() }
  } );

  // Default values for all dynamic fields, this ensures every key is always present in the snapshot
  // Actual values from QSettings will overwrite these below

  // Boolean feature flags
  properties.insert( QStringLiteral( "filtering" ), false );
  properties.insert( QStringLiteral( "map_sketching" ), false );
  properties.insert( QStringLiteral( "map_measuring" ), false );
  properties.insert( QStringLiteral( "external_gps" ), false );
  properties.insert( QStringLiteral( "autosync" ), false );
  properties.insert( QStringLiteral( "reuse_last_value" ), false );
  properties.insert( QStringLiteral( "bulk_editing" ), false );
  properties.insert( QStringLiteral( "photo_sketching" ), false );
  properties.insert( QStringLiteral( "created_project" ), false );
  properties.insert( QStringLiteral( "stakeout" ), false );
  properties.insert( QStringLiteral( "layers_search" ), false );
  properties.insert( QStringLiteral( "features_search" ), false );

  // Numeric counters
  properties.insert( QStringLiteral( "captured_images" ), 0 );
  properties.insert( QStringLiteral( "attached_images" ), 0 );
  properties.insert( QStringLiteral( "workspace_switches" ), 0 );
  properties.insert( QStringLiteral( "ping_success_count" ), 0 );
  properties.insert( QStringLiteral( "ping_fail_count" ), 0 );
  properties.insert( QStringLiteral( "ping_total_count" ), 0 );
  properties.insert( QStringLiteral( "avg_project_load_time_ms" ), 0 );

  // String data
  properties.insert( QStringLiteral( "external_connection_type" ), QString() );
  properties.insert( QStringLiteral( "external_brand" ), QString() );
  properties.insert( QStringLiteral( "highest_role" ), QString() );
  properties.insert( QStringLiteral( "industry" ), mMerginApi->workspaceIndustry() );

  // Merge accumulated dynamic data (overwrites defaults with actual values)
  settings.beginGroup( QStringLiteral( "usage_report/data" ) );
  const QStringList keys = settings.childKeys();
  QVariantMap sentData;
  for ( const QString &key : keys )
  {
    const QVariant stored = settings.value( key );
    sentData.insert( key, stored );

    // Not every platform keeps value types in QSettings (e.g. ini files on Android), values can be read back as strings.
    // Send the type of the default, so the payload is the same on all platforms.
    QVariant value = stored;
    const QVariant defaultValue = properties.value( key );
    if ( defaultValue.isValid() && value.metaType() != defaultValue.metaType() )
      value.convert( defaultValue.metaType() );
    properties.insert( key, value );
  }
  settings.endGroup();

  // Compute average project load time from running totals
  const qint64 totalMs = properties.value( QStringLiteral( "total_load_time_ms" ), 0 ).toLongLong();
  const int loadCount = properties.value( QStringLiteral( "load_count" ), 0 ).toInt();
  if ( loadCount > 0 )
    properties.insert( QStringLiteral( "avg_project_load_time_ms" ), totalMs / loadCount );
  properties.remove( QStringLiteral( "total_load_time_ms" ) );
  properties.remove( QStringLiteral( "load_count" ) );

  // Last reported at
  properties.insert( QStringLiteral( "last_reported_at" ), lastReported.isValid() ? lastReported.toString( Qt::ISODate ) : QString() );

  const QJsonObject body
  {
    { QStringLiteral( "device_id" ), telemetryId },
    { QStringLiteral( "timestamp" ), now.toString( Qt::ISODate ) },
    { QStringLiteral( "properties" ), QJsonObject::fromVariantMap( properties ) }
  };

  QUrl url( USAGE_REPORT_ENDPOINT );
  QNetworkRequest request( url );
  request.setHeader( QNetworkRequest::ContentTypeHeader, QStringLiteral( "application/json" ) );
  request.setAttribute( QNetworkRequest::Http2AllowedAttribute, false );

  mSnapshotInFlight = true;
  QNetworkReply *reply = mNam.post( request, QJsonDocument( body ).toJson( QJsonDocument::Compact ) );
  connect( reply, &QNetworkReply::finished, this, [this, reply, sentData]()
  {
    mSnapshotInFlight = false;
    // Skip if the user opted out meanwhile, opt-out already cleared the data
    if ( reply->error() == QNetworkReply::NoError && isEnabled() )
    {
      QSettings s;
      // Reset sent data, keeping whatever changed while the request was in flight
      s.beginGroup( QStringLiteral( "usage_report/data" ) );
      for ( auto it = sentData.cbegin(); it != sentData.cend(); ++it )
      {
        const QVariant current = s.value( it.key() );
        if ( current == it.value() )
        {
          s.remove( it.key() );
          continue;
        }

        // counters incremented in the meantime keep only the new increments,
        // a lower value means the counter was reset (opt-out and back in), keep it as is
        bool sentIsNumber = false;
        bool currentIsNumber = false;
        const qlonglong sentNumber = it.value().toLongLong( &sentIsNumber );
        const qlonglong currentNumber = current.toLongLong( &currentIsNumber );
        if ( sentIsNumber && currentIsNumber && currentNumber >= sentNumber )
          s.setValue( it.key(), currentNumber - sentNumber );
      }
      s.endGroup();
      // Update last reported
      s.setValue( QStringLiteral( "usage_report/last_reported_at" ), QDateTime::currentDateTimeUtc() );
      emit snapshotSubmitted();
    }
    // On network error: ignore, data preserved for next attempt
    reply->deleteLater();
  } );
}
