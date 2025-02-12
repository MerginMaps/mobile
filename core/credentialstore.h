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
#include <QDateTime>

#include <qt6keychain/keychain.h>

class CredentialStore : public QObject
{
    Q_OBJECT

  public:
    explicit CredentialStore( QObject *parent = nullptr );
    ~CredentialStore() = default;

    static const QString KEYCHAIN_GROUP;
    static const QString KEYCHAIN_ENTRY_CREDENTIALS;
    static const QString KEYCHAIN_ENTRY_TOKEN;

    static const QString KEY_USERNAME;
    static const QString KEY_PASSWORD;
    static const QString KEY_USERID;
    static const QString KEY_TOKEN;
    static const QString KEY_EXPIRE;

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

  private:

    //! Reads a key from keychain and stores the value in the intermediary results
    //! The method recursively calls itself to read both Keychain entries
    void readKeyRecursively( const QString &key );

    void finishReadingOperation();

    QMap<QString, QString> mReadResults; // to store intermediary read results

    QKeychain::WritePasswordJob *mWriteJob = nullptr; // owned by this
    QKeychain::ReadPasswordJob *mReadJob = nullptr; // owned by this
};

#endif // CREDENTIALSTORE_H
