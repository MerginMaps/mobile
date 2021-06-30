/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "relationfeaturesmodel.h"
#include "qgsvectorlayer.h"

RelationFeaturesModel::RelationFeaturesModel( QObject *parent )
  : FeaturesListModel( parent )
{
}

RelationFeaturesModel::~RelationFeaturesModel()
{
  QObject::disconnect( mRelation.referencingLayer(), nullptr, nullptr, nullptr );
}

void RelationFeaturesModel::setup()
{
  if ( !mRelation.isValid() || !mParentFeatureLayerPair.isValid() )
    return;

  QObject::connect( mRelation.referencingLayer(), &QgsVectorLayer::afterCommitChanges, this, &RelationFeaturesModel::onChildLayerChanged );
  populate();
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
  emit featuresCountChanged( mFeatures.count() );
}

void RelationFeaturesModel::setParentFeatureLayerPair( FeatureLayerPair pair )
{
  if ( mParentFeatureLayerPair != pair )
  {
    mParentFeatureLayerPair = pair;
    emit parentFeatureLayerPairChanged( mParentFeatureLayerPair );

    setup();
  }
}

void RelationFeaturesModel::setRelation( QgsRelation relation )
{
  if ( mRelation.id() != relation.id() )
  {
    mRelation = relation;
    emit relationChanged( mRelation );

    setup();
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

void RelationFeaturesModel::onChildLayerChanged()
{
  populate();
}
