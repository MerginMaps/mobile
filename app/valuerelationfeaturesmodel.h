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

#include "featuresmodel.h"
#include "featurelayerpair.h"

#include <QObject>

/**
 * ValueRelationFeaturesModel class lists features from a specific layer regarding to a filterExpression of value relations.
 * It is used as a model in ValueRelations QML editors.
 */
class ValueRelationFeaturesModel : public FeaturesModel
{
    Q_OBJECT

    Q_PROPERTY( FeatureLayerPair pair READ pair WRITE setPair NOTIFY pairChanged )
    Q_PROPERTY( QVariantMap config READ config WRITE setConfig NOTIFY configChanged )

  public:

    explicit ValueRelationFeaturesModel( QObject *parent = nullptr );
    ~ValueRelationFeaturesModel() override;

    void setup() override;
    void reset() override;
    void setupFeatureRequest( QgsFeatureRequest &request ) override;
    QVariant featureTitle( const FeatureLayerPair &pair ) const override;

    Q_INVOKABLE QVariant convertToKey( const QVariant &id );
    Q_INVOKABLE QVariant convertToQgisType( const QVariantList &featureIds ); // feature id -> key
    Q_INVOKABLE QVariant convertFromQgisType( QVariant qgsValue, FeaturesModel::ModelRoles ); // key -> other role (feature id/title)

    FeatureLayerPair pair() const;
    void setPair( const FeatureLayerPair &newPair );

    QVariantMap config() const;
    void setConfig( const QVariantMap &newConfig );

  signals:
    void pairChanged( const FeatureLayerPair &pair );
    void configChanged( const QVariantMap &config );
    void invalidate(); // invalidate signal is emitted when value to convert is not present in model

  private:
    QMap<QVariant, QVariant> mCache;

    QVariantMap mConfig;
    FeatureLayerPair mPair; // feature layer pair that has opened the form

    bool mAllowMulti = false;
    QString mKeyField;
    QString mTitleField;
    QString mFilterExpression;

    bool mIsInitialized = false; // model successfully read config and is ready to use
};

#endif // VALUERELATIONFEATURESMODEL_H
