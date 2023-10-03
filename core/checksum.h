/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QHash>

/**
 * Calculates the checksums of local files and store the results in the local binary file
 */
class Checksum
{
  public:
    Checksum( const QString &projectDir );

    //! Loads cache from mProjectDir/sCacheFile
    void load();
    //! Saves cache to mProjectDir/sCacheFile
    void save();

    /**
     * Returns Sha1 checksum of file (with-caching)
     * Recalculates checksum for all entries not in cache
     */
    QString get( const QString &path );

    /**
     * Returns Sha1 checksum of file (no-caching)
     * This is potentially resourcing-costly operation
     */
    static QByteArray calculate( const QString &filePath );

    //! Name of the file in which the cache for the project is stored
    static const QString sCacheFile;

  private:
    struct CacheValue
    {
      QDateTime mtime; //!< associated file modification date when checksum was calculated
      QString checksum; //!< calculated checksum
    };

    QString mProjectDir;
    QHash<QString, CacheValue> mCache; //!< key -> file relative path to mProjectDir
    bool mCacheModified = false;
};



#endif
