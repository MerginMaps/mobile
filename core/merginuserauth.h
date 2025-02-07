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

#include "credentialstore.h"

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
    void credentialsLoaded();

  public:
    //! Returns true if username/password is set, but that does not
    //! necessarily mean that we have managed to log in and get a token.
    Q_INVOKABLE bool hasAuthData();

    //! Returns true if we have a token and it is not expired,
    //! i.e. we should be good to do authenticated requests.
    Q_INVOKABLE bool hasValidToken() const;

    //! Returns whether user is currently logged in
    Q_INVOKABLE bool isLoggedIn();

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
    QString mUsername;
    QString mPassword;
    int mUserId = -1;
    QByteArray mAuthToken;
    QDateTime mTokenExpiration;
    CredentialStore *mCredentialStore = nullptr;
};

#endif // MERGINUSERAUTH_H
