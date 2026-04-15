/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "valuemapfiltermodel.h"

#include <QtConcurrentRun>

ValueMapFilterModel::ValueMapFilterModel( QObject *parent )
  : QAbstractListModel( parent )
{
  connect( &mResultWatcher, &QFutureWatcher<QList<Item>>::finished, this, &ValueMapFilterModel::onLoadingFinished );
}

int ValueMapFilterModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mItems.size();
}

QVariant ValueMapFilterModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() || index.row() >= mItems.size() )
    return {};

  const Item &item = mItems.at( index.row() );

  switch ( role )
  {
    case Qt::DisplayRole:
      return item.description;
    case KeyRole:
      return item.key;
    default:
      return {};
  }
}

QHash<int, QByteArray> ValueMapFilterModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
  roles[KeyRole] = QStringLiteral( "Key" ).toLatin1();

  return roles;
}

QVariantMap ValueMapFilterModel::config() const
{
  return mConfig;
}

bool ValueMapFilterModel::isLoading() const
{
  return mIsLoading;
}

void ValueMapFilterModel::setConfig( const QVariantMap &config )
{
  if ( mConfig == config )
    return;

  mConfig = config;
  emit configChanged();

  if ( mResultWatcher.isRunning() )
  {
    mHasPendingLoad = true;
    return;
  }

  startLoad();
}

void ValueMapFilterModel::startLoad()
{
  mIsLoading = true;
  emit isLoadingChanged();

  mResultWatcher.setFuture( QtConcurrent::run( &ValueMapFilterModel::loadItems, mConfig ) );
}

QList<ValueMapFilterModel::Item> ValueMapFilterModel::loadItems( const QVariantMap &config )
{
  const QVariantList mapList = config.value( QStringLiteral( "map" ) ).toList();

  QList<Item> items;
  items.reserve( mapList.size() );

  for ( const QVariant &entry : mapList )
  {
    const QVariantMap entryMap = entry.toMap();

    if ( entryMap.isEmpty() )
      continue;

    // Each entry is a single-key map: {"Display Text": "stored_value"}
    Item item;
    item.description = entryMap.constBegin().key();
    item.key = entryMap.constBegin().value().toString();

    items.append( item );
  }

  return items;
}

void ValueMapFilterModel::onLoadingFinished()
{
  beginResetModel();

  mItems = mResultWatcher.result();

  endResetModel();
  emit countChanged();

  if ( mHasPendingLoad )
  {
    mHasPendingLoad = false;
    startLoad();
  }
  else
  {
    mIsLoading = false;
    emit isLoadingChanged();
  }
}
