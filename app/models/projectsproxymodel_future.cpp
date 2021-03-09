/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectsproxymodel_future.h"

ProjectsProxyModel_future::ProjectsProxyModel_future( ProjectModelTypes modelType, QObject *parent ) :
  QSortFilterProxyModel( parent ),
  mModelType( modelType )
{

}

bool ProjectsProxyModel_future::filterAcceptsRow( int, const QModelIndex & ) const
{
  // return true if it passes search filter
  return true;
}
