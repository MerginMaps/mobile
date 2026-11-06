/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "geodiffutils.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryFile>
#include <QUuid>

#include <geodiff.h>
#include "coreutils.h"

/**
 * Singleton pattern - to create context
 */
class GeodiffContext
{

  public:
    GeodiffContext( GeodiffContext const & )  = delete;
    void operator=( GeodiffContext const & )  = delete;
    static GeodiffContext &instance();

    GEODIFF_ContextH handle() const;
  private:
    GeodiffContext()
    {
      mHandle = GEODIFF_createContext();
      GEODIFF_CX_setLoggerCallback( mHandle, &GeodiffUtils::log );
      GEODIFF_CX_setMaximumLoggerLevel( mHandle, GEODIFF_LoggerLevel::LevelDebug );
    }
    ~GeodiffContext()
    {
      if ( mHandle )
      {
        GEODIFF_CX_destroy( mHandle );
        mHandle = nullptr;
      }
    }

    GEODIFF_ContextH mHandle = nullptr;
};

GeodiffContext &GeodiffContext::instance()
{
  static GeodiffContext instance;
  return instance;
}

GEODIFF_ContextH GeodiffContext::handle() const
{
  return mHandle;
}

void GeodiffUtils::init()
{
  Q_UNUSED( GeodiffContext::instance() );
}

QString GeodiffUtils::diffableFilePendingChanges( const QString &projectDir, const QString &filePath, bool onlySummary )
{
  QString diffName;
  int res = createChangeset( projectDir, filePath, diffName );
  QString diffPath = projectDir + "/.mergin/" + diffName;
  QString basePath = projectDir + "/.mergin/" + filePath;

  if ( res == GEODIFF_SUCCESS )
  {
    QTemporaryFile f;
    if ( !f.open() )
      return "ERROR";
    QString jsonPath = f.fileName();
    f.close();

    int resList = onlySummary ? GEODIFF_listChangesSummary( GeodiffContext::instance().handle(), diffPath.toUtf8(), jsonPath.toUtf8() ) : GEODIFF_listChanges( GeodiffContext::instance().handle(), diffPath.toUtf8(), jsonPath.toUtf8() );

    QFile::remove( diffPath );  // we don't need the temporary diff file anymore

    if ( resList == GEODIFF_SUCCESS )
    {
      QFile fJson( jsonPath );
      fJson.open( QIODevice::ReadOnly );
      QString json = QString::fromUtf8( fJson.readAll() );
      fJson.close();
      QFile::remove( jsonPath );  // we don't need the temporary json file anymore
      return json;
    }
  }

  return "ERROR";
}


int GeodiffUtils::createChangeset( const QString &projectDir, const QString &fileName, QString &diffName )
{
  QString uuid = CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
  diffName = fileName + "-diff-" + uuid;
  QString modifiedAbsPath = projectDir + "/" + fileName;
  QString baseAbsPath = projectDir + "/.mergin/" + fileName;
  QString diffAbsPath = projectDir + "/.mergin/" + diffName;
  return GEODIFF_createChangeset( GeodiffContext::instance().handle(), baseAbsPath.toUtf8(), modifiedAbsPath.toUtf8(), diffAbsPath.toUtf8() );
}

bool GeodiffUtils::hasPendingChanges( const QString &projectDir, const QString &filePath )
{
  QString summaryJson = diffableFilePendingChanges( projectDir, filePath, true );
  if ( summaryJson.startsWith( "ERROR" ) )
    return true;  // something went wrong - let's assume the file has changed

  return !GeodiffUtils::parseChangesetSummary( summaryJson ).isEmpty();
}

GeodiffUtils::ChangesetSummary GeodiffUtils::parseChangesetSummary( const QString &json )
{
  ChangesetSummary summary;
  QJsonDocument doc = QJsonDocument::fromJson( json.toUtf8() );
  const QJsonArray lst = doc.object()["geodiff_summary"].toArray();
  for ( const auto &item : lst )
  {
    QJsonObject itemObj = item.toObject();
    QString tableName = itemObj["table"].toString();
    TableSummary tableSummary;
    tableSummary.inserts = itemObj["insert"].toInt();
    tableSummary.updates = itemObj["update"].toInt();
    tableSummary.deletes = itemObj["delete"].toInt();
    summary[tableName] = tableSummary;
  }
  return summary;
}


bool GeodiffUtils::applyDiffs( const QString &src, const QStringList &diffFiles )
{
  if ( diffFiles.isEmpty() )
  {
    CoreUtils::log( "GEODIFF", "assemble server file fail: no input diff files!" );
    return false;
  }

  for ( QString diffFile : diffFiles )
  {
    int res = GEODIFF_applyChangeset( GeodiffContext::instance().handle(), src.toUtf8().constData(), diffFile.toUtf8().constData() );
    if ( res != GEODIFF_SUCCESS )
    {
      CoreUtils::log( "GEODIFF", "assemble server file fail: apply changeset failed " + diffFile );
      return false;
    }
  }
  return true;
}

