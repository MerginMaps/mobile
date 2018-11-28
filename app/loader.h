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


#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include "qgsproject.h"
#include "qgsquickutils.h"

class Loader: public QObject {
    Q_OBJECT
    Q_PROPERTY(QgsProject* project READ project NOTIFY projectChanged) // never changes

public:
    explicit Loader(QObject* parent = nullptr );

    QgsProject *project();

    Q_INVOKABLE void load(const QString& filePath);
    Q_INVOKABLE void zoomToProject(QgsQuickMapSettings *mapSettings);
    Q_INVOKABLE QStringList mapTip(QgsQuickFeatureLayerPair pair);

signals:
    void projectChanged();
    void projectReloaded();

private:
    QgsProject mProject;
};

#endif // LOADER_H
