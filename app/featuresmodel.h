/***************************************************************************
  layerfeaturesmodel.h
  --------------------------------------
  Date                 : July 2020
  Copyright            : (C) 2020 by Tomas Mizera
  Email                : tomas.mizera2 at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef LAYERFEATURESMODEL_H
#define LAYERFEATURESMODEL_H

#include <QAbstractListModel>
#include "loader.h"
#include "layersmodel.h"

#include "qgsmaplayer.h"
#include "qgsvectorlayer.h"
#include "qgsfeaturerequest.h"
#include "qgsfeatureiterator.h"
#include "qgsquickfeaturelayerpair.h"
#include "qgsgeometry.h"
#include "qgswkbtypes.h"

class FeaturesModel : public QAbstractListModel
{
    Q_OBJECT

    enum roleNames
    {
      featureTitle = Qt::UserRole + 1,
      description, // secondary text in list view
      geometryType // type of geometry (point, line, ..)
    };

  public:
    explicit FeaturesModel( LayersModel &lm, Loader &loader, QObject *parent = nullptr );

    // Basic functionality:
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QHash<int, QByteArray> roleNames() const override;

    // Editable:
    bool setData( const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole ) override;

    Qt::ItemFlags flags( const QModelIndex &index ) const override;

  public slots:
    void reloadDataFromLayerName( const QString &layerName ); // mock
    void reloadDataFromLayer( QgsVectorLayer *layer );


  private:
    QList<QgsQuickFeatureLayerPair> mFeatures;
    LayersModel &mLayersModel;
    Loader &mLoader;

    const char FEATURES_LIMIT = 100;
};

#endif // LAYERFEATURESMODEL_H
