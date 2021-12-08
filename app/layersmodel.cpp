/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsvectorlayer.h"

#include "layersmodel.h"

LayersModel::LayersModel()
{
}

QVariant LayersModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( role < LayerNameRole ) // if requested role from parent
    return QgsMapLayerModel::data( index, role );

  QgsMapLayer *layer = layerFromIndex( index );
  if ( !layer || !layer->isValid() ) return QVariant();

  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

  switch ( role )
  {
    case LayerNameRole: return layer->name();
    case VectorLayerRole: return vectorLayer ? QVariant::fromValue<QgsVectorLayer *>( vectorLayer ) : QVariant();
    case HasGeometryRole: return vectorLayer ? vectorLayer->wkbType() != QgsWkbTypes::NoGeometry && vectorLayer->wkbType() != QgsWkbTypes::Type::Unknown : QVariant();
    case IconSourceRole:
    {
      if ( vectorLayer )
      {
        QgsWkbTypes::GeometryType type = vectorLayer->geometryType();
        switch ( type )
        {
          case QgsWkbTypes::GeometryType::PointGeometry: return QStringLiteral( "qrc:/mIconPointLayer.svg" );
          case QgsWkbTypes::GeometryType::LineGeometry: return QStringLiteral( "qrc:/mIconLineLayer.svg" );
          case QgsWkbTypes::GeometryType::PolygonGeometry: return QStringLiteral( "qrc:/mIconPolygonLayer.svg" );

          case QgsWkbTypes::GeometryType::UnknownGeometry: // fall through
          case QgsWkbTypes::GeometryType::NullGeometry: return QStringLiteral( "qrc:/mIconTableLayer.svg" );
        }
        return QVariant();
      }
      else return QStringLiteral( "qrc:/mIconRaster.svg" );
    }
    case LayerIdRole: return layer->id();
  }
  return QVariant();
}

QHash<int, QByteArray> LayersModel::roleNames() const
{
  QHash<int, QByteArray> roles = QgsMapLayerModel::roleNames();
  roles[LayerNameRole] = QStringLiteral( "layerName" ).toLatin1();
  roles[IconSourceRole] = QStringLiteral( "iconSource" ).toLatin1();
  roles[HasGeometryRole] = QStringLiteral( "hasGeometry" ).toLatin1();
  roles[VectorLayerRole] = QStringLiteral( "vectorLayer" ).toLatin1();
  roles[LayerIdRole] = QStringLiteral( "layerId" ).toLatin1();
  return roles;
}
