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
  : FeaturesListModel( parent )
{
}

QVariant RelationReferenceFeaturesModel::foreignKeyFromAttribute( FeaturesListModel::modelRoles fromAttribute, const QVariant &attributeValue )
{
  if ( mPrimaryKeyField.isEmpty() )
    return QVariant();

  for ( int i = 0; i < mFeatures.count(); ++i )
  {
    if ( FeaturesListModel::data( index( i, 0 ), fromAttribute ) == attributeValue )
    {
      return mFeatures[i].feature().attribute( mPrimaryKeyField );
    }
  }

  return QVariant();
}

QVariant RelationReferenceFeaturesModel::attributeFromForeignKey( const QVariant &fkValue, FeaturesListModel::modelRoles expectedAttribute )
{
  if ( mPrimaryKeyField.isEmpty() )
    return QVariant();

  for ( int i = 0; i < mFeatures.count(); ++i )
  {
    if ( mFeatures[i].feature().attribute( mPrimaryKeyField ) == fkValue )
    {
      return FeaturesListModel::data( index( i, 0 ), expectedAttribute );
    }
  }

  return QVariant();
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

void RelationReferenceFeaturesModel::setup()
{
  if ( mConfig.empty() || !mProject ) return;

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

  // TODO: this could run on a different thread to not block UI
  populateFromLayer( layer );
  emit populated();
}
