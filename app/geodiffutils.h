#ifndef GEODIFFUTILS_H
#define GEODIFFUTILS_H

#include <QMap>
#include <QString>


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

    //! Runs geodiff on a local project's file, compares it to locally cached original and creates a diff file
    //! (diff file path returned in the argument, returns geodiff return value - zero on success)
    static int createChangeset( const QString &projectDir, const QString &filePath, QString &diffPath, QString &basePath );

    //! Takes "src" file and applies a sequence of changesets for the list in "diffFiles"
    static bool applyDiffs( const QString &src, const QStringList &diffFiles );
};

#endif // GEODIFFUTILS_H
