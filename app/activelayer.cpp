/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "activelayer.h"

ActiveLayer::ActiveLayer()
{
}

QgsMapLayer *ActiveLayer::layer() const
{
  return mLayer;
}

QgsVectorLayer *ActiveLayer::vectorLayer() const
{
  if ( mLayer )
    return qobject_cast<QgsVectorLayer *>( mLayer );

  return nullptr;
}

QString ActiveLayer::layerId() const
{
  if ( mLayer )
    return mLayer->id();

  return QString();
}

QString ActiveLayer::layerName() const
{
  if ( mLayer )
    return mLayer->name();

  return QString();
}

void ActiveLayer::activeMapThemeChanged()
{
  // check if active layer is visible in current map theme too
  QgsLayerTree *root = QgsProject::instance()->layerTreeRoot();
  foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
  {
    if ( nodeLayer->isVisible() )
    {
      QgsMapLayer *layer = nodeLayer->layer();
      if ( layer->isValid() && layer->id() == layerId() )
      {
        emit activeLayerChanged( layerName() ); // needs to be emitted to update indexes
        return;
      }
    }
  }

  // if it is not, reset active layer
  setActiveLayer( nullptr );
}

void ActiveLayer::setActiveLayer( QgsMapLayer *layer )
{
  if ( !layer ) // reset index
  {
    mLayer = nullptr;
    emit activeLayerChanged( layerName() );
  }
  else if ( !mLayer || layer->id() != mLayer->id() )
  {
    mLayer = layer;
    emit activeLayerChanged( layerName() );
  }
}
