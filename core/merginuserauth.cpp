/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "merginuserauth.h"
#include "credentialstore.h"
#include "coreutils.h"

MerginUserAuth::MerginUserAuth( QObject *parent )
  : QObject( parent )
  , mCredentialStore( new CredentialStore( this ) )
{
  clear();
}

void MerginUserAuth::clear()
{
  mLogin = "";
  mPassword = "";
  mAuthToken.clear();
  mTokenExpiration = QDateTime();
  mMethod = AuthMethod::Password;

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
  switch ( mMethod )
  {
    case AuthMethod::Password: return !mLogin.isEmpty() && !mPassword.isEmpty();
    case AuthMethod::SSO: return true;
  }

  return false;
}

void MerginUserAuth::setFromJson( QJsonObject docObj )
{
  mMethod = AuthMethod::Password;
  // parse profile data
  mLogin = docObj.value( QStringLiteral( "username" ) ).toString();

  // parse session data
  QJsonObject session = docObj.value( QStringLiteral( "session" ) ).toObject();
  mAuthToken = session.value( QStringLiteral( "token" ) ).toString().toUtf8();
  mTokenExpiration = QDateTime::fromString( session.value( QStringLiteral( "expire" ) ).toString(), Qt::ISODateWithMs ).toUTC();
  emit authChanged();
}

void MerginUserAuth::setFromSso( const QString &authToken, const QDateTime &tokenExpiration )
{
  mLogin = QString();
  mPassword = QString();
  mAuthToken = authToken.toUtf8();
  mTokenExpiration = tokenExpiration.toUTC();
  mMethod = AuthMethod::SSO;

  emit authChanged();
}

MerginUserAuth::AuthMethod MerginUserAuth::authMethod() const
{
  return mMethod;
}

void MerginUserAuth::saveData()
{
  if ( !mCredentialStore )
  {
    CoreUtils::log( "Auth", QString( "Credential store is not available for writing!!" ) );
    return;
  }

  mCredentialStore->writeAuthData( mLogin, mPassword, mAuthToken, mTokenExpiration, mMethod );
}

void MerginUserAuth::loadData()
{
  if ( !mCredentialStore )
  {
    CoreUtils::log( "Auth", QString( "Credential store is not available for reading!!" ) );
    return;
  }

  connect( mCredentialStore, &CredentialStore::authDataRead, this, [this]
           ( const QString & login,
             const QString & password,
             const QString & token,
             const QDateTime & tokenExpiration,
             int method )
  {
    mLogin = login;
    mPassword = password;
    mAuthToken = token.toUtf8();
    mTokenExpiration = tokenExpiration;
    mMethod = static_cast<AuthMethod>( method );

    emit authChanged();
  }, Qt::SingleShotConnection );

  mCredentialStore->readAuthData();
}

QString MerginUserAuth::login() const
{
  return mLogin;
}

void MerginUserAuth::setLogin( const QString &newLogin )
{
  mLogin = newLogin;
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
