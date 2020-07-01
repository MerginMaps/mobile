/***************************************************************************
  layerfeaturesmodel.cpp
  --------------------------------------
  Date                 : July 2020
  Copyright            : (C) 2020 by Tomas Mizera
  Email                : tomas.mizera2 at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "layerfeaturesmodel.h"
#include <QDebug>

LayerFeaturesModel::LayerFeaturesModel( LayersModel &lm, Loader &loader, QObject *parent )
  : QAbstractListModel( parent ),
    mLayersModel( lm ),
    mLoader( loader )
{
}

int LayerFeaturesModel::rowCount( const QModelIndex &parent ) const
{
  // For list models only the root node (an invalid parent) should return the list's size. For all
  // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
  if ( parent.isValid() )
    return 0;

  return mFeatures.count();
}

QVariant LayerFeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFeatures.count() )
    return QVariant();

  if ( role < roleNames::featureTitle || role > roleNames::description )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  QgsQuickFeatureLayerPair feat = mFeatures.at( index.row() );

  switch ( role )
  {
    case featureTitle:
      return mLoader.featureTitle( feat );
    case description:
      return QVariant( QString( "description" ) );
  }

  return QVariant();
}

void LayerFeaturesModel::reloadDataFromLayerName( const QString &layerName )
{
  Q_UNUSED( layerName );

  // We mock layerName because it is not yet implemented
  QgsMapLayer *mockedLayer = mLayersModel.activeLayer();

  if ( mockedLayer->type() == QgsMapLayerType::VectorLayer )
    this->reloadDataFromLayer( qobject_cast<QgsVectorLayer *>( mockedLayer ) );
}

void LayerFeaturesModel::reloadDataFromLayer( QgsVectorLayer *layer )
{
  beginResetModel();
  mFeatures.clear();

  if ( layer )
  {
    QgsFeatureRequest req;

    req.setLimit( FEATURES_LIMIT );

    QgsFeatureIterator it = layer->getFeatures( req );
    QgsFeature f;

    while ( it.nextFeature( f ) )
    {
      mFeatures << QgsQuickFeatureLayerPair( f, layer );
    }
  }

  endResetModel();
}

QHash<int, QByteArray> LayerFeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[featureTitle] = "featureTitle";
  roleNames[description] = "description";
  return roleNames;
}

bool LayerFeaturesModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  // Mocked method - for future when attributes will be editable (it changes data)
  Q_UNUSED( index );
  Q_UNUSED( value );
  Q_UNUSED( role );
  return false;
}

Qt::ItemFlags LayerFeaturesModel::flags( const QModelIndex &index ) const
{
  // Mocked method - for future when attributes will be editable (it checks if data is editable)
  if ( !index.isValid() )
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable;
}
