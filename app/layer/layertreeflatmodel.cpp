/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreeflatmodel.h"

#include "qgslayertree.h"

LayerTreeFlatModel::LayerTreeFlatModel( QObject *parent )
  : QStandardItemModel( parent )
{
  connect( this, &LayerTreeFlatModel::qgsProjectChanged, this, &LayerTreeFlatModel::populate );
}

QVariant LayerTreeFlatModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() || index.row() < 0 )
  {
    return QVariant();
  }

  if ( role == NodeIsVisible )
  {
    QgsLayerTreeNode *node = data( index, Node ).value<QgsLayerTreeNode *>();

    if ( node && mLayerTreeModel )
    {
      return mLayerTreeModel->visible( node );
    }
  }
  return QStandardItemModel::data( index, role );
}

QHash<int, QByteArray> LayerTreeFlatModel::roleNames() const
{
  QHash<int, QByteArray> roles = QStandardItemModel::roleNames();
  roles[Node] = QByteArray( "node" );
  roles[NodePath] = QByteArray( "nodePath" );
  roles[NodeType] = QByteArray( "nodeType" );
  roles[NodeIsVisible] = QByteArray( "nodeIsVisible" );
  roles[SerializedNode] = QByteArray( "serializedNode" );
  return roles;
}

LayerTreeFlatModel::~LayerTreeFlatModel() = default;

QgsProject *LayerTreeFlatModel::qgsProject() const
{
  return mQgsProject;
}

void LayerTreeFlatModel::setQgsProject( QgsProject *newQgsProject )
{
  if ( mQgsProject == newQgsProject )
    return;
  mQgsProject = newQgsProject;
  emit qgsProjectChanged( mQgsProject );
}

void LayerTreeFlatModel::reset()
{
  setQgsProject( nullptr );
}

void LayerTreeFlatModel::populate()
{
  beginResetModel();

  clear();

  if ( mLayerTreeModel )
  {
    disconnect( mLayerTreeModel.get() );
  }

  mLayerTreeModel.reset();

  if ( mQgsProject )
  {
    mLayerTreeModel = std::make_unique<LayerTreeModel>( this );
    mLayerTreeModel->setQgsProject( mQgsProject );

    // listen on changes in source model - node visibility might change
    connect( mLayerTreeModel.get(), &LayerTreeModel::dataChanged, this, [this]()
    {
      emit dataChanged( index( 0, 0 ), index( rowCount() - 1, 0 ) );
    } );

    // scrape the layer tree
    QgsLayerTree *root = mQgsProject->layerTreeRoot();

    // bfs
    QList<QgsLayerTreeNode *> leaves = root->children();
    int ix = 0;

    while ( !leaves.isEmpty() )
    {
      QgsLayerTreeNode *node = leaves.takeFirst();

      if ( !node )
        continue;

      QModelIndex modelIndex = mLayerTreeModel->node2index( node );

      QStandardItem *item = new QStandardItem( node->name() );
      item->setIcon( mLayerTreeModel->data( modelIndex, Qt::DecorationRole ).value<QIcon>() );
      item->setData( mLayerTreeModel->data( modelIndex, Node ), Node );
      item->setData( mLayerTreeModel->data( modelIndex, NodePath ), NodePath );
      item->setData( mLayerTreeModel->data( modelIndex, NodeType ), NodeType );
      item->setData( mLayerTreeModel->data( modelIndex, NodeIsVisible ), NodeIsVisible );
      item->setData( ix, SerializedNode );

      appendRow( item ); // TODO: is the ownership of item transfered?
      ++ix;

      if ( QgsLayerTree::isGroup( node ) )
      {
        leaves.append( node->children() );
      }
    }
  }

  endResetModel();

  emit modelInitialized();
}
