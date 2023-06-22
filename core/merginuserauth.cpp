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
{
}

void MerginUserAuth::clear()
{
  mLogin = "";
  mPassword = "";

  mUsername = "";
  mAuthToken.clear();
  mTokenExpiration.setTime( QTime() );
  mUserId = -1;

  emit authChanged();
}

bool MerginUserAuth::hasAuthData() const
{
  return !mLogin.isEmpty() && !mPassword.isEmpty();
}

bool MerginUserAuth::hasValidToken() const
{
  return ( !mAuthToken.isEmpty() && mTokenExpiration > QDateTime::currentDateTimeUtc() );
}

void MerginUserAuth::setLoginCredentials( const QString login, const QString password )
{
  Q_ASSERT( !login.isEmpty() && !password.isEmpty() );

  bool changed = false;

  if ( mLogin != login )
  {
    mLogin = login;
    changed = true;
  }

  if ( mPassword != password )
  {
    mPassword = password;
    changed = true;
  }

  if ( changed )
  {
    mUsername = "";
    mAuthToken.clear();
    mTokenExpiration.setTime( QTime() );
    mUserId = -1;
  }
}

void MerginUserAuth::setFromJson( QJsonObject docObj )
{
  bool changed = false;

  // parse profile data
  int userId = docObj.value( QStringLiteral( "user" ) ).toInt();
  if ( mUserId != userId )
  {
    mUserId = userId;
    changed = true;
  }

  QString username = docObj.value( QStringLiteral( "username" ) ).toString();
  if ( mUsername != username )
  {
    mUsername = username;
    changed = true;
  }


  // parse session data
  QJsonObject session = docObj.value( QStringLiteral( "session" ) ).toObject();
  QByteArray authToken = session.value( QStringLiteral( "token" ) ).toString().toUtf8();
  if ( mAuthToken != authToken )
  {
    mAuthToken = authToken;
    changed = true;
  }

  QDateTime tokenExpiration = QDateTime::fromString( session.value( QStringLiteral( "expire" ) ).toString(), Qt::ISODateWithMs ).toUTC();
  if ( mTokenExpiration != tokenExpiration )
  {
    mTokenExpiration = tokenExpiration;
    changed = true;
  }

  if ( changed )
  {
    emit authChanged();
  }
}

void MerginUserAuth::saveAuthData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );

  settings.setValue( "login", mLogin );
  settings.setValue( "password", mPassword );

  settings.setValue( "username", mUsername );
  settings.setValue( "userId", mUserId );
  settings.setValue( "token", mAuthToken );
  settings.setValue( "expire", mTokenExpiration );

  settings.endGroup();
}

void MerginUserAuth::loadAuthData()
{
  QSettings settings;
  settings.beginGroup( QStringLiteral( "Input/" ) );

  if ( settings.contains( QStringLiteral( "login" ) ) )
  {
    mLogin = settings.value( QStringLiteral( "login" ) ).toString();
  }
  else
  {
    // for backward compatibility for Input App <= 2.2
    mLogin = settings.value( QStringLiteral( "username" ) ).toString();
  }
  mPassword = settings.value( QStringLiteral( "password" ) ).toString();

  mUsername = settings.value( QStringLiteral( "username" ) ).toString();
  mUserId = settings.value( QStringLiteral( "userId" ) ).toInt();
  mTokenExpiration = settings.value( QStringLiteral( "expire" ) ).toDateTime();
  mAuthToken = settings.value( QStringLiteral( "token" ) ).toByteArray();

  settings.endGroup();
}


QString MerginUserAuth::login() const
{
  return mLogin;
}

QString MerginUserAuth::password() const
{
  return mPassword;
}

QString MerginUserAuth::username() const
{
  return mUsername;
}

int MerginUserAuth::userId() const
{
  return mUserId;
}

QByteArray MerginUserAuth::authToken() const
{
  return mAuthToken;
}


QDateTime MerginUserAuth::tokenExpiration() const
{
  return mTokenExpiration;
}

