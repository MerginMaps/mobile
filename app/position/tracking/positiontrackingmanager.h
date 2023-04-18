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
class QgsProject;

class PositionTrackingManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QgsVectorLayer *layer READ layer NOTIFY layerChanged )
    Q_PROPERTY( QgsGeometry trackedGeometry READ trackedGeometry NOTIFY trackedGeometryChanged )
    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( AbstractTrackingBackend *trackingBackend READ trackingBackend WRITE setTrackingBackend NOTIFY trackingBackendChanged )
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    explicit PositionTrackingManager( QObject *parent = nullptr );

    /**
     * Factory method to construct new TrackingBackend based on a device type
     */
    Q_INVOKABLE static AbstractTrackingBackend *constructTrackingBackend( QgsProject *project, PositionKit *positionKit = nullptr );

    //! Returns the layer used for tracking
    QgsVectorLayer *layer() const;

    //! Returns the current tracked geometry
    QgsGeometry trackedGeometry() const;

    //! Gets and sets the tracking backend
    //! Setter method moves ownership of the backend to this
    AbstractTrackingBackend *trackingBackend() const;
    void setTrackingBackend( AbstractTrackingBackend *newTrackingBackend );

    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *newMapSettings );

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

  public slots:
    void addPoint( GeoPosition position );

  signals:

    void layerChanged( QgsVectorLayer *layer );

    void mapSettingsChanged( InputMapSettings *mapSettings );

    void trackedGeometryChanged( QgsGeometry trackedGeometry );

    void trackingBackendChanged( AbstractTrackingBackend *trackingBackend );

    void qgsProjectChanged( QgsProject *qgsProject );

  private:
    void setLayer( QgsVectorLayer *newLayer );
    void setup();

    std::unique_ptr<AbstractTrackingBackend> mTrackingBackend; // owned

    QgsVectorLayer *mLayer = nullptr; // not owned
    QgsProject *mQgsProject = nullptr; // not owned
    InputMapSettings *mMapSettings = nullptr; // not owned

    QgsGeometry mTrackedGeometry;
};

#endif // POSITIONTRACKINGMANAGER_H
