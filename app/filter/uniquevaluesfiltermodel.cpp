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
#include <QLocale>


UniqueValuesFilterModel::UniqueValuesFilterModel( QObject *parent ) : QAbstractListModel( parent )
{
  connect( &mResultWatcher, &QFutureWatcher<QVariantList>::finished, this, &UniqueValuesFilterModel::onLoadingFinished );
}

QHash<int, QByteArray> UniqueValuesFilterModel::roleNames() const
{
  QHash<int, QByteArray> roleMap = QAbstractListModel::roleNames();
  roleMap.insert( ValueRole, QStringLiteral( "value" ).toLatin1() );
  return roleMap;
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
    {
      const QVariant &item = mItems.at( index.row() );

      // for NULL values, which are gotten as empty strings, we want to return some meaningful text for users
      if ( item.isNull() )
        return { tr( "No value" ) };

      if ( item.typeId() == QMetaType::QDate )
        return QLocale().toString( item.toDate(), QLocale::ShortFormat );

      if ( item.typeId() == QMetaType::QDateTime )
        return QLocale().toString( item.toDateTime().toLocalTime(), QLocale::ShortFormat );

      return item;
    }
    case ValueRole:
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

void UniqueValuesFilterModel::populate( FilterController *controller )
{
  if ( mLayerId.isEmpty() || mFieldName.isEmpty() || !controller ) return;

  QgsMapLayer *mapLayer = QgsProject::instance()->mapLayer( mLayerId );
  if ( !mapLayer )
  {
    CoreUtils::log( QStringLiteral( "Filtering" ), QStringLiteral( "Could not get layer %1" ).arg( mLayerId ) );
  }
  const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( mapLayer );

  if ( !layer ) return;

  int fieldIndex = layer->fields().lookupField( mFieldName );
  if ( fieldIndex < 0 )
  {
    CoreUtils::log( QStringLiteral( "Filtering" ), QStringLiteral( "Error, field %1 could not be found, dropdown filter won't work." ).arg( mFieldName ) );
    return;
  }

  // model already populated?
  if ( !mItems.empty() ) return;

  if ( mResultWatcher.isRunning() ) return;

  QgsVectorLayer *layerClone = controller->getUnfilteredLayerCopy( mLayerId );
  if ( !layerClone ) return;

  mIsLoading = true;
  emit isLoadingChanged();

  mResultWatcher.setFuture( QtConcurrent::run( &UniqueValuesFilterModel::loadUniqueValues, layerClone, fieldIndex ) );
}

QVariantList UniqueValuesFilterModel::loadUniqueValues( QgsVectorLayer *layer, int fieldIndex )
{
  std::unique_ptr<QgsVectorLayer> l( layer );

  QSet<QVariant> uniqueValues = l->uniqueValues( fieldIndex, 1000000 );

  // both empty string and null value show up in the same way, let's remove one to have only one "No value" option in UI
  const QVariant nullValidQVariant = QVariant( QMetaType( QMetaType::QString ) );
  if ( uniqueValues.contains( QVariant( "" ) ) && uniqueValues.contains( nullValidQVariant ) )
  {
    uniqueValues.remove( QVariant( "" ) );
  }

  QVariantList results;

  results.reserve( uniqueValues.size() );

  for ( const QVariant &v : uniqueValues )
  {
    // Typed null QVariants (e.g. null bool) lose nullness when passed to QML —
    // Qt's bridge converts them to the type's zero-default (false, 0, etc.).
    if ( v.isNull() )
    {
      results.append( QVariant() );
    }
    else
    {
      results.append( v );
    }
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

  endResetModel();
  emit countChanged();

  mIsLoading = false;
  emit isLoadingChanged();
}

bool UniqueValuesFilterModel::isLoading() const
{
  return mIsLoading;
}
