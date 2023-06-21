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
    Q_INVOKABLE bool hasLoginDetails() const;
    bool hasValidToken() const;

    //! Sets login details
    void setLoginDetails( const QString login, const QString password );
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

    //! Resets all auth - both login and auth details
    void clear();

    //! Persists all data in QSettings
    void persist();
    //! Loads all data from QSettings
    void load();

  private:
    QString mLogin;
    QString mPassword;

    QString mUsername;
    int mUserId = -1;
    QByteArray mAuthToken;
    QDateTime mTokenExpiration;
};

#endif // MERGINUSERAUTH_H
