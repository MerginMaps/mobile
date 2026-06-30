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

static constexpr char TMM_CALLBACK_SCHEME[] = "merginmaps-tmm";
static constexpr char TMM_REGISTER_SCHEME[] = "tmmregister";

TrimbleRegistration::TrimbleRegistration( QObject *parent )
  : QObject( parent )
{
}

TrimbleRegistration::~TrimbleRegistration() = default;

void TrimbleRegistration::requestRegistration( const QString &appId )
{
  QDesktopServices::setUrlHandler( QString::fromLatin1( TMM_CALLBACK_SCHEME ), this, "handleCallback" );

  QJsonObject payload;
  payload[QStringLiteral( "application_id" )] = appId;
  payload[QStringLiteral( "returl" )] = QStringLiteral( "%1://registration" ).arg( QLatin1String( TMM_CALLBACK_SCHEME ) );

  const QByteArray jsonBytes = QJsonDocument( payload ).toJson( QJsonDocument::Compact );
  const QString base64 = QString::fromLatin1( jsonBytes.toBase64() );

  // Build the raw URL string and open via UIKit directly.
  // QDesktopServices::openUrl() routes through QUrl which percent-encodes or rejects
  // the base64 payload ('+', '/', '=' are special in URLs), breaking the scheme.
  const QString rawUrlString = QStringLiteral( "%1://?%2" )
                               .arg( QLatin1String( TMM_REGISTER_SCHEME ), base64 );
  NSString *nsUrlString = rawUrlString.toNSString();
  NSURL *nsUrl = [NSURL URLWithString:nsUrlString];

  qDebug() << "TrimbleRegistration: opening URL:" << rawUrlString;
  qDebug() << "TrimbleRegistration: NSURL valid:" << ( nsUrl != nil );

  if ( !nsUrl )
  {
    emit failed( tr( "Could not open Trimble Mobile Manager. Is it installed?" ) );
    return;
  }

  QPointer<TrimbleRegistration> self( this );
  [[UIApplication sharedApplication] openURL:nsUrl options:@{} completionHandler:^( BOOL success )
  {
    qDebug() << "TrimbleRegistration: openURL completionHandler success:" << success;
    if ( !success && self )
    {
      emit self->failed( tr( "Could not open Trimble Mobile Manager. Is it installed?" ) );
    }
  }];
}

Q_INVOKABLE void TrimbleRegistration::handleCallback( const QUrl &url )
{
  qDebug() << "TrimbleRegistration: handleCallback called with URL:" << url.toString();

  if ( url.scheme() != QLatin1String( TMM_CALLBACK_SCHEME ) )
  {
    qDebug() << "TrimbleRegistration: unexpected scheme, ignoring:" << url.scheme();
    return;
  }

  const QString fragment = url.fragment();
  const QString query = url.query();
  const QString base64 = fragment.isEmpty() ? query : fragment;
  qDebug() << "TrimbleRegistration: raw callback payload:" << base64;

  const QByteArray jsonBytes = QByteArray::fromBase64( base64.toLatin1() );
  const QJsonDocument doc = QJsonDocument::fromJson( jsonBytes );
  if ( doc.isNull() || !doc.isObject() )
  {
    qDebug() << "TrimbleRegistration: failed to parse callback JSON:" << jsonBytes;
    emit failed( tr( "Invalid response from Trimble Mobile Manager" ) );
    return;
  }

  const QJsonObject obj = doc.object();
  const QString result = obj.value( QStringLiteral( "registrationResult" ) ).toString();
  const QJsonValue dbgPort = obj.value( QStringLiteral( "locationV2Port" ) );
  qDebug() << "TrimbleRegistration: registrationResult:" << result
           << "locationV2Port:" << ( dbgPort.isString() ? dbgPort.toString() : QString::number( dbgPort.toInt( 0 ) ) );

  if ( result != QLatin1String( "OK" ) )
  {
    emit failed( tr( "Trimble Mobile Manager registration failed: %1" ).arg( result ) );
    return;
  }

  // TMM returns port values as JSON strings, not numbers — use toString().toInt()
  const QJsonValue portVal = obj.value( QStringLiteral( "locationV2Port" ) );
  const int port = portVal.isString() ? portVal.toString().toInt() : portVal.toInt( 0 );
  if ( port <= 0 )
  {
    emit failed( tr( "Trimble Mobile Manager returned invalid port" ) );
    return;
  }

  QDesktopServices::unsetUrlHandler( QString::fromLatin1( TMM_CALLBACK_SCHEME ) );
  emit registered( port );
}

#endif // Q_OS_IOS
