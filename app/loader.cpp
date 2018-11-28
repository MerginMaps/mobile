/***************************************************************************
  app.h
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : peter.petrik@lutraconsulting.co.uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "loader.h"
#include "qgsvectorlayer.h"
#include <QDebug>

Loader::Loader(QObject* parent):QObject(parent)
{}

QgsProject* Loader::project() {
    return &mProject;
}

void Loader::load(const QString& filePath) {
    qDebug() << "Loading " << filePath;
    if (mProject.fileName() != filePath) {
        bool res = mProject.read(filePath);
        Q_ASSERT(res);
        qDebug() << " ******** OK ";

        emit projectReloaded();
    } else {
        qDebug() << " ******** SKIPPED ";
    }
}

void Loader::zoomToProject(QgsQuickMapSettings *mapSettings)
{
    if (!mapSettings) {
        qDebug() << "Cannot zoom to layers extent, mapSettings is not defined";
        return;
    }

    const QVector<QgsMapLayer*> layers = mProject.layers<QgsMapLayer*>();
    QgsRectangle extent;
    for (const QgsMapLayer* layer: layers) {
        QgsRectangle layerExtent = layer->extent();
        extent.combineExtentWith(layerExtent);
    }
    extent.scale(1.05);
    mapSettings->setExtent(extent);
}

QStringList Loader::mapTip(QgsQuickFeatureLayerPair pair)
{
    QgsExpressionContext context( QgsExpressionContextUtils::globalProjectLayerScopes( pair.layer() ) );
    QString mapTip = pair.layer()->mapTipTemplate();

    int LIMIT = 2;
    if (mapTip.isEmpty()) {
        QStringList fields;
        for (QgsField field: pair.layer()->fields()) {
            fields << field.name();
            if (fields.length() == LIMIT) return fields;
        }
    }
    return mapTip.split("\n").mid(0, LIMIT);
}
