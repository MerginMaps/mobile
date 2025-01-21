/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsvectorlayer.h"
#include "layersproxymodel.h"

#include "qgsproject.h"
#include "qgslayertree.h"

LayersProxyModel::LayersProxyModel( QObject *parent ) :
  QgsMapLayerProxyModel{ parent }
{
  QObject::connect( this, &LayersProxyModel::rowsInserted, this, &LayersProxyModel::countChanged );
  QObject::connect( this, &LayersProxyModel::rowsRemoved, this, &LayersProxyModel::countChanged );

  updateFilterFunction();
}

bool LayersProxyModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( !QgsMapLayerProxyModel::filterAcceptsRow( source_row, source_parent ) )
    return false;

  // get layer from row and parent index
  QModelIndex index = mModel->index( source_row, 0, source_parent );
  QgsMapLayer *layer = mModel->layerFromIndex( index );

  return filterFunction( layer );
}

QList<QgsMapLayer *> LayersProxyModel::layers() const
{
  QList<QgsMapLayer *> filteredLayers;

  if ( !mModel )
    return filteredLayers;

  QList<QgsMapLayer *> allLayers = mModel->layers();
  int layersCount = allLayers.size();

  for ( int i = 0; i < layersCount; i++ )
  {
    if ( filterFunction( allLayers.at( i ) ) )
      filteredLayers << allLayers.at( i );
  }
  return filteredLayers;
}

void LayersProxyModel::refreshData()
{
  invalidate();
}

QgsMapLayer *LayersProxyModel::firstUsableLayer() const
{
  QList<QgsMapLayer *> filteredLayers = layers();

  if ( filteredLayers.size() > 0 )
  {
    return filteredLayers.at( 0 );
  }

  return nullptr;
}

QModelIndex LayersProxyModel::indexFromLayerId( QString layerId ) const
{
  if ( layerId.isEmpty() )
    return QModelIndex();

  QgsVectorLayer *layer = layerFromLayerId( layerId );

  return mModel->indexFromLayer( layer ); // return source model index to skip converting indexes in proxy model
}

QgsVectorLayer *LayersProxyModel::layerFromLayerId( QString layerId ) const
{
  QList<QgsMapLayer *> filteredLayers = layers();

  for ( int i = 0; i < filteredLayers.count(); i++ )
  {
    if ( filteredLayers.at( i )->id() == layerId )
    {
      QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( filteredLayers.at( i ) );
      if ( layer )
        return layer;
    }
  }
  return nullptr;
}

QgsVectorLayer *LayersProxyModel::layerFromLayerName( const QString &layerName ) const
{
  QList<QgsMapLayer *> filteredLayers = layers();

  for ( int i = 0; i < filteredLayers.count(); i++ )
  {
    if ( filteredLayers.at( i )->name() == layerName )
    {
      QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( filteredLayers.at( i ) );
      if ( layer )
        return layer;
    }
  }
  return nullptr;
}

QVariant LayersProxyModel::getData( QModelIndex index, int role ) const
{
  return sourceModel()->data( index, role );
}

QgsProject *LayersProxyModel::qgsProject() const
{
  return mProject;
}

void LayersProxyModel::setQgsProject( QgsProject *project )
{
  if ( mProject != project )
  {
    mProject = project;
    emit qgsProjectChanged();
  }
}

LayersProxyModel::LayerModelTypes LayersProxyModel::modelType() const
{
  return mModelType;
}

void LayersProxyModel::setModelType( LayerModelTypes type )
{
  if ( mModelType != type )
  {
    mModelType = type;

    updateFilterFunction();

    emit modelTypeChanged();
  }
}

LayersModel *LayersProxyModel::model() const
{
  return mModel;
}

void LayersProxyModel::setModel( LayersModel *model )
{
  if ( mModel != model )
  {
    mModel = model;
    setSourceModel( mModel );
    emit modelChanged();
  }
}

void LayersProxyModel::updateFilterFunction()
{
  switch ( mModelType )
  {
    case ActiveLayerSelection:
      filterFunction = [this]( QgsMapLayer * layer )
      {
        return InputUtils::recordingAllowed( layer, mProject );
      };
      break;
    default:
      filterFunction = []( QgsMapLayer * )
      {
        return true;
      };
      break;
  }

  refreshData();
}
