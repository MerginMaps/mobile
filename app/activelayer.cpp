/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "activelayer.h"

ActiveLayer::ActiveLayer( LayersProxyModel &model, AppSettings &appSettings ) :
  mModel( model ),
  mAppSettings( appSettings ),
  mIndex( -1 )
{
}

int ActiveLayer::index() const
{
  return mIndex;
}

QModelIndex ActiveLayer::modelIndex() const
{
  if ( mIndex < 0 )
    return QModelIndex();

  return mModel.index( mIndex, 0 );
}

QgsVectorLayer *ActiveLayer::layer() const
{
  QModelIndex index = modelIndex();
  QgsMapLayer *layer = nullptr;

  if ( index.isValid() )
    layer = mModel.data( index, ALayersModel::VectorLayerRole ).value<QgsMapLayer *>();

  return qobject_cast<QgsVectorLayer *>( layer );
}

QString ActiveLayer::layerId() const
{
  return mModel.data( modelIndex(), QgsMapLayerModel::LayerIdRole ).toString();
}

void ActiveLayer::activeMapThemeChanged()
{
  mModel.invalidate();
}

void ActiveLayer::setActiveLayer( int index )
{
  if ( index < 0 ) // only reset index
    mIndex = index;
  else if ( index < mModel.rowCount() )
  {
    if ( mIndex != index )
    {
      mIndex = index;

      emit activeLayerChanged( layer()->name() );
    }
  }
}

void ActiveLayer::setActiveLayer( QString layerName )
{
  int index = mModel.indexAccordingName( layerName );

  if ( index == -1 )
    index = mModel.firstUsableIndex();

  setActiveLayer( index );
}
