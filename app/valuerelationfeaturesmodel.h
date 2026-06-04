/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VALUERELATIONFEATURESMODEL_H
#define VALUERELATIONFEATURESMODEL_H

#include "layerfeaturesmodel.h"
#include "featurelayerpair.h"

#include <QObject>
#include <QVariantMap>

/**
 * ValueRelationFeaturesModel backs the selection drawer in the value-relation
 * drawers.
 *
 * Features are never loaded automatically, caller must call reloadFeatures()
 * explicitly.
 *
 * The inherited searchExpression property triggers an async re-query with
 * the user's search text combined with any FilterExpression (if a valid feature
 * is provided).
 *
 * Model is reduced to load only KeyColumn and ValueColumn attributes.
 * It searches only within ValueColumn attribute.
 */
class ValueRelationFeaturesModel : public LayerFeaturesModel
{
    Q_OBJECT

    Q_PROPERTY( QVariantMap config READ config WRITE setConfig NOTIFY configChanged )

    /**
     * Used solely during setupFeatureRequest() to build a form-scope expression
     * context so that form-scoped filter expressions (e.g. current_value())
     * resolve correctly for drill-down / cascading value relations.
     */
    Q_PROPERTY( FeatureLayerPair pair READ pair WRITE setPair NOTIFY pairChanged )

  public:

    enum ValueRelationRoles
    {
      KeyColumn   = LayerFeaturesModel::LayerModelRoles::LastRole + 1,
      ValueColumn = KeyColumn + 1,
      LastRole    = ValueColumn
    };
    Q_ENUM( ValueRelationRoles );

    explicit ValueRelationFeaturesModel( QObject *parent = nullptr );
    ~ValueRelationFeaturesModel() override;

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setup() override;
    void reset() override;
    void setupFeatureRequest( QgsFeatureRequest &request ) override;
    QString buildSearchExpression() override;

    QVariantMap config() const;
    void setConfig( const QVariantMap &newConfig );

    FeatureLayerPair pair() const;
    void setPair( const FeatureLayerPair &newPair );

  signals:
    void configChanged( const QVariantMap &config );
    void pairChanged( const FeatureLayerPair &pair );

  private:

    static constexpr int VR_FEATURES_LIMIT = 1000;

    QVariantMap mConfig;
    FeatureLayerPair mPair;
    QString mKeyField;
    QString mValueField;
    int mKeyFieldIndex = -1;
    int mValueFieldIndex = -1;
    QString mFilterExpression;
    bool mIsInitialized = false;

    QString mOrderByField;
    bool mOrderByAsc = false;
};

#endif // VALUERELATIONFEATURESMODEL_H
