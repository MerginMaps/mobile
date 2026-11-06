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

#include "projectchecksumcache.h"
#include "coreutils.h"
#include "merginapi.h"

const QString ProjectChecksumCache::sCacheFile = QStringLiteral( "checksum.cache" );

QString ProjectChecksumCache::cacheFilePath() const
{
  return cacheDirPath() + "/" + sCacheFile;
}

QString ProjectChecksumCache::cacheDirPath() const
{
  return mProjectDir + "/" + MerginApi::sMetadataFolder;
}

ProjectChecksumCache::ProjectChecksumCache( const QString &projectDir )
  : mProjectDir( projectDir )
{
  QFile f( cacheFilePath() );

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

ProjectChecksumCache::~ProjectChecksumCache()
{
  if ( !mCacheModified )
    return;

  // Make sure the directory exists
  QDir dir;
  if ( !dir.exists( cacheDirPath() ) )
    dir.mkpath( cacheDirPath() );

  QFile f( cacheFilePath() );
  if ( f.open( QIODevice::WriteOnly ) ) // implies Truncate
  {
    QDataStream stream( &f );
    stream.setVersion( QDataStream::Qt_6_5 );

    for ( auto it = mCache.constBegin(); it != mCache.constEnd(); ++it )
    {
      stream << it.key() << it.value().checksum << it.value().mtime;
    }
  }
  else
  {
    CoreUtils::log( "projectchecksumcache", QStringLiteral( "Unable to save cache %1" ).arg( cacheFilePath() ) );
  }
}

QString ProjectChecksumCache::get( const QString &path )
{
  const QString checksum = mainFileChecksum( path );
  if ( checksum.isEmpty() || !MerginApi::isFileDiffable( path ) )
    return checksum;

  // changes may live only in the -wal file of a gpkg, without touching the main file
  const QString walPath = mProjectDir + "/" + path + QStringLiteral( "-wal" );
  if ( QFileInfo( walPath ).size() <= 0 )
    return checksum;

  // the -wal file is usually small (it gets checkpointed into the main file regularly), so it is not cached
  QCryptographicHash hash( QCryptographicHash::Sha1 );
  hash.addData( checksum.toLatin1() );
  hash.addData( CoreUtils::calculateChecksum( walPath ) );
  return QString::fromLatin1( hash.result().toHex() );
}

QString ProjectChecksumCache::mainFileChecksum( const QString &path )
{
  QDateTime localLastModified = QFileInfo( mProjectDir + "/" + path ).lastModified();

  auto match = mCache.find( path );

  if ( match != mCache.end() )
  {
    if ( match.value().mtime == localLastModified )
    {
      return match.value().checksum;
    }
  }

  QByteArray localChecksumBytes = CoreUtils::calculateChecksum( mProjectDir + "/" + path );
  QString localChecksum = QString::fromLatin1( localChecksumBytes.data(), localChecksumBytes.size() );

  CacheValue entry;
  entry.checksum = localChecksum;
  entry.mtime = localLastModified;
  mCache.insert( path, entry );
  mCacheModified = true;

  return localChecksum;
}
