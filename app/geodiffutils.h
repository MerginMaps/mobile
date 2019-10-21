#ifndef GEODIFFUTILS_H
#define GEODIFFUTILS_H

#include <QMap>
#include <QString>


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

    //! Test whether the file has changed according to geodiff compared to the original server version
    static bool hasPendingChanges( const QString &projectDir, const QString &filePath );

    //! Take JSON changeset summary string and parse it
    static ChangesetSummary parseChangesetSummary( const QString &json );

    //! Returns JSON with local pending changes o a diffable file
    static QString diffableFilePendingChanges( const QString &projectDir, const QString &filePath, bool onlySummary );

    //! run geodiff on a local project's file, compare it to locally cached original and create a diff file
    //! (diff file path returned in the argument, returns geodiff return value - zero on success)
    static int createChangeset( const QString &projectDir, const QString &filePath, QString &diffPath, QString &basePath );

};

#endif // GEODIFFUTILS_H
