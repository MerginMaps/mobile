/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef RECORDINGLAYERSPROXYMODEL_H
#define RECORDINGLAYERSPROXYMODEL_H

#include <QObject>

#include "inputconfig.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerproxymodel.h"
#include "qgsvectorlayer.h"
#include "inpututils.h"

#include "layersmodel.h"

class RecordingLayersProxyModel : public QgsMapLayerProxyModel
{
    Q_OBJECT

    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )
    //Q_PROPERTY( LayerModelTypes modelType READ modelType WRITE setModelType NOTIFY modelTypeChanged )
    Q_PROPERTY( LayersModel *model READ model WRITE setModel NOTIFY modelChanged )

  public:
    Q_INVOKABLE explicit RecordingLayersProxyModel( QObject *parent = nullptr );

    enum LayerModelTypes
    {
      AllLayers,
      ActiveLayerSelection
    };
    Q_ENUM( LayerModelTypes );

    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;

    //! Helper methods that convert layer to/from index/name
    Q_INVOKABLE QModelIndex indexFromLayerId( QString layerId ) const;
    Q_INVOKABLE QgsVectorLayer *layerFromLayerId( QString layerId ) const;

    QgsVectorLayer *layerFromLayerName( const QString &layerName ) const;

    //! Helper method to get data from source model to skip converting indexes
    Q_INVOKABLE QVariant getData( QModelIndex index, int role ) const;

    //! Returns first layer from proxy model's layers list (filtered with filter function)
    Q_INVOKABLE QgsMapLayer *firstUsableLayer() const;

    /**
     * @brief layers method return layers from source model filtered with filter function
     */
    QList<QgsMapLayer *> layers() const;

    //! Filter current model according to its type
    //void updateFilterFunction();

    //! Getters and setters
    // LayerModelTypes modelType() const;
    // void setModelType( LayerModelTypes type );

    LayersModel *model() const;
    void setModel( LayersModel *model );

  signals:
    void countChanged();
    void qgsProjectChanged();
    void modelTypeChanged();
    void modelChanged();

  public slots:
    void refreshData();

  private:
    LayerModelTypes mModelType;
    LayersModel *mModel;

    /**
     * @brief filterFunction method takes layer and outputs if \bold this model type accepts layer.
     * Returns true for proxy model built without specific type or AllLayers type.
     *
     * In future will allow dependency injection of custom filter functions.
     */
    std::function<bool( QgsMapLayer * )> filterFunction;
};

#endif // RECORDINGLAYERSPROXYMODEL_H
