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
  mWriteJob = new QKeychain::WritePasswordJob( KEY_MERGIN, this );
  mWriteJob->setAutoDelete( false );

  mReadJob = new QKeychain::ReadPasswordJob( KEY_MERGIN, this );
  mReadJob->setAutoDelete( false );
}

void CredentialStore::writeKey( const QString &key, const QVariant &value )
{
  mWriteJob->setKey( key );

  if ( value.type() == QVariant::ByteArray )
  {
    mWriteJob->setBinaryData( value.toByteArray() );
  }
  else if ( value.type() == QVariant::DateTime )
  {
    mWriteJob->setTextData( value.toDateTime().toString( Qt::ISODate ) );
  }
  else
  {
    mWriteJob->setTextData( value.toString() );
  }

  disconnect( mWriteJob, nullptr, this, nullptr );

  connect( mWriteJob, &QKeychain::Job::finished, this, [this, key]()
  {
    if ( !mWriteJob->error() )
    {
      QSettings settings;
      settings.beginGroup( "Input/" );
      settings.remove( KEY_USERNAME );
      settings.remove( KEY_PASSWORD );
      settings.remove( KEY_USERID );
      settings.remove( KEY_TOKEN );
      settings.remove( KEY_EXPIRE );
      settings.endGroup();
    }
    else
    {
      CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( key, mWriteJob->errorString() ) );
    }
  } );

  mWriteJob->start();
}

void CredentialStore::readKey( const QString &key )
{
  mReadJob->setKey( key );

  disconnect( mReadJob, nullptr, this, nullptr );

  connect( mReadJob, &QKeychain::Job::finished, this, [this, key]()
  {
    if ( mReadJob->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain read error (%1): %2" ).arg( key, mReadJob->errorString() ) );
    }
  } );

  mReadJob->start();
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

  writeKey( "/auth", jsonString );
}

void CredentialStore::readAuthData()
{
  mReadJob->setKey( "/auth" );

  disconnect( mReadJob, nullptr, this, nullptr );

  connect( mReadJob, &QKeychain::Job::finished, this, [this]()
  {
    if ( !mReadJob->error() && !mReadJob->textData().isEmpty() )
    {
      QJsonDocument doc = QJsonDocument::fromJson( mReadJob->textData().toUtf8() );
      if ( !doc.isNull() && doc.isObject() )
      {
        QJsonObject authObject = doc.object();

        QString username = authObject.value( KEY_USERNAME ).toString();
        QString password = authObject.value( KEY_PASSWORD ).toString();
        int userId = authObject.value( KEY_USERID ).toInt();
        QString token = authObject.value( KEY_TOKEN ).toString();

        QString expireStr = authObject.value( KEY_EXPIRE ).toString();
        QDateTime tokenExpiration = QDateTime::fromString( expireStr, Qt::ISODate );

        emit authDataRead( username, password, userId, token, tokenExpiration );
        return;
      }
    }
    else
    {
      // if JSON parsing failed or no data was found => fallback to QSettings
      QSettings settings;
      settings.beginGroup( "Input/" );
      QString username = settings.value( KEY_USERNAME ).toString();
      QString password = settings.value( KEY_PASSWORD ).toString();
      int userId = settings.value( KEY_USERID ).toInt();
      QString token = settings.value( KEY_TOKEN ).toByteArray();
      QDateTime tokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
      settings.endGroup();

      emit authDataRead( username, password, userId, token, tokenExpiration );
      return;
    }

    // if an error occurred => emit empty values
    emit authDataRead( QString(), QString(), -1, QString(), QDateTime() );
  } );

  mReadJob->start();
}
