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
  rebuild();
}

WorkspacesModel::~WorkspacesModel() = default;

void WorkspacesModel::rebuild()
{
  beginResetModel();

  clear();

  if ( mApi )
  {
    QStringList workspaces = mApi->userInfo()->workspaces();

    for ( const QString &w : workspaces )
    {
      QStandardItem *item = new QStandardItem( w );
      appendRow( item );
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
