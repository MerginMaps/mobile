/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "staticfeaturesmodel.h"
#include "inputmapsettings.h"
#include "inpututils.h"


StaticFeaturesModel::StaticFeaturesModel( QObject *parent )
  : FeaturesModel( parent )
{
}

StaticFeaturesModel::~StaticFeaturesModel() = default;

void StaticFeaturesModel::populate( const FeatureLayerPairs &pairs )
{
  beginResetModel();
  mFeatures.clear();
  mFeatures.append( pairs );
  endResetModel();
  emit countChanged( rowCount() );
}

void StaticFeaturesModel::append( const FeatureLayerPair &pair )
{
  for ( const FeatureLayerPair &feature : std::as_const( mFeatures ) )
  {
    if ( feature == pair )
      return;
  }

  beginInsertRows( QModelIndex(), rowCount(), rowCount() );
  mFeatures.append( pair );
  endInsertRows();
  emit countChanged( rowCount() );
}

void StaticFeaturesModel::remove( const FeatureLayerPair &pair )
{
  beginRemoveRows( QModelIndex(), rowCount(), rowCount() );
  const bool removed = mFeatures.removeOne( pair );
  endRemoveRows();
  if ( removed )
    emit countChanged( rowCount() );
}

QgsGeometry StaticFeaturesModel::collectGeometries( InputMapSettings *targetSettings ) const
{
  QVector<QgsGeometry> geoms;

  for ( const FeatureLayerPair &pair : std::as_const( mFeatures ) )
  {
    if ( !pair.isValid() )
      continue;

    const QgsGeometry geom = InputUtils::transformGeometry( pair.feature().geometry(),
                             pair.layer()->crs(),
                             targetSettings->destinationCrs(),
                             targetSettings->transformContext() );
    geoms.append( geom );
  }

  return QgsGeometry::collectGeometry( geoms );
}

FeatureLayerPairs StaticFeaturesModel::features() const
{
  return mFeatures;
}
