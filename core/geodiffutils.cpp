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

    int resList = onlySummary ? GEODIFF_listChangesSummary( diffPath.toUtf8(), jsonPath.toUtf8() ) : GEODIFF_listChanges( diffPath.toUtf8(), jsonPath.toUtf8() );

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
  return GEODIFF_createChangeset( baseAbsPath.toUtf8(), modifiedAbsPath.toUtf8(), diffAbsPath.toUtf8() );
}


bool GeodiffUtils::hasPendingChanges( const QString &projectDir, const QString &filePath )
{
  QString summaryJson = GeodiffUtils::diffableFilePendingChanges( projectDir, filePath, true );
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
    int res = GEODIFF_applyChangeset( src.toUtf8().constData(), diffFile.toUtf8().constData() );
    if ( res != GEODIFF_SUCCESS )
    {
      CoreUtils::log( "GEODIFF", "assemble server file fail: apply changeset failed " + diffFile );
      return false;
    }
  }
  return true;
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
