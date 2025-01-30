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

  emit authChanged();
}

void MerginUserAuth::clearTokenData()
{
  mTokenExpiration = QDateTime().currentDateTime().addDays( -42 ); // to make it expired arbitrary days ago
  mAuthToken.clear();
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
  const QString group = QStringLiteral( "Input/" );

  auto writeJob = [this, group]( const QString & key, const QVariant & value )
  {
    auto *job = new QKeychain::WritePasswordJob( QStringLiteral( "mergin_maps_auth" ), this );
    job->setAutoDelete( false );
    job->setKey( group + key );

    if ( value.type() == QVariant::ByteArray )
      job->setTextData( QString::fromUtf8( value.toByteArray().toBase64() ) );
    else if ( value.type() == QVariant::DateTime )
      job->setTextData( value.toDateTime().toString( Qt::ISODate ) );
    else
      job->setTextData( value.toString() );

    connect( job, &QKeychain::Job::finished, this, [this, job, key]()
    {
      if ( job->error() )
      {
        CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( key, job->errorString() ) );
      }
      job->deleteLater();
    } );

    job->start();
  };

  writeJob( QStringLiteral( "username" ), mUsername );
  writeJob( QStringLiteral( "password" ), mPassword );
  writeJob( QStringLiteral( "userId" ), mUserId );
  writeJob( QStringLiteral( "token" ), mAuthToken );
  writeJob( QStringLiteral( "expire" ), mTokenExpiration );

#else
  // desktop => QSettings
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );
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
  const QString group = QStringLiteral( "Input/" );

  auto readJob = [this, group]( const QString & key, auto & destination, std::function<typename std::decay<decltype( destination )>::type( const QString & )> converter )
  {
    auto *job = new QKeychain::ReadPasswordJob( QStringLiteral( "mergin_maps_auth" ), this );
    job->setAutoDelete( false );
    job->setKey( group + key );

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
  };

  readJob( QStringLiteral( "username" ), mUsername, []( const QString & v ) { return v; } );
  readJob( QStringLiteral( "password" ), mPassword, []( const QString & v ) { return v; } );
  readJob( QStringLiteral( "userId" ), mUserId, []( const QString & v ) { return v.toInt(); } );

  readJob( QStringLiteral( "token" ), mAuthToken, []( const QString & v )
  {
    return QByteArray::fromBase64( v.toUtf8() );
  } );

  readJob( QStringLiteral( "expire" ), mTokenExpiration, []( const QString & v )
  {
    return QDateTime::fromString( v, Qt::ISODate );
  } );

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
