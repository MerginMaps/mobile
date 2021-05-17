/***************************************************************************
  attributetabproxymodel.cpp
  --------------------------------------
  Date                 : 20.4.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "attributetabproxymodel.h"
#include "attributetabmodel.h"
#include "attributecontroller.h"

AttributeTabProxyModel::AttributeTabProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent )
{
}

AttributeTabProxyModel::~AttributeTabProxyModel() = default;

bool AttributeTabProxyModel::AttributeTabProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
  if ( !sourceModel() )
    return false;
  QModelIndex sourceIndex = sourceModel()->index( sourceRow, 0, sourceParent );
  return sourceModel()->data( sourceIndex, AttributeTabModel::AttributeTabRoles::Visible ).toBool();
}

AttributeFormProxyModel *AttributeTabProxyModel::attributeFormProxyModel( int sourceRow ) const
{
  if ( !sourceModel() )
    return nullptr;

  return qobject_cast<AttributeTabModel *>( sourceModel() )->attributeFormProxyModel( sourceRow );
}
