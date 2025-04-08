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
  if ( !mModel )
    return QList<QgsMapLayer *>();

  return mModel->layers();
}

void RecordingLayersProxyModel::refreshData()
{
  invalidate();
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
