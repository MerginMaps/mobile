/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "workspacesmodel.h"

WorkspacesModel::WorkspacesModel( QObject *parent )
  : QStandardItemModel( parent )
{
  connect( this, &WorkspacesModel::merginApiChanged, this, &WorkspacesModel::initializeModel );
}

WorkspacesModel::~WorkspacesModel() = default;

void WorkspacesModel::initializeModel()
{
  if ( !mApi )
  {
    return;
  }

  QObject::connect( mApi, &MerginApi::listWorkspacesFinished, this, &WorkspacesModel::onListWorkspacesFinished );
  listWorkspaces();

  emit modelInitialized();
}

void WorkspacesModel::listWorkspaces()
{
  mApi->listWorkspaces();
  setModelIsLoading( true );
}

void WorkspacesModel::onListWorkspacesFinished( const QMap<int, QString> &workspaces )
{
  beginResetModel();

  clear();

  QMap<int, QString>::const_iterator it = workspaces.constBegin();
  while ( it != workspaces.constEnd() )
  {
    QStandardItem *item = new QStandardItem( it.value() );
    item->setData( it.value(), Qt::DisplayRole );
    item->setData( it.key(), Qt::WhatsThisRole );
    appendRow( item );
    ++it;
  }

  endResetModel();

  setModelIsLoading( false );
}

MerginApi *WorkspacesModel::merginApi() const
{
  return mApi;
}

void WorkspacesModel::setMerginApi( MerginApi *merginApi )
{
  if ( !merginApi || mApi == merginApi )
    return;

  mApi = merginApi;
  emit merginApiChanged( mApi );
}

bool WorkspacesModel::isLoading() const
{
  return mModelIsLoading;
}

void WorkspacesModel::setModelIsLoading( bool state )
{
  mModelIsLoading = state;
  emit isLoadingChanged( mModelIsLoading );
}
