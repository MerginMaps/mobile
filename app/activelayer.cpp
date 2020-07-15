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
