/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFile>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDataStream>

#include "checksum.h"

static const int CHUNK_SIZE = 65536;
const QString Checksum::sCacheFile = QStringLiteral( ".checksum.cache" );

Checksum::Checksum( const QString &projectDir )
  : mProjectDir( projectDir )
{

}

void Checksum::load()
{
  mCache.clear();
  mCacheModified = false;

  QFile f( mProjectDir + "/" + sCacheFile );

  if ( f.open( QIODevice::ReadOnly ) )
  {
    QDataStream stream( &f );
    stream.setVersion( QDataStream::Qt_6_5 );
    QString path;
    QString checksum;
    QDateTime mtime;
    CacheValue entry;

    while ( stream.atEnd() == false )
    {
      stream >> path >> checksum >> mtime;
      entry.checksum = checksum;
      entry.mtime = mtime;
      mCache.insert( path, entry );
    }
  }
}

void Checksum::save()
{
  if ( !mCacheModified )
    return;

  QFile f( mProjectDir + "/" + sCacheFile );

  if ( f.open( QIODevice::WriteOnly ) ) // implies Truncate
  {
    QDataStream stream( &f );
    stream.setVersion( QDataStream::Qt_6_5 );

    for ( auto it = mCache.constBegin(); it != mCache.constEnd(); ++it )
    {
      stream << it.key() << it.value().checksum << it.value().mtime;
    }
  }
}

QString Checksum::get( const QString &path )
{
  QDateTime localLastModified = QFileInfo( mProjectDir + path ).lastModified();

  auto match = mCache.find( path );

  if ( match != mCache.end() )
  {
    if ( match.value().mtime == localLastModified )
    {
      return match.value().checksum;
    }
    else
    {
      // invalid entry - remove from cache and recalculate
      mCache.remove( path );
    }
  }

  QByteArray localChecksumBytes = calculate( mProjectDir + path );
  QString localChecksum = QString::fromLatin1( localChecksumBytes.data(), localChecksumBytes.size() );

  CacheValue entry;
  entry.checksum = localChecksum;
  entry.mtime = localLastModified;
  mCache.insert( path, entry );
  mCacheModified = true;

  return localChecksum;
}

QByteArray Checksum::calculate( const QString &filePath )
{
  QFile f( filePath );
  if ( f.open( QFile::ReadOnly ) )
  {
    QCryptographicHash hash( QCryptographicHash::Sha1 );
    QByteArray chunk = f.read( CHUNK_SIZE );
    while ( !chunk.isEmpty() )
    {
      hash.addData( chunk );
      chunk = f.read( CHUNK_SIZE );
    }
    f.close();
    return hash.result().toHex();
  }

  return QByteArray();
}
