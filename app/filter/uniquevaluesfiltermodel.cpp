/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "uniquevaluesfiltermodel.h"
#include "coreutils.h"

#include <qgsproject.h>
#include <qgsvectorlayer.h>
#include <QtConcurrentRun>


UniqueValuesFilterModel::UniqueValuesFilterModel( QObject *parent ) : QAbstractListModel( parent )
{
  connect( &mResultWatcher, &QFutureWatcher<QVariantList>::finished, this, &UniqueValuesFilterModel::onLoadingFinished );
}

int UniqueValuesFilterModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mItems.size();
}

QVariant UniqueValuesFilterModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() || index.row() >= mItems.size() )
    return {};

  switch ( role )
  {
    case Qt::DisplayRole:
      return mItems.at( index.row() );
    default:
      return {};
  }
}

QString UniqueValuesFilterModel::layerId() const
{
  return mLayerId;
}

void UniqueValuesFilterModel::setLayerId( const QString &layerId )
{
  if ( mLayerId == layerId )
    return;

  mLayerId = layerId;
  emit layerIdChanged();
}

QString UniqueValuesFilterModel::fieldName() const
{
  return mFieldName;
}

void UniqueValuesFilterModel::setFieldName( const QString &fieldName )
{
  if ( mFieldName == fieldName )
    return;

  mFieldName = fieldName;
  emit fieldNameChanged();
}

void UniqueValuesFilterModel::populate()
{
  if ( mLayerId.isEmpty() || mFieldName.isEmpty() ) return;

  QgsMapLayer *mapLayer = QgsProject::instance()->mapLayer( mLayerId );
  if ( !mapLayer )
  {
    CoreUtils::log( QStringLiteral("Filtering"), QStringLiteral("Could not get layer %1").arg(mLayerId));
  }
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( mapLayer );

  if ( !layer ) return;

  int fieldIndex = layer->fields().lookupField( mFieldName );
  if ( fieldIndex < 0 )
  {
    CoreUtils::log( QStringLiteral( "Filtering" ), QStringLiteral( "Error, field %1 could not be found, dropdown filter won't work." ).arg( mFieldName ) );
    return;
  }

  // model already populated?
  if ( mItems.size() > 0 ) return;

  if ( mResultWatcher.isRunning() ) return;

  QgsVectorLayer *layerClone = layer->clone();

  mResultWatcher.setFuture( QtConcurrent::run( &UniqueValuesFilterModel::loadUniqueValues, this, layerClone, fieldIndex ) );
}

QVariantList UniqueValuesFilterModel::loadUniqueValues( QgsVectorLayer *layer, int fieldIndex )
{
  std::unique_ptr<QgsVectorLayer> l( layer );

  const QSet<QVariant> uniqueValues = l->uniqueValues( fieldIndex, 1000000 );

  QVariantList results;

  results.reserve( uniqueValues.size() );

  for ( const QVariant &v : uniqueValues )
  {
    results.append( v );
  }

  std::sort( results.begin(), results.end(), []( const QVariant & a, const QVariant & b )
  {
    return a.toString() < b.toString();
  } );

  return results;
}

void UniqueValuesFilterModel::onLoadingFinished()
{
  beginResetModel();

  mItems.clear();
  mItems = mResultWatcher.result();

  // TODO: do we need boolean to indicate if the model is loading?
  // TODO: measure how long it takes to move results from future result to mItems ~ there might be a way to avoid the copy

  endResetModel();
}
