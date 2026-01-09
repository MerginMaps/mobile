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

#include <QJsonObject>
#include <QJsonDocument>

const QString CredentialStore::KEYCHAIN_GROUP = QStringLiteral( "mergin_maps" );
const QString CredentialStore::KEYCHAIN_ENTRY_CREDENTIALS = QStringLiteral( "credentials" );
const QString CredentialStore::KEYCHAIN_ENTRY_TOKEN = QStringLiteral( "token" );

const QString CredentialStore::KEY_LOGIN = QStringLiteral( "u" );
const QString CredentialStore::KEY_PASSWORD = QStringLiteral( "p" );
const QString CredentialStore::KEY_TOKEN = QStringLiteral( "t" );
const QString CredentialStore::KEY_EXPIRE = QStringLiteral( "e" );
const QString CredentialStore::KEY_METHOD = QStringLiteral( "m" );

CredentialStore::CredentialStore( QObject *parent )
  : QObject( parent )
{
  mWriteJob = new QKeychain::WritePasswordJob( KEYCHAIN_GROUP, this );
  mWriteJob->setAutoDelete( false );

  mReadJob = new QKeychain::ReadPasswordJob( KEYCHAIN_GROUP, this );
  mReadJob->setAutoDelete( false );
}


void CredentialStore::writeAuthData
( const QString &login,
  const QString &password,
  const QString &token,
  const QDateTime &tokenExpiration,
  int method )
{
  //
  // 1. Split the data into two jsons
  //

  QJsonObject credentialsJsonObj;
  credentialsJsonObj.insert( KEY_LOGIN, login );
  credentialsJsonObj.insert( KEY_PASSWORD, password );
  credentialsJsonObj.insert( KEY_METHOD, method );

  QJsonDocument credentialsJson( credentialsJsonObj );

  QJsonObject tokenJsonObj;
  tokenJsonObj.insert( KEY_TOKEN, token );
  tokenJsonObj.insert( KEY_EXPIRE, tokenExpiration.toString( Qt::ISODateWithMs ) );

  QJsonDocument tokenJson( tokenJsonObj );

  //
  // 2. Store JSONs one by one
  //

  mWriteJob->setKey( KEYCHAIN_ENTRY_CREDENTIALS );
  mWriteJob->setBinaryData( credentialsJson.toJson( QJsonDocument::Compact ) );

  connect( mWriteJob, &QKeychain::Job::finished, this, [this, tokenJson]()
  {
    if ( mWriteJob->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( KEYCHAIN_ENTRY_CREDENTIALS, mWriteJob->errorString() ) );
      return; // do not try to store the token either
    }

    // let's store the token now
    mWriteJob->setKey( KEYCHAIN_ENTRY_TOKEN );
    mWriteJob->setBinaryData( tokenJson.toJson( QJsonDocument::Compact ) );

    mWriteJob->start();

  }, Qt::SingleShotConnection );

  mWriteJob->start();

  //
  // 3. Clear any previous data from QSettings (migration from the previous QSettings)
  //

  // TODO: pass
}

void CredentialStore::readAuthData()
{
  mReadResults.clear();
  readKeyRecursively( KEYCHAIN_ENTRY_CREDENTIALS );
}

void CredentialStore::readKeyRecursively( const QString &key )
{
  //
  // 1. Read both entries from keychain (async)
  //

  mReadJob->setKey( key );

  connect( mReadJob, &QKeychain::Job::finished, this, [this, key]()
  {
    if ( mReadJob->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain read error: %1" ).arg( mReadJob->errorString() ) );
      emit authDataRead( QString(), QString(), QString(), QDateTime(), -1 );
      return;
    }

    mReadResults[ key ] = mReadJob->textData();

    if ( key == KEYCHAIN_ENTRY_CREDENTIALS )
    {
      readKeyRecursively( KEYCHAIN_ENTRY_TOKEN ); // Read the second entry
    }
    else if ( key == KEYCHAIN_ENTRY_TOKEN )
    {
      finishReadingOperation(); // We have all the data now, let's wrap it up and return back
    }

  }, Qt::SingleShotConnection );

  mReadJob->start();
}

void CredentialStore::finishReadingOperation()
{
  //
  // 2. Construct JSONs from the intermediary results and emit the data
  //

  QString login, password;
  QByteArray token;
  QDateTime tokenExpiration;
  int method = 0;

  if ( mReadResults.size() != 2 )
  {
    CoreUtils::log( QStringLiteral( "Auth" ),
                    QString( "Something ugly happened when reading, invalid size of the intermediary results, size:" ).arg( mReadResults.size() )
                  );
    emit authDataRead( login, password, token, tokenExpiration, method );
    return;
  }

  QString credentialsJsonString = mReadResults.value( KEYCHAIN_ENTRY_CREDENTIALS, QString() );
  QString tokenJsonString = mReadResults.value( KEYCHAIN_ENTRY_TOKEN, QString() );

  if ( credentialsJsonString.isEmpty() || tokenJsonString.isEmpty() )
  {
    CoreUtils::log(
      QStringLiteral( "Auth" ),
      QString( "Something ugly happened when reading, one of the read jsons is empty (%1, %2)" ).arg( credentialsJsonString.length(), tokenJsonString.length() )
    );
    emit authDataRead( login, password, token, tokenExpiration, method );
    return;
  }

  QJsonParseError parsingError;
  QJsonDocument credentialsJson = QJsonDocument::fromJson( credentialsJsonString.toUtf8(), &parsingError );

  if ( parsingError.error != QJsonParseError::NoError )
  {
    CoreUtils::log( QStringLiteral( "Auth" ), QString( "Could not construct credentials JSON when reading, error: %1" ).arg( parsingError.errorString() ) );
    emit authDataRead( login, password, token, tokenExpiration, method );
    return;
  }

  QJsonDocument tokenJson = QJsonDocument::fromJson( tokenJsonString.toUtf8(), &parsingError );

  if ( parsingError.error != QJsonParseError::NoError )
  {
    CoreUtils::log( QStringLiteral( "Auth" ), QString( "Could not construct token JSON when reading, error: %1" ).arg( parsingError.errorString() ) );
    emit authDataRead( login, password, token, tokenExpiration, method );
    return;
  }

  QJsonObject credentialsJsonObject = credentialsJson.object();
  QJsonObject tokenJsonObject = tokenJson.object();

  login = credentialsJsonObject.value( KEY_LOGIN ).toString();
  password = credentialsJsonObject.value( KEY_PASSWORD ).toString();
  if ( credentialsJsonObject.contains( KEY_METHOD ) )
    method = credentialsJsonObject.value( KEY_METHOD ).toInt();

  token = tokenJsonObject.value( KEY_TOKEN ).toString().toUtf8();
  tokenExpiration = QDateTime::fromString( tokenJsonObject.value( KEY_EXPIRE ).toString(), Qt::ISODateWithMs );

  //
  // If credentials are empty, we should look at QSettings as we previously stored credentials there.
  // Freshly upgraded app might not have the auth data migrated yet.
  //
  // TODO: pass...
  //

  emit authDataRead( login, password, token, tokenExpiration, method );
}
