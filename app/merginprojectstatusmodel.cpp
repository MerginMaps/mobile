#include "merginprojectstatusmodel.h"
#include "geodiffutils.h"
#include "inpututils.h"

MerginProjectStatusModel::MerginProjectStatusModel( LocalProjectsManager &localProjects, QObject *parent )
  : QAbstractListModel( parent )
  , mLocalProjects( localProjects )
{
}

int MerginProjectStatusModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mItems.length();
}

QHash<int, QByteArray> MerginProjectStatusModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Status] = "fileStatus";
  roleNames[Text] = "itemText";
  roleNames[Filename] = "filename";
  roleNames[Inserts] = "inserts";
  roleNames[Deletes] = "deletes";
  roleNames[Updates] = "updates";
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
    case Filename: return item.filename;
    case Inserts: return item.inserts;
    case Deletes: return item.deletes;
    case Updates: return item.updates;
    case Section: return item.section;
  }
  return QVariant();

}

void MerginProjectStatusModel::insertIntoItems( const QSet<QString> &files, const ProjectChangelogStatus &status, const QString &projectDir )
{
  for ( QString file : files )
  {
    if ( !MerginApi::isInIgnore( QFileInfo( projectDir + "/" + file ) ) )
    {
      ProjectStatusItem item;
      item.status = status;
      item.text = file;
      item.section = "Changes";
      mItems.append( item );
    }
  }
}

void MerginProjectStatusModel::infoProjectUpdated( const ProjectDiff &projectDiff, const QString &projectDir )
{
  beginResetModel();
  mItems.clear();

  insertIntoItems( projectDiff.localUpdated, ProjectChangelogStatus::Updated, projectDir );
  insertIntoItems( projectDiff.localAdded, ProjectChangelogStatus::Added, projectDir );
  insertIntoItems( projectDiff.localDeleted, ProjectChangelogStatus::Deleted, projectDir );

  for ( QString file : projectDiff.localUpdated )
  {
    if ( MerginApi::isFileDiffable( file ) )
    {
      QString summaryJson = GeodiffUtils::diffableFilePendingChanges( projectDir, file, true );
      if ( summaryJson.startsWith( "ERROR" ) )
      {
        InputUtils::log( "MerginProjectStatusModel", QString( "Diff summary JSON for %1 in %2 has an error." ).arg( projectDir ).arg( file ) );

        ProjectStatusItem item;
        item.status = ProjectChangelogStatus::Message;
        item.text =  QStringLiteral( "Unable to determine changes" );
        item.filename = file;
        item.section = file;

        mItems.append( item );
      }
      else
      {
        GeodiffUtils::ChangesetSummary summary = GeodiffUtils::parseChangesetSummary( summaryJson ) ;
        for ( QString key : summary.keys() )
        {

          ProjectStatusItem item;
          item.status = ProjectChangelogStatus::Changelog;
          item.text =  key;
          item.filename = file;
          item.inserts = summary[key].inserts;
          item.updates = summary[key].updates;
          item.deletes = summary[key].deletes;
          item.section = file;

          mItems.append( item );
        }

      }
    }
  }
  endResetModel();
}

bool MerginProjectStatusModel::loadProjectInfo( const QString &projectFullName )
{
  LocalProjectInfo projectInfo = mLocalProjects.projectFromMerginName( projectFullName );
  if ( !projectInfo.projectDir.isEmpty() )
  {
    ProjectDiff diff = MerginApi::localProjectChanges( projectInfo.projectDir );
    if ( diff.localAdded.isEmpty() && diff.localUpdated.isEmpty() && diff.localDeleted.isEmpty() )
      return false;
    else
    {
      infoProjectUpdated( diff, projectInfo.projectDir );
      return true;
    }
  }
  return false;
}
