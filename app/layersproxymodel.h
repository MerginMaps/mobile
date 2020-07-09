/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef BROWSEDATALAYERSMODEL_H
#define BROWSEDATALAYERSMODEL_H

#include <QObject>

#include "qgsmaplayerproxymodel.h"
#include "qgsmaplayermodel.h"
#include "qgsvectorlayer.h"
#include "qgsmaplayer.h"

enum ModelTypes
{
  ActiveLayerSelection,
  BrowseDataLayerSelection,
  AllLayers
};

class LayersProxyModel : public QgsMapLayerProxyModel
{
    Q_OBJECT

  public:
    LayersProxyModel( ModelTypes modelType = ModelTypes::AllLayers );

    //! Additional roles, can be omitted when added to QGIS
    enum LayerRoles
    {
      LayerNameRole = Qt::UserRole + 100, //! Reserve for QgsMapLayerModel roles
      VectorLayerRole,
      IconSourceRole
    };
    Q_ENUMS( LayerRoles )

    //! Methods overridden from QgsMapLayerProxyModel
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;
    QHash<int, QByteArray> roleNames() const override;

  private:

    //! returns if input layer is capable of recording new features
    bool recordingAllowed( QgsMapLayer *layer ) const;

    //! filters if input layer should be visible for browsing
    bool browsingAllowed( QgsMapLayer *layer ) const;

    ModelTypes mModelType;
};

#endif // BROWSEDATALAYERSMODEL_H
