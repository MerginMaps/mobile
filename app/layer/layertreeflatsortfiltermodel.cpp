/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreeflatsortfiltermodel.h"

#include "qgslayertree.h"

LayerTreeFlatSortFilterModel::LayerTreeFlatSortFilterModel( QObject *parent )
  : QSortFilterProxyModel{parent}
{
  setFilterCaseSensitivity( Qt::CaseInsensitive );
}

LayerTreeFlatSortFilterModel::~LayerTreeFlatSortFilterModel() = default;

bool LayerTreeFlatSortFilterModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( mLayerTreeFlatModel )
  {
    QModelIndex modelIndex = mLayerTreeFlatModel->index( source_row, 0, source_parent );

    QStandardItem *item = mLayerTreeFlatModel->itemFromIndex( modelIndex );
    if ( item )
    {
      QgsLayerTreeNode *node = item->data( LayerTreeFlatModel::Node ).value<QgsLayerTreeNode *>();
      if ( node && QgsLayerTree::isLayer( node ) )
      {
        QgsLayerTreeLayer *layerLeaf = QgsLayerTree::toLayer( node );

        if ( layerLeaf )
        {
          QgsMapLayer *layer = layerLeaf->layer();

          if ( layer )
          {
            bool isPrivate = layer->flags() & QgsMapLayer::LayerFlag::Private;

            if ( isPrivate )
            {
              return false;
            }
          }
        }
      }
    }
  }

  return QSortFilterProxyModel::filterAcceptsRow( source_row, source_parent );
}

const QString &LayerTreeFlatSortFilterModel::searchExpression() const
{
  return mSearchExpression;
}

void LayerTreeFlatSortFilterModel::setSearchExpression( const QString &newSearchExpression )
{
  if ( mSearchExpression == newSearchExpression )
    return;
  mSearchExpression = newSearchExpression;
  emit searchExpressionChanged( mSearchExpression );

  setFilterFixedString( mSearchExpression );
}

LayerTreeFlatModel *LayerTreeFlatSortFilterModel::layerTreeFlatModel() const
{
  return mLayerTreeFlatModel;
}

void LayerTreeFlatSortFilterModel::setLayerTreeFlatModel( LayerTreeFlatModel *newLayerTreeFlatModel )
{
  if ( mLayerTreeFlatModel == newLayerTreeFlatModel )
    return;

  if ( mLayerTreeFlatModel )
  {
    disconnect( mLayerTreeFlatModel );
  }

  mLayerTreeFlatModel = newLayerTreeFlatModel;
  emit layerTreeFlatModelChanged( mLayerTreeFlatModel );

  if ( mLayerTreeFlatModel )
  {
    connect( mLayerTreeFlatModel, &LayerTreeFlatModel::modelInitialized, this, &LayerTreeFlatSortFilterModel::onSourceModelInitialized );
    setSourceModel( mLayerTreeFlatModel );
  }
}

void LayerTreeFlatSortFilterModel::onSourceModelInitialized()
{
  sort( 0 );
}
