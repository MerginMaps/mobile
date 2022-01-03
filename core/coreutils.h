/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COREUTILS_H
#define COREUTILS_H

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include <QObject>
#include <QtGlobal>
#include <QUuid>


class CoreUtils
{
  public:
    explicit CoreUtils( ) = default;
    ~CoreUtils() = default;

    static QString appInfo();
    static QString appVersion();

    static QString localizedDateFromUTFString( QString timestamp );
    static bool removeDir( const QString &projectDir );

    /**
     * Returns name of temporary file indicating first time download of project is in progress
     * \param projectName
     */
    static QString downloadInProgressFilePath( const QString &projectDir );

    static QString uuidWithoutBraces( const QUuid &uuid );
    static QDateTime getLastModifiedFileDateTime( const QString &path );
    static int getProjectFilesCount( const QString &path );

    /**
    * Returns given path if doesn't exists, otherwise the slightly modified non-existing path by adding a number to given path.
    * \param QString path
    * \param QString isPathDir True if the result path suppose to be a folder
    */
    static QString findUniquePath( const QString &path, bool isPathDir = true );

    //! Creates a unique project directory for given project name (used for initial download of a project)
    static QString createUniqueProjectDirectory( const QString &baseDataDir, const QString &projectName );

    static bool createEmptyFile( const QString &filePath );

    /**
    * Returns modified path for a conflict file in following form:
    * <path>/<filename>_<copy/edit>_conflict_<username>_v<version>.<file_extension>
    */
    static QString generateCopyConflictFileName( const QString &path, const QString &username, int version );
    static QString generateEditConflictFileName( const QString &path, const QString &username, int version );

    /**
     * Sets the filename of the internal text log file
     * - Use LOG_TO_DEVNULL to do not output any logs
     * - Use LOG_TO_STDOUT to output to stdout
     * - Use filename to output to any file
     */
    static const QString LOG_TO_DEVNULL;
    static const QString LOG_TO_STDOUT;
    static void setLogFilename( const QString &value );

    static QString logFilename();

    /**
     * Add a log entry to internal log text file
     *
     * \see setLogFilename()
     */
    static void log( const QString &topic, const QString &info );

  private:
    static QString sLogFile;
    static void appendLog( const QByteArray &data, const QString &path );
};

#endif // COREUTILS_H
