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
#include <qt6keychain/keychain.h>

class CredentialStore : public QObject
{
    Q_OBJECT

  public:
    explicit CredentialStore( QObject *parent = nullptr );
    ~CredentialStore() = default;

    //! Write a key/value in keychain
    void writeKey( const QString &key, const QVariant &value );

    //! Reads a key from keychain and emits a signal with the value when job is finished
    void readKey( const QString &key );

  signals:
    //! Emitted when a key is read, with both key and its retrieved value.
    void keyRead( const QString &key, const QString &value );

  private:
    QKeychain::WritePasswordJob *mWriteJob = nullptr;
    QKeychain::ReadPasswordJob *mReadJob = nullptr;
};

#endif // CREDENTIALSTORE_H
