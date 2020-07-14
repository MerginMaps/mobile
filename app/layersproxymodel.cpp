/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "layersproxymodel.h"

LayersProxyModel::LayersProxyModel( ALayersModel *model, ModelTypes modelType ) :
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
  QList<QgsMapLayer *> layers;

  if ( !mModel )
    return layers;

  int layersCount = rowCount();

  for ( int i = 0; i < layersCount; i++ )
  {
    QModelIndex ix = index( i, 0, QModelIndex() );
    QgsMapLayer *l = mModel->layerFromIndex( ix );

    if ( filterFunction( l ) )
      layers << l;
  }
  return layers;
}

int LayersProxyModel::indexAccordingName( QString layerName ) const
{
  QList<QgsMapLayer *> _layers = layers();
  int index = 0;

  for ( QgsMapLayer *ix : _layers )
  {
    if ( ix->name() == layerName )
    {
      return index;
    }
    index++;
  }

  return -1;
}

int LayersProxyModel::firstUsableIndex() const
{
  QList<QgsMapLayer *> _layers = layers();

  if ( _layers.size() > 0 )
  {
    return 0;
  }

  return -1;
}
