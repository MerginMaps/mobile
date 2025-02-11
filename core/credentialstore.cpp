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

CredentialStore::CredentialStore( QObject *parent )
  : QObject( parent )
{
  mWriteJob = new QKeychain::WritePasswordJob( KEY_MM, this );
  mWriteJob->setAutoDelete( false );
  mReadJob = new QKeychain::ReadPasswordJob( KEY_MM, this );
  mReadJob->setAutoDelete( false );

#ifdef USE_INSECURE_KEYCHAIN_FALLBACK
  mReadJob->setInsecureFallback( true );
  mWriteJob->setInsecureFallback( true );
#endif
}

void CredentialStore::writeKey( const QString &key, const QString &value )
{
  mWriteJob->setKey( key );

  mWriteJob->setTextData( value );

  disconnect( mWriteJob, nullptr, this, nullptr );

  connect( mWriteJob, &QKeychain::Job::finished, this, [this, key]()
  {
    if ( mWriteJob->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( key, mWriteJob->errorString() ) );
    }
  } );

  mWriteJob->start();
}

void CredentialStore::writeAuthData( const QString &username,
                                     const QString &password,
                                     int userId,
                                     const QString &token,
                                     const QDateTime &tokenExpiration )
{

  QJsonObject authObject;
  authObject.insert( KEY_USERNAME, username );
  authObject.insert( KEY_PASSWORD, password );
  authObject.insert( KEY_USERID, userId );
  authObject.insert( KEY_TOKEN, token );
  authObject.insert( KEY_EXPIRE, tokenExpiration.toUTC().toString( Qt::ISODate ) );

  QJsonDocument doc( authObject );
  QString jsonString = QString::fromUtf8( doc.toJson( QJsonDocument::Compact ) );

  writeKey( KEY_AUTH_ENTRY, jsonString );

  // remove any legacy QSettings credentials to ensure we're using secure storage going forward
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.remove( KEY_USERNAME );
  settings.remove( KEY_PASSWORD );
  settings.remove( KEY_USERID );
  settings.remove( KEY_TOKEN );
  settings.remove( KEY_EXPIRE );
  settings.endGroup();
}

void CredentialStore::readAuthData()
{
  mReadJob->setKey( KEY_AUTH_ENTRY );

  disconnect( mReadJob, nullptr, this, nullptr );

  connect( mReadJob, &QKeychain::Job::finished, this, [this]()
  {
    if ( mReadJob->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain read error: %1" ).arg( mReadJob->errorString() ) );
      emit authDataRead( QString(), QString(), -1, QString(), QDateTime() );
      return;
    }

    QString username, password, token;
    int userId = -1;
    QDateTime tokenExpiration;

    QJsonDocument doc = QJsonDocument::fromJson( mReadJob->textData().toUtf8() );

    if ( !doc.isNull() && doc.isObject() )
    {
      QJsonObject authObject = doc.object();

      username = authObject.value( KEY_USERNAME ).toString();
      password = authObject.value( KEY_PASSWORD ).toString();
      userId = authObject.value( KEY_USERID ).toInt();
      token = authObject.value( KEY_TOKEN ).toString();

      QString expireStr = authObject.value( KEY_EXPIRE ).toString();
      tokenExpiration = QDateTime::fromString( expireStr, Qt::ISODate );
    }

    if ( username.isEmpty() && password.isEmpty() )
    {
      // fallback to legacy credential storage using QSettings
      QSettings settings;
      settings.beginGroup( "Input/" );
      username = settings.value( KEY_USERNAME ).toString();
      password = settings.value( KEY_PASSWORD ).toString();
      userId = settings.value( KEY_USERID ).toInt();
      token = settings.value( KEY_TOKEN ).toByteArray();
      tokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
      settings.endGroup();
    }

    emit authDataRead( username, password, userId, token, tokenExpiration );
  } );

  mReadJob->start();
}
