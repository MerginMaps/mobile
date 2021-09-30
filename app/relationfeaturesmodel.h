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

#include "featuresmodel.h"
#include "featurelayerpair.h"

#include "qgsrelation.h"

#include <QObject>

/**
 * RelationFeaturesModel class lists features from a specific layer based on provided QgsRelation.
 * It is used as a model in Relation QML editors and offers helpful methods for manipulation with features.
 */
class RelationFeaturesModel : public FeaturesModel
{
    Q_OBJECT

    Q_PROPERTY( QgsRelation relation READ relation WRITE setRelation NOTIFY relationChanged )

    //! Home path of the active project used to evaluate absolute path of referencved images
    Q_PROPERTY( QString homePath READ homePath WRITE setHomePath NOTIFY homePathChanged )

    //! parent feature layer pair represents a feature from parent relation layer for which we gather related child features
    Q_PROPERTY( FeatureLayerPair parentFeatureLayerPair READ parentFeatureLayerPair WRITE setParentFeatureLayerPair NOTIFY parentFeatureLayerPairChanged )

    /**
     * Flag to distinguish what data type suppose to be displayed. By default text data type is expected, otherwise image.
     * Property is set to False only if there is any photo field (see more: RelationFeaturesModel::photoFieldIndex)
     */
    Q_PROPERTY( bool isTextType READ isTextType WRITE setIsTextType NOTIFY isTextTypeChanged )

  public:

    enum relationModelRoles
    {
      PhotoPath = Qt::UserRole + 100,
    };
    Q_ENUM( relationModelRoles );

    explicit RelationFeaturesModel( QObject *parent = nullptr );
    ~RelationFeaturesModel() {};

    QVariant data( const QModelIndex &index, int role ) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setup() override;
    void setupFeatureRequest( QgsFeatureRequest &request ) override;

    void setParentFeatureLayerPair( FeatureLayerPair pair );
    void setRelation( QgsRelation relation );

    FeatureLayerPair parentFeatureLayerPair() const;
    QgsRelation relation() const;

    QString homePath() const;
    void setHomePath( const QString &homePath );

    bool isTextType() const;
    void setIsTextType( bool isTextType );

  signals:
    void parentFeatureLayerPairChanged( FeatureLayerPair pair );
    void relationChanged( QgsRelation relation );
    void homePathChanged();
    void isTextTypeChanged();

  private:
    QVariant relationPhotoPath( const FeatureLayerPair &featurePair ) const;

    /**
     * Searches and returns first index of photo field if the field is type of 'ExternalResource'.
     * @param layer Referencing layer of the relation.
     * @return Index of photo field, otherwise -1 if not found any.
     */
    int photoFieldIndex( QgsVectorLayer *layer ) const;

    QgsRelation mRelation; // associated relation
    FeatureLayerPair mParentFeatureLayerPair; // parent feature (with relation widget in form)
    QString mHomePath;
    bool mIsTextType = true;
};

#endif // RELATIONFEATURESMODEL_H
