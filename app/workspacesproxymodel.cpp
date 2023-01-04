/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "workspacesproxymodel.h"

WorkspacesProxyModel::WorkspacesProxyModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

QString WorkspacesProxyModel::searchExpression() const
{
  return mSearchExpression;
}

WorkspacesModel *WorkspacesProxyModel::sourceModel() const
{
  return mModel;
}

void WorkspacesProxyModel::setSearchExpression( QString searchExpression )
{
  if ( mSearchExpression == searchExpression )
    return;

  mSearchExpression = searchExpression;
  setFilterFixedString( mSearchExpression );
  emit searchExpressionChanged( mSearchExpression );
}

void WorkspacesProxyModel::setSourceModel( WorkspacesModel *sourceModel )
{
  if ( mModel == sourceModel )
    return;

  mModel = sourceModel;
  QSortFilterProxyModel::setSourceModel( mModel );
}
