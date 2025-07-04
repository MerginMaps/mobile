/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "credentialstore.h"
#include "coreutils.h"

#include <QSettings>

const QString CredentialStore::KEYCHAIN_GROUP = QStringLiteral( "Input/" );
const QString CredentialStore::KEYCHAIN_ENTRY_CREDENTIALS = QStringLiteral( "" ); // unused
const QString CredentialStore::KEYCHAIN_ENTRY_TOKEN = QStringLiteral( "" ); // unused

const QString CredentialStore::KEY_LOGIN = QStringLiteral( "login" );
const QString CredentialStore::KEY_PASSWORD = QStringLiteral( "password" );
const QString CredentialStore::KEY_TOKEN = QStringLiteral( "token" );
const QString CredentialStore::KEY_EXPIRE = QStringLiteral( "expire" );
const QString CredentialStore::KEY_METHOD = QStringLiteral( "method" );

//
// We store credentials in QSettings and read them in synchronous operation
//

CredentialStore::CredentialStore( QObject *parent )
  : QObject( parent )
{
  // mWriteJob and mReadJob are unused
}

void CredentialStore::writeAuthData
( const QString &login,
  const QString &password,
  const QString &token,
  const QDateTime &tokenExpiration,
  int method )
{
  QSettings settings;
  settings.beginGroup( KEYCHAIN_GROUP );

  settings.setValue( KEY_LOGIN, login );
  settings.setValue( KEY_PASSWORD, password );
  settings.setValue( KEY_TOKEN, token );
  settings.setValue( KEY_EXPIRE, tokenExpiration );
  settings.setValue( KEY_METHOD, method );

  settings.endGroup();
}

void CredentialStore::readAuthData()
{
  QString login, password;
  QByteArray token;
  QDateTime tokenExpiration;
  int method;

  QSettings settings;
  settings.beginGroup( KEYCHAIN_GROUP );

  login = settings.value( KEY_LOGIN ).toString();
  password = settings.value( KEY_PASSWORD ).toString();
  token = settings.value( KEY_TOKEN ).toByteArray();
  tokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
  method = settings.value( KEY_METHOD, 0 ).toInt();

  if ( login.isEmpty() && !password.isEmpty() )
  {
    // We migrated the "username" to "login", let's try reading the old key so that
    // we do not sign out everyone on the first launch after app upgrade. This can be
    // dropped in a few months time.
    const QString oldUsernameEntry = settings.value( "username" ).toString();
    if ( !oldUsernameEntry.isEmpty() )
    {
      login = oldUsernameEntry;
      CoreUtils::log( QStringLiteral( "CredentialStore" ), QStringLiteral( "Read login from the deprecated username key to keep user signed in" ) );
    }
  }

  settings.endGroup();

  emit authDataRead( login, password, token, tokenExpiration, method );
}

void CredentialStore::readKeyRecursively( const QString &key )
{
  // no op
}

void CredentialStore::finishReadingOperation()
{
  // no op
}
