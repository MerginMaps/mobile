/***************************************************************************
  featurelayerpair.cpp
 ---------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsvectorlayer.h"
#include "qgsfeature.h"

#include "featurelayerpair.h"

FeatureLayerPair::FeatureLayerPair() = default;

FeatureLayerPair::FeatureLayerPair( const QgsFeature &feature, QgsVectorLayer *layer )
  : mLayer( layer )
  , mFeature( feature )
{
}

QgsVectorLayer *FeatureLayerPair::layer() const
{
  return mLayer;
}

QgsFeature FeatureLayerPair::feature() const
{
  return mFeature;
}

QgsFeature &FeatureLayerPair::featureRef()
{
  return mFeature;
}

bool FeatureLayerPair::isValid() const
{
  return ( mLayer && mFeature.isValid() && hasValidGeometry() );
}

bool FeatureLayerPair::operator==( const FeatureLayerPair &other ) const
{
  return ( mLayer == other.layer() ) && ( mFeature == other.feature() );
}

bool FeatureLayerPair::operator!=( const FeatureLayerPair &other ) const
{
  return ( mLayer != other.layer() ) || ( mFeature != other.feature() );
}

bool FeatureLayerPair::hasValidGeometry() const
{
  Q_ASSERT( mLayer );

  if ( !mFeature.hasGeometry() && !mLayer->isSpatial() )
    return true;

  if ( !mFeature.hasGeometry() || mFeature.geometry().type() != mLayer->geometryType() )
    return false;

  return true;
}
