/***************************************************************************
  mapthemesmodel.cpp
  --------------------------------------
  Date                 : Nov 2018
  Copyright            : (C) 2018 by Viktor Sklencar
  Email                : vsklencar at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mapthemesmodel.h"

#include "qgsmapthemecollection.h"

MapThemesModel::MapThemesModel( QObject *parent )
  : QStandardItemModel( parent )
{
  connect( this, &MapThemesModel::qgsProjectChanged, this, &MapThemesModel::setupModel );
}

MapThemesModel::~MapThemesModel() = default;

void MapThemesModel::setupModel()
{
  beginResetModel();

  clear();

  if ( mQgsProject )
  {
    QgsMapThemeCollection *collection = mQgsProject->mapThemeCollection();

    if ( collection )
    {
      QStringList themes = collection->mapThemes();

      for ( const QString &name : themes )
      {
        QStandardItem *item = new QStandardItem( name );
        appendRow( item );
      }
    }
  }

  endResetModel();
}

QgsProject *MapThemesModel::qgsProject() const
{
  return mQgsProject;
}

void MapThemesModel::setQgsProject( QgsProject *newQgsProject )
{
  if ( mQgsProject == newQgsProject )
    return;
  mQgsProject = newQgsProject;
  emit qgsProjectChanged( mQgsProject );
}

void MapThemesModel::reset()
{
  setQgsProject( nullptr );
}

