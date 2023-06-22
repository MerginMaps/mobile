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

#include "inputconfig.h"

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QJsonObject>

/**
 * It consists of 2 parts: login details and auth data
 *
 * Login details (login + password) is the entry typed by user in login/registration form (and eventually stored/loaded in QSettings)
 * With login details it is possible to request auth data from Mergin server (username, id, token and its expiration)
 *
 * We recognize 4 states
 * 1. we do not have login credentials -> typically before registration or first user login (hasAuthData() == false)
 * 2. we have login credentials but not auth data (username, id, token) -> typically user entered login credentials, but we were not able to get data from server
 *    OR we have auth data (username, id, token) -> typically at some point in history we managed to get auth token ( hasAuthData() == true && hasValidToken() == false )
 * 3. we have VALID auth token -> mean that we have valid auth bearer token from server that is still not expired (hasValidToken() == true)
 *
 */

class MerginUserAuth: public QObject
{
#if defined(INPUT_TEST)
    friend class TestMerginApi;
#endif

    Q_OBJECT
    Q_PROPERTY( QString username READ username NOTIFY authChanged )
    Q_PROPERTY( int userId READ userId NOTIFY authChanged )

  public:
    explicit MerginUserAuth( QObject *parent = nullptr );
    ~MerginUserAuth() = default;

  signals:
    void authChanged();

  public:
    //! Contains login + password
    Q_INVOKABLE bool hasAuthData() const;
    //! Contains all data and valid token
    bool hasValidToken() const;

    //! Sets login details and clears auth data
    //! Does not emit any signal, requester should ask for auth if needed
    void setLoginCredentials( const QString login, const QString password );
    //! Resets all auth - both login and auth details
    void clear();
    //! Sets auth data from server JSON response
    void setFromJson( QJsonObject docObj );

    //! Returns username or email for login action
    QString login() const;
    //! Returns password for login action
    QString password() const;
    //! Returns associated username on Mergin server with login name
    QString username() const;
    //! Returns associated user ID on Mergin server with login name
    int userId() const;
    //! Returns server generated bearer login token
    QByteArray authToken() const;
    //! Returns expiration time of server generated bearer token
    QDateTime tokenExpiration() const;

    //! Persists all data in QSettings
    void saveAuthData();
    //! Loads all data from QSettings
    void loadAuthData();

  private:
    QString mLogin;
    QString mPassword;

    QString mUsername;
    int mUserId = -1;
    QByteArray mAuthToken;
    QDateTime mTokenExpiration;
};

#endif // MERGINUSERAUTH_H
