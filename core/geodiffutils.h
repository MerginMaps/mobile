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

#include <QHash>
#include <QMap>
#include <QString>

#include <geodiff.h>

/**
 * Utility functions for working with geodiff library
 */
class GeodiffUtils
{
  public:
    //! Create geodiff context
    static void init();

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

    //! Table name with list of mapped old IDs to new IDs
    using FeatureIdMapping = QHash<QString, QList<QPair<qint64, qint64>>>;

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

    //! Applies changeset file (binary) to BASE
    static bool applyChangeset( const QString &src, const QString &changeset );

    //! Rebases local modified version from base to modified_their version
    static bool rebase( const QString &base, const QString &modified_their, const QString &modified, const QString &conflictfile );

    /**
     * Finds primary keys of locally inserted rows that have been changed by rebase (when both
     * we and the server inserted a row with the same primary key, our row gets a new one).
     * \param dbPath geopackage file, used to find primary key columns
     * \param localChangesJson local changes (listChanges output) before rebase
     * \param rebasedChangesJson local changes (listChanges output) after rebase
     */
    static FeatureIdMapping findRemappedFeatureIds( const QString &dbPath, const QString &localChangesJson, const QString &rebasedChangesJson );

  private:

    /**
     * Based on primary key indices in tables parse changeset of geopackage and return map of table names with list of primary keys, which have been inserted.
     * \param primaryKeyColumns map of table names and index of primary key column
     * \param jsonChangeset Geodiff generated changeset
     */
    static QHash<QString, QList<qint64>> extractPrimaryKeysfromChangeset( const QHash<QString, int> &primaryKeyColumns, const QString &jsonChangeset );
};

#endif // GEODIFFUTILS_H
