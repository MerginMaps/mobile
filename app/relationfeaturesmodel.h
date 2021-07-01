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

#include "featureslistmodel.h"
#include "featurelayerpair.h"

#include "qgsrelation.h"

#include <QObject>

class RelationFeaturesModel : public FeaturesListModel
{
    Q_OBJECT

    Q_PROPERTY( QgsRelation relation READ relation WRITE setRelation NOTIFY relationChanged )

    //! parent feature layer pair represents a feature from parent relation layer for which we gather related child features
    Q_PROPERTY( FeatureLayerPair parentFeatureLayerPair READ parentFeatureLayerPair WRITE setParentFeatureLayerPair NOTIFY parentFeatureLayerPairChanged )

  public:

    explicit RelationFeaturesModel( QObject *parent = nullptr );
    virtual ~RelationFeaturesModel();

    void setup(); // will be override
    void populate(); // will be override

    void setParentFeatureLayerPair( FeatureLayerPair pair );
    void setRelation( QgsRelation relation );

    FeatureLayerPair parentFeatureLayerPair() const;
    QgsRelation relation() const;

  public slots:
    void onChildLayerChanged();

  signals:
    void parentFeatureLayerPairChanged( FeatureLayerPair pair );
    void relationChanged( QgsRelation relation );

  private:

    QgsRelation mRelation; // associated relation
    FeatureLayerPair mParentFeatureLayerPair; // parent feature (with relation widget in form)
};

#endif // RELATIONFEATURESMODEL_H
