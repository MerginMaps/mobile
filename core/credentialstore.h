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

    static inline const QString KEY_MM   = QStringLiteral( "mergin_maps" );
    static inline const QString KEY_AUTH_ENTRY   = QStringLiteral( "auth" );
    static inline const QString KEY_USERNAME = QStringLiteral( "username" );
    static inline const QString KEY_PASSWORD = QStringLiteral( "password" );
    static inline const QString KEY_USERID   = QStringLiteral( "userId" );
    static inline const QString KEY_TOKEN    = QStringLiteral( "token" );
    static inline const QString KEY_EXPIRE   = QStringLiteral( "expire" );

    //! Write authentication values data to keychain
    void writeAuthData( const QString &username,
                        const QString &password,
                        int userId,
                        const QString &token,
                        const QDateTime &tokenExpiration );

    //! Reads authentication data from keychain and emits a signal with all auth values
    void readAuthData();


  signals:
    //! Emitted when authentication data is read, including all authentication key values
    void authDataRead( const QString &username,
                       const QString &password,
                       int userId,
                       const QString &token,
                       const QDateTime &tokenExpiration );

    //! Emitted when a key is read, with both key and its retrieved value.
    void keyRead( const QString &key, const QString &value );

  private:
    //! Reads a key from keychain and emits a signal with the value when job is finished
    //! Do not call it multiple times without waiting for key reading to finish
    void readKey( const QString &key );

    //! Write a key/value in keychain
    //! Do not call it multiple times without waiting for key writing to finish
    void writeKey( const QString &key, const QString &value );

    void readCredentialsFromJson();

    void readCredentialsFromChain();

    QKeychain::WritePasswordJob *mWriteJob = nullptr;
    QKeychain::ReadPasswordJob *mReadJob = nullptr;
    QMetaObject::Connection mCredentialChainConnection;
    QMetaObject::Connection mCredentialChainWriteConnection;
};

#endif // CREDENTIALSTORE_H
