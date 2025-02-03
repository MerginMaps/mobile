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
  , mUsername( "" )
  , mPassword( "" )
  , mUserId( -1 )
  , mAuthToken()
  , mTokenExpiration()
  , mCredentialStore( new CredentialStore( this ) )
{
}

void MerginUserAuth::clear()
{
  mUsername = "";
  mPassword = "";
  mAuthToken.clear();
  mTokenExpiration.setTime( QTime() );
  mUserId = -1;

// #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
//   mCredentialStore->deleteKey( "username" );
//   mCredentialStore->deleteKey( "password" );
//   mCredentialStore->deleteKey( "userId" );
//   mCredentialStore->deleteKey( "token" );
//   mCredentialStore->deleteKey( "expire" );
// #else
//   QSettings settings;
//   settings.beginGroup( "Input/" );
//   settings.remove( "username" );
//   settings.remove( "password" );
//   settings.remove( "userId" );
//   settings.remove( "token" );
//   settings.remove( "expire" );
//   settings.endGroup();
// #endif

  emit authChanged();
}

void MerginUserAuth::clearTokenData()
{
  mTokenExpiration = QDateTime().currentDateTime().addDays( -42 ); // to make it expired arbitrary days ago
  mAuthToken.clear();

// #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
//   mCredentialStore->deleteKey( QStringLiteral( "token" ) );
//   mCredentialStore->deleteKey( QStringLiteral( "expire" ) );
// #else
//   QSettings settings;
//   settings.beginGroup( "Input/" );
//   settings.remove( "token" );
//   settings.remove( "expire" );
//   settings.endGroup();
// #endif

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
  // mobile => QtKeychain via CredentialStore
  mCredentialStore->writeKey( QStringLiteral( "username" ), mUsername );
  mCredentialStore->writeKey( QStringLiteral( "password" ), mPassword );
  mCredentialStore->writeKey( QStringLiteral( "userId" ), mUserId );
  mCredentialStore->writeKey( QStringLiteral( "token" ), mAuthToken );
  mCredentialStore->writeKey( QStringLiteral( "expire" ), mTokenExpiration );
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

// void MerginUserAuth::loadAuthData()
// {
// #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
//   // mobile => QtKeychain
//   connect( mCredentialStore, &CredentialStore::keyRead, this, [this]( const QString & key, const QVariant & value ) //read credentials in chain to emit authChanged only
//   {
//     if ( key == "username" )
//     {
//       mUsername = value.toString();
//       mCredentialStore->readKey( "password" );
//     }
//     else if ( key == "password" )
//     {
//       mPassword = value.toString();
//       mCredentialStore->readKey( "userId" );
//     }
//     else if ( key == "userId" )
//     {
//       mUserId = value.toInt();
//       mCredentialStore->readKey( "token" );
//     }
//     else if ( key == "token" )
//     {
//       mAuthToken = value.toByteArray();
//       mCredentialStore->readKey( "expire" );
//     }
//     else if ( key == "expire" )
//     {
//       mTokenExpiration = value.toDateTime();

//       if ( mTokenExpiration < QDateTime::currentDateTimeUtc() )
//       {
//         clearTokenData();
//       }

//       emit authChanged();
//     }
//   } );

//   mCredentialStore->readKey( "username" );
// #else
//   // desktop => QSettings
//   QSettings settings;
//   settings.beginGroup( QStringLiteral( "Input/" ) );
//   mUsername = settings.value( QStringLiteral( "username" ) ).toString();
//   mPassword = settings.value( QStringLiteral( "password" ) ).toString();
//   mUserId = settings.value( QStringLiteral( "userId" ) ).toInt();
//   mTokenExpiration = settings.value( QStringLiteral( "expire" ) ).toDateTime();
//   mAuthToken = settings.value( QStringLiteral( "token" ) ).toByteArray();
//   settings.endGroup();

//   emit authChanged();
// #endif
// }

void MerginUserAuth::loadAuthData()
{
  qDebug() << "TESTLOADAUTHDATA: Starting loadAuthData()";

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
  // mobile => QtKeychain via CredentialStore asynchronous chain
  if ( mCredentialStore )
  {
    qDebug() << "TESTLOADAUTHDATA: Using CredentialStore for authentication";

    connect( mCredentialStore, &CredentialStore::keyRead, this, [this]( const QString & key, const QString & value )
    {
      qDebug() << "TESTLOADAUTHDATA: keyRead signal received - key:" << key << ", value:" << value;

      if ( key == "username" )
      {
        mUsername = value;
        qDebug() << "TESTLOADAUTHDATA: Username set to" << mUsername;
        mCredentialStore->readKey( QStringLiteral( "password" ) );
      }
      else if ( key == "password" )
      {
        mPassword = value;
        qDebug() << "TESTLOADAUTHDATA: Password set";
        mCredentialStore->readKey( QStringLiteral( "userId" ) );
      }
      else if ( key == "userId" )
      {
        mUserId = value.toInt();
        qDebug() << "TESTLOADAUTHDATA: UserId set to" << mUserId;
        mCredentialStore->readKey( QStringLiteral( "token" ) );
      }
      else if ( key == "token" )
      {
        mAuthToken = QByteArray::fromBase64( value.toUtf8() );
        qDebug() << "TESTLOADAUTHDATA: Token set (Base64 decoded)";
        mCredentialStore->readKey( QStringLiteral( "expire" ) );
      }
      else if ( key == "expire" )
      {
        mTokenExpiration = QDateTime::fromString( value, Qt::ISODate );
        qDebug() << "TESTLOADAUTHDATA: Token expiration set to" << mTokenExpiration.toString(Qt::ISODate);

        if ( mTokenExpiration < QDateTime::currentDateTimeUtc() )
        {
          qDebug() << "TESTLOADAUTHDATA: Token is expired.";
          CoreUtils::log( "Auth", "Token is expired." );
        }

        emit authChanged();
        qDebug() << "TESTLOADAUTHDATA: authChanged() emitted";

        emit credentialsLoaded();
        qDebug() << "TESTLOADAUTHDATA: credentialsLoaded() emitted";
      }
    } );

    qDebug() << "TESTLOADAUTHDATA: Reading username from CredentialStore";
    mCredentialStore->readKey( QStringLiteral( "username" ) );
  }
  else
  {
    qDebug() << "TESTLOADAUTHDATA: mCredentialStore is null!";
  }
#else
  // desktop => QSettings
  qDebug() << "TESTLOADAUTHDATA: Using QSettings for authentication";
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );

  mUsername = settings.value( QStringLiteral( "username" ) ).toString();
  qDebug() << "TESTLOADAUTHDATA: Username set to" << mUsername;

  mPassword = settings.value( QStringLiteral( "password" ) ).toString();
  qDebug() << "TESTLOADAUTHDATA: Password set";

  mUserId = settings.value( QStringLiteral( "userId" ) ).toInt();
  qDebug() << "TESTLOADAUTHDATA: UserId set to" << mUserId;

  mTokenExpiration = settings.value( QStringLiteral( "expire" ) ).toDateTime();
  qDebug() << "TESTLOADAUTHDATA: Token expiration set to" << mTokenExpiration.toString(Qt::ISODate);

  mAuthToken = settings.value( QStringLiteral( "token" ) ).toByteArray();
  qDebug() << "TESTLOADAUTHDATA: Token set (raw bytes)";

  settings.endGroup();
#endif

  qDebug() << "TESTLOADAUTHDATA: Finished loadAuthData()";
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
