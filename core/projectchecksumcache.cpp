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
  return mProjectDir + "/" + MerginApi::sMetadataFolder + "/" + sCacheFile;
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
  QDateTime localLastModified = QFileInfo( mProjectDir + "/" + path ).lastModified();

  auto match = mCache.find( path );

  if ( match != mCache.end() )
  {
    if ( match.value().mtime == localLastModified )
    {
      return match.value().checksum;
    }
  }

  QByteArray localChecksumBytes = CoreUtils::calculate( mProjectDir + "/" + path );
  QString localChecksum = QString::fromLatin1( localChecksumBytes.data(), localChecksumBytes.size() );

  CacheValue entry;
  entry.checksum = localChecksum;
  entry.mtime = localLastModified;
  mCache.insert( path, entry );
  mCacheModified = true;

  return localChecksum;
}
