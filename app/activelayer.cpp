/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
  

#include "activelayer.h"

ActiveLayer::ActiveLayer() :
  mLayer( nullptr )
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

void ActiveLayer::setActiveLayer( QgsMapLayer *layer )
{
  if ( !layer )
    return resetActiveLayer();

  if ( !mLayer || !mLayer->isValid() || layer->id() != mLayer->id() )
  {
    mLayer = layer;
    emit activeLayerChanged( layerName() );
  }
}

void ActiveLayer::resetActiveLayer()
{
  // mLayer can be already invalid pointer
  // as a leftover from unloaded QGIS project
  void *lp = static_cast<void *>( mLayer );
  if ( lp != nullptr )
  {
    mLayer = nullptr;
    emit activeLayerChanged( QString() );
  }
}
