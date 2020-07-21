/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "layersproxymodel.h"

#include "qgsvectorlayer.h"
#include "qgsproject.h"
#include "qgslayertree.h"

LayersProxyModel::LayersProxyModel( LayersModel *model, ModelTypes modelType ) :
  mModelType( modelType ),
  mModel( model )
{
  setSourceModel( mModel );

  switch ( mModelType )
  {
    case ActiveLayerSelection:
      filterFunction = [this]( QgsMapLayer * layer ) { return recordingAllowed( layer ); };
      break;
    case BrowseDataLayerSelection:
      filterFunction = [this]( QgsMapLayer * layer ) { return browsingAllowed( layer ); };
      break;
    default:
      filterFunction = []( QgsMapLayer * ) { return true; };
      break;
  }
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

bool layerHasGeometry( const QgsVectorLayer *layer )
{
  if ( !layer || !layer->isValid() )
    return false;
  return layer->wkbType() != QgsWkbTypes::NoGeometry && layer->wkbType() != QgsWkbTypes::Type::Unknown;
}

bool LayersProxyModel::recordingAllowed( QgsMapLayer *layer ) const
{
  if ( !layer || !layer->isValid() )
    return false;

  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
  return ( vectorLayer && !vectorLayer->readOnly() && layerHasGeometry( vectorLayer ) && layerVisible( layer ) );
}

bool LayersProxyModel::browsingAllowed( QgsMapLayer *layer ) const
{
  if ( !layer || !layer->isValid() )
    return false;

  bool isIdentifiable = layer->flags() & QgsMapLayer::LayerFlag::Identifiable;
  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

  return ( vectorLayer && isIdentifiable );
}

bool LayersProxyModel::layerVisible( QgsMapLayer *layer ) const
{
  QgsLayerTree *root = QgsProject::instance()->layerTreeRoot();
  QgsLayerTreeLayer *layerTree = root->findLayer( layer );

  if ( layerTree )
    return layerTree->isVisible();

  return false;
}

QList<QgsMapLayer *> LayersProxyModel::layers() const
{
  QList<QgsMapLayer *> allLayers = mModel->layers();
  QList<QgsMapLayer *> filteredLayers;

  if ( !mModel )
    return filteredLayers;

  int layersCount = allLayers.size();

  for ( int i = 0; i < layersCount; i++ )
  {
    if ( filterFunction( allLayers.at( i ) ) )
      filteredLayers << allLayers.at( i );
  }
  return filteredLayers;
}

void LayersProxyModel::onMapThemeChanged()
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

int LayersProxyModel::indexFromLayer( QgsMapLayer *layer ) const
{
  if ( !layer )
    return -1;

  QList<QgsMapLayer *> filteredLayers = layers();

  for ( int i = 0; i < filteredLayers.count(); i++ )
  {
    if ( filteredLayers.at( i )->id() == layer->id() )
      return i;
  }

  return -1;
}

QgsMapLayer *LayersProxyModel::layerFromIndex( int index ) const
{
  QList<QgsMapLayer *> filteredLayers = layers();

  if ( index >= 0 && index < filteredLayers.size() )
    return filteredLayers.at( index );

  return nullptr;
}
