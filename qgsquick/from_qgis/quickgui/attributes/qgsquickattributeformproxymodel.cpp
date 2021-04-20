/***************************************************************************
 qgsquickattributeformproxymodel.cpp
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

#include "qgsquickattributeformproxymodel.h"
#include "qgsquickattributeformmodel.h"
#include "qgsquickattributecontroller.h"

QgsQuickAttributeFormProxyModel::QgsQuickAttributeFormProxyModel( QObject *parent )
  : QSortFilterProxyModel( parent )
{
}

QgsQuickAttributeFormProxyModel::~QgsQuickAttributeFormProxyModel() = default;

bool QgsQuickAttributeFormProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
  QModelIndex sourceIndex = sourceModel()->index( sourceRow, 0, sourceParent );
  return sourceModel()->data( sourceIndex, QgsQuickAttributeFormModel::AttributeFormRoles::Visible ).toBool();
}
