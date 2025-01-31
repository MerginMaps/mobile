/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MERGINUSERAUTH_H
#define MERGINUSERAUTH_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QJsonObject>
#include <qt6keychain/keychain.h>

class MerginUserAuth: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString username READ username NOTIFY authChanged )
    Q_PROPERTY( int userId READ userId NOTIFY authChanged )

  public:
    explicit MerginUserAuth( QObject *parent = nullptr );
    ~MerginUserAuth() = default;
  signals:
    void authChanged();

  public:
    //! Returns true if username/password is set, but that does not
    //! necessarily mean that we have managed to log in and get a token.
    Q_INVOKABLE bool hasAuthData();

    //! Returns true if we have a token and it is not expired,
    //! i.e. we should be good to do authenticated requests.
    Q_INVOKABLE bool hasValidToken() const;

    void clear();

    QString username() const;
    void setUsername( const QString &username );

    QString password() const;
    void setPassword( const QString &password );

    int userId() const;
    void setUserId( int userId );

    QByteArray authToken() const;
    void setAuthToken( const QByteArray &authToken );

    QDateTime tokenExpiration() const;
    void setTokenExpiration( const QDateTime &tokenExpiration );

    void clearTokenData();
    void saveAuthData();
    void loadAuthData();
    void setFromJson( QJsonObject docObj );

  private:
    //! Delete a key in keychain
    void deleteKey( const QString &key );

    //! Write a key/value in keychain
    void writeKey( const QString &key, const QVariant &value );

    //! Read a key from keychain into a destination variable
    template <typename T, typename Converter>
    void readKey( const QString &key, T &destination, Converter converter );

    QString mUsername;
    QString mPassword;
    int mUserId = -1;
    QByteArray mAuthToken;
    QDateTime mTokenExpiration;
};

#endif // MERGINUSERAUTH_H
