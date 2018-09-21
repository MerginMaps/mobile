/***************************************************************************
  qgsquicklayertreemodel.cpp
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layersmodel.h"

#include <qgslayertreemodel.h>
#include <qgslayertreenode.h>
#include <qgslayertree.h>
#include <qgsvectorlayer.h>
#include <qgslayertreemodellegendnode.h>
#include <qgsproject.h>
#include "qgsvectorlayer.h"
#include <qgsmaplayer.h>

#include <QString>

LayersModel::LayersModel(QgsProject* project, QObject* parent )
  : QAbstractListModel( parent )
  , mProject(project)
{
    reloadLayers();
}

LayersModel::~LayersModel() {
}


void LayersModel::reloadLayers() {
    QgsLayerTreeGroup* root = mProject->layerTreeRoot();

    // Get list of all visible and valid layers in the project
    QList< QgsMapLayer* > allLayers;
    foreach (QgsLayerTreeLayer* nodeLayer, root->findLayers())
    {
      if (nodeLayer->isVisible()) {
       QgsMapLayer* layer = nodeLayer->layer();
       if (layer->isValid()) {
        allLayers << layer;
        qDebug() << "Found layer: " << layer->name();
       }
      }
    }
    if (mLayers != allLayers) {
        beginResetModel();
        mLayers = allLayers;
        endResetModel();

        emit layersChanged();
    }
}

QVariant LayersModel::data( const QModelIndex& index, int role ) const
{
  int row = index.row();
  if (row < 0 || row > mLayers.count())
    return QVariant("");

  if (row == 0)
  {
    switch ( role )
    {
      case Name:
        return "(none)";
      case isVector:
        return true;
      case isReadOnly:
        return false;
      case VectorLayer:
        return QVariant::fromValue<QgsVectorLayer*>( nullptr );
    }
    return QVariant();
  }

  // TODO should be moved to index() ?
  QgsMapLayer* layer = mLayers.at(row-1);

  switch ( role )
  {
    case Name:
    {
      return layer->name();
    }
    case isVector:
    {
      return layer->type() == QgsMapLayer::VectorLayer;
    }
    case isReadOnly:
    {
       return layer->readOnly();
    }
    case VectorLayer:
    {
      QgsVectorLayer* vectorLayer = qobject_cast<QgsVectorLayer*>( layer );
      if (vectorLayer) {
        return QVariant::fromValue<QgsVectorLayer*>( vectorLayer );
      }
    }
  }

  return QVariant();
}

QHash<int, QByteArray> LayersModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  roleNames[isVector] = "isVector";
  roleNames[isReadOnly] = "isReadOnly";
  roleNames[VectorLayer] = "vectorLayer";
  return roleNames;
}

QModelIndex LayersModel::index( int row ) const {
    return createIndex(row, 0, nullptr);
}

int LayersModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return mLayers.count() + 1;
}

QList<QgsMapLayer*> LayersModel::layers() const {
    return mLayers;
}
