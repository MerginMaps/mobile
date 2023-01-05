/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "invitationsproxymodel.h"

InvitationsProxyModel::InvitationsProxyModel( QObject *parent ) : QSortFilterProxyModel( parent )
{
}

QString InvitationsProxyModel::searchExpression() const
{
  return mSearchExpression;
}

InvitationsModel *InvitationsProxyModel::invitationsSourceModel() const
{
  return mModel;
}

void InvitationsProxyModel::setSearchExpression( QString searchExpression )
{
  if ( mSearchExpression == searchExpression )
    return;

  mSearchExpression = searchExpression;
  setFilterFixedString( mSearchExpression );
  emit searchExpressionChanged( mSearchExpression );
}

void InvitationsProxyModel::setInvitationsSourceModel( InvitationsModel *sourceModel )
{
  if ( mModel == sourceModel )
    return;

  mModel = sourceModel;
  setSourceModel( mModel );
}
