/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectsproxymodel.h"

ProjectsProxyModel::ProjectsProxyModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

void ProjectsProxyModel::initialize()
{
  setSourceModel( mModel );
  mModelType = mModel->modelType();

  if ( mModelType != ProjectsModel::PublicProjectsModel ) // do not sort at all in public projects, they come sorted from Mergin
  {
    if ( mModelType == ProjectsModel::CreatedProjectsModel )
      setFilterRole( ProjectsModel::ProjectName );
    else
      setFilterRole( ProjectsModel::ProjectFullName );

    setFilterCaseSensitivity( Qt::CaseInsensitive );

    sort( 0, Qt::AscendingOrder );
  }
}

QString ProjectsProxyModel::searchExpression() const
{
  return mSearchExpression;
}

ProjectsModel *ProjectsProxyModel::projectSourceModel() const
{
  return mModel;
}

void ProjectsProxyModel::setSearchExpression( QString searchExpression )
{
  if ( mSearchExpression == searchExpression )
    return;

  mSearchExpression = searchExpression;
  setFilterFixedString( mSearchExpression );
  emit searchExpressionChanged( mSearchExpression );
}

void ProjectsProxyModel::setProjectSourceModel( ProjectsModel *sourceModel )
{
  if ( mModel == sourceModel )
    return;

  mModel = sourceModel;
  QObject::connect( mModel, &ProjectsModel::modelInitialized, this, &ProjectsProxyModel::initialize );
}

bool ProjectsProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
  if ( mModelType == ProjectsModel::LocalProjectsModel )
  {
    bool lProjectIsMergin = mModel->data( left, ProjectsModel::ProjectIsMergin ).toBool();
    bool rProjectIsMergin = mModel->data( right, ProjectsModel::ProjectIsMergin ).toBool();

    /**
     * Ordering of local projects: first non-mergin projects (using folder name),
     * then mergin projects (sorted first by namespace, then project name)
     */

    if ( !lProjectIsMergin && !rProjectIsMergin )
    {
      QString lProjectFullName = mModel->data( left, ProjectsModel::ProjectFullName ).toString();
      QString rProjectFullName = mModel->data( right, ProjectsModel::ProjectFullName ).toString();

      return lProjectFullName.compare( rProjectFullName, Qt::CaseInsensitive ) < 0;
    }
    if ( !lProjectIsMergin && rProjectIsMergin )
    {
      return true;
    }
    if ( lProjectIsMergin && !rProjectIsMergin )
    {
      return false;
    }
  }

  // comparing 2 mergin projects
  QString lNamespace = mModel->data( left, ProjectsModel::ProjectNamespace ).toString();
  QString lProjectName = mModel->data( left, ProjectsModel::ProjectName ).toString();
  QString rNamespace = mModel->data( right, ProjectsModel::ProjectNamespace ).toString();
  QString rProjectName = mModel->data( right, ProjectsModel::ProjectName ).toString();

  if ( lNamespace == rNamespace )
  {
    return lProjectName.compare( rProjectName, Qt::CaseInsensitive ) < 0;
  }
  return lNamespace.compare( rNamespace, Qt::CaseInsensitive ) < 0;
}
