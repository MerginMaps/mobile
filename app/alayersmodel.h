#ifndef ALAYERSMODEL_H
#define ALAYERSMODEL_H

#include <QObject>

#include "qgsmaplayermodel.h"
#include "qgsmaplayer.h"
#include "qgsvectorlayer.h"

class ALayersModel : public QgsMapLayerModel
{
    Q_OBJECT
  public:
    ALayersModel();

    enum LayerRoles
    {
      LayerNameRole = Qt::UserRole + 100, //! Reserved for QgsMapLayerModel roles
      VectorLayerRole,
      IconSourceRole
    };
    Q_ENUMS( LayerRoles )

    //! Methods overridden from QgsMapLayerModel
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;
};

#endif // ALAYERSMODEL_H
