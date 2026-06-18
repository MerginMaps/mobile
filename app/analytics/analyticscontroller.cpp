/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "analyticscontroller.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

#include "mmconfig.h"
#include "coreutils.h"
#include "inpututils.h"

// PostHog EU Cloud — data stays in Frankfurt.
// Set via CMake: -DPOSTHOG_API_KEY="phc_yourkey"
// The key is intentionally public (write-only capture key, no read access).
#ifndef POSTHOG_API_KEY
#define POSTHOG_API_KEY ""
#endif

const QString AnalyticsController::ENDPOINT = QStringLiteral( "https://eu.i.posthog.com/capture/" );
const QString AnalyticsController::API_KEY = QStringLiteral( POSTHOG_API_KEY );
const QString AnalyticsController::SETTINGS_KEY = QStringLiteral( "analytics/enabled" );

AnalyticsController::AnalyticsController( QObject *parent )
  : QObject( parent )
{
  QSettings settings;
  mEnabled = settings.value( SETTINGS_KEY, true ).toBool();
  mDistinctId = CoreUtils::deviceUuid();
}

bool AnalyticsController::enabled() const
{
  return mEnabled;
}

void AnalyticsController::setEnabled( bool enabled )
{
  if ( mEnabled == enabled )
    return;

  mEnabled = enabled;
  QSettings settings;
  settings.setValue( SETTINGS_KEY, mEnabled );
  emit enabledChanged( mEnabled );
}

void AnalyticsController::capture( const QString &event, const QVariantMap &properties )
{
  if ( !mEnabled || API_KEY.isEmpty() )
    return;

  sendEvent( event, properties );
}

void AnalyticsController::sendEvent( const QString &event, QVariantMap properties )
{
  // Always suppress IP and attach base properties — never omit these.
  properties.insert( QStringLiteral( "$ip" ), QString() );
  properties.insert( QStringLiteral( "platform" ), InputUtils::appPlatform() );
  properties.insert( QStringLiteral( "app_version" ), CoreUtils::appVersion() );

  const QJsonObject body
  {
    { QStringLiteral( "api_key" ), API_KEY },
    { QStringLiteral( "event" ), event },
    { QStringLiteral( "distinct_id" ), mDistinctId },
    { QStringLiteral( "timestamp" ), QDateTime::currentDateTimeUtc().toString( Qt::ISODate ) },
    { QStringLiteral( "properties" ), QJsonObject::fromVariantMap( properties ) }
  };

  QUrl url( ENDPOINT );
  QNetworkRequest request( url );
  request.setHeader( QNetworkRequest::ContentTypeHeader, QStringLiteral( "application/json" ) );
  // Consistent with the rest of the app — see main.cpp (QTBUG-111417)
  request.setAttribute( QNetworkRequest::Http2AllowedAttribute, false );

  QNetworkReply *reply = mManager.post( request, QJsonDocument( body ).toJson( QJsonDocument::Compact ) );
  // Fire-and-forget: analytics failures are silent and non-blocking
  connect( reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater );
}
