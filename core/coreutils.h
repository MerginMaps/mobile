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
    static QString appVersionCode();

    static QString localizedDateFromUTFString( QString timestamp );
    static bool removeDir( const QString &projectDir );

    /**
     * Returns name of temporary file indicating first time download of project is in progress
     * \param projectName
     */
    static QString downloadInProgressFilePath( const QString &projectDir );

    static QString uuidWithoutBraces( const QUuid &uuid );

    /**
     * Returns Sha1 checksum of file (no-caching)
     * This is potentially resourcing-costly operation
     * \param filePath full path to the file on disk
     */
    static QByteArray calculateChecksum( const QString &filePath );

    /**
    * Returns given path if it does not exist yet, otherwise adds a number to the path in format:
    *  - if path is a directory: "folder" -> "folder (1)"
    *  - if path is a file: "filename.txt" -> "filename (1).txt"
    */
    static QString findUniquePath( const QString &path );

    //! Creates a unique project directory for given project name (used for initial download of a project)
    static QString createUniqueProjectDirectory( const QString &baseDataDir, const QString &projectName );

    static bool createEmptyFile( const QString &filePath );

    /**
    * Returns generated name of a conflict file in the following form:
    * <filename>_<copy/edit>_conflict_<username>_v<version>.<file_extension>, example:
    * data (conflicted copy/edit conflict, martin v5).gpkg
    */
    static QString generateConflictedCopyFileName( const QString &file, const QString &username, int version );
    static QString generateEditConflictFileName( const QString &file, const QString &username, int version );

    /**
     * Sets the filename of the internal text log file
     * - Use LOG_TO_DEVNULL to do not output any logs
     * - Use LOG_TO_STDOUT to output to stdout
     * - Use filename to output to any file
     */
    static const QString LOG_TO_DEVNULL;
    static const QString LOG_TO_STDOUT;
    static const QString AUTH_CONFIG_FILENAME;
    static void setLogFilename( const QString &value );

    static QString logFilename();

    /**
     * Add a log entry to internal log text file
     *
     * \see setLogFilename()
     */
    static void log( const QString &topic, const QString &info );

    //! Checks whether file path has a QGIS project suffix (qgs or qgz)
    static bool hasProjectFileExtension( const QString filePath );

    // Checks whether file is the authentication configuration
    static bool isAuthConfigFile( const QString filePath );

    /**
     * Check whether given project/user name is valid
     */
    static bool isValidName( const QString &name );

    /**
     * Get name abbreviation
     * E.g.
     *   Pat Norris -> PN
     *   Pat -> P
     *   from <empty> -> <empty>
     */
    static QString nameAbbr( const QString &name, const QString &email );

    /**
     *  Returns unique identifier of the device
     *  The UUID is stored in QSettings and is randomly generated on first app run.
     */
    static QString deviceUuid();

    static const QString QSETTINGS_APP_GROUP_NAME;

    /**
     * Returns available device storage
    */
    static QString getAvailableDeviceStorage();

    /**
     * Returns total device storage
    */
    static QString getTotalDeviceStorage();

    /**
     * Converts bytes to human readable size (e.g. 1GB, 500MB)
     */
    static QString bytesToHumanSize( double bytes );

    /**
     * Returns path to project metadata file for a given project directory
     */
    static QString getProjectMetadataPath( QString projectDir );

    /**
     * Updates a value in a JSON file at the specified top-level key
     */
    static bool replaceValueInJson( const QString &filePath, const QString &key, const QJsonValue &value );

    /**
     * We do some very basic checks if the string looks like email.
     */
    static bool isValidEmail( const QString &email );

  private:
    static QString sLogFile;
    static int CHECKSUM_CHUNK_SIZE;

    static void appendLog( const QByteArray &data, const QString &path );
};

#endif // COREUTILS_H
