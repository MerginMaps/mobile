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
#include "merginsubscriptioninfo.h"
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
static const int USAGE_REPORT_INTERVAL_SECS = 7 * 24 * 3600; // 1 week

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
  connect( mPingTimer, &QTimer::timeout, this, [this]()
  {
    if ( !isEnabled() || mMerginApi->apiRoot().isEmpty() ) return;

    QUrl url( mMerginApi->apiRoot() );
    QNetworkRequest request( url );
    request.setAttribute( QNetworkRequest::Http2AllowedAttribute, false );

    QNetworkReply *reply = mNam.head( request );
    connect( reply, &QNetworkReply::finished, reply, [this, reply]()
    {
      if ( reply->error() == QNetworkReply::NoError )
        incrementCounter( QStringLiteral( "ping_success_count" ) );
      else
        incrementCounter( QStringLiteral( "ping_fail_count" ) );
      reply->deleteLater();
    } );
  } );

  connect( mAppSettings, &AppSettings::usageReportEnabledChanged, this, [this]( bool enabled )
  {
    if ( !enabled )
      mPingTimer->stop();
    else
      mPingTimer->start( 5 * 60 * 1000 );
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

void UsageReportController::startLoadTimer()
{
  mLoadTimer.start();
}

void UsageReportController::recordLoadTime()
{
  if ( !mLoadTimer.isValid() ) return;

  const qint64 elapsed = mLoadTimer.elapsed();
  QSettings s;
  const QString prefix = QStringLiteral( "usage_report/data/" );
  s.setValue( prefix + QStringLiteral( "total_load_time_ms" ), s.value( prefix + QStringLiteral( "total_load_time_ms" ), 0 ).toLongLong() + elapsed );
  s.setValue( prefix + QStringLiteral( "load_count" ), s.value( prefix + QStringLiteral( "load_count" ), 0 ).toInt() + 1 );
  mLoadTimer.invalidate();
}

void UsageReportController::startPingTimer()
{
  // TODO: check the new endpoint and decide on the interval
  mPingTimer->start( 5 * 60 * 1000 ); // 5 minutes
}

void UsageReportController::trySubmitSnapshot()
{
  if ( !isEnabled() )
    return;

  QSettings settings;
  const QDateTime lastReported = settings.value( QStringLiteral( "usage_report/last_reported_at" ) ).toDateTime();
  const QDateTime now = QDateTime::currentDateTimeUtc();

  if ( lastReported.isValid() && lastReported.secsTo( now ) < USAGE_REPORT_INTERVAL_SECS )
    return;

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
  properties.insert( QStringLiteral( "plan_name" ),
                     mMerginApi->subscriptionInfo() ? mMerginApi->subscriptionInfo()->planAlias() : QString() );

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
  properties.insert( QStringLiteral( "avg_project_load_time_ms" ), 0 );

  // String data
  properties.insert( QStringLiteral( "external_connection_type" ), QString() );
  properties.insert( QStringLiteral( "external_name" ), QString() );
  properties.insert( QStringLiteral( "highest_role" ), QString() );
  // TODO: populate from GET /v2/workspaces/<id>/service once the endpoint is extended
  properties.insert( QStringLiteral( "industry" ), QString() );

  // Merge accumulated dynamic data (overwrites defaults with actual values)
  settings.beginGroup( QStringLiteral( "usage_report/data" ) );
  const QStringList keys = settings.childKeys();
  for ( const QString &key : keys )
    properties.insert( key, settings.value( key ) );
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
    { QStringLiteral( "telemetry_id" ), telemetryId },
    { QStringLiteral( "timestamp" ), now.toString( Qt::ISODate ) },
    { QStringLiteral( "properties" ), QJsonObject::fromVariantMap( properties ) }
  };

  QUrl url( USAGE_REPORT_ENDPOINT );
  QNetworkRequest request( url );
  request.setHeader( QNetworkRequest::ContentTypeHeader, QStringLiteral( "application/json" ) );
  request.setAttribute( QNetworkRequest::Http2AllowedAttribute, false );

  QNetworkReply *reply = mNam.post( request, QJsonDocument( body ).toJson( QJsonDocument::Compact ) );
  connect( reply, &QNetworkReply::finished, reply, [reply]()
  {
    if ( reply->error() == QNetworkReply::NoError )
    {
      QSettings s;
      // Reset dynamic data
      s.beginGroup( QStringLiteral( "usage_report/data" ) );
      s.remove( QString() );
      s.endGroup();
      // Update last reported
      s.setValue( QStringLiteral( "usage_report/last_reported_at" ), QDateTime::currentDateTimeUtc() );
    }
    // On network error: ignore, data preserved for next attempt
    reply->deleteLater();
  } );
}
