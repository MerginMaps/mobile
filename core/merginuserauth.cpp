/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginuserauth.h"

MerginUserAuth::MerginUserAuth( QObject *parent )
  : QObject( parent )
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
  if ( !mCredentialStore )
  {
    CoreUtils::log( "Auth", QString( "Credential store is not available for writing!!" ) );
    return;
  }

  qDebug() << "Saving auth data:"
           << "Username:" << mUsername
           << "Password:" << mPassword
           << "UserID:" << mUserId
           << "AuthToken:" << mAuthToken
           << "TokenExpiration:" << mTokenExpiration;

  mCredentialStore->writeAuthData( mUsername, mPassword, mUserId, mAuthToken, mTokenExpiration );
}

void MerginUserAuth::loadAuthData()
{
  if ( !mCredentialStore )
  {
    CoreUtils::log( "Auth", QString( "Credential store is not available for reading!!" ) );
    return;
  }

  connect( mCredentialStore, &CredentialStore::authDataRead, this, [this]
           ( const QString & username,
             const QString & password,
             int userId,
             const QString & token,
             const QDateTime & tokenExpiration )
  {
    mUsername = username;
    mPassword = password;
    mUserId = userId;
    mAuthToken = token.toUtf8();
    mTokenExpiration = tokenExpiration;

    qDebug() << "Read auth data:"
             << "Username:" << mUsername
             << "Password:" << mPassword
             << "UserID:" << mUserId
             << "AuthToken:" << mAuthToken
             << "TokenExpiration:" << mTokenExpiration;

    emit authChanged();
    emit credentialsLoaded();
  }, Qt::SingleShotConnection );

  mCredentialStore->readAuthData();
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
