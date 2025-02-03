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
}

void CredentialStore::writeKey( const QString &key, const QVariant &value )
{
  auto *job = new QKeychain::WritePasswordJob( "mergin_maps_auth", this );
  job->setAutoDelete( false );
  job->setKey( "Input/" + key );

  if ( value.type() == QVariant::ByteArray )
  {
    job->setTextData( QString::fromUtf8( value.toByteArray().toBase64() ) );
  }
  else if ( value.type() == QVariant::DateTime )
  {
    job->setTextData( value.toDateTime().toString( Qt::ISODate ) );
  }
  else
  {
    job->setTextData( value.toString() );
  }

  connect( job, &QKeychain::Job::finished, this, [this, job, key]()
  {
    if ( job->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain write error (%1): %2" ).arg( key, job->errorString() ) );
    }
    job->deleteLater();
  } );

  job->start();
}

void CredentialStore::deleteKey( const QString &key )
{
  auto *job = new QKeychain::DeletePasswordJob( "mergin_maps_auth", this );
  job->setAutoDelete( false );
  job->setKey( "Input/" + key );

  connect( job, &QKeychain::Job::finished, this, [this, job, key]()
  {
    if ( job->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain delete error (%1): %2" ).arg( key, job->errorString() ) );
    }
    job->deleteLater();
  } );

  job->start();
}

void CredentialStore::readKey( const QString &key )
{
  auto *job = new QKeychain::ReadPasswordJob( "mergin_maps_auth", this );
  job->setAutoDelete( false );
  job->setKey( "Input/" + key );

  connect( job, &QKeychain::Job::finished, this, [this, job, key]()
  {
    if ( !job->error() && !job->textData().isEmpty() )
    {
      emit keyRead( key, job->textData() );
    }
    else if ( job->error() )
    {
      CoreUtils::log( "Auth", QString( "Keychain read error (%1): %2" ).arg( key, job->errorString() ) );
    }
    job->deleteLater();
  } );

  job->start();
}
