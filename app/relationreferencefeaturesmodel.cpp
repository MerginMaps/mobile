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
  : FeaturesModel( parent )
{
}

QVariant RelationReferenceFeaturesModel::foreignKeyFromAttribute( FeaturesModel::ModelRoles fromAttribute, const QVariant &attributeValue )
{
  if ( mPrimaryKeyField.isEmpty() )
    return QVariant();

  for ( int i = 0; i < FeaturesModel::rowCount(); ++i )
  {
    if ( FeaturesModel::data( index( i, 0 ), fromAttribute ) == attributeValue )
    {
      QgsFeature feature = FeaturesModel::data( index( i, 0 ), FeaturesModel::Feature ).value<QgsFeature>();
      return feature.attribute( mPrimaryKeyField );
    }
  }

  return QVariant();
}

QVariant RelationReferenceFeaturesModel::attributeFromForeignKey( const QVariant &fkValue, FeaturesModel::ModelRoles expectedAttribute )
{
  if ( mPrimaryKeyField.isEmpty() )
    return QVariant();

  for ( int i = 0; i < FeaturesModel::rowCount(); ++i )
  {
    QgsFeature feature = FeaturesModel::data( index( i, 0 ), FeaturesModel::Feature ).value<QgsFeature>();
    if ( feature.attribute( mPrimaryKeyField ) == fkValue )
    {
      return FeaturesModel::data( index( i, 0 ), expectedAttribute );
    }
  }

  return QVariant();
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
  QgsRelation relation = rManager->relation( relationId );

  if ( !relation.isValid() )
  {
    CoreUtils::log( "RelationReference", "Relation from config is not valid! Editor will not work." );
    return;
  }

  mPrimaryKeyField = relation.fieldPairs().at( 0 ).second;

  QString layerId = mConfig.value( QStringLiteral( "ReferencedLayerId" ) ).toString();
  QgsVectorLayer *layer = mProject->mapLayer<QgsVectorLayer *>( layerId );

  if ( !layer ) return;

  FeaturesModel::setLayer( layer );

  populate();
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
