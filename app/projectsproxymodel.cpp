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

  // do not sort at all in public projects, they come sorted from Mergin
  if ( mModelType == ProjectsModel::LocalProjectsModel )
  {
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

void ProjectsProxyModel::setActiveProjectAlwaysFirst( const bool value )
{
  mActiveProjectAlwaysFirst = value;
  invalidate();
}

bool ProjectsProxyModel::activeProjectAlwaysFirst() const
{
  return mActiveProjectAlwaysFirst;
}

void ProjectsProxyModel::setSearchExpression( const QString &searchExpression )
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
  connect( mModel, &ProjectsModel::modelInitialized, this, &ProjectsProxyModel::initialize );
}

bool ProjectsProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
  if ( mModelType == ProjectsModel::LocalProjectsModel )
  {
    if ( mActiveProjectAlwaysFirst )
    {
      const bool lProjectIsActive = mModel->data( left, ProjectsModel::ProjectIsActiveProject ).toBool();
      const bool rProjectIsActive = mModel->data( right, ProjectsModel::ProjectIsActiveProject ).toBool();
      if ( lProjectIsActive || rProjectIsActive )
        return lProjectIsActive;
    }

    const bool lProjectIsMergin = mModel->data( left, ProjectsModel::ProjectIsMergin ).toBool();
    const bool rProjectIsMergin = mModel->data( right, ProjectsModel::ProjectIsMergin ).toBool();

    /**
     * Ordering of local projects: first non-mergin projects (using folder name),
     * then mergin projects (sorted first by namespace, then project name)
     */

    if ( !lProjectIsMergin && !rProjectIsMergin )
    {
      const QString lProjectFullName = mModel->data( left, ProjectsModel::ProjectFullName ).toString();
      const QString rProjectFullName = mModel->data( right, ProjectsModel::ProjectFullName ).toString();

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
  const QString lNamespace = mModel->data( left, ProjectsModel::ProjectNamespace ).toString();
  const QString lProjectName = mModel->data( left, ProjectsModel::ProjectName ).toString();
  const QString rNamespace = mModel->data( right, ProjectsModel::ProjectNamespace ).toString();
  const QString rProjectName = mModel->data( right, ProjectsModel::ProjectName ).toString();

  if ( lNamespace == rNamespace )
  {
    return lProjectName.compare( rProjectName, Qt::CaseInsensitive ) < 0;
  }
  return lNamespace.compare( rNamespace, Qt::CaseInsensitive ) < 0;
}
