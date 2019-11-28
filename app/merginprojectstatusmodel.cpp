#include "merginprojectstatusmodel.h"
#include "geodiffutils.h"

MerginProjectStatusModel::MerginProjectStatusModel( LocalProjectsManager &localProjects, QObject *parent ) : QAbstractListModel( parent )
{

}

int MerginProjectStatusModel::rowCount( const QModelIndex &parent ) const
{
  return mItems.length();
}

QHash<int, QByteArray> MerginProjectStatusModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Status] = "fileStatus";
  roleNames[Text] = "text";
  roleNames[Section] = "section";
  return roleNames;
}

QVariant MerginProjectStatusModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mItems.count() )
    return QVariant();

  ProjectStatusItem item = mItems.at( row );

  switch ( role )
  {
    case Status: return item.status;
    case Text: return item.text;
    case Section: return item.section;
  }
  return QVariant();

}

void MerginProjectStatusModel::insertIntoItems( const QSet<QString> &files, const QString &status, const QString &projectDir )
{
  for ( QString file : files )
  {
      if (!MerginApi::isInIgnore(QFileInfo(projectDir + "/" + file))) {
          ProjectStatusItem item;
          item.status = status;
          item.text = file;
          item.section = "Changelog";
          mItems.append( item );
      }
  }
}

void MerginProjectStatusModel::infoProjectUpdated( const ProjectDiff &projectDiff, const QString &projectDir )
{
  beginResetModel();
  mItems.clear();

  for ( QString file : projectDiff.localUpdated )
  {
    if ( MerginApi::isFileDiffable( file ) )
    {
      QString summaryJson = GeodiffUtils::diffableFilePendingChanges( projectDir, file, true );
      if ( !summaryJson.startsWith( "ERROR" ) )
      {
        GeodiffUtils::ChangesetSummary summary = GeodiffUtils::parseChangesetSummary( summaryJson ) ;
        for ( QString key : summary.keys() ) {
            ProjectStatusItem item;
            item.status = "Changelog";
            item.text = QString("%1 - inserted %2, updated %3, deleted %4").arg(key)
                    .arg(summary[key].inserts).arg(summary[key].updates)
                    .arg( summary[key].updates).arg( summary[key].deletes);
            item.section = file;
            mItems.append( item );
        }
      }
    }
  }

  insertIntoItems( projectDiff.localUpdated, "Updated", projectDir );
  insertIntoItems( projectDiff.localAdded, "Added", projectDir );
  insertIntoItems( projectDiff.localDeleted, "Deleted", projectDir );
  endResetModel();
}
