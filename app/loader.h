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
#include "qgsquickpositionkit.h"

class Loader: public QObject {
    Q_OBJECT
    Q_PROPERTY(QgsProject* project READ project NOTIFY projectChanged) // never changes
    Q_PROPERTY(QgsQuickPositionKit* positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged)
    Q_PROPERTY(bool recording READ isRecording WRITE setRecording NOTIFY recordingChanged)

public:
    explicit Loader(QObject* parent = nullptr );

    QgsProject *project();

    QgsQuickPositionKit *positionKit() const { return mPositionKit; }
    void setPositionKit( QgsQuickPositionKit *kit );

    bool isRecording() const { return mRecording; }
    void setRecording( bool isRecording );

    Q_INVOKABLE bool load(const QString& filePath);
    Q_INVOKABLE void zoomToProject(QgsQuickMapSettings *mapSettings);
    Q_INVOKABLE QStringList mapTip(QgsQuickFeatureLayerPair pair);

signals:
    void projectChanged();
    void projectReloaded();

    void positionKitChanged();
    void recordingChanged();

public slots:
     void appStateChanged(Qt::ApplicationState state);
private:
    QgsProject mProject;
    QgsQuickPositionKit *mPositionKit = nullptr;
    bool mRecording = false;
};

#endif // LOADER_H
