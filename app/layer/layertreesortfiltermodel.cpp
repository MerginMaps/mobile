/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreesortfiltermodel.h"
#include "QDebug"

LayerTreeSortFilterModel::LayerTreeSortFilterModel( QObject *parent )
  : QSortFilterProxyModel{parent}
{
}

LayerTreeModel *LayerTreeSortFilterModel::layerTreeModel() const
{
  return mLayerTreeModel;
}

void LayerTreeSortFilterModel::setLayerTreeModel( LayerTreeModel *newLayerTreeModel )
{
  if ( mLayerTreeModel )
  {
    disconnect( mLayerTreeModel );
  }

  if ( mLayerTreeModel != newLayerTreeModel )
  {
    mLayerTreeModel = newLayerTreeModel;
    emit layerTreeModelChanged( mLayerTreeModel );
  }

  if ( mLayerTreeModel )
  {
    setSourceModel( mLayerTreeModel );
    connect( mLayerTreeModel, &LayerTreeModel::modelInitialized, this, &LayerTreeSortFilterModel::onSourceModelInitialized );
  }
}

LayerTreeSortFilterModel::~LayerTreeSortFilterModel() = default;

//const QString &LayerTreeSortFilterModel::searchExpression() const
//{
//  return mSearchExpression;
//}

//void LayerTreeSortFilterModel::setSearchExpression( const QString &newSearchExpression )
//{
//  if ( mSearchExpression == newSearchExpression )
//    return;
//  mSearchExpression = newSearchExpression;
//  emit searchExpressionChanged( mSearchExpression );
//}

QModelIndex LayerTreeSortFilterModel::getModelIndex( int row, int column, const QModelIndex &parent ) const
{
  return index( row, column, parent );
}

void LayerTreeSortFilterModel::onSourceModelInitialized()
{
  sort( 0 );
}
