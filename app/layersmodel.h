/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef LAYERSMODEL_H
#define LAYERSMODEL_H

#include <QObject>
#include <QPointer>

#include "qgsmaplayer.h"
#include "qgsmaplayermodel.h"

class LayersModel : public QgsMapLayerModel
{
    Q_OBJECT

  public:
    LayersModel();

    //! Returns list of all layers
    QList<QgsMapLayer *> layers() const { return mLayers; };

    enum LayerRoles
    {
      LayerNameRole = Qt::UserRole + 100, //! Reserved for QgsMapLayerModel roles
      VectorLayerRole,
      HasGeometryRole,
      IconSourceRole,
      LayerIdRole
    };
    Q_ENUMS( LayerRoles )

    //! Methods overridden from QgsMapLayerModel
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;
};

#endif // LAYERSMODEL_H
