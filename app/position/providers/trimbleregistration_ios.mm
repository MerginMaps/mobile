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
#include <QUrl>
#include <QUrlQuery>

#import <UIKit/UIKit.h>

static constexpr char TMM_CALLBACK_SCHEME[] = "merginmaps-tmm";
static constexpr char TMM_REGISTER_SCHEME[] = "tmmregister";

TrimbleRegistration::TrimbleRegistration( QObject *parent )
  : QObject( parent )
{
}

void TrimbleRegistration::requestRegistration( const QString &appId )
{
  QDesktopServices::setUrlHandler( QString::fromLatin1( TMM_CALLBACK_SCHEME ), this, "handleCallback" );

  QJsonObject payload;
  payload[QStringLiteral( "application_id" )] = appId;
  payload[QStringLiteral( "returl" )] = QStringLiteral( "%1://registration" ).arg( QLatin1String( TMM_CALLBACK_SCHEME ) );

  const QByteArray jsonBytes = QJsonDocument( payload ).toJson( QJsonDocument::Compact );
  const QString base64 = QString::fromLatin1( jsonBytes.toBase64() );
  const QUrl tmmUrl( QStringLiteral( "%1://?%2" ).arg( QLatin1String( TMM_REGISTER_SCHEME ), base64 ) );

  if ( !QDesktopServices::openUrl( tmmUrl ) )
  {
    emit failed( tr( "Could not open Trimble Mobile Manager. Is it installed?" ) );
  }
}

Q_INVOKABLE void TrimbleRegistration::handleCallback( const QUrl &url )
{
  if ( url.scheme() != QLatin1String( TMM_CALLBACK_SCHEME ) )
    return;

  const QString fragment = url.fragment();
  const QString query = url.query();
  const QString base64 = fragment.isEmpty() ? query : fragment;

  const QByteArray jsonBytes = QByteArray::fromBase64( base64.toLatin1() );
  const QJsonDocument doc = QJsonDocument::fromJson( jsonBytes );
  if ( doc.isNull() || !doc.isObject() )
  {
    emit failed( tr( "Invalid response from Trimble Mobile Manager" ) );
    return;
  }

  const QJsonObject obj = doc.object();
  const QString result = obj.value( QStringLiteral( "registrationResult" ) ).toString();

  if ( result != QLatin1String( "OK" ) )
  {
    emit failed( tr( "Trimble Mobile Manager registration failed: %1" ).arg( result ) );
    return;
  }

  const int port = obj.value( QStringLiteral( "locationV2Port" ) ).toInt( 0 );
  if ( port <= 0 )
  {
    emit failed( tr( "Trimble Mobile Manager returned invalid port" ) );
    return;
  }

  QDesktopServices::unsetUrlHandler( QString::fromLatin1( TMM_CALLBACK_SCHEME ) );
  emit registered( port );
}

#endif // Q_OS_IOS
