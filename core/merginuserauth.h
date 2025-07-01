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

class CredentialStore;

class MerginUserAuth: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString login READ login NOTIFY authChanged )

  public:
    enum AuthMethod
    {
      Password = 0,
      SSO,
    };
    Q_ENUM( AuthMethod )

    explicit MerginUserAuth( QObject *parent = nullptr );
    ~MerginUserAuth() = default;

  signals:
    void authChanged();
    void credentialsLoaded();

  public:
    //! Returns true if login/password is set, but that does not
    //! necessarily mean that we have managed to log in and get a token.
    Q_INVOKABLE bool hasAuthData();

    //! Returns true if we have a token and it is not expired,
    //! i.e. we should be good to do authenticated requests.
    Q_INVOKABLE bool hasValidToken() const;

    Q_INVOKABLE bool isUsingSso() const { return mMethod == AuthMethod::SSO; }

    void clear();

    /**
     * The identifier used to sign in the current user 
     *
     * For AuthMethod::Password this is username
     * For AuthMethod::SSO this is email address
     */
    QString login() const;
    void setLogin( const QString &newLogin );

    QString password() const;
    void setPassword( const QString &password );

    QByteArray authToken() const;
    void setAuthToken( const QByteArray &authToken );

    QDateTime tokenExpiration() const;
    void setTokenExpiration( const QDateTime &tokenExpiration );

    void clearTokenData();
    void saveAuthData();
    void loadAuthData();
    void setFromJson( QJsonObject docObj );
    void setFromSso( const QString &authToken, const QDateTime &tokenExpiration );

    AuthMethod authMethod() const;

  private:
    QString mLogin;
    QString mPassword;
    QByteArray mAuthToken;
    QDateTime mTokenExpiration;

    CredentialStore *mCredentialStore = nullptr; // owned by this

    AuthMethod mMethod = AuthMethod::Password;
};

#endif // MERGINUSERAUTH_H
