/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreesortfiltermodel.h"

LayerTreeSortFilterModel::LayerTreeSortFilterModel( QObject *parent )
  : QSortFilterProxyModel{parent}
{

}

LayerTreeSortFilterModel::~LayerTreeSortFilterModel() = default;

LayerTreeModel *LayerTreeSortFilterModel::layerTreeModel() const
{
  return mLayerTreeModel;
}

void LayerTreeSortFilterModel::setLayerTreeModel( LayerTreeModel *newLayerTreeModel )
{
  if ( mLayerTreeModel == newLayerTreeModel )
    return;

  mLayerTreeModel = newLayerTreeModel;
  emit layerTreeModelChanged( mLayerTreeModel );
}

const QString &LayerTreeSortFilterModel::searchExpression() const
{
  return mSearchExpression;
}

void LayerTreeSortFilterModel::setSearchExpression( const QString &newSearchExpression )
{
  if ( mSearchExpression == newSearchExpression )
    return;
  mSearchExpression = newSearchExpression;
  emit searchExpressionChanged( mSearchExpression );
}

//QModelIndex LayerTreeSortFilterModel::index(int row, int column, const QModelIndex &parent) const
//{
//  return createIndex( row, column, nullptr );
//}

//bool LayerTreeSortFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
//{
//  if ( !mLayerTreeModel )
//    return false;


//}

//bool ProjectsProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
//{
//  if ( mModelType == ProjectsModel::LocalProjectsModel )
//  {
//    bool lProjectIsMergin = mModel->data( left, ProjectsModel::ProjectIsMergin ).toBool();
//    bool rProjectIsMergin = mModel->data( right, ProjectsModel::ProjectIsMergin ).toBool();

//    /**
//     * Ordering of local projects: first non-mergin projects (using folder name),
//     * then mergin projects (sorted first by namespace, then project name)
//     */

//    if ( !lProjectIsMergin && !rProjectIsMergin )
//    {
//      QString lProjectFullName = mModel->data( left, ProjectsModel::ProjectFullName ).toString();
//      QString rProjectFullName = mModel->data( right, ProjectsModel::ProjectFullName ).toString();

//      return lProjectFullName.compare( rProjectFullName, Qt::CaseInsensitive ) < 0;
//    }
//    if ( !lProjectIsMergin && rProjectIsMergin )
//    {
//      return true;
//    }
//    if ( lProjectIsMergin && !rProjectIsMergin )
//    {
//      return false;
//    }
//  }

//  // comparing 2 mergin projects
//  QString lNamespace = mModel->data( left, ProjectsModel::ProjectNamespace ).toString();
//  QString lProjectName = mModel->data( left, ProjectsModel::ProjectName ).toString();
//  QString rNamespace = mModel->data( right, ProjectsModel::ProjectNamespace ).toString();
//  QString rProjectName = mModel->data( right, ProjectsModel::ProjectName ).toString();

//  if ( lNamespace == rNamespace )
//  {
//    return lProjectName.compare( rProjectName, Qt::CaseInsensitive ) < 0;
//  }
//  return lNamespace.compare( rNamespace, Qt::CaseInsensitive ) < 0;
//}
