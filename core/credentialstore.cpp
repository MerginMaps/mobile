/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "credentialstore.h"
#include "coreutils.h"

CredentialStore::CredentialStore( QObject *parent )
  : QObject( parent )
{
  mWriteJob = new QKeychain::WritePasswordJob( "mergin_maps_auth", this );
  mWriteJob->setAutoDelete( false );

  mReadJob = new QKeychain::ReadPasswordJob( "mergin_maps_auth", this );
  mReadJob->setAutoDelete( false );
}

void CredentialStore::writeKey( const QString &key, const QVariant &value )
{
  mWriteJob->setKey( "Input/" + key );

  if ( value.type() == QVariant::ByteArray )
  {
    mWriteJob->setTextData( QString::fromUtf8( value.toByteArray().toBase64() ) );
  }
  else if ( value.type() == QVariant::DateTime )
  {
    mWriteJob->setTextData( value.toDateTime().toString( Qt::ISODate ) );
  }
  else
  {
    mWriteJob->setTextData( value.toString() );
  }

  disconnect( mWriteJob, nullptr, this, nullptr );

  connect( mWriteJob, &QKeychain::Job::finished, this, [this, key]()
  {
    if ( mWriteJob->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( key, mWriteJob->errorString() ) );
    }
  } );

  mWriteJob->start();
}

void CredentialStore::readKey( const QString &key )
{
  mReadJob->setKey( "Input/" + key );

  disconnect( mReadJob, nullptr, this, nullptr );
  connect( mReadJob, &QKeychain::Job::finished, this, [this, key]()
  {
    if ( !mReadJob->error() && !mReadJob->textData().isEmpty() )
    {
      emit keyRead( key, mReadJob->textData() );
    }
    else if ( mReadJob->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain read error (%1): %2" ).arg( key, mReadJob->errorString() ) );
    }
  } );

  mReadJob->start();
}
