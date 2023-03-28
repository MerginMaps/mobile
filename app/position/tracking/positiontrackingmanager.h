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
class PositionKit;

class PositionTrackingManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( QgsGeometry trackedGeometry READ trackedGeometry NOTIFY trackedGeometryChanged )
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )

  public:
    explicit PositionTrackingManager( QObject *parent = nullptr );

    static AbstractTrackingBackend *constructTrackingBackend( PositionKit *positionKit = nullptr );

    QgsVectorLayer *layer() const;
    void setLayer( QgsVectorLayer *newLayer );

    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *newMapSettings );

    QgsGeometry trackedGeometry() const;

    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *newPositionKit );

  public slots:
    void addPoint( GeoPosition position );

  signals:

    void layerChanged( QgsVectorLayer *layer );

    void mapSettingsChanged( InputMapSettings *mapSettings );

    void trackedGeometryChanged( QgsGeometry trackedGeometry );

    void positionKitChanged( PositionKit *positionKit );

  private:
    void setup();

    std::unique_ptr<AbstractTrackingBackend> mTrackingBackend; // owned

    QgsVectorLayer *mLayer = nullptr; // not owned
    InputMapSettings *mMapSettings = nullptr; // not owned
    PositionKit *mPositionKit = nullptr; // not owned

    QgsGeometry mTrackedGeometry;
};

#endif // POSITIONTRACKINGMANAGER_H
