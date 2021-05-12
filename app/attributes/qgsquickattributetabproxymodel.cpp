/***************************************************************************
  qgsquickattributetabproxymodel.cpp
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

#include "qgsquickattributetabproxymodel.h"
#include "qgsquickattributetabmodel.h"
#include "qgsquickattributecontroller.h"

QgsQuickAttributeTabProxyModel::QgsQuickAttributeTabProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent )
{
}

QgsQuickAttributeTabProxyModel::~QgsQuickAttributeTabProxyModel() = default;

bool QgsQuickAttributeTabProxyModel::QgsQuickAttributeTabProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
  if ( !sourceModel() )
    return false;
  QModelIndex sourceIndex = sourceModel()->index( sourceRow, 0, sourceParent );
  return sourceModel()->data( sourceIndex, QgsQuickAttributeTabModel::AttributeTabRoles::Visible ).toBool();
}

QgsQuickAttributeFormProxyModel *QgsQuickAttributeTabProxyModel::attributeFormProxyModel( int sourceRow ) const
{
  if ( !sourceModel() )
    return nullptr;

  return qobject_cast<QgsQuickAttributeTabModel *>( sourceModel() )->attributeFormProxyModel( sourceRow );
}
