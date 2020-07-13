#include "alayersmodel.h"

ALayersModel::ALayersModel()
{

}

QVariant ALayersModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( role < LayerNameRole ) // if requested role from parent
    return QgsMapLayerModel::data( index, role );

  QgsVectorLayer *vectorLayer;
  QgsMapLayer *layer;

  layer = layerFromIndex( index );
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

QHash<int, QByteArray> ALayersModel::roleNames() const
{
  QHash<int, QByteArray> roles = QgsMapLayerModel::roleNames();
  roles[LayerNameRole] = QStringLiteral( "layerName" ).toLatin1();
  roles[IconSourceRole] = QStringLiteral( "iconSource" ).toLatin1();
  roles[VectorLayerRole] = QStringLiteral( "vectorLayer" ).toLatin1();
  return roles;
}
