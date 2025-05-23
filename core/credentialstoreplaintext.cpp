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

const QString CredentialStore::KEY_USERNAME = QStringLiteral( "username" );
const QString CredentialStore::KEY_PASSWORD = QStringLiteral( "password" );
const QString CredentialStore::KEY_USERID = QStringLiteral( "userId" );
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
( const QString &username,
  const QString &password,
  int userId,
  const QString &token,
  const QDateTime &tokenExpiration,
  int method )
{
  QSettings settings;
  settings.beginGroup( KEYCHAIN_GROUP );

  settings.setValue( KEY_USERNAME, username );
  settings.setValue( KEY_PASSWORD, password );
  settings.setValue( KEY_USERID, userId );
  settings.setValue( KEY_TOKEN, token );
  settings.setValue( KEY_EXPIRE, tokenExpiration );
  settings.setValue( KEY_METHOD, method );

  settings.endGroup();
}

void CredentialStore::readAuthData()
{
  QString username, password;
  int userid = -1;
  QByteArray token;
  QDateTime tokenExpiration;
  int method;

  QSettings settings;
  settings.beginGroup( KEYCHAIN_GROUP );

  username = settings.value( KEY_USERNAME ).toString();
  password = settings.value( KEY_PASSWORD ).toString();
  userid = settings.value( KEY_USERID ).toInt();
  token = settings.value( KEY_TOKEN ).toByteArray();
  tokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
  method = settings.value( KEY_METHOD, 0 ).toInt();

  settings.endGroup();

  emit authDataRead( username, password, userid, token, tokenExpiration, method );
}

void CredentialStore::readKeyRecursively( const QString &key )
{
  // no op
}

void CredentialStore::finishReadingOperation()
{
  // no op
}
