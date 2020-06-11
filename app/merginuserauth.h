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
    Q_INVOKABLE bool hasAuthData();

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
};

#endif // MERGINUSERAUTH_H
