/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATICFEATURESMODEL_H
#define STATICFEATURESMODEL_H


#include "featuresmodel.h"

class InputMapSettings;

/**
 * StaticFeaturesModel is a model of FeatureLayerPairs not tied to a layer.
 * It may be used as a generic container for FeatureLayerPairs.
 */
class StaticFeaturesModel : public FeaturesModel
{
    Q_OBJECT

  public:

    explicit StaticFeaturesModel( QObject *parent = nullptr );

    /**
     * \brief populate populates a static model using the supplied \a pairs
     * \param pairs to populate the model with
     * This can be used to set the initial contents of the model.
     * Any existing feature layer pairs will be removed from the model.
     */
    Q_INVOKABLE void populate( FeatureLayerPairs pairs );

    /**
     * \brief append adds a \a pair to the model
     * \param pair to be added to the model
     * If \a pair already exists in the model, it is not added a second time.
     */
    Q_INVOKABLE void append( FeatureLayerPair pair );

    /**
     * \brief remove removes a \a pair from the model
     * \param pair to be removed from the model
     * If \a pair does not exists in the model, nothing happens.
     */
    Q_INVOKABLE void remove( FeatureLayerPair pair );

    /**
     * \brief collectGeometries returns all the geometries in the model as a multipart geometry
     * \param targetSettings the setting to use for reprojection to map canvas crs
     * \return a multipart geometry consisting of all geometries in the model, reprojected to the map's crs
     */
    Q_INVOKABLE QgsGeometry collectGeometries( InputMapSettings *targetSettings ) const;

    /**
     * \brief features returns all FeatureLayerPairs in the model
     */
    FeatureLayerPairs features() const;

  private:

    friend class TestModels;
};

#endif // STATICFEATURESMODEL_H
