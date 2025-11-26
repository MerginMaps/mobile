/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "relationreferencefeaturesmodel.h"
#include "coreutils.h"
#include "qgsrelationmanager.h"

RelationReferenceFeaturesModel::RelationReferenceFeaturesModel( QObject *parent )
  : LayerFeaturesModel( parent )
{
}

RelationReferenceFeaturesModel::~RelationReferenceFeaturesModel() = default;

QVariant RelationReferenceFeaturesModel::foreignKeyFromReferencedFeatureId( QgsFeatureId fid )
{
  if ( !mRelation.isValid() )
    return {};

  const QgsAttributeList refFields = mRelation.referencedFields();
  QgsFeatureRequest request( fid );
  request.setFlags( Qgis::FeatureRequestFlag::NoGeometry );
  request.setSubsetOfAttributes( refFields );

  QgsFeatureIterator it = mRelation.referencedLayer()->getFeatures( request );
  QgsFeature f;

  if ( it.nextFeature( f ) )
    return f.attribute( refFields.constFirst() ); // TODO: multiple keys support

  return {};
}

QVariant RelationReferenceFeaturesModel::attributeFromForeignKey( const QVariant &fkValue, const FeaturesModel::ModelRoles expectedAttribute )
{
  if ( !mRelation.isValid() )
    return {};

  // Attributes from the referencing layer
  QgsAttributes attrs = QgsAttributes( mRelation.referencingLayer()->fields().count() );
  // Set the value on the foreign key field of the referencing record
  const QgsAttributeList refFields = mRelation.referencingFields();
  for ( const int fieldIndex : refFields )
    attrs[ fieldIndex ] = fkValue; // TODO: multiple keys support

  const QgsFeatureRequest request = mRelation.getReferencedFeatureRequest( attrs );
  QgsVectorLayer *reffedLayer = mRelation.referencedLayer();
  QgsFeatureIterator it = reffedLayer->getFeatures( request );
  QgsFeature f;

  if ( it.nextFeature( f ) )
  {
    const FeatureLayerPair pair( f, reffedLayer );

    if ( expectedAttribute == FeaturesModel::FeaturePair )
      return QVariant::fromValue<FeatureLayerPair>( pair );
    if ( expectedAttribute == FeaturesModel::FeatureTitle )
      return featureTitle( pair );
  }

  return {};
}

void RelationReferenceFeaturesModel::setup()
{
  if ( mConfig.isEmpty() || !mProject ) return;

  bool allowNull = mConfig.value( QStringLiteral( "AllowNULL" ) ).toBool();
  if ( allowNull != mAllowNull )
  {
    mAllowNull = allowNull;
    emit allowNullChanged( mAllowNull );
  }

  QgsRelationManager *rManager = mProject->relationManager();
  QString relationId = mConfig.value( QStringLiteral( "Relation" ) ).toString();
  mRelation = rManager->relation( relationId );

  if ( !mRelation.isValid() )
  {
    CoreUtils::log( "RelationReference", "Relation from config is not valid! Editor will not work." );
    return;
  }

  QgsVectorLayer *layer = mRelation.referencedLayer();
  if ( !layer )
    return;

  LayerFeaturesModel::setLayer( layer );

  populate();
}

void RelationReferenceFeaturesModel::setupFeatureRequest( QgsFeatureRequest &request )
{
  // We only need to fetch fields used in the layer's display expression and those used as relation keys
  const QgsVectorLayer *layer = mRelation.referencedLayer();
  const QgsExpression expr( layer->displayExpression() );
  QSet<int> attrs = expr.referencedAttributeIndexes( layer->fields() );
  const QgsAttributeList reffedFields = mRelation.referencedFields();
  for ( const int fieldIndex : reffedFields )
    attrs.insert( fieldIndex );
  mAttributeList = qgis::setToList( attrs );

  LayerFeaturesModel::setupFeatureRequest( request );
}

QVariantMap RelationReferenceFeaturesModel::config() const
{
  return mConfig;
}

QgsProject *RelationReferenceFeaturesModel::project() const
{
  return mProject;
}

bool RelationReferenceFeaturesModel::allowNull() const
{
  return mAllowNull;
}

void RelationReferenceFeaturesModel::setConfig( QVariantMap config )
{
  if ( mConfig != config )
  {
    mConfig = config;
    emit configChanged( mConfig );

    setup();
  }
}

void RelationReferenceFeaturesModel::setProject( QgsProject *project )
{
  if ( mProject != project )
  {
    mProject = project;
    emit projectChanged( mProject );

    setup();
  }
}
