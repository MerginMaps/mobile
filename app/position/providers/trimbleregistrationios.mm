/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trimbleregistration.h"

#ifdef Q_OS_IOS

#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointer>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

#import <UIKit/UIKit.h>

static QString MM_CALLBACK_SCHEME = QStringLiteral( "merginmaps" );
static QString MM_CALLBACK_URL = QStringLiteral( "tmm-registration" );

TrimbleRegistration::TrimbleRegistration( QObject *parent )
  : QObject( parent )
{
}

void TrimbleRegistration::requestRegistration( const QString &appId )
{
  QDesktopServices::setUrlHandler( MM_CALLBACK_SCHEME, this, "handleCallback" );

  QJsonObject payload;
  payload[QStringLiteral( "application_id" )] = appId;
  payload[QStringLiteral( "returl" )] = QStringLiteral( "%1://%2" ).arg( MM_CALLBACK_SCHEME, MM_CALLBACK_URL );

  const QByteArray jsonBytes = QJsonDocument( payload ).toJson( QJsonDocument::Compact );
  const QString base64 = QString::fromLatin1( jsonBytes.toBase64() );

  // QDesktopServices::openUrl() routes through QUrl which percent-encodes or rejects
  // the base64 payload, breaking the scheme.
  NSString *nsUrlString = QStringLiteral( "%1://?%2" ).arg( QStringLiteral( "tmmregister" ), base64 ).toNSString();
  NSURL *trimbleUrl = [NSURL URLWithString:nsUrlString];

  if ( !trimbleUrl )
  {
    emit failed( tr( "Registration failed, no response from Trimble Mobile Manager, probably it's missing." ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, trimble URL is malformed." ) );
    return;
  }

  QPointer<TrimbleRegistration> self( this );
  [[UIApplication sharedApplication] openURL:trimbleUrl options:@ {} completionHandler: ^ ( BOOL success )
  {
    if ( !success && self )
    {
      emit self->failed( tr( "Registration failed, no response from Trimble Mobile Manager, probably it's missing." ) );
      CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, failed to open Trimble Mobile Manager, probably it's missing." ) );
    }
  }];
}

void TrimbleRegistration::handleCallback( const QUrl &url )
{
  if ( url.scheme() != MM_CALLBACK_SCHEME )
  {
    emit self->failed( tr( "Registration failed, wrong response from Trimble Mobile Manager." ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, Trimble Mobile Manager responded with wrong scheme." ) );
    return;
  }

  if ( url.host() != MM_CALLBACK_URL )
  {
    emit self->failed( tr( "Registration failed, wrong response from Trimble Mobile Manager." ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, Trimble Mobile Manager responded to wrong url." ) );
    QDesktopServices::unsetUrlHandler( QString::fromLatin1( MM_CALLBACK_SCHEME ) );
    return;
  }

  const QByteArray jsonBytes = QByteArray::fromBase64( url.query().toLatin1() );
  const QJsonDocument doc = QJsonDocument::fromJson( jsonBytes );
  if ( doc.isNull() || !doc.isObject() )
  {
    emit failed( tr( "Registration failed, wrong response from Trimble Mobile Manager." ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, Trimble Mobile Manager responded with malformed JSON." ) );
    QDesktopServices::unsetUrlHandler( QString::fromLatin1( MM_CALLBACK_SCHEME ) );
    return;
  }

  const QJsonObject obj = doc.object();
  const QString registerResult = obj.value( QStringLiteral( "registrationResult" ) ).toString();

  if ( registerResult != QLatin1String( "OK" ) )
  {
    emit failed( tr( "Trimble Mobile Manager registration failed: %1" ).arg( registerResult ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, registration has been refused by Trimble Mobile Manager. Reason: %1." ).arg( registerResult ) );
    QDesktopServices::unsetUrlHandler( QString::fromLatin1( MM_CALLBACK_SCHEME ) );
    return;
  }

  const int locationPort = obj.value( QStringLiteral( "locationV2Port" ) ).toInt( -1 );
  if ( locationPort <= 0 )
  {
    emit failed( tr( "Trimble Mobile Manager returned invalid port" ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, Trimble Mobile Manager returned malformed location data port." ) );
    QDesktopServices::unsetUrlHandler( QString::fromLatin1( MM_CALLBACK_SCHEME ) );
    return;
  }

  QDesktopServices::unsetUrlHandler( QString::fromLatin1( MM_CALLBACK_SCHEME ) );
  emit registered( locationPort );
}

#endif // Q_OS_IOS
