/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreesortfiltermodel.h"
#include "qgslayertree.h"

LayerTreeSortFilterModel::LayerTreeSortFilterModel( QObject *parent )
  : QSortFilterProxyModel{parent}
{
  setSortCaseSensitivity( Qt::CaseInsensitive );
}

bool LayerTreeSortFilterModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( mLayerTreeModel )
  {
    QModelIndex modelIndex = mLayerTreeModel->index( source_row, 0, source_parent );

    QgsLayerTreeNode *node = mLayerTreeModel->index2node( modelIndex );

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

  return QSortFilterProxyModel::filterAcceptsRow( source_row, source_parent );
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

QModelIndex LayerTreeSortFilterModel::node2index( QgsLayerTreeNode *node ) const
{
  if ( !node )
  {
    return QModelIndex();
  }

  QModelIndex srcIndex = mLayerTreeModel->node2index( node );

  if ( !srcIndex.isValid() )
  {
    return QModelIndex();
  }

  return mapFromSource( srcIndex );
}

void LayerTreeSortFilterModel::onSourceModelInitialized()
{
  bool sortMethod = mLayerTreeModel->qgsProject()->readNumEntry( QStringLiteral( "Mergin" ), QStringLiteral( "SortLayersMethod/Method" ), 1 );
  if ( sortMethod == SortMethodEnum::Alphabetical ) {
    sort( 0 );
  }
  else{
    // pass
  }

}
