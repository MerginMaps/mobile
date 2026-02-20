/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RELATIONFEATURESMODEL_H
#define RELATIONFEATURESMODEL_H

#include "layerfeaturesmodel.h"
#include "featurelayerpair.h"

#include "qgsrelation.h"

#include <QObject>

/**
 * RelationFeaturesModel class lists features from a specific layer based on provided QgsRelation.
 * It is used as a model in Relation QML editors and offers helpful methods for manipulation with features.
 *
 * For regular 1:N relations set \a relation and \a parentFeatureLayerPair.
 * For N:M relations additionally set \a nmRelation: the model will then perform a two-step lookup
 * (parent → join table → child) and expose child layer features directly, hiding the join table.
 */
class RelationFeaturesModel : public LayerFeaturesModel
{
    Q_OBJECT

    Q_PROPERTY( QgsRelation relation READ relation WRITE setRelation NOTIFY relationChanged )

    //! Home path of the active project used to evaluate absolute path of referencved images
    Q_PROPERTY( QString homePath READ homePath WRITE setHomePath NOTIFY homePathChanged )

    //! parent feature layer pair represents a feature from parent relation layer for which we gather related child features
    Q_PROPERTY( FeatureLayerPair parentFeatureLayerPair READ parentFeatureLayerPair WRITE setParentFeatureLayerPair NOTIFY parentFeatureLayerPairChanged )

    //! Second relation for n-m (many-to-many) configurations. When valid, the model shows features from
    //! the child layer instead of the join table. Leave invalid/unset for regular 1:n behaviour.
    Q_PROPERTY( QgsRelation nmRelation READ nmRelation WRITE setNmRelation NOTIFY nmRelationChanged )

    //! Returns true when the model is operating in n-m mode (nmRelation is valid)
    Q_PROPERTY( bool isNmRelation READ isNmRelation NOTIFY nmRelationChanged )

  public:

    enum RelationModelRoles
    {
      PhotoPath = LayerModelRoles::LastRole + 1,
    };
    Q_ENUM( RelationModelRoles );

    explicit RelationFeaturesModel( QObject *parent = nullptr );
    ~RelationFeaturesModel() override;

    QVariant data( const QModelIndex &index, int role ) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setup() override;
    void setupFeatureRequest( QgsFeatureRequest &request ) override;
    void populate() override;

    void setParentFeatureLayerPair( FeatureLayerPair pair );
    void setRelation( QgsRelation relation );
    void setNmRelation( QgsRelation nmRelation );

    FeatureLayerPair parentFeatureLayerPair() const;
    QgsRelation relation() const;
    QgsRelation nmRelation() const;
    bool isNmRelation() const;

    QString homePath() const;
    void setHomePath( const QString &homePath );

  signals:
    void parentFeatureLayerPairChanged( FeatureLayerPair pair );
    void relationChanged( QgsRelation relation );
    void nmRelationChanged();
    void homePathChanged();

  private:
    QVariant relationPhotoPath( const FeatureLayerPair &featurePair ) const;

    /**
     * Searches and returns first index of photo field if the field is type of 'ExternalResource'.
     * @param layer Referencing layer of the relation.
     * @return Index of photo field, otherwise -1 if not found any.
     */
    int photoFieldIndex( QgsVectorLayer *layer ) const;

    /**
     * For n-m mode: queries the join table and collects the primary key values of the child
     * features that are related to the current parent feature. Results are stored in mChildPkValues.
     * This is a synchronous operation and must be called on the main thread before populate().
     */
    void fetchChildPkValues();

    QgsRelation mRelation;   // associated (joining) relation
    QgsRelation mNmRelation; // second relation for n-m; invalid for 1:n
    FeatureLayerPair mParentFeatureLayerPair; // parent feature (with relation widget in form)
    QList<QVariant> mChildPkValues; // child PK values collected during n-m two-step lookup
    QString mHomePath;
};

#endif // RELATIONFEATURESMODEL_H
