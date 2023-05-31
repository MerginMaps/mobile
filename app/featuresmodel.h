/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FEATURESMODEL_H
#define FEATURESMODEL_H

#include <QAbstractListModel>
#include <QFutureWatcher>
#include <QAtomicInt>

#include "qgsvectorlayer.h"
#include "featurelayerpair.h"

#include "inputconfig.h"


class QgsVectorLayerFeatureSource;


/**
 * FeaturesModel class fetches features from layer and provides them via Qt Model's interface
 */
class FeaturesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )

    /**
     * Search expression represents a filter used when querying for data in current layer.
     * Changing this property results in reloading features from layer with new search expression.
     */
    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )

    // Limits maximum number of features that will be fetched from layer
    Q_PROPERTY( int featuresLimit READ featuresLimit NOTIFY featuresLimitChanged )

    // Returns number of features in layer (property). Can be different number than rowCount() due to a searchExpression
    Q_PROPERTY( int layerFeaturesCount READ layerFeaturesCount NOTIFY layerFeaturesCountChanged )

    // Returns if there is a pending feature request that will populate the model
    Q_PROPERTY( bool fetchingResults READ fetchingResults NOTIFY fetchingResultsChanged )
  public:

    enum ModelRoles
    {
      FeatureTitle = Qt::UserRole + 10,
      FeatureId,
      Feature,
      FeaturePair,
      Description, // secondary text in list view
      SearchResult // pair of attribute and its value by which the feature was found, empty if search expression is empty
    };
    Q_ENUM( ModelRoles );

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
     * \param fromRole role to find from modelRoles
     * \param fromValue value to find
     * \param toRole a role whose value is returned
     * \return If feature is found by role and value, method returns value for requested role. Returns empty QVariant if no feature is found. If more features
     * match requested role and value, value for first is returned.
     */
    Q_INVOKABLE QVariant convertRoleValue( const int fromRole, const QVariant &fromValue, const int toRole ) const;

    /**
     * \brief fetchingResults checks if there is a pending feature request to the layer
     * \return TRUE if there is a pending request, FALSE if no requests are pending
     */
    bool fetchingResults() const;

    int featuresLimit() const;
    QgsVectorLayer *layer() const;
    QString searchExpression() const;

    void setSearchExpression( const QString &searchExpression );
    void setLayer( QgsVectorLayer *newLayer );

    int layerFeaturesCount() const;

  signals:

    void featuresLimitChanged( int featuresLimit );
    void searchExpressionChanged( const QString &searchExpression );
    void layerChanged( QgsVectorLayer *layer );

    void layerFeaturesCountChanged( int layerFeaturesCount );

    void fetchingResultsChanged();

  protected:

    virtual void setupFeatureRequest( QgsFeatureRequest &request );

    virtual void populate();
    virtual void setup();
    virtual void reset();

    virtual QVariant featureTitle( const FeatureLayerPair &featurePair ) const;

  private slots:
    void onFutureFinished();

  private:
    QString buildSearchExpression();

    //! Performs getFeatures on layer. Takes ownership of \a layer and tries to move it to current thread.
    QgsFeatureList fetchFeatures( QgsVectorLayerFeatureSource *layer, QgsFeatureRequest req, int searchId );

    //! Returns found attribute and its value from search expression for feature
    QString searchResultPair( const FeatureLayerPair &feat ) const;

    const int FEATURES_LIMIT = 10000; //! Number of maximum features loaded from layer

    FeatureLayerPairs mFeatures;
    QString mSearchExpression;
    QgsVectorLayer *mLayer = nullptr;

    QAtomicInt mNextSearchId = 0;
    QFutureWatcher<QgsFeatureList> mSearchResultWatcher;
    bool mFetchingResults = false;
};

#endif // FEATURESMODEL_H
