/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MULTIEDITMANAGER_H
#define MULTIEDITMANAGER_H

#include <QObject>

#include "featurelayerpair.h"
#include "staticfeaturesmodel.h"

class QgsVectorLayer;

/**
 * @brief The MultiEditManager class allows editing of attributes in multiple features of the same layer at once
 *
 * When features to be edited are added to the model, a temporary layer with the appropriate fields is created.
 * We then open a form with the FeatureLayerPair returned with editableFeature(). When this feature gets saved
 * applyEdits() gets triggered writing any changes to all features in the model.
 */
class MultiEditManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsVectorLayer *layer MEMBER mLayer NOTIFY layerChanged )
    Q_PROPERTY( StaticFeaturesModel *model READ model CONSTANT )
    Q_PROPERTY( QgsGeometry geometry READ collectGeometry NOTIFY geometryChanged )
    Q_PROPERTY( InputMapSettings *mapSettings MEMBER mMapSettings )

  public:
    explicit MultiEditManager( QObject *parent = nullptr );

    //! initialize the manager, populate the model with \a pair
    Q_INVOKABLE void initialize( const FeatureLayerPair &pair );

    //! add \a pair to selection or remove it if it already exists. Does nothing if pair.layer != mLayer
    Q_INVOKABLE void toggleSelect( const FeatureLayerPair &pair );

    //! Returns a FeatureLayerPair with the feature in mTempLayer
    Q_INVOKABLE FeatureLayerPair editableFeature();
    Q_INVOKABLE FeatureLayerPair deleteFeature();

    //! Returns multipart geometry of all geometries in the model, in map crs
    QgsGeometry collectGeometry() const { return mModel->collectGeometries( mMapSettings ); }

    StaticFeaturesModel *model() const { return mModel.get(); }

  signals:
    void layerChanged();
    void geometryChanged();

  private slots:
    bool applyEdits();

  private:
    //! create mTempLayer with fields from the layer in the model data, and a single empty feature
    void createTemporaryLayer();

    std::unique_ptr<QgsVectorLayer> mTempLayer;
    std::unique_ptr<StaticFeaturesModel> mModel;
    QgsVectorLayer *mLayer = nullptr;
    InputMapSettings *mMapSettings = nullptr;
};

#endif // MULTIEDITMANAGER_H
