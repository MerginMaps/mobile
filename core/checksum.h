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

class Checksum
{
  public:
    Checksum( const QString &projectDir );

    //! Load cache from mProjectDir/sCacheFile
    void load();
    //! Save cache to mProjectDir/sCacheFile
    void save();

    QString get( const QString &path );

    /**
     * Returns Sha1 checksum of file
     * This is potentially resourcing-costly operation on big files
     */
    static QByteArray calculate( const QString &filePath );

    static const QString sCacheFile;

  private:
    struct CacheValue
    {
      QDateTime mtime; //!< associated file modification date when checksum was calculated
      QString checksum; //!< calculated checksum
    };

    QString mProjectDir;
    QHash<QString, CacheValue> mCache; //!< key -> file relative path to mProjectDir
};



#endif
