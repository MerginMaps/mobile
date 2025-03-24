/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsvectorlayer.h"
#include "recordinglayersproxymodel.h"

#include "qgsproject.h"
#include "qgslayertree.h"

RecordingLayersProxyModel::RecordingLayersProxyModel( QObject *parent ) :
  QgsMapLayerProxyModel{ parent }
{
  QObject::connect( this, &RecordingLayersProxyModel::rowsInserted, this, &RecordingLayersProxyModel::countChanged );
  QObject::connect( this, &RecordingLayersProxyModel::rowsRemoved, this, &RecordingLayersProxyModel::countChanged );
}

bool RecordingLayersProxyModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( !QgsMapLayerProxyModel::filterAcceptsRow( source_row, source_parent ) )
    return false;

  // get layer from row and parent index
  QModelIndex index = mModel->index( source_row, 0, source_parent );
  QgsMapLayer *layer = mModel->layerFromIndex( index );

  return mModel->data( index, LayersModel::LayerVisible ).toBool();
}

QList<QgsMapLayer *> RecordingLayersProxyModel::layers() const
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

void RecordingLayersProxyModel::refreshData()
{
  invalidate();
}

QgsMapLayer *RecordingLayersProxyModel::firstUsableLayer() const
{
  QList<QgsMapLayer *> filteredLayers = layers();

  if ( filteredLayers.size() > 0 )
  {
    return filteredLayers.at( 0 );
  }

  return nullptr;
}

QModelIndex RecordingLayersProxyModel::indexFromLayerId( QString layerId ) const
{
  if ( layerId.isEmpty() )
    return QModelIndex();

  QgsVectorLayer *layer = layerFromLayerId( layerId );

  return mModel->indexFromLayer( layer ); // return source model index to skip converting indexes in proxy model
}

QgsVectorLayer *RecordingLayersProxyModel::layerFromLayerId( QString layerId ) const
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

QgsVectorLayer *RecordingLayersProxyModel::layerFromLayerName( const QString &layerName ) const
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

QVariant RecordingLayersProxyModel::getData( QModelIndex index, int role ) const
{
  return sourceModel()->data( index, role );
}

LayersModel *RecordingLayersProxyModel::model() const
{
  return mModel;
}

void RecordingLayersProxyModel::setModel( LayersModel *model )
{
  if ( mModel != model )
  {
    mModel = model;
    setSourceModel( mModel );
    setFilters( Qgis::LayerFilter::HasGeometry | Qgis::LayerFilter::WritableLayer );
    emit modelChanged();
  }
}
