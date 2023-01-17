/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "invitationsmodel.h"

InvitationsModel::InvitationsModel( QObject *parent )
  : QStandardItemModel( parent )
{
  connect( this, &InvitationsModel::merginApiChanged, this, &InvitationsModel::initializeModel );
}

InvitationsModel::~InvitationsModel() = default;

void InvitationsModel::initializeModel()
{
  if ( !mApi )
  {
    return;
  }

  QObject::connect( mApi, &MerginApi::listInvitationsFinished, this, &InvitationsModel::onListInvitationsFinished );
  listInvitations();

  emit modelInitialized();
}

void InvitationsModel::listInvitations()
{
  mApi->listInvitations();
  setModelIsLoading( true );
}

void InvitationsModel::onListInvitationsFinished( const QList<MerginInvitation> &invitations )
{
  beginResetModel();

  clear();

  for ( const MerginInvitation &invite : invitations )
  {
    QStandardItem *item = new QStandardItem( invite.workspace );
    item->setData( invite.role, Qt::ToolTipRole );
    item->setData( invite.uuid, Qt::WhatsThisRole );
    appendRow( item );
  }

  endResetModel();

  setModelIsLoading( false );
}

MerginApi *InvitationsModel::merginApi() const
{
  return mApi;
}

void InvitationsModel::setMerginApi( MerginApi *merginApi )
{
  if ( !merginApi || mApi == merginApi )
    return;

  mApi = merginApi;
  emit merginApiChanged( mApi );
}

bool InvitationsModel::isLoading() const
{
  return mModelIsLoading;
}

void InvitationsModel::setModelIsLoading( bool state )
{
  mModelIsLoading = state;
  emit isLoadingChanged( mModelIsLoading );
}
