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
#include <QEventLoop>

CredentialStore::CredentialStore( QObject *parent )
  : QObject( parent )
{
  mWriteJob = new QKeychain::WritePasswordJob( KEY_MM, this );
  mWriteJob->setAutoDelete( false );
  mReadJob = new QKeychain::ReadPasswordJob( KEY_MM, this );
  mReadJob->setAutoDelete( false );
  mDeleteJob = new QKeychain::DeletePasswordJob( KEY_MM, this );
  mDeleteJob->setAutoDelete( false );

#ifdef USE_INSECURE_KEYCHAIN_FALLBACK
  mReadJob->setInsecureFallback( true );
  mWriteJob->setInsecureFallback( true );
#endif
}

void CredentialStore::writeKeyAsync( const QString &key, const QString &value )
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

void CredentialStore::writeAuthData
( const QString &username,
  const QString &password,
  int userId,
  const QString &token,
  const QDateTime &tokenExpiration )
{
#ifdef ANDROID
  // on Android, credentials are stored across multiple keys using a synchronous approach
  writeMultipleCredentials( username, password, userId, token, tokenExpiration );
#else
  // on other platforms, all credential data is stored as a single JSON object asynchronously
  writeCredentialsAsJson( username, password, userId, token, tokenExpiration );
#endif
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
#ifdef ANDROID
  // on Android, credentials are stored across multiple keys using a synchronous approach
  readMultipleCredentials();
#else
  // on other platforms, all credential data is stored as a single JSON object asynchronously
  readCredentialsFromJson();
#endif
}

void CredentialStore::writeCredentialsAsJson
( const QString &username,
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

  writeKeyAsync( KEY_AUTH_ENTRY, jsonString );
}

void CredentialStore::readCredentialsFromJson()
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

void CredentialStore::writeMultipleCredentials
( const QString &username,
  const QString &password,
  int userId,
  const QString &token,
  const QDateTime &tokenExpiration )
{
  writeKeySync( KEY_USERNAME, username );
  writeKeySync( KEY_PASSWORD, password );
  writeKeySync( KEY_USERID, QString::number( userId ) );
  writeKeySync( KEY_TOKEN, token );
  writeKeySync( KEY_EXPIRE, tokenExpiration.toUTC().toString( Qt::ISODate ) );
}

void CredentialStore::readMultipleCredentials()
{
  QString username = readKeySync( KEY_USERNAME );
  QString password = readKeySync( KEY_PASSWORD );
  QString userIdStr = readKeySync( KEY_USERID );
  int userId = userIdStr.toInt();
  QString token = readKeySync( KEY_TOKEN );
  QString tokenExpireStr = readKeySync( KEY_EXPIRE );
  QDateTime tokenExpiration = QDateTime::fromString( tokenExpireStr, Qt::ISODate );

  if ( username.isEmpty() && password.isEmpty() )
  {
    // fallback => load from QSettings
    QSettings settings;
    settings.beginGroup( "Input/" );
    username = settings.value( KEY_USERNAME ).toString();
    password = settings.value( KEY_PASSWORD ).toString();
    userId = settings.value( KEY_USERID ).toInt();
    token = settings.value( KEY_TOKEN ).toByteArray();
    tokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
    settings.endGroup();
  }
  else if ( tokenExpiration < QDateTime::currentDateTimeUtc() )
  {
    CoreUtils::log( "Auth", "Token is expired." );
  }

  // emit final result with whichever credentials we have
  emit authDataRead( username, password, userId, token, tokenExpiration );
}

bool CredentialStore::writeKeySync( const QString &key, const QString &value )
{
  // if ( value.isEmpty() ) // if empty value, delete it
  //   return deleteKeySync( key );

  QEventLoop loop;
  disconnect( mWriteJob, nullptr, this, nullptr );
  connect( mWriteJob, &QKeychain::Job::finished, &loop, [&]() { loop.quit(); } );

  mWriteJob->setKey( key );
  mWriteJob->setTextData( value );
  mWriteJob->start();

  loop.exec();

  if ( mWriteJob->error() )
  {
    CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( key, mWriteJob->errorString() ) );
    return false;
  }
  return true;
}

QString CredentialStore::readKeySync( const QString &key )
{
  QEventLoop loop;
  disconnect( mReadJob, nullptr, this, nullptr );
  connect( mReadJob, &QKeychain::Job::finished, &loop, [&]() { loop.quit(); } );

  mReadJob->setKey( key );
  mReadJob->start();

  loop.exec();

  if ( mReadJob->error() )
  {
    CoreUtils::log( "Auth", QString( "Keychain read error (%1): %2" ).arg( key, mReadJob->errorString() ) );
    return QString();
  }
  return mReadJob->textData();
}

bool CredentialStore::deleteKeySync( const QString &key )
{
  QEventLoop loop;
  disconnect( mDeleteJob, nullptr, this, nullptr );
  connect( mDeleteJob, &QKeychain::Job::finished, &loop, [&]() { loop.quit(); } );

  mDeleteJob->setKey( key );
  mDeleteJob->start();

  loop.exec();

  if ( mDeleteJob->error() )
  {
    CoreUtils::log( "Auth", QString( "Keychain delete error (%1): %2" ).arg( key, mDeleteJob->errorString() ) );
    return false;
  }
  return true;
}

