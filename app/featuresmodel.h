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

#include "featurelayerpair.h"


class QgsVectorLayerFeatureSource;


/**
 * FeaturesModel is a base class for models containing FeatureLayerPairs
 */
class FeaturesModel : public QAbstractListModel
{
    Q_OBJECT

    // Returns a number of fetched features currently in the model
    // It is different from layerFeaturesCount -> it says how many features are in the layer
    // Name of the property is intentionally `count` so that it matches ListModel's count property
    Q_PROPERTY( int count READ count NOTIFY countChanged )

  public:

    enum ModelRoles
    {
      FeatureTitle = Qt::UserRole + 10,
      FeatureId,
      Feature,
      FeaturePair,
      Description, // secondary text in list view
      LayerName,
      LayerIcon,
    };
    Q_ENUM( ModelRoles );

    explicit FeaturesModel( QObject *parent = nullptr );
    virtual ~FeaturesModel();

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;

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

    int count() const;

  signals:

    void countChanged( int featuresCount );

  protected:

    virtual void setup();
    virtual void reset();

    virtual QVariant featureTitle( const FeatureLayerPair &featurePair ) const;

    FeatureLayerPairs mFeatures;

  private:

    friend class TestModels;
};

#endif // FEATURESMODEL_H
