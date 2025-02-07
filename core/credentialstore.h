/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CREDENTIALSTORE_H
#define CREDENTIALSTORE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QSettings>
#include <QJsonObject>
#include <QJsonDocument>
#include <qt6keychain/keychain.h>

class CredentialStore : public QObject
{
    Q_OBJECT

  public:
    explicit CredentialStore( QObject *parent = nullptr );
    ~CredentialStore() = default;

    static inline const QString KEY_GROUP    = QStringLiteral( "mergin_auth_data" );
    static inline const QString KEY_USERNAME = QStringLiteral( "username" );
    static inline const QString KEY_PASSWORD = QStringLiteral( "password" );
    static inline const QString KEY_USERID   = QStringLiteral( "userId" );
    static inline const QString KEY_TOKEN    = QStringLiteral( "token" );
    static inline const QString KEY_EXPIRE   = QStringLiteral( "expire" );

    //! Write a key/value in keychain
    void writeKey( const QString &key, const QVariant &value );

    //! Reads a key from keychain and emits a signal with the value when job is finished
    void readKey( const QString &key );

    //! Write authentication values data to keychain
    void writeAuthData( const QString &username,
                        const QString &password,
                        int userId,
                        const QByteArray &token,
                        const QDateTime &tokenExpiration );

    //! Reads authentication data from keychain and emits a signal with all auth values
    void readAuthData();

  signals:
    //! Emitted when authentication data is read, including all authentication key values
    void authDataRead( const QString &username,
                       const QString &password,
                       int userId,
                       const QByteArray &token,
                       const QDateTime &tokenExpiration );

  private:
    QKeychain::WritePasswordJob *mWriteJob = nullptr;
    QKeychain::ReadPasswordJob *mReadJob = nullptr;
};

#endif // CREDENTIALSTORE_H
