/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#include "layersproxymodel.h"

LayersProxyModel::LayersProxyModel( ModelTypes modelType ) :
  mModelType( modelType )
{
}

QVariant LayersProxyModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( role < LayerNameRole ) // if requested role from parent
    return QgsMapLayerProxyModel::data( index, role );

  QgsVectorLayer *vectorLayer;
  QgsMapLayer *layer;

  layer = sourceLayerModel()->layerFromIndex( index );
  if ( !layer || !layer->isValid() ) return QVariant();

  vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

  switch ( role )
  {
    case LayerNameRole: return layer->name();
    case VectorLayerRole: return vectorLayer ? QVariant::fromValue<QgsVectorLayer *>( vectorLayer ) : QVariant();
    case IconSourceRole:
    {
      if ( vectorLayer )
      {
        QgsWkbTypes::GeometryType type = vectorLayer->geometryType();
        switch ( type )
        {
          case QgsWkbTypes::GeometryType::PointGeometry: return "mIconPointLayer.svg";
          case QgsWkbTypes::GeometryType::LineGeometry: return "mIconLineLayer.svg";
          case QgsWkbTypes::GeometryType::PolygonGeometry: return "mIconPolygonLayer.svg";
          case QgsWkbTypes::GeometryType::UnknownGeometry: return "";
          case QgsWkbTypes::GeometryType::NullGeometry: return "";
        }
        return QVariant();
      }
      else return "mIconRaster.svg";
    }
  }
  return QVariant();
}

QHash<int, QByteArray> LayersProxyModel::roleNames() const
{
  QHash<int, QByteArray> roles = sourceLayerModel()->roleNames();
  roles[LayerNameRole] = QStringLiteral( "layerName" ).toLatin1();
  roles[IconSourceRole] = QStringLiteral( "iconSource" ).toLatin1();
  roles[VectorLayerRole] = QStringLiteral( "vectorLayer" ).toLatin1();
  return roles;
}

bool LayersProxyModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( !QgsMapLayerProxyModel::filterAcceptsRow( source_row, source_parent ) )
    return false;

  // get layer from row and parent index
  QModelIndex index = sourceLayerModel()->index( source_row, 0, source_parent );
  QgsMapLayer *layer = sourceLayerModel()->layerFromIndex( index );

  switch ( mModelType )
  {
    case ActiveLayerSelection: return recordingAllowed( layer );
    case BrowseDataLayerSelection: return browsingAllowed( layer );
    default: return true; // by default accept all layers
  }
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

  return ( vectorLayer && !vectorLayer->readOnly() && layerHasGeometry( vectorLayer ) );
}

bool LayersProxyModel::browsingAllowed( QgsMapLayer *layer ) const
{
  if ( !layer || !layer->isValid() )
    return false;

  bool isIdentifiable = layer->flags() & QgsMapLayer::LayerFlag::Identifiable;
  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

  return ( vectorLayer && isIdentifiable && layerHasGeometry( vectorLayer ) );
}


