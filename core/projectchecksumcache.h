/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECTCHECKSUMCACHE_H
#define PROJECTCHECKSUMCACHE_H

#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QHash>

#include "inputconfig.h"

#if defined(MM_TEST)
class TestProjectChecksumCache;
#endif

/**
 * Calculates the checksums of local files and store the results in the local binary file
 */
class ProjectChecksumCache
{
  public:
    ProjectChecksumCache( const QString &projectDir );
    ~ProjectChecksumCache();

    /**
     * Returns Sha1 checksum of file (with-caching)
     * Recalculates checksum for an entry not in cache
     * \param path relative path of the file to mProjectDir
     */
    QString get( const QString &path );

    //! Name of the file in which the cache for the project is stored
    static const QString sCacheFile;

#if defined(MM_TEST)
    friend class TestProjectChecksumCache;
#endif

  private:
    QString cacheFilePath() const;
    QString cacheDirPath() const;

    struct CacheValue
    {
      QDateTime mtime; //!< associated file modification date when checksum was calculated
      QString checksum; //!< calculated checksum
    };

    QString mProjectDir;
    QHash<QString, CacheValue> mCache; //!< key -> file relative path to mProjectDir
    bool mCacheModified = false;
};

#endif // PROJECTCHECKSUMCACHE_H
