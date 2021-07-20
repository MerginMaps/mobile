/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "relationreferencefeaturesmodel.h"

RelationReferenceFeaturesModel::RelationReferenceFeaturesModel( QObject *parent )
  : FeaturesListModel( parent )
{
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

  QString layerId = mConfig.value( QStringLiteral( "ReferencedLayerId" ) ).toString();
  QgsVectorLayer *layer = mProject->mapLayer<QgsVectorLayer *>( layerId );

  if ( !layer ) return;

  // TODO: this could run on a different thread to not block UI
  populateFromLayer( layer );
  emit populated();
}
