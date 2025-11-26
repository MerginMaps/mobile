/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RELATIONREFERENCEFEATURESMODEL_H
#define RELATIONREFERENCEFEATURESMODEL_H

#include <QObject>

#include "qgsproject.h"
#include "layerfeaturesmodel.h"

/**
 * \brief The RelationReferenceFeaturesModel class serve as a helper class for relation reference widget.
 * It is a subclass of FeaturesModel so it is a model containing features from parent layer of the relation reference.
 * Config and Project must be provided in order for this model to work.
 */
class RelationReferenceFeaturesModel : public LayerFeaturesModel
{
    Q_OBJECT

    //! widget's config
    Q_PROPERTY( QVariantMap config READ config WRITE setConfig NOTIFY configChanged )
    //! active project
    Q_PROPERTY( QgsProject *project READ project WRITE setProject NOTIFY projectChanged )
    //! states if the relation widget allows Null (from relation configuration)
    Q_PROPERTY( bool allowNull READ allowNull NOTIFY allowNullChanged )

  public:

    explicit RelationReferenceFeaturesModel( QObject *parent = nullptr );
    ~RelationReferenceFeaturesModel() override;

    //! Returns foreign key of referenced feature with specified \a fid
    Q_INVOKABLE QVariant foreignKeyFromReferencedFeatureId( QgsFeatureId fid );

    //! Returns role value from feature with fkValue in foreign key
    Q_INVOKABLE QVariant attributeFromForeignKey( const QVariant &fkValue, const ModelRoles expectedAttribute );

    QVariantMap config() const;
    QgsProject *project() const;
    bool allowNull() const;

    void setConfig( QVariantMap config );
    void setProject( QgsProject *project );

    //! Reads config and with project instance queries all features from parent layer. Emits populated signal after loading features.
    void setup() override;
    void setupFeatureRequest( QgsFeatureRequest &request ) override;

  signals:
    void configChanged( QVariantMap config );
    void projectChanged( QgsProject *project );
    void allowNullChanged( bool allowNull );

  private:

    QgsRelation mRelation;
    QVariantMap mConfig;
    QgsProject *mProject = nullptr;
    bool mAllowNull = false;
};

#endif // RELATIONREFERENCEFEATURESMODEL_H
