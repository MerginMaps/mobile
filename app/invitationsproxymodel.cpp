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

InvitationsModel *InvitationsProxyModel::invitationsSourceModel() const
{
  return mModel;
}

bool InvitationsProxyModel::filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const
{
  // filter out expired invitations
  QModelIndex sourceIndex = sourceModel()->index( sourceRow, 0, sourceParent );
  QDateTime expiration = sourceModel()->data( sourceIndex, Qt::StatusTipRole ).toDateTime();

  if ( !expiration.isValid() )
  {
    return true;
  }

  return expiration > QDateTime::currentDateTimeUtc();
}

void InvitationsProxyModel::setInvitationsSourceModel( InvitationsModel *sourceModel )
{
  if ( mModel == sourceModel )
    return;

  mModel = sourceModel;
  setSourceModel( mModel );
  emit invitationsSourceModelChanged();
}
