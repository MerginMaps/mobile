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

    //! Returns layers regarding model type
    QList<QgsMapLayer *> layers() const;

    int indexAccordingName( QString layerName ) const;

    int firstUsableIndex() const;

  private:

    //! returns if input layer is capable of recording new features
    bool recordingAllowed( QgsMapLayer *layer ) const;

    //! filters if input layer should be visible for browsing
    bool browsingAllowed( QgsMapLayer *layer ) const;

    //! filters if input layer is visible in current map theme
    bool layerVisible( QgsMapLayer *layer ) const;

    ModelTypes mModelType;
    LayersModel *mModel;

    std::function<bool( QgsMapLayer * )> filterFunction;
};

#endif // BROWSEDATALAYERSMODEL_H
