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
  if ( !mCredentialStore )
    return;

  QJsonObject authObject;

  authObject.insert( KEY_USERNAME, mUsername );
  authObject.insert( KEY_PASSWORD, mPassword );
  authObject.insert( KEY_USERID, mUserId );
  authObject.insert( KEY_TOKEN, QString::fromUtf8( mAuthToken.toBase64() ) );
  authObject.insert( KEY_EXPIRE, mTokenExpiration.toUTC().toString( Qt::ISODate ) );

  QJsonDocument doc( authObject );
  QString jsonString = QString::fromUtf8( doc.toJson( QJsonDocument::Compact ) );

  mCredentialStore->writeKey( "authData", jsonString );
}

void MerginUserAuth::loadAuthData()
{
  if ( !mCredentialStore )
    return;

  connect( mCredentialStore, &CredentialStore::keyRead, this, [this]( const QString & key, const QString & value )
  {
    if ( key == "authData" )
    {
      QJsonDocument doc = QJsonDocument::fromJson( value.toUtf8() );
      if ( !doc.isNull() && doc.isObject() )
      {
        QJsonObject authObject = doc.object();

        mUsername = authObject.value( KEY_USERNAME ).toString();
        mPassword = authObject.value( KEY_PASSWORD ).toString();
        mUserId = authObject.value( KEY_USERID ).toInt();

        QString tokenStr = authObject.value( KEY_TOKEN ).toString();
        mAuthToken = QByteArray::fromBase64( tokenStr.toUtf8() );

        mTokenExpiration = QDateTime::fromString( authObject.value( KEY_EXPIRE ).toString(), Qt::ISODate );
      }
      else
      {

        qDebug() << "fallback reached";
        QSettings settings;
        settings.beginGroup( "Input/" );
        mUsername = settings.value( KEY_USERNAME ).toString();
        mPassword = settings.value( KEY_PASSWORD ).toString();
        mUserId = settings.value( KEY_USERID ).toInt();
        mAuthToken = settings.value( KEY_TOKEN ).toByteArray();
        mTokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
        settings.endGroup();
      }

      emit authChanged();
      emit credentialsLoaded();
    }
  } );

  mCredentialStore->readKey( "authData" );
}

// void MerginUserAuth::saveAuthData()
// {
//   if ( mCredentialStore )
//   {
//     mCredentialChainConnection = connect( mCredentialStore, &CredentialStore::keyWritten, this, [this]( const QString & key )
//     {
//       if ( key == KEY_USERNAME )
//       {
//         mCredentialStore->writeKey( KEY_PASSWORD, mPassword );
//       }
//       else if ( key == KEY_PASSWORD )
//       {
//         mCredentialStore->writeKey( KEY_USERID, mUserId );
//       }
//       else if ( key == KEY_USERID )
//       {
//         mCredentialStore->writeKey( KEY_TOKEN, mAuthToken );
//       }
//       else if ( key == KEY_TOKEN )
//       {
//         mCredentialStore->writeKey( KEY_EXPIRE, mTokenExpiration );
//       }
//       else if ( key == KEY_EXPIRE )
//       {
//         disconnect( mCredentialChainConnection );
//       }
//     } );

//     mCredentialStore->writeKey( KEY_USERNAME, mUsername );
//   }
// }

// void MerginUserAuth::loadAuthData()
// {
//   // loads credentials data via chained key reads; falls back to QSettings if any are invalid
//   if ( mCredentialStore )
//   {
//     mCredentialChainConnection = connect( mCredentialStore, &CredentialStore::keyRead, this, [this]( const QString & key, const QString & value )
//     {
//       if ( key == KEY_USERNAME )
//       {
//         mUsername = value;
//         mCredentialStore->readKey( KEY_PASSWORD );
//       }
//       else if ( key == KEY_PASSWORD )
//       {
//         mPassword = value;
//         mCredentialStore->readKey( KEY_USERID );
//       }
//       else if ( key == KEY_USERID )
//       {
//         mUserId = value.toInt();
//         mCredentialStore->readKey( KEY_TOKEN );
//       }
//       else if ( key == KEY_TOKEN )
//       {
//         mAuthToken = QByteArray::fromBase64( value.toUtf8() );
//         mCredentialStore->readKey( KEY_EXPIRE );
//       }
//       else if ( key == KEY_EXPIRE )
//       {
//         mTokenExpiration = QDateTime::fromString( value, Qt::ISODate );

//         disconnect( mCredentialChainConnection );

//         bool hasAllCredentials = !mUsername.isEmpty() && !mPassword.isEmpty() && ( mUserId != 0 ) && !mAuthToken.isEmpty() && mTokenExpiration.isValid();

//         if ( !hasAllCredentials ) // fallback => load all credentials from QSettings
//         {
//           qDebug() << "fallback reached";
//           // QSettings settings;
//           // settings.beginGroup( "Input/" );
//           // mUsername = settings.value( KEY_USERNAME ).toString();
//           // mPassword = settings.value( KEY_PASSWORD ).toString();
//           // mUserId = settings.value( KEY_USERID ).toInt();
//           // mAuthToken = settings.value( KEY_TOKEN ).toByteArray();
//           // mTokenExpiration = settings.value( KEY_EXPIRE ).toDateTime();
//           // settings.endGroup();
//         }
//         else if ( mTokenExpiration < QDateTime::currentDateTimeUtc() )
//         {
//           CoreUtils::log( "Auth", "Token is expired." );
//         }

//         emit authChanged();
//         emit credentialsLoaded();
//       }
//     } );

//     mCredentialStore->readKey( KEY_USERNAME );
//   }
// }


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
