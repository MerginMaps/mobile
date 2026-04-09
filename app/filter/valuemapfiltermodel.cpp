/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "valuemapfiltermodel.h"

ValueMapFilterModel::ValueMapFilterModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

int ValueMapFilterModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mFilteredItems.size();
}

QVariant ValueMapFilterModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() || index.row() >= mFilteredItems.size() )
    return {};

  const Item &item = mFilteredItems.at( index.row() );

  switch ( role )
  {
    case Qt::DisplayRole:
    case TextRole:
      return item.text;
    case ValueRole:
      return item.value;
    default:
      return {};
  }
}

QHash<int, QByteArray> ValueMapFilterModel::roleNames() const
{
  return
  {
    { TextRole, "text" },
    { ValueRole, "value" },
  };
}

QVariantMap ValueMapFilterModel::config() const
{
  return mConfig;
}

void ValueMapFilterModel::setConfig( const QVariantMap &config )
{
  if ( mConfig == config )
    return;

  mConfig = config;
  emit configChanged();

  populate();
}

QString ValueMapFilterModel::searchText() const
{
  return mSearchText;
}

void ValueMapFilterModel::setSearchText( const QString &searchText )
{
  if ( mSearchText == searchText )
    return;

  mSearchText = searchText;
  emit searchTextChanged();

  applyFilter();
}

void ValueMapFilterModel::populate()
{
  mItems.clear();

  const QVariantList mapList = mConfig.value( QStringLiteral( "map" ) ).toList();
  mItems.reserve( mapList.size() );

  for ( const QVariant &entry : mapList )
  {
    const QVariantMap entryMap = entry.toMap();
    if ( entryMap.isEmpty() )
      continue;

    // Each entry is a single-key map: {"Display Text": "stored_value"}
    Item item;
    item.text = entryMap.constBegin().key();
    item.value = entryMap.constBegin().value().toString();
    mItems.append( item );
  }

  applyFilter();
}

void ValueMapFilterModel::applyFilter()
{
  beginResetModel();
  mFilteredItems.clear();

  for ( const Item &item : std::as_const( mItems ) )
  {
    if ( mSearchText.isEmpty() || item.text.contains( mSearchText, Qt::CaseInsensitive ) )
    {
      mFilteredItems.append( item );
    }
  }

  endResetModel();
}
