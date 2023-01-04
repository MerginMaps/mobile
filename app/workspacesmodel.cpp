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
  connect( this, &WorkspacesModel::merginApiChanged, this, &WorkspacesModel::rebuild );
}

WorkspacesModel::~WorkspacesModel() = default;

void WorkspacesModel::rebuild()
{
  beginResetModel();

  clear();

  if ( mApi )
  {
    QMap<int, QString> workspaces = mApi->userInfo()->workspaces();
    QMap<int, QString>::const_iterator it = workspaces.constBegin();
    while ( it != workspaces.constEnd() )
    {
      QStandardItem *item = new QStandardItem( it.value() );
      item->setData( it.key() );
      appendRow( item );
      ++it;
    }
  }

  endResetModel();
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
