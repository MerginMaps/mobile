/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginuserauth.h"
#include "coreutils.h"

MerginUserAuth::MerginUserAuth( QObject *parent )
  : QObject( parent )
{
  clear();
}

void MerginUserAuth::clear()
{
  mUsername = "";
  mPassword = "";
  mAuthToken.clear();
  mTokenExpiration.setTime( QTime() );
  mUserId = -1;

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
  deleteKey( "username" );
  deleteKey( "password" );
  deleteKey( "userId" );
  deleteKey( "token" );
  deleteKey( "expire" );
#else
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.remove( "username" );
  settings.remove( "password" );
  settings.remove( "userId" );
  settings.remove( "token" );
  settings.remove( "expire" );
  settings.endGroup();
#endif

  emit authChanged();
}

void MerginUserAuth::clearTokenData()
{
  mTokenExpiration = QDateTime().currentDateTime().addDays( -42 ); // to make it expired arbitrary days ago
  mAuthToken.clear();

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
  deleteKey( QStringLiteral( "token" ) );
  deleteKey( QStringLiteral( "expire" ) );
#else
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.remove( "token" );
  settings.remove( "expire" );
  settings.endGroup();
#endif

  emit authChanged();
}

bool MerginUserAuth::hasAuthData()
{
  return !mUsername.isEmpty() && !mPassword.isEmpty();
}

void MerginUserAuth::setFromJson( QJsonObject docObj )
{
  // parse profile data
  mUserId = docObj.value( QStringLiteral( "user" ) ).toInt();
  mUsername = docObj.value( QStringLiteral( "username" ) ).toString();

  // parse session data
  QJsonObject session = docObj.value( QStringLiteral( "session" ) ).toObject();
  mAuthToken = session.value( QStringLiteral( "token" ) ).toString().toUtf8();
  mTokenExpiration = QDateTime::fromString( session.value( QStringLiteral( "expire" ) ).toString(), Qt::ISODateWithMs ).toUTC();
  emit authChanged();
}

void MerginUserAuth::saveAuthData()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
  // mobile => QtKeychain
  writeKey( QStringLiteral( "username" ), mUsername );
  writeKey( QStringLiteral( "password" ), mPassword );
  writeKey( QStringLiteral( "userId" ), mUserId );
  writeKey( QStringLiteral( "token" ), mAuthToken );
  writeKey( QStringLiteral( "expire" ), mTokenExpiration );
#else
  // desktop => QSettings
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.setValue( QStringLiteral( "username" ), mUsername );
  settings.setValue( QStringLiteral( "password" ), mPassword );
  settings.setValue( QStringLiteral( "userId" ), mUserId );
  settings.setValue( QStringLiteral( "token" ), mAuthToken );
  settings.setValue( QStringLiteral( "expire" ), mTokenExpiration );
  settings.endGroup();
#endif
}

void MerginUserAuth::loadAuthData()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
  // mobile => QtKeychain
  readKey( QStringLiteral( "username" ), mUsername, []( const QString & v ) { return v; } );
  readKey( QStringLiteral( "password" ), mPassword, []( const QString & v ) { return v; } );
  readKey( QStringLiteral( "userId" ), mUserId, []( const QString & v ) { return v.toInt(); } );
  readKey( QStringLiteral( "token" ), mAuthToken, []( const QString & v ) { return QByteArray::fromBase64( v.toUtf8() ); } );
  readKey( QStringLiteral( "expire" ), mTokenExpiration, []( const QString & v ) { return QDateTime::fromString( v, Qt::ISODate ); } );
#else
  // desktop => QSettings
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );
  mUsername = settings.value( QStringLiteral( "username" ) ).toString();
  mPassword = settings.value( QStringLiteral( "password" ) ).toString();
  mUserId = settings.value( QStringLiteral( "userId" ) ).toInt();
  mTokenExpiration = settings.value( QStringLiteral( "expire" ) ).toDateTime();
  mAuthToken = settings.value( QStringLiteral( "token" ) ).toByteArray();
  settings.endGroup();
#endif
}

QString MerginUserAuth::username() const
{
  return mUsername;
}

void MerginUserAuth::setUsername( const QString &username )
{
  mUsername = username;
  emit authChanged();
}

QString MerginUserAuth::password() const
{
  return mPassword;
}

void MerginUserAuth::setPassword( const QString &password )
{
  mPassword = password;
  emit authChanged();
}

int MerginUserAuth::userId() const
{
  return mUserId;
}

void MerginUserAuth::setUserId( int userId )
{
  mUserId = userId;
  emit authChanged();
}

QByteArray MerginUserAuth::authToken() const
{
  return mAuthToken;
}

void MerginUserAuth::setAuthToken( const QByteArray &authToken )
{
  mAuthToken = authToken;
  emit authChanged();
}

QDateTime MerginUserAuth::tokenExpiration() const
{
  return mTokenExpiration;
}

void MerginUserAuth::setTokenExpiration( const QDateTime &tokenExpiration )
{
  mTokenExpiration = tokenExpiration;
  emit authChanged();
}

bool MerginUserAuth::hasValidToken() const
{
  return !mAuthToken.isEmpty() && mTokenExpiration >= QDateTime().currentDateTimeUtc();
}

void MerginUserAuth::deleteKey( const QString &key )
{
  auto *job = new QKeychain::DeletePasswordJob( "mergin_maps_auth", this );
  job->setAutoDelete( false );
  job->setKey( "Input/" + key );

  connect( job, &QKeychain::Job::finished, this, [this, job, key]()
  {
    if ( job->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain delete error (%1): %2" ).arg( key, job->errorString() ) );
    }
    job->deleteLater();
  } );

  job->start();
}

void MerginUserAuth::writeKey( const QString &key, const QVariant &value )
{
  auto *job = new QKeychain::WritePasswordJob( "mergin_maps_auth", this );
  job->setAutoDelete( false );
  job->setKey( "Input/" + key );

  if ( value.type() == QVariant::ByteArray )
  {
    job->setTextData( QString::fromUtf8( value.toByteArray().toBase64() ) );
  }
  else if ( value.type() == QVariant::DateTime )
  {
    job->setTextData( value.toDateTime().toString( Qt::ISODate ) );
  }
  else
  {
    job->setTextData( value.toString() );
  }

  connect( job, &QKeychain::Job::finished, this, [this, job, key]()
  {
    if ( job->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( key, job->errorString() ) );
    }
    job->deleteLater();
  } );

  job->start();
}

template <typename T, typename Converter>
void MerginUserAuth::readKey( const QString &key, T &destination, Converter converter )
{
  auto *job = new QKeychain::ReadPasswordJob( "mergin_maps_auth", this );
  job->setAutoDelete( false );
  job->setKey( "Input/" + key );

  connect( job, &QKeychain::Job::finished, this, [this, job, key, &destination, converter]()
  {
    if ( !job->error() && !job->textData().isEmpty() )
    {
      destination = converter( job->textData() );
    }
    else if ( job->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain read error (%1): %2" ).arg( key, job->errorString() ) );
    }
    job->deleteLater();
  } );

  job->start();
}
