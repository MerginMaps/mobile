#include "recordinglayersmodel.h"

RecordingLayersModel::RecordingLayersModel()
{
  setFilters(
      WritableLayer |
      VectorLayer |
      HasGeometry
  );
}

// Not neccessary, filters in constructor are enough
//bool RecordingLayersModel::layersFilter( int source_row, const QModelIndex &source_parent ) const
//{
//  QModelIndex index = sourceModel()->index( source_row, 0, source_parent );

//  QgsMapLayer* layer = sourceLayerModel()->layerFromIndex( index );

//  bool ret = false;

//  ret = layer->type() == QgsMapLayerType::VectorLayer;
//  ret = ret && !layer->readOnly();

//  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
//  if ( vectorLayer )
//  {
//    ret = ret && ( vectorLayer->wkbType() != QgsWkbTypes::NoGeometry && vectorLayer->wkbType() != QgsWkbTypes::Unknown );
//  }

//  return ret;
//}
