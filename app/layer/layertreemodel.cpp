/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreemodel.h"

#include <QPixmap>
#include <QDebug>

#include "qgslayertree.h"

LayerTreeModel::LayerTreeModel( QObject *parent )
  : QgsLayerTreeModel( new QgsLayerTree(), parent )
{
  connect( this, &LayerTreeModel::qgsProjectChanged, this, &LayerTreeModel::setupModel );
  rootGroup()->setParent( this );
}

LayerTreeModel::~LayerTreeModel() = default;

void LayerTreeModel::setupModel()
{
  if ( !mQgsProject || mQgsProject->homePath().isEmpty() )
  {
    QgsLayerTree *emptyRoot = new QgsLayerTree();
    emptyRoot->setParent( this );
    setRootGroup( emptyRoot );
    return;
  }

  QgsLayerTree *root = mQgsProject->layerTreeRoot();

  setRootGroup( root );

  emit modelInitialized();
}

void LayerTreeModel::reset()
{
  setQgsProject( nullptr );
}

QVariant LayerTreeModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() || index.column() > 1 )
  {
    return QVariant();
  }

  QgsLayerTreeNode *node = index2node( index );

  if ( role == Node )
  {
    return QVariant::fromValue( node );
  }
  else if ( role == NodeType )
  {
    if ( node )
    {
      if ( QgsLayerTree::isGroup( node ) )
      {
        return QStringLiteral( "group" );
      }
      else if ( QgsLayerTree::isLayer( node ) )
      {
        return QStringLiteral( "layer" );
      }
    }
  }
  else if ( role == NodePath )
  {
    // return parent groups to this layer in format "groupA/group B/"
    QString path;

    QModelIndex parentIndex = parent( index );

    while ( parentIndex.isValid() )
    {
      path.append( QgsLayerTreeModel::data( parentIndex, Qt::DisplayRole ).toString() + "/" );
      parentIndex = parent( parentIndex );
    }

    return path;
  }
  else if ( role == NodeIsVisible )
  {
    return visible( node );
  }
  else if ( role == SerializedNode )
  {
    return serializeNode( node );
  }

  return QgsLayerTreeModel::data( index, role );
}

QHash<int, QByteArray> LayerTreeModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
  roles[Node] = QByteArray( "node" );
  roles[NodeType] = QByteArray( "nodeType" );
  roles[NodePath] = QByteArray( "nodePath" );
  roles[NodeIsVisible] = QByteArray( "nodeIsVisible" );
  roles[SerializedNode] = QByteArray( "serializedNode" );
  return roles;
}

QgsProject *LayerTreeModel::qgsProject() const
{
  return mQgsProject;
}

void LayerTreeModel::setQgsProject( QgsProject *newQgsProject )
{
  if ( mQgsProject == newQgsProject )
    return;

  mQgsProject = newQgsProject;
  emit qgsProjectChanged( mQgsProject );
}

QString LayerTreeModel::serializeNode( QgsLayerTreeNode *node ) const
{
  if ( !node )
  {
    return QString();
  }

  QModelIndex index = node2index( node );

  QString hash = QString::number( index.row() );

  QModelIndex parentIndex = parent( index );

  while ( parentIndex.isValid() )
  {
    hash.prepend( QString::number( parentIndex.row() ) + '-' );
    parentIndex = parent( parentIndex );
  }

  return hash;
}

QgsLayerTreeNode *LayerTreeModel::deserializeNode( const QString &nodeId ) const
{
  QStringList rows = nodeId.split( '-' );

  if ( rows.empty() )
  {
    qDebug() << "Invalid id in image provider!" << nodeId;
    return nullptr;
  }

  bool ok = false;
  int row = rows[0].toInt( &ok );

  if ( !ok )
  {
    qDebug() << "Invalid row id in image provider!" << nodeId;
    return nullptr;
  }

  QModelIndex parentIndex = index( row, 0 );
  QModelIndex indexIterator = parentIndex;

  if ( !parentIndex.isValid() )
  {
    qDebug() << "Invalid indexIterator in image provider!" << nodeId;
    return nullptr;
  }

  // start from next item
  for ( int i = 1; i < rows.count(); i++ )
  {
    bool ok = false;
    int row = rows[i].toInt( &ok );

    if ( !ok )
    {
      qDebug() << "Invalid row id in image provider!" << nodeId;
      return nullptr;
    }

    parentIndex = indexIterator;
    indexIterator = index( row, 0, parentIndex );

    if ( !indexIterator.isValid() )
    {
      qDebug() << "Invalid indexIterator in image provider!" << nodeId;
      return nullptr;
    }
  }

  return index2node( indexIterator );
}

QString LayerTreeModel::visible( QgsLayerTreeNode *node ) const
{
  if ( !node )
  {
    return QString();
  }

  if ( QgsLayerTree::isLayer( node ) )
  {
    QgsLayerTreeLayer *nodeLayer = QgsLayerTree::toLayer( node );
    if ( nodeLayer && nodeLayer->layer() && !nodeLayer->layer()->isSpatial() )
    {
      return QString(); // not a spatial layer - do not show eye icon at all
    }
  }

  if ( node->isVisible() )
  {
    return QStringLiteral( "yes" );
  }
  else
  {
    return QStringLiteral( "no" );
  }
}
