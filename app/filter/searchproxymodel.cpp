/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "searchproxymodel.h"

SearchProxyModel::SearchProxyModel( QObject *parent )
  : QSortFilterProxyModel{parent}
{
  setFilterCaseSensitivity( Qt::CaseInsensitive );
}

QString SearchProxyModel::searchString() const
{
  return mSearchString;
}

void SearchProxyModel::setSearchString( const QString &search )
{
  if ( mSearchString == search )
    return;
  mSearchString = search;
  setFilterFixedString( search );
  emit searchStringChanged();
}
