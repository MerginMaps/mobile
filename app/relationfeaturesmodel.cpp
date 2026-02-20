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
#include "inpututils.h"

#include "qgsexpression.h"
#include "qgsfeatureiterator.h"
#include "qgsproject.h"

RelationFeaturesModel::RelationFeaturesModel( QObject *parent )
  : LayerFeaturesModel( parent )
{
}

RelationFeaturesModel::~RelationFeaturesModel() = default;

QVariant RelationFeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= FeaturesModel::rowCount() )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  if ( role == PhotoPath )
  {
    const FeatureLayerPair pair = LayerFeaturesModel::data( index, FeaturesModel::FeaturePair ).value<FeatureLayerPair>();
    return relationPhotoPath( pair );
  }
  else
    return LayerFeaturesModel::data( index, role );
}

QHash<int, QByteArray> RelationFeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roles = LayerFeaturesModel::roleNames();
  roles[PhotoPath] = QStringLiteral( "PhotoPath" ).toLatin1();

  return roles;
}

void RelationFeaturesModel::setup()
{
  if ( !mRelation.isValid() || !mParentFeatureLayerPair.isValid() )
    return;

  if ( mNmRelation.isValid() )
  {
    // N-M mode: connect both join table and child layer commits, show child layer features
    QObject::connect( mRelation.referencingLayer(), &QgsVectorLayer::afterCommitChanges, this, &RelationFeaturesModel::populate );
    QObject::connect( mNmRelation.referencedLayer(), &QgsVectorLayer::afterCommitChanges, this, &RelationFeaturesModel::populate );
    LayerFeaturesModel::setLayer( mNmRelation.referencedLayer() );
  }
  else
  {
    // 1:N mode: connect child layer commits, show referencing layer features
    QObject::connect( mRelation.referencingLayer(), &QgsVectorLayer::afterCommitChanges, this, &RelationFeaturesModel::populate );
    LayerFeaturesModel::setLayer( mRelation.referencingLayer() );
  }

  populate();
}

void RelationFeaturesModel::populate()
{
  if ( mNmRelation.isValid() )
    fetchChildPkValues();

  LayerFeaturesModel::populate();
}

void RelationFeaturesModel::fetchChildPkValues()
{
  mChildPkValues.clear();

  if ( !mNmRelation.isValid() || !mParentFeatureLayerPair.isValid() )
    return;

  QgsVectorLayer *joinLayer = mRelation.referencingLayer();
  if ( !joinLayer || !joinLayer->isValid() )
    return;

  const QList<QgsRelation::FieldPair> nmFieldPairs = mNmRelation.fieldPairs();
  if ( nmFieldPairs.isEmpty() )
    return;

  // The FK field in the join table that points to the child layer's PK
  const QString joinFkField = nmFieldPairs.first().referencingField();

  // Query the join table for rows related to the current parent feature
  const QgsFeatureRequest joinRequest = mRelation.getRelatedFeaturesRequest( mParentFeatureLayerPair.feature() );
  QgsFeatureIterator it = joinLayer->getFeatures( joinRequest );
  QgsFeature joinFeature;

  while ( it.nextFeature( joinFeature ) )
  {
    const QVariant fkValue = joinFeature.attribute( joinFkField );
    if ( fkValue.isValid() && !fkValue.isNull() )
      mChildPkValues << fkValue;
  }
}

void RelationFeaturesModel::setupFeatureRequest( QgsFeatureRequest &request )
{
  LayerFeaturesModel::setupFeatureRequest( request );

  if ( mNmRelation.isValid() )
  {
    if ( mChildPkValues.isEmpty() )
    {
      // No related child features — return an empty result set
      request.combineFilterExpression( QStringLiteral( "FALSE" ) );
    }
    else
    {
      // Build: "child_pk_field" IN (val1, val2, ...)
      const QString childPkField = mNmRelation.fieldPairs().first().referencedField();
      QStringList quotedValues;
      quotedValues.reserve( mChildPkValues.size() );

      for ( const QVariant &v : std::as_const( mChildPkValues ) )
        quotedValues << QgsExpression::quotedValue( v );

      const QString filterExpr = QStringLiteral( "%1 IN (%2)" )
                                   .arg( QgsExpression::quotedColumnRef( childPkField ),
                                         quotedValues.join( ',' ) );
      request.combineFilterExpression( filterExpr );
    }
  }
  else
  {
    const QgsFeatureRequest e = mRelation.getRelatedFeaturesRequest( mParentFeatureLayerPair.feature() );
    request.combineFilterExpression( e.filterExpression()->operator QString() );
  }
}

void RelationFeaturesModel::setParentFeatureLayerPair( FeatureLayerPair pair )
{
  if ( mParentFeatureLayerPair == pair )
    return;

  mParentFeatureLayerPair = pair;
  emit parentFeatureLayerPairChanged( mParentFeatureLayerPair );

  if ( !InputUtils::isFeatureIdValid( pair.feature().id() ) )
  {
    //
    // Clear the model in case parent feature has invalid id (e.g. is new) and do not populate it
    //

    beginResetModel();
    reset();
    endResetModel();
  }
  else
  {
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

void RelationFeaturesModel::setNmRelation( QgsRelation nmRelation )
{
  if ( mNmRelation.id() != nmRelation.id() )
  {
    mNmRelation = nmRelation;
    emit nmRelationChanged();

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

QgsRelation RelationFeaturesModel::nmRelation() const
{
  return mNmRelation;
}

bool RelationFeaturesModel::isNmRelation() const
{
  return mNmRelation.isValid();
}

QVariant RelationFeaturesModel::relationPhotoPath( const FeatureLayerPair &featurePair ) const
{
  int fieldIndex = photoFieldIndex( featurePair.layer() );
  QgsEditorWidgetSetup setup = featurePair.layer()->editorWidgetSetup( fieldIndex );
  QVariantMap config = setup.config();

  const QgsFeature feature = featurePair.feature();
  QString path = feature.attribute( fieldIndex ).toString();

  QString finalPath = InputUtils::resolvePath( path, homePath(), config, featurePair, QgsProject::instance() );

  return QVariant( finalPath );
}

int RelationFeaturesModel::photoFieldIndex( QgsVectorLayer *layer ) const
{
  if ( !layer )
  {
    return -1;
  }

  QgsFields fields = layer->fields();
  for ( int i = 0; i < fields.size(); i++ )
  {
    // Lets try by widget type
    QgsEditorWidgetSetup setup = layer->editorWidgetSetup( i );
    if ( setup.type() == QStringLiteral( "ExternalResource" ) )
    {
      return i;
    }
  }
  return -1;
}

QString RelationFeaturesModel::homePath() const
{
  return mHomePath;
}

void RelationFeaturesModel::setHomePath( const QString &homePath )
{
  if ( homePath != mHomePath )
  {
    mHomePath = homePath;
    emit homePathChanged();
  }
}
