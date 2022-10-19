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

#include "qgslayertree.h"
#include "inpututils.h"

LayerTree::LayerTree( QObject *parent )
{
// TODO: fix destruction of this -- singleton?
}

LayerTree::~LayerTree() = default;

LayerTreeModel::LayerTreeModel( QObject *parent )
  : QgsLayerTreeModel( new LayerTree( this ), parent ) // TODO: fix destruction of temp QgsLayerTree()
{
  connect( this, &LayerTreeModel::qgsProjectChanged, this, &LayerTreeModel::setupModel );
}

LayerTreeModel::~LayerTreeModel() = default;

void LayerTreeModel::setupModel()
{
  if ( !mQgsProject || mQgsProject->homePath().isEmpty() )
    return;

  QgsLayerTree *root = mQgsProject->layerTreeRoot();

  setRootGroup( root );

  emit modelInitialized();
}

void LayerTreeModel::reset()
{
  mQgsProject = nullptr;
}

QVariant LayerTreeModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() || index.column() > 1 )
  {
    return QVariant();
  }

  if ( role == Qt::WhatsThisRole )
  {
    QgsLayerTreeNode *node = index2node( index );
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
  else if ( role == Qt::ToolTipRole )
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
  else if ( role == Qt::DecorationRole )
  {
    QIcon icon = QgsLayerTreeModel::data( index, role ).value<QIcon>();

    if ( icon.isNull() )
    {
      // find icon for this node - either layer icon (based on type and geometry) or group
      QgsLayerTreeNode *node = index2node( index );
      QString iconPath;

      if ( !node )
        return QVariant();

      if ( QgsLayerTree::isGroup( node ) )
      {
        iconPath = "qrc:/mIconGroup.svg";
      }
      else if ( QgsLayerTree::isLayer( node ) )
      {
        QgsLayerTreeLayer *layerNode = QgsLayerTree::toLayer( node );
        if ( !layerNode )
          return QVariant();

        QgsMapLayer *mapLayer = layerNode->layer();
        if ( !mapLayer )
          return QVariant();

        iconPath = InputUtils::loadIconFromLayer( mapLayer );
      }

      return iconPath;
    }

    return icon;
  }
  else if ( role == Qt::StatusTipRole )
  {
    // returns whether this node is visible or not
    QgsLayerTreeNode *node = index2node( index );

    if ( node )
    {
      return node->isVisible();
    }

    return false;
  }

  return QgsLayerTreeModel::data( index, role );
}

QgsLayerTreeNode *LayerTreeModel::node( QModelIndex modelIndex ) const
{
  return index2node( modelIndex );
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
