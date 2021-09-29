/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include <QAbstractListModel>

#include "qgsvectorlayer.h"
#include "featurelayerpair.h"

/**
 */
class FeaturesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )

    //! Read only property holding number of fetched features from layer
    Q_PROPERTY( int featuresCount READ featuresCount NOTIFY featuresCountChanged )

    /**
     * Read only property holding true number of features in layer - not only requested features
     * Changing search expression does not result in changing this number, only layer change
     */
    Q_PROPERTY( int layerFeaturesCount READ layerFeaturesCount NOTIFY layerFeaturesCountChanged )

    /**
     * Search expression represents a filter used when querying for data in current layer.
     * Changing this property results in reloading features from layer with new search expression.
     */
    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )

    //! Read only property limiting maximum number of features that can be fetched
    Q_PROPERTY( int featuresLimit READ featuresLimit NOTIFY featuresLimitChanged )

  public:

    enum modelRoles
    {
      FeatureTitle = Qt::UserRole + 10,
      FeatureId,
      Feature,
      FeaturePair,
      Description, //! secondary text in list view
      SearchResult //! pair of attribute and its value by which the feature was found, empty if search expression is empty
    };
    Q_ENUM( modelRoles );

    explicit FeaturesModel( QObject *parent = nullptr );
    virtual ~FeaturesModel();

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;

    /**
     * \brief reloadFeatures reloads features from current layer
     */
    Q_INVOKABLE void reloadFeatures();

    /**
     * \brief rowFromRoleValue finds feature with requested role and value, returns its row
     * \param role to find from modelRoles
     * \param value to find
     * \return Row index for found feature, returns -1 if no feature is found. If more features
     * match requested role and value, index of first is returned.
     */
    Q_INVOKABLE int rowFromRoleValue( const int role, const QVariant &value ) const;

    /**
     * \brief convertRoleValue helpful method to get value of a different role for a feature specified with another role and its value
     * \param role role to find from modelRoles
     * \param value value to find
     * \param requestedRole a role whose value is returned
     * \return If feature is found by role and value, method returns value for requested role. Returns empty QVariant if no feature is found. If more features
     * match requested role and value, value for first is returned.
     */
    Q_INVOKABLE QVariant convertRoleValue( const int fromRole, const QVariant &fromValue, const int toRole ) const;

    int featuresLimit() const;
    int featuresCount() const;
    QgsVectorLayer *layer() const;
    int layerFeaturesCount() const;
    QString searchExpression() const;

    void setSearchExpression( const QString &searchExpression );
    void setLayer( QgsVectorLayer *newLayer );

  signals:

    void featuresCountChanged( int featuresCount );
    void featuresLimitChanged( int featuresLimit );
    void searchExpressionChanged( const QString &searchExpression );
    void layerChanged( QgsVectorLayer *layer );
    void layerFeaturesCountChanged( int );

  protected:

    virtual void setupFeatureRequest( QgsFeatureRequest &request );

    virtual void populate();
    virtual void setup();
    virtual void reset();

    virtual QVariant featureTitle( const FeatureLayerPair &featurePair ) const;

  private:
    QString buildSearchExpression();

    //! Returns found attribute and its value from search expression for feature
    QString searchResultPair( const FeatureLayerPair &feat ) const;

    const int FEATURES_LIMIT = 10000; //! Number of maximum features loaded from layer

    FeatureLayerPairs mFeatures;
    QString mSearchExpression;
    QgsVectorLayer *mLayer = nullptr;
    int mLayerFeaturesCount;
};
