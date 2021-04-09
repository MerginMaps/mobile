/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GEODIFFUTILS_H
#define GEODIFFUTILS_H

#include <QMap>
#include <QString>

#include <geodiff.h>

/**
 * Utility functions for working with geodiff library
 */
class GeodiffUtils
{
  public:

    struct TableSummary
    {
      TableSummary( int ins = 0, int upd = 0, int del = 0 )
        : inserts( ins ), updates( upd ), deletes( del ) {}

      bool operator==( const TableSummary &other ) const
      {
        return inserts == other.inserts && updates == other.updates && deletes == other.deletes;
      }

      int inserts;
      int updates;
      int deletes;
    };

    typedef QMap<QString, TableSummary> ChangesetSummary;

    //! Tests whether the file has changed according to geodiff compared to the original server version
    static bool hasPendingChanges( const QString &projectDir, const QString &filePath );

    //! Takes JSON changeset summary string and parses it
    static ChangesetSummary parseChangesetSummary( const QString &json );

    //! Returns JSON with local pending changes o a diffable file
    static QString diffableFilePendingChanges( const QString &projectDir, const QString &filePath, bool onlySummary );

    /**
     *  Runs geodiff on a local project's file, compares it to locally cached original and creates a diff file
     * (diff file path returned in the argument is RELATIVE to "projectDir/.mergin/" DIR)
     * \returns geodiff return value - zero on success
     */
    static int createChangeset( const QString &projectDir, const QString &fileName, QString &diffName );

    //! Takes "src" file and applies a sequence of changesets for the list in "diffFiles"
    static bool applyDiffs( const QString &src, const QStringList &diffFiles );

    //! Geodiff logger callback function used to forward logs to Input.
    static void log( GEODIFF_LoggerLevel level, const char *msg );
};

#endif // GEODIFFUTILS_H
