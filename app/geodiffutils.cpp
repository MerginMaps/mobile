#include "geodiffutils.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryFile>
#include <QUuid>

#include <geodiff.h>

#include "inpututils.h"


QString GeodiffUtils::diffableFilePendingChanges( const QString &projectDir, const QString &filePath, bool onlySummary )
{
  QString diffPath, basePath;
  int res = createChangeset( projectDir, filePath, diffPath, basePath );
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


int GeodiffUtils::createChangeset( const QString &projectDir, const QString &filePath, QString &diffPath, QString &basePath )
{
  QString uuid = QUuid::createUuid().toString( QUuid::WithoutBraces );
  QString diffName = filePath + "-diff-" + uuid;
  QString modifiedPath = projectDir + "/" + filePath;
  basePath = projectDir + "/.mergin/" + filePath;
  diffPath = projectDir + "/.mergin/" + diffName;
  return GEODIFF_createChangeset( basePath.toUtf8(), modifiedPath.toUtf8(), diffPath.toUtf8() );
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
  for ( QString diffFile : diffFiles )
  {
    int res = GEODIFF_applyChangeset( src.toUtf8().constData(), diffFile.toUtf8().constData() );
    if ( res != GEODIFF_SUCCESS )
    {
      InputUtils::log( "GEODIFF", "assemble server file fail: apply changeset failed " + diffFile );
      return false;
    }
  }
  return true;
}
