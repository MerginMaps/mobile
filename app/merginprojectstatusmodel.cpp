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
  roleNames[Filename] = "filename";
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
    case Filename: return item.filename;
    case Section: return item.section;
  }
  return QVariant();

}

void MerginProjectStatusModel::insertIntoItems( const QSet<QString> &files, const QString &status, const QString &projectDir )
{
  for ( QString file : files )
  {
    ProjectStatusItem item;
    item.status = status;
    item.filename = file;
    item.section = "Changelog";
    mItems.append( item );
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
      // TODO changelog
      QString summaryJson = GeodiffUtils::diffableFilePendingChanges( projectDir, file, true );
      if ( !summaryJson.startsWith( "ERROR" ) )
      {
        GeodiffUtils::ChangesetSummary summary = GeodiffUtils::parseChangesetSummary( summaryJson ) ;
        ProjectStatusItem item;
        item.status = "Added";
        //item.filename = file;
        item.filename = "SUMMARY";
        item.section = file;
        mItems.append( item );
      }
    }
  }

  insertIntoItems( projectDiff.localUpdated, "Updated", projectDir );
  insertIntoItems( projectDiff.localAdded, "Added", projectDir );
  insertIntoItems( projectDiff.localDeleted, "Deleted", projectDir );
  endResetModel();
}
