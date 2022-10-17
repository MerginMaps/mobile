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
#include "qdebug.h"

#include "qgslayertree.h"

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

  // TODO: any other signals?

//  emit dataChanged( index(0,0), index(rowCount(), 0) );
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
      return QgsLayerTree::isGroup( node );
    }
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
