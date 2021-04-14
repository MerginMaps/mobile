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

#include <qgslayertreemodel.h>
#include <qgslayertreenode.h>
#include <qgslayertree.h>
#include <qgsvectorlayer.h>
#include <qgslayertreemodellegendnode.h>
#include <qgsproject.h>
#include "qgsmapthemecollection.h"

#include <QString>
#include <QDebug>

MapThemesModel::MapThemesModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

MapThemesModel::~MapThemesModel()
{
}

void MapThemesModel::reloadMapThemes( QgsProject *project )
{
  if ( !project ) return;

  mProject = project;
  QList<QString>allThemes;
  QgsMapThemeCollection *collection = project->mapThemeCollection();
  for ( QString name : collection->mapThemes() )
  {
    allThemes << name;
    qDebug() << "Found map theme: " << name;
  }

  if ( mMapThemes != allThemes )
  {
    beginResetModel();
    mMapThemes = allThemes;
    endResetModel();
  }

  //! Update active theme even though project has same map theme collections - reloadMapThemes funtion is most likely envoked by reloading a project
  updateMapThemeByProject();
  emit mapThemesReloaded();
}

void MapThemesModel::updateMapThemeByProject()
{
  if ( !mProject ) return;

  QgsLayerTree *root = mProject->layerTreeRoot();
  QgsLayerTreeModel model( root );
  QgsMapThemeCollection::MapThemeRecord rec = mProject->mapThemeCollection()->createThemeFromCurrentState( root, &model );

  const auto constMapThemes = mProject->mapThemeCollection()->mapThemes();
  for ( const QString &themeName : constMapThemes )
  {
    if ( rec == mProject->mapThemeCollection()->mapThemeState( themeName ) )
    {
      updateMapTheme( themeName );
      return;
    }
  }

  //! No matching map theme found
  setActiveThemeIndex( -1 );
}

void MapThemesModel::updateMapTheme( const QString name )
{
  int row = rowAccordingName( name, -1 );
  setActiveThemeIndex( row );
}

int MapThemesModel::activeThemeIndex() const
{
  return mActiveThemeIndex;
}

QString MapThemesModel::setActiveThemeIndex( int activeThemeIndex )
{
  mActiveThemeIndex = activeThemeIndex;
  QString name;
  if ( activeThemeIndex >= 0 && activeThemeIndex < mMapThemes.length() )
  {
    name = mMapThemes.at( activeThemeIndex );
    applyTheme( name );
  }
  emit activeThemeIndexChanged();

  return name;
}

QVariant MapThemesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mMapThemes.count() )
    return QVariant();

  switch ( role )
  {
    case Name:
      return mMapThemes.at( index.row() );
  }
  return QVariant();
}

QHash<int, QByteArray> MapThemesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  return roleNames;
}

QModelIndex MapThemesModel::index( int row, int column, const QModelIndex &parent ) const
{
  Q_UNUSED( column )
  Q_UNUSED( parent )
  return createIndex( row, 0, nullptr );
}

int MapThemesModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mMapThemes.count();
}

QList<QString> MapThemesModel::mapThemes() const
{
  return mMapThemes;
}

void MapThemesModel::setMapThemes( const QList<QString> &mapThemes )
{
  if ( mMapThemes == mapThemes )
    return;

  mMapThemes = mapThemes;
  emit mapThemesReloaded();
}

void MapThemesModel::applyTheme( const QString &name )
{
  if ( !mProject ) return;

  QgsLayerTree *root = mProject->layerTreeRoot();
  QgsLayerTreeModel model( root );
  mProject->mapThemeCollection()->applyTheme( name, root, &model );
  emit mapThemeChanged( name );
}

int MapThemesModel::rowAccordingName( QString name, int defaultRow ) const
{
  int index = mMapThemes.indexOf( name );
  if ( index < 0 )
    return defaultRow;
  else
    return index;
}
