/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectsproxymodel_future.h"

ProjectsProxyModel_future::ProjectsProxyModel_future( ProjectsModel_future *projectsSourceModel, QObject *parent ) :
  QSortFilterProxyModel( parent ),
  mModel( projectsSourceModel )
{
  setSourceModel( mModel );
  mModelType = mModel->modelType();

  setSortRole( ProjectsModel_future::Roles::ProjectFullName );
  setSortCaseSensitivity( Qt::CaseSensitivity::CaseInsensitive );
}

QString ProjectsProxyModel_future::searchExpression() const
{
  return mSearchExpression;
}

void ProjectsProxyModel_future::setSearchExpression( QString searchExpression )
{
  if ( mSearchExpression == searchExpression )
    return;

  mSearchExpression = searchExpression;

  if ( mSearchExpression.isEmpty() )
    invalidate();
  else
    setFilterRegularExpression( QRegularExpression( mSearchExpression ) );

  emit searchExpressionChanged( mSearchExpression );
}

bool ProjectsProxyModel_future::filterAcceptsRow( int, const QModelIndex &sourceParent ) const
{
  // return true if it passes search filter
  QString projectName = sourceModel()->data( sourceParent, ProjectsModel_future::Roles::ProjectName ).toString();
  QString projectNamespace = sourceModel()->data( sourceParent, ProjectsModel_future::Roles::ProjectNamespace ).toString();

  QRegExp filter = filterRegExp();
  if ( filter.isEmpty() )
    return true;

  return ( projectName.contains( filter ) || projectNamespace.contains( filter ) );
}

//bool ProjectsProxyModel_future::lessThan( const QModelIndex &left, const QModelIndex &right ) const
//{
//  TODO: Maybe simply setting sort role as projectFullName would work the same

//  if ( mModelType == LocalProjectsModel )
//  {
    /**
     * Ordering of local projects: first non-mergin projects (using folder name),
     * then mergin projects (sorted first by namespace, then project name)
     */

//    if ( projectNamespace.isEmpty() && other.projectNamespace.isEmpty() )
//    {
//      return folderName.compare( other.folderName, Qt::CaseInsensitive ) < 0;
//    }
//    if ( !projectNamespace.isEmpty() && other.projectNamespace.isEmpty() )
//    {
//      return false;
//    }
//    if ( projectNamespace.isEmpty() && !other.projectNamespace.isEmpty() )
//    {
//      return true;
//    }

//    if ( projectNamespace.compare( other.projectNamespace, Qt::CaseInsensitive ) == 0 )
//    {
//      return projectName.compare( other.projectName, Qt::CaseInsensitive ) < 0;
//    }
//    if ( projectNamespace.compare( other.projectNamespace, Qt::CaseInsensitive ) < 0 )
//    {
//      return true;
//    }
//    else
//      return false;
//    return true;
//  }
//}
