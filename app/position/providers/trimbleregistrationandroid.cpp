/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trimbleregistration.h"

#ifdef Q_OS_ANDROID

#include <QtCore/private/qandroidextras_p.h>

#include "coreutils.h"

static constexpr int TMM_REGISTER_REQUEST_CODE = 0x544D4D52; // "TMMR" - custom value to pair intent result

void TrimbleResultReceiver::handleActivityResult( const int receiverRequestCode, const int resultCode, const QJniObject &data )
{
  Q_UNUSED( resultCode )
  if ( receiverRequestCode != TMM_REGISTER_REQUEST_CODE )
    return;

  if ( !data.isValid() )
  {
    emit registrationFailed( TrimbleRegistration::tr( "No response from Trimble Mobile Manager" ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, no response from Trimble Mobile Manager, probably it's missing." ) );
    return;
  }

  const QAndroidIntent intent( data );
  // we can't use intent.extraVariant() function here as it throws errors in Qt code
  const QString registerResult = intent.handle().callObjectMethod<jstring>( "getStringExtra", QJniObject::fromString( QStringLiteral( "registrationResult" ) ).object<jstring>() ).toString();

  if ( registerResult != QStringLiteral( "OK" ) )
  {
    emit registrationFailed( TrimbleRegistration::tr( "Trimble Mobile Manager registration failed: %1" ).arg( registerResult ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, registration has been refused by Trimble Mobile Manager. Reason: %1." ).arg( registerResult ) );
    return;
  }
  // we can't use intent.extraVariant() function here as it doesn't parse the data correctly
  const int locationPortResult = intent.handle().callMethod<jint>( "getIntExtra", QJniObject::fromString( QStringLiteral( "locationV2Port" ) ).object<jstring>(), 0 );

  if ( !locationPortResult )
  {
    emit registrationFailed( TrimbleRegistration::tr( "Trimble Mobile Manager returned invalid port" ) );
    CoreUtils::log( QStringLiteral( "TrimblePositionProvider" ), QStringLiteral( "Registration failed, Trimble Mobile Manager returned malformed location data port." ) );
    return;
  }

  emit registrationSucceeded( locationPortResult );
}

TrimbleRegistration::TrimbleRegistration( QObject *parent )
  : QObject( parent )
  , mResultReceiver( std::make_unique<TrimbleResultReceiver>() )
{
  connect( mResultReceiver.get(), &TrimbleResultReceiver::registrationFailed, this, &TrimbleRegistration::failed );
  connect( mResultReceiver.get(), &TrimbleResultReceiver::registrationSucceeded, this, &TrimbleRegistration::registered );
}

void TrimbleRegistration::requestRegistration( const QString &appId )
{
  const QAndroidIntent intent( QStringLiteral( "com.trimble.tmm.REGISTER" ) );
  // we can't use intent.extraVariant() function here as it writes the string as byte array instead of strings
  intent.handle().callObjectMethod<QtJniTypes::Intent>( "putExtra", QJniObject::fromString( QStringLiteral( "applicationID" ) ).object<jstring>(), QJniObject::fromString( appId ).object<jstring>() );

  QtAndroidPrivate::startActivity( intent, TMM_REGISTER_REQUEST_CODE, mResultReceiver.get() );
}

#endif // ANDROID
