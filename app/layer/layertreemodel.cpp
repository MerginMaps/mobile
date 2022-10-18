/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreemodel.h"

#include <QDir>
#include <QUuid>
#include <QPixmap>
#include <QBuffer>

#include "qgslayertree.h"
#include "inpututils.h"

LayerTreeModel::LayerTreeModel( QObject *parent )
  : QAbstractItemModel( parent )
{
  connect( this, &LayerTreeModel::qgsProjectChanged, this, &LayerTreeModel::setupModel );
}

LayerTreeModel::~LayerTreeModel() = default;

void LayerTreeModel::setupModel()
{
  if ( !mQgsProject || mQgsProject->homePath().isEmpty() )
    return;

  QgsLayerTree *root = mQgsProject->layerTreeRoot();
  mModel = std::make_unique<QgsLayerTreeModel>( root );

  connect( mModel.get(), &QgsLayerTreeModel::dataChanged, this, &LayerTreeModel::dataChanged );
  connect( mModel.get(), &QgsLayerTreeModel::rowsAboutToBeInserted, this, &LayerTreeModel::rowsAboutToBeInserted );
  connect( mModel.get(), &QgsLayerTreeModel::rowsInserted, this, &LayerTreeModel::rowsInserted );
  connect( mModel.get(), &QgsLayerTreeModel::modelAboutToBeReset, this, &LayerTreeModel::modelAboutToBeReset );
  connect( mModel.get(), &QgsLayerTreeModel::modelReset, this, &LayerTreeModel::modelReset );
  connect( mModel.get(), &QgsLayerTreeModel::columnsAboutToBeInserted, this, &LayerTreeModel::columnsInserted );

  emit modelInitialized();
}

void LayerTreeModel::reset()
{
  if ( mModel )
  {
    disconnect( mModel.get() );
  }

  mModel.reset();
  mQgsProject = nullptr;
}

int LayerTreeModel::rowCount( const QModelIndex &parent ) const
{
  if ( !mModel )
    return 0;

  return mModel->rowCount( parent );
}

int LayerTreeModel::columnCount( const QModelIndex &parent ) const
{
  if ( !mModel )
    return 0;

  return mModel->columnCount( parent );
}

QModelIndex LayerTreeModel::index( int row, int column, const QModelIndex &parent ) const
{
  if ( !mModel )
    return QModelIndex();

  return mModel->index( row, column, parent );
}

QModelIndex LayerTreeModel::parent( const QModelIndex &child ) const
{
  if ( !mModel )
    return QModelIndex();

  return mModel->parent( child );
}

QVariant LayerTreeModel::data( const QModelIndex &index, int role ) const
{
  if ( !mModel )
    return QVariant();

  if ( role == Qt::WhatsThisRole )
  {
    QgsLayerTreeNode *node = mModel->index2node( index );
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
      path.append( data( parentIndex, Qt::DisplayRole ).toString() + "/" );
      parentIndex = parent( parentIndex );
    }

    return path;
  }
  else if ( role == Qt::DecorationRole )
  {
    QIcon icon = mModel->data( index, role ).value<QIcon>();

    if ( icon.isNull() )
    {
      // find icon for this node - either layer icon (based on type and geometry) or group
      QgsLayerTreeNode *node = mModel->index2node( index );
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

  return mModel->data( index, role );
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
