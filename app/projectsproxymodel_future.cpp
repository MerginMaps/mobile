/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectsproxymodel_future.h"

ProjectsProxyModel_future::ProjectsProxyModel_future( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

void ProjectsProxyModel_future::initialize()
{
  setSourceModel( mModel );
  mModelType = mModel->modelType();

  setFilterRole( ProjectsModel_future::ProjectFullName );
  setFilterCaseSensitivity( Qt::CaseInsensitive );

  sort( 0, Qt::AscendingOrder );
}

QString ProjectsProxyModel_future::searchExpression() const
{
  return mSearchExpression;
}

ProjectsModel_future *ProjectsProxyModel_future::projectSourceModel() const
{
  return mModel;
}

void ProjectsProxyModel_future::setSearchExpression( QString searchExpression )
{
  if ( mSearchExpression == searchExpression )
    return;

  mSearchExpression = searchExpression;
  setFilterFixedString( mSearchExpression );
  emit searchExpressionChanged( mSearchExpression );
}

void ProjectsProxyModel_future::setProjectSourceModel( ProjectsModel_future *sourceModel )
{
  if ( mModel == sourceModel )
    return;

  mModel = sourceModel;
  QObject::connect( mModel, &ProjectsModel_future::modelInitialized, this, &ProjectsProxyModel_future::initialize );
}


bool ProjectsProxyModel_future::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
  if ( mModelType == ProjectsModel_future::LocalProjectsModel )
  {
    bool lProjectIsMergin = mModel->data( left, ProjectsModel_future::ProjectIsMergin ).toBool();
    bool rProjectIsMergin = mModel->data( right, ProjectsModel_future::ProjectIsMergin ).toBool();

    /**
     * Ordering of local projects: first non-mergin projects (using folder name),
     * then mergin projects (sorted first by namespace, then project name)
     */

    if ( !lProjectIsMergin && !rProjectIsMergin )
    {
      QString lProjectFullName = mModel->data( left, ProjectsModel_future::ProjectFullName ).toString();
      QString rProjectFullName = mModel->data( right, ProjectsModel_future::ProjectFullName ).toString();
      qDebug() << "Comparing " << lProjectFullName << rProjectFullName;

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

    QString lNamespace = mModel->data( left, ProjectsModel_future::ProjectNamespace ).toString();
    QString lProjectName = mModel->data( left, ProjectsModel_future::ProjectName ).toString();
    QString rNamespace = mModel->data( right, ProjectsModel_future::ProjectNamespace ).toString();
    QString rProjectName = mModel->data( right, ProjectsModel_future::ProjectName ).toString();

    if ( lNamespace == rNamespace )
    {
      return lProjectName.compare( rProjectName, Qt::CaseInsensitive ) < 0;
    }
    return lNamespace.compare( rNamespace, Qt::CaseInsensitive ) < 0;
  }
  return false;
}
