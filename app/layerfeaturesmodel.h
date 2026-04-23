/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERFEATURESMODEL_H
#define LAYERFEATURESMODEL_H

#include <QFutureWatcher>
#include <QAtomicInt>

#include "qgsvectorlayer.h"
#include "featurelayerpair.h"
#include "featuresmodel.h"


class QgsVectorLayerFeatureSource;


/**
 * LayerFeaturesModel class fetches features from layer and provides them via Qt Model's interface
 */
class LayerFeaturesModel : public FeaturesModel
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
    Q_PROPERTY( bool fetchingResults MEMBER mFetchingResults NOTIFY fetchingResultsChanged )

    // Returns if the model should be sorted according to the layer's attribute table configuration sort order
    Q_PROPERTY( bool useAttributeTableSortOrder MEMBER mUseAttributeTableSortOrder )

    Q_PROPERTY( QgsAttributeList attributeList MEMBER mAttributeList )

  public:

    enum LayerModelRoles
    {
      SearchResult = ModelRoles::LastRole + 1, // pair of attribute and its value by which the feature was found, empty if search expression is empty
      LastRole = SearchResult
    };
    Q_ENUM( LayerModelRoles );

    explicit LayerFeaturesModel( QObject *parent = nullptr );
    ~LayerFeaturesModel() override;

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;

    /**
     * \brief reloadFeatures reloads features from current layer
     */
    Q_INVOKABLE void reloadFeatures();

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

    //! \a isFetching is TRUE when still fetching results, FALSE when done fetching
    bool fetchingResultsChanged( bool isFetching );

  protected:

    virtual void setupFeatureRequest( QgsFeatureRequest &request );

    virtual void populate();
    void reset() override;

    //! These are the attributes that will be fetched from the data source when populating the model
    QgsAttributeList mAttributeList;

  private slots:
    void onFutureFinished();

  private:
    QString buildSearchExpression();

    //! Performs getFeatures on layer. Takes ownership of \a layer and tries to move it to current thread.
    QgsFeatureList fetchFeatures( QgsVectorLayerFeatureSource *layer, QgsFeatureRequest req, int searchId );

    //! Returns found attribute and its value from search expression for feature
    QString searchResultPair( const FeatureLayerPair &feat ) const;

    const int FEATURES_LIMIT = 10000; //!< Number of maximum features loaded from layer

    QString mSearchExpression;
    QgsVectorLayer *mLayer = nullptr;

    QAtomicInt mNextSearchId = 0;
    QFutureWatcher<QgsFeatureList> mSearchResultWatcher;
    bool mFetchingResults = false;
    bool mUseAttributeTableSortOrder = false;

    std::unique_ptr<QgsFeedback> mFeedback;

    friend class TestModels;
};

#endif // LAYERFEATURESMODEL_H
