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
#include <QMetaObject>

MerginUserAuth::MerginUserAuth( QObject *parent )
  : QObject( parent )
  , mUsername( "" )
  , mPassword( "" )
  , mUserId( -1 )
  , mAuthToken()
  , mTokenExpiration()
  , mCredentialStore( new CredentialStore( this ) )
{
  clear();
}

void MerginUserAuth::clear()
{
  mUsername = "";
  mPassword = "";
  mAuthToken.clear();
  mTokenExpiration = QDateTime();
  mUserId = -1;

  if ( mCredentialStore )
  {
    mCredentialStore->writeKey( KEY_USERNAME, "" );
    mCredentialStore->writeKey( KEY_PASSWORD, "" );
    mCredentialStore->writeKey( KEY_USERID, "" );
    mCredentialStore->writeKey( KEY_TOKEN, "" );
    mCredentialStore->writeKey( KEY_EXPIRE, "" );
  }

  emit authChanged();
}

void MerginUserAuth::clearTokenData()
{
  mTokenExpiration = QDateTime().currentDateTime().addDays( -42 ); // to make it expired arbitrary days ago
  mAuthToken.clear();

  if ( mCredentialStore )
  {
    mCredentialStore->writeKey( KEY_TOKEN, "" );
    mCredentialStore->writeKey( KEY_EXPIRE, "" );
  }

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
  mCredentialStore->writeKey( KEY_USERNAME, mUsername );
  mCredentialStore->writeKey( KEY_PASSWORD, mPassword );
  mCredentialStore->writeKey( KEY_USERID, mUserId );
  mCredentialStore->writeKey( KEY_TOKEN, mAuthToken );
  mCredentialStore->writeKey( KEY_EXPIRE, mTokenExpiration );
}

void MerginUserAuth::loadAuthData()
{
  // loads credentials data via chained key reads; falls back to QSettings if any are invalid
  if ( mCredentialStore )
  {
    mCredentialChainConnection = connect( mCredentialStore, &CredentialStore::keyRead, this, [this]( const QString & key, const QString & value )
    {
      if ( key == KEY_USERNAME )
      {
        mUsername = value;
        mCredentialStore->readKey( KEY_PASSWORD );
      }
      else if ( key == KEY_PASSWORD )
      {
        mPassword = value;
        mCredentialStore->readKey( KEY_USERID );
      }
      else if ( key == KEY_USERID )
      {
        mUserId = value.toInt();
        mCredentialStore->readKey( KEY_TOKEN );
      }
      else if ( key == KEY_TOKEN )
      {
        mAuthToken = QByteArray::fromBase64( value.toUtf8() );
        mCredentialStore->readKey( KEY_EXPIRE );
      }
      else if ( key == KEY_EXPIRE )
      {
        mTokenExpiration = QDateTime::fromString( value, Qt::ISODate );

        disconnect( mCredentialChainConnection );

        bool hasAllCredentials = !mUsername.isEmpty() && !mPassword.isEmpty() && ( mUserId != 0 ) && !mAuthToken.isEmpty() && mTokenExpiration.isValid();

        if ( !hasAllCredentials ) // fallback => load all credentials from QSettings
        {
          QSettings settings;
          settings.beginGroup( "Input/" );
          mUsername = settings.value( KEY_USERNAME ).toString();
          mPassword = settings.value( KEY_PASSWORD ).toString();
          mUserId = settings.value( KEY_USERID ).toInt();
          mAuthToken = settings.value( KEY_TOKEN ).toByteArray();
          mTokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
          settings.endGroup();
        }
        else if ( mTokenExpiration < QDateTime::currentDateTimeUtc() )
        {
          CoreUtils::log( "Auth", "Token is expired." );
        }

        emit authChanged();
        emit credentialsLoaded();
      }
    } );

    mCredentialStore->readKey( KEY_USERNAME );
  }
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

bool MerginUserAuth::isLoggedIn()
{
  return hasAuthData() && hasValidToken();
}
