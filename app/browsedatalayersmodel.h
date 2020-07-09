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

class BrowseDataLayersModel : public QgsMapLayerProxyModel
{
    Q_OBJECT

  public:
    BrowseDataLayersModel();

    enum layerRoles
    {
      LayerNameRole = Qt::UserRole + 100, //! Reserve for QgsMapLayerModel roles
      VectorLayerRole,
      IconSourceRole
    };
    Q_ENUMS( layerRoles )

    //! Methods needed from QgsMapLayerProxyModel
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;
    QHash<int, QByteArray> roleNames() const override;

  private:
    virtual bool layersFilter( int source_row, const QModelIndex &source_parent ) const;
};

#endif // BROWSEDATALAYERSMODEL_H
