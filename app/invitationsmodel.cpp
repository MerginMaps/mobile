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

  if ( mFetchFromServer )
  {
    QObject::connect( mApi, &MerginApi::listInvitationsFinished, this, &InvitationsModel::onListInvitationsFinished );
    listInvitations();
  }
  else
  {
    setModelIsLoading( true );
    onListInvitationsFinished( mApi->userInfo()->invitations() );
  }

  emit modelInitialized();
}

void InvitationsModel::listInvitations()
{
  mApi->listInvitations();
  setModelIsLoading( true );
}

void InvitationsModel::onListInvitationsFinished( const QList<MerginInvitation> &invitations )
{
  clear();

  beginResetModel();

  for ( const MerginInvitation &invite : invitations )
  {
    QStandardItem *item = new QStandardItem( invite.workspace );
    item->setData( invite.role, Qt::ToolTipRole );
    item->setData( invite.uuid, Qt::WhatsThisRole );
    item->setData( invite.expiration, Qt::StatusTipRole );
    item->setData( invite.workspaceId, Qt::BackgroundRole );
    appendRow( item );
  }

  endResetModel();

  setModelIsLoading( false );
}

void InvitationsModel::processInvitation( const QString &uuid, bool accept )
{
  if ( !mApi )
  {
    return;
  }

  setModelIsLoading( true );
  mApi->processInvitation( uuid, accept );
}

MerginApi *InvitationsModel::merginApi() const
{
  return mApi;
}

void InvitationsModel::setMerginApi( MerginApi *merginApi )
{
  if ( !merginApi || mApi == merginApi )
    return;

  if ( mApi )
  {
    disconnect( mApi );
  }

  mApi = merginApi;

  if ( mApi )
  {
    connect( mApi, &MerginApi::processInvitationFinished, this, &InvitationsModel::listInvitations );
  }

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

bool InvitationsModel::fetchFromServer() const
{
  return mFetchFromServer;
}

void InvitationsModel::setFetchFromServer( bool fetch )
{
  mFetchFromServer = fetch;
  emit fetchFromServerChanged( mFetchFromServer );
}
