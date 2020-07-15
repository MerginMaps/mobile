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
#include "qgsproject.h"
#include "qgslayertree.h"
#include "qgslayertreelayer.h"

#include "layersmodel.h"

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
    LayersProxyModel( LayersModel *model, ModelTypes modelType = ModelTypes::AllLayers );

    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;

    //! Helper methods that convert layer to/from index/name
    Q_INVOKABLE QgsMapLayer *layerFromName( QString layerName ) const;
    Q_INVOKABLE QgsMapLayer *layerFromIndex( int index ) const;
    Q_INVOKABLE int indexFromLayer( QgsMapLayer *layer ) const;

    //! Returns first layer from proxy model's layers list (filtered with filter function)
    Q_INVOKABLE QgsMapLayer *firstUsableLayer() const;

    /**
     * @brief layers method return layers from source model filtered with filter function
     */
    QList<QgsMapLayer *> layers() const;

  private:

    //! returns if input layer is capable of recording new features
    bool recordingAllowed( QgsMapLayer *layer ) const;

    //! filters if input layer should be visible for browsing
    bool browsingAllowed( QgsMapLayer *layer ) const;

    //! filters if input layer is visible in current map theme
    bool layerVisible( QgsMapLayer *layer ) const;

    ModelTypes mModelType;
    LayersModel *mModel;

    /**
     * @brief filterFunction method takes layer and outputs if \bold this model type accepts layer.
     * Returns true for proxy model built without specific type or AllLayers type.
     *
     * In future will allow dependency injection of custom filter functions.
     */
    std::function<bool( QgsMapLayer * )> filterFunction;
};

#endif // BROWSEDATALAYERSMODEL_H
