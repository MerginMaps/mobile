#include "merginprojectmodel.h"

#include <QString>

MerginProjectModel::MerginProjectModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

QVariant MerginProjectModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mMerginProjects.count() )
    return QVariant();

  const MerginProject *project = mMerginProjects.at( row ).get();

  switch ( role )
  {
    case Name:
      return QVariant( project->name );
    case ProjectNamespace: return QVariant( project->projectNamespace );
    case ProjectInfo:
    {
      if ( !project->updated.isValid() )
      {
        return project->serverUpdated.toString();
      }
      else
      {
        return project->updated.toString();
      }
    }

    case Status:
    {
      switch ( project->status )
      {
        case ProjectStatus::OutOfDate:
          return QVariant( QStringLiteral( "outOfDate" ) );
        case ProjectStatus::UpToDate:
          return QVariant( QStringLiteral( "upToDate" ) );
        case ProjectStatus::NoVersion:
          return QVariant( QStringLiteral( "noVersion" ) );
        case ProjectStatus::Modified:
          return QVariant( QStringLiteral( "modified" ) );
      }
    }
    case Pending: return QVariant( project->pending );
    case PassesFilter:
    {
      if ( mFilterCreator >= 0 )
      {
        return project->creator == mFilterCreator;
      }
      else if ( mFilterWriter >= 0 )
      {
        return project->creator != mFilterWriter && project->writers.contains( mFilterWriter );
      }
      return true;
    }
  }

  return QVariant();
}

void MerginProjectModel::setPending( int row, bool pending )
{
  if ( row < 0 || row > mMerginProjects.length() - 1 ) return;
  QModelIndex ix = index( row );
  mMerginProjects.at( row )->pending = pending;
  emit dataChanged( ix, ix );
}

QHash<int, QByteArray> MerginProjectModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  roleNames[ProjectNamespace] = "projectNamespace";
  roleNames[ProjectInfo] = "projectInfo";
  roleNames[Status] = "status";
  roleNames[Pending] = "pendingProject";
  roleNames[PassesFilter] = "passesFilter";
  return roleNames;
}

ProjectList MerginProjectModel::projects()
{
  return mMerginProjects;
}

int MerginProjectModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent );
  return mMerginProjects.count();
}

void MerginProjectModel::resetProjects( const ProjectList &merginProjects )
{
  mMerginProjects.clear();
  beginResetModel();
  mMerginProjects = merginProjects;
  endResetModel();
}

void MerginProjectModel::syncProjectFinished( const QString &projectFolder, const QString &projectFullName, bool successfully )
{
  Q_UNUSED( projectFolder );
  int row = 0;
  for ( std::shared_ptr<MerginProject> project : mMerginProjects )
  {
    if ( project->projectNamespace + "/" + project->name == projectFullName )
    {
      if ( successfully )
      {
        project->status = ProjectStatus::UpToDate;
      }
      setPending( row, false ); // emits dataChanged
      return;
    }
    row++;
  }
}

int MerginProjectModel::filterWriter() const
{
  return mFilterWriter;
}

void MerginProjectModel::setFilterWriter( int filterWriter )
{
  mFilterWriter = filterWriter;
}

int MerginProjectModel::filterCreator() const
{
  return mFilterCreator;
}

void MerginProjectModel::setFilterCreator( int filterCreator )
{
  mFilterCreator = filterCreator;
}
