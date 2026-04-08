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


UniqueValuesFilterModel::UniqueValuesFilterModel( QObject *parent )
  : QAbstractListModel( parent )
{
  // <TODO> remove me once this class is used! :)
  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vectorLayer )
    {
      setLayer( vectorLayer );
      setFieldName( "fid" );
      break;
    }
  }
  // </TODO>

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

QgsVectorLayer *UniqueValuesFilterModel::layer() const
{
  return mLayer;
}

void UniqueValuesFilterModel::setLayer( QgsVectorLayer *layer )
{
  if ( mLayer == layer )
    return;

  mLayer = layer;
  emit layerChanged();
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
  if ( !mLayer || mFieldName.isEmpty() )
    return;

  int fieldIndex = mLayer->fields().lookupField( mFieldName );
  if ( fieldIndex < 0 )
  {
    CoreUtils::log( QStringLiteral( "Filtering" ), QStringLiteral( "Error, field %1 could not be found, dropdown filter won't work." ).arg( mFieldName ) );
    return;
  }

  if ( mItems.size() > 0 ) return;

  if ( mResultWatcher.isRunning() ) return;

  QgsVectorLayer *layerClone = mLayer->clone();

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
