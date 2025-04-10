/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "multieditmanager.h"
#include "mixedattributevalue.h"
#include "staticfeaturesmodel.h"
#include "featurelayerpair.h"
#include "qgsmemoryproviderutils.h"
#include "qgsexpressioncontextutils.h"
#include "qgis.h"


MultiEditManager::MultiEditManager( QObject *parent )
  : QObject( parent )
  , mModel( std::make_unique<StaticFeaturesModel>() )
{
  connect( this, &MultiEditManager::layerChanged, this, &MultiEditManager::createTemporaryLayer );
  connect( mModel.get(), &StaticFeaturesModel::countChanged, this, &MultiEditManager::geometryChanged );
}

void MultiEditManager::initialize( const FeatureLayerPair &pair )
{
  mModel->populate( { pair } );

  if ( mLayer != pair.layer() )
  {
    mLayer = pair.layer();
    emit layerChanged();
  }
}

void MultiEditManager::toggleSelection( const FeatureLayerPair &pair )
{
  if ( !pair.isValid() ||
       ( mLayer && mLayer != pair.layer() ) )
    return;

  if ( mModel->count() == 0 )
  {
    mModel->populate( { pair } );
    return;
  }

  if ( mModel->contains( pair ) )
  {
    mModel->remove( pair );
  }
  else
  {
    mModel->append( pair );
  }
}

bool MultiEditManager::applyEdits()
{
  if ( !mModel || mModel->count() == 0 || !mLayer || !mTempLayer )
    return false;

  QgsFeature oneFeature;
  mTempLayer->getFeatures().nextFeature( oneFeature );

  // toMap() will exclude null values. MixedAttributeValues have been saved as null on the temp layer
  QgsAttributeMap attrs = oneFeature.attributes().toMap();

  if ( attrs.isEmpty() )
    return false;

  // let's create a context once to avoid re-creating it for every feature
  std::unique_ptr<QgsExpressionContext> context = std::make_unique<QgsExpressionContext>( QgsExpressionContextUtils::globalProjectLayerScopes( mLayer ) );
  QgsVectorLayerToolsContext toolsContext;
  toolsContext.setExpressionContext( context.get() );

  mLayer->startEditing();
  mLayer->beginEditCommand( QStringLiteral( "Multi Feature Edit" ) );
  bool success = true;

  const FeatureLayerPairs pairs = mModel->features();
  for ( const FeatureLayerPair &pair : pairs )
  {
    const QgsFeature feature = pair.feature();
    success &= mLayer->changeAttributeValues( feature.id(), attrs, {}, false, &toolsContext );

    if ( !success )
      break;
  }

  if ( success )
  {
    mLayer->endEditCommand();
  }
  else
  {
    mLayer->destroyEditCommand();
  }

  success &= mLayer->commitChanges( true );

  return success;
}

FeatureLayerPair MultiEditManager::editableFeature()
{
  if ( !mModel || mModel->count() == 0 || !mTempLayer )
    return FeatureLayerPair();

  QgsFeature oneFeature;
  mTempLayer->getFeatures().nextFeature( oneFeature );

  const FeatureLayerPairs pairs = mModel->features();

  // Not much to do if it's a single feature pair, return it as is
  if ( mModel->count() == 1 )
    return pairs.first();

  for ( int pairIdx = 0; pairIdx < pairs.count(); ++pairIdx )
  {
    QgsFeature feature = pairs.at( pairIdx ).feature();

    for ( int i = 0; i < feature.attributeCount(); ++i )
    {
      // skip fid and other unique attributes
      if ( mTempLayer->fieldConstraints( i ) & QgsFieldConstraints::Constraint::ConstraintUnique )
        continue;

      if ( pairIdx == 0 )
      {
        // for the first pair, we add all attributes
        oneFeature.setAttribute( i, feature.attribute( i ) );
        continue;
      }

      // for the rest of the pairs, if a field has a different value than our oneFeature, we replace it with a MixedAttributeValue
      if ( oneFeature.attribute( i ) != feature.attribute( i ) )
      {
        oneFeature.setAttribute( i, MixedAttributeValue() );
      }
    }
  }

  return FeatureLayerPair( oneFeature, mTempLayer.get() );
}

void MultiEditManager::createTemporaryLayer()
{
  mTempLayer.reset( QgsMemoryProviderUtils::createMemoryLayer(
                      QStringLiteral( "multi_edit_layer" ),
                      mLayer->fields(),
                      Qgis::WkbType::NoGeometry ) );
  QgsFeature feature( mLayer->fields() );
  const bool added = mTempLayer->dataProvider()->addFeature( feature );

  mTempLayer->setAttributeTableConfig( mLayer->attributeTableConfig() );

  for ( int i = 0; i < mLayer->fields().count(); i++ )
  {
    mTempLayer->setFieldAlias( i, mLayer->attributeAlias( i ) );
    mTempLayer->setFieldConfigurationFlags( i, mLayer->fieldConfigurationFlags( i ) );
    mTempLayer->setEditorWidgetSetup( i, mLayer->editorWidgetSetup( i ) );
    mTempLayer->setConstraintExpression( i, mLayer->constraintExpression( i ), mLayer->constraintDescription( i ) );
    mTempLayer->setDefaultValueDefinition( i, mLayer->defaultValueDefinition( i ) );

    QMap< QgsFieldConstraints::Constraint, QgsFieldConstraints::ConstraintStrength> constraints = mLayer->fieldConstraintsAndStrength( i );
    for ( auto constraintIt = constraints.constBegin() ; constraintIt != constraints.constEnd(); ++constraintIt )
    {
      if ( !( mTempLayer->fieldConstraints( i ) & constraintIt.key() ) && constraintIt.value() != QgsFieldConstraints::ConstraintStrength::ConstraintStrengthNotSet )
        mTempLayer->setFieldConstraint( i, constraintIt.key(), constraintIt.value() );
    }

    if ( mLayer->fields().fieldOrigin( i ) == Qgis::FieldOrigin::Expression )
    {
      mTempLayer->addExpressionField( mLayer->expressionField( i ), mLayer->fields().at( i ) );
    }
  }

  mTempLayer->setEditFormConfig( mLayer->editFormConfig() );
  connect( mTempLayer.get(), &QgsVectorLayer::committedAttributeValuesChanges, this, &MultiEditManager::applyEdits );
}
