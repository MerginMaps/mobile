/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONTRACKINGMANAGER_H
#define POSITIONTRACKINGMANAGER_H

#include <QObject>
#include <qglobal.h>
#include <QQmlEngine>

#include "abstracttrackingbackend.h"

#include "qgsgeometry.h"

class QgsVectorLayer;
class InputMapSettings;

class PositionTrackingManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QgsGeometry trackedGeometry READ trackedGeometry NOTIFY trackedGeometryChanged )

  public:
    explicit PositionTrackingManager( QObject *parent = nullptr );

    QgsVectorLayer *layer() const;
    void setLayer( QgsVectorLayer *newLayer );

    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *newMapSettings );

    QgsGeometry trackedGeometry() const;

  signals:

    void layerChanged( QgsVectorLayer *layer );

    void mapSettingsChanged( InputMapSettings *mapSettings );

    void trackedGeometryChanged( QgsGeometry trackedGeometry );

  private:

    std::unique_ptr<AbstractTrackingBackend> mTrackingBackend; // owned

    QgsVectorLayer *mLayer = nullptr; // not owned
    InputMapSettings *mMapSettings = nullptr; // not owned

    QgsGeometry mTrackedGeometry;
};

#endif // POSITIONTRACKINGMANAGER_H
