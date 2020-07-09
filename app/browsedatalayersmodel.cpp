/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#include "browsedatalayersmodel.h"

BrowseDataLayersModel::BrowseDataLayersModel( ModelTypes modelType ) :
  mModelType( modelType )
{
}

QVariant BrowseDataLayersModel::data( const QModelIndex &index, int role ) const
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
    case LayerNameRole: return vectorLayer ? vectorLayer->name() : QVariant();
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

bool BrowseDataLayersModel::layersFilter( int source_row, const QModelIndex &source_parent ) const
{
  QModelIndex index = sourceLayerModel()->index( source_row, 0, source_parent );
  QgsMapLayer *layer = sourceLayerModel()->layerFromIndex( index );

  bool isIdentifiable = layer->flags() & QgsMapLayer::LayerFlag::Identifiable;
  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

  if ( vectorLayer && isIdentifiable )
  {
    bool hasGeometry = vectorLayer->wkbType() != QgsWkbTypes::NoGeometry && vectorLayer->wkbType() != QgsWkbTypes::Type::Unknown;
    if ( hasGeometry )
      return true;
  }
  return false;
}

bool BrowseDataLayersModel::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( !QgsMapLayerProxyModel::filterAcceptsRow( source_row, source_parent ) )
    return false;

  switch ( mModelType )
  {
    case ActiveLayerSelection: return true;
    case BrowseDataLayerSelection: return layersFilter( source_row, source_parent );
    default: return true;
  }
}

QHash<int, QByteArray> BrowseDataLayersModel::roleNames() const
{
  QHash<int, QByteArray> roles = sourceLayerModel()->roleNames();
  roles[LayerNameRole] = QStringLiteral( "layerName" ).toLatin1();
  roles[IconSourceRole] = QStringLiteral( "iconSource" ).toLatin1();
  roles[VectorLayerRole] = QStringLiteral( "vectorLayer" ).toLatin1();
  return roles;
}
