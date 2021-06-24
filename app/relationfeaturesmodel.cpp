/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "relationfeaturesmodel.h"

RelationFeaturesModel::RelationFeaturesModel( QObject *parent )
  : FeaturesListModel( parent )
{
}

void RelationFeaturesModel::setup()
{
  emptyData();
}

void RelationFeaturesModel::populate()
{
  if ( !mRelation.isValid() || !mParentFeatureLayerPair.isValid() )
    return;

  beginResetModel();
  mFeatures.clear();

  QgsFeatureIterator it = mRelation.getRelatedFeatures( mParentFeatureLayerPair.feature() );
  QgsFeature feat;

  while ( it.nextFeature( feat ) )
  {
    mFeatures << FeatureLayerPair( feat, mRelation.referencingLayer() );
  }

  endResetModel();
}

void RelationFeaturesModel::setParentFeatureLayerPair( FeatureLayerPair pair )
{
  if ( mParentFeatureLayerPair != pair )
  {
    mParentFeatureLayerPair = pair;
    emit parentFeatureLayerPairChanged( mParentFeatureLayerPair );

    populate();
  }
}

void RelationFeaturesModel::setRelation( QgsRelation relation )
{
  if ( mRelation.id() != relation.id() )
  {
    mRelation = relation;
    emit relationChanged( mRelation );

    populate();
  }
}

FeatureLayerPair RelationFeaturesModel::parentFeatureLayerPair() const
{
  return mParentFeatureLayerPair;
}

QgsRelation RelationFeaturesModel::relation() const
{
  return mRelation;
}