bool GeodiffUtils::applyChangeset( const QString &src, const QString &changeset )
{
  int res = GEODIFF_applyChangeset( GeodiffContext::instance().handle(), src.toUtf8(), changeset.toUtf8() );
  return ( res == GEODIFF_SUCCESS );
}

bool GeodiffUtils::rebase( const QString &base, const QString &modified_their, const QString &modified, const QString &conflictfile )
{
  int res = GEODIFF_rebase( GeodiffContext::instance().handle(),
                            base.toUtf8().constData(),
                            modified_their.toUtf8().constData(),
                            modified.toUtf8().constData(),
                            conflictfile.toUtf8().constData()
                          );
  return ( res == GEODIFF_SUCCESS );
}

GeodiffUtils::FeatureIdMapping GeodiffUtils::findRemappedFeatureIds( const QString &dbPath, const QString &localChangesJson, const QString &rebasedChangesJson )
{
  // fetch GeoPackage schema, extract table names and indices of primary key columns
  QHash<QString, int> pkColumns;
  {
    QTemporaryFile schemaFile;
    if ( !schemaFile.open() )
    {
      return {};
    }
    schemaFile.close();

    const int res = GEODIFF_schema( GeodiffContext::instance().handle(), "sqlite", nullptr, dbPath.toUtf8(), schemaFile.fileName().toUtf8() );
    if ( res != GEODIFF_SUCCESS || !schemaFile.open() )
    {
      return {};
    }

    const QJsonArray tables = QJsonDocument::fromJson( schemaFile.readAll() ).object()["geodiff_schema"].toArray();
    for ( const auto &table : tables )
    {
      const QJsonArray columns = table.toObject()["columns"].toArray();
      for ( int i = 0; i < columns.size(); ++i )
      {
        if ( columns[i].toObject()["primary_key"].toBool() )
        {
          pkColumns.insert( table.toObject()["table"].toString(), i );
          break;
        }
      }
    }
  }

  const QHash<QString, QList<qint64>> localInserts = extractPrimaryKeysfromChangeset( pkColumns, localChangesJson );
  if ( localInserts.isEmpty() )
  {
    return {};
  }

  // create the final mapping of old IDs to new IDs, we make an assumption here: we expect geodiff to create the new IDs list in the same order
  // as the old IDs list.
  // NOTE: Geodiff should expose this functionality as API, this is just temporary solution until the API is available
  FeatureIdMapping mapping;
  const QHash<QString, QList<qint64>> rebasedInserts = extractPrimaryKeysfromChangeset( pkColumns, rebasedChangesJson );
  for ( const QString &layer : localInserts.keys() )
  {
    const QList<qint64> idList = localInserts.value( layer );
    for ( int i = 0; i < idList.size(); i++ )
    {
      const QPair<qint64, qint64> ids = { idList.at( i ), rebasedInserts.value( layer ).at( i )};
      if ( mapping.contains( layer ) )
      {
        mapping[layer].append( ids );
      }
      else
      {
        mapping.insert( layer, {ids} );
      }
    }
  }

  return mapping;
}

QHash<QString, QList<qint64>> GeodiffUtils::extractPrimaryKeysfromChangeset( const QHash<QString, int> &primaryKeyColumns, const QString &jsonChangeset )
{
  QHash<QString, QList<qint64>> tableIds;
  const QJsonArray entries = QJsonDocument::fromJson( jsonChangeset.toUtf8() ).object()["geodiff"].toArray();
  for ( const auto &entry : entries )
  {
    const QJsonObject entryObj = entry.toObject();
    const QString table = entryObj["table"].toString();
    if ( entryObj["type"].toString() != QLatin1String( "insert" ) || !primaryKeyColumns.contains( table ) )
    {
      continue;
    }

    const QJsonArray changes = entryObj["changes"].toArray();
    for ( const auto &change : changes )
    {
      const QJsonObject changeObj = change.toObject();
      if ( changeObj["column"].toInt() == primaryKeyColumns[table] )
      {
        if ( tableIds.contains( table ) )
        {
          tableIds[table].append( changeObj["new"].toInteger( -1 ) );
        }
        else
        {
          tableIds.insert( table, {changeObj["new"].toInteger( -1 ) } );
        }
      }
    }
  }
  return tableIds;
}

void GeodiffUtils::log( GEODIFF_LoggerLevel level, const char *msg )
{
  QString prefix;
  switch ( level )
  {
    case LevelError: prefix = "GEODIFF error"; break;
    case LevelWarning: prefix = "GEODIFF warning"; break;
    case LevelInfo: prefix = "GEODIFF info"; break;
    case LevelDebug: prefix = "GEODIFF debug"; break;
    default: break;
  }
  CoreUtils::log( prefix, msg );
}
