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
  return mModel.index( mIndex, 0 );
}

QgsMapLayer *ActiveLayer::layer() const
{
  QModelIndex index = modelIndex();
  QgsMapLayer* layer;

  if ( index.isValid() )
    layer = mModel.data( index, QgsMapLayerModel::LayerRole ).value<QgsMapLayer *>();
  else
    layer = mModel.layers().at( 0 ); // first writable index

  return layer;
}

QString ActiveLayer::layerId() const
{
  return mModel.data( modelIndex(), QgsMapLayerModel::LayerIdRole).toString();
}

void ActiveLayer::activeMapThemeChanged()
{
  // TODO: react on map theme change ~ change active layer to default layer

  mModel.invalidate();
  mIndex = -1;
}

void ActiveLayer::setActiveLayer( int index )
{
  if ( index >= 0 && index < mModel.rowCount() )
  {
    if ( mIndex != index )
    {
      mIndex = index;

//      mAppSettings.setDefaultLayer( name );

      emit activeLayerChanged();
    }
  }
}
