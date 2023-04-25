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
#include "qgsfeature.h"
#include "qgscoordinatereferencesystem.h"

class QgsProject;
class PositionKit;
class QgsVectorLayer;
class InputMapSettings;
class VariablesManager;

class PositionTrackingManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString layerId READ layerId NOTIFY layerIdChanged )
    Q_PROPERTY( QgsGeometry trackedGeometry READ trackedGeometry NOTIFY trackedGeometryChanged )
    Q_PROPERTY( QDateTime startTime READ startTime NOTIFY startTimeChanged )

    // properties to be set from QML
    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( VariablesManager *variablesManager READ variablesManager WRITE setVariablesManager NOTIFY variablesManagerChanged )
    Q_PROPERTY( AbstractTrackingBackend *trackingBackend READ trackingBackend WRITE setTrackingBackend NOTIFY trackingBackendChanged )

  public:
    explicit PositionTrackingManager( QObject *parent = nullptr );

    /**
     * Factory method to construct new TrackingBackend based on a device type
     */
    Q_INVOKABLE static AbstractTrackingBackend *constructTrackingBackend( QgsProject *project, PositionKit *positionKit = nullptr );

    //! Returns the id of the layer used for tracking
    QString layerId() const;

    //! Returns the current tracked geometry
    QgsGeometry trackedGeometry() const;

    //! Gets and sets the tracking backend
    //! Setter method moves ownership of the backend to this
    AbstractTrackingBackend *trackingBackend() const;
    void setTrackingBackend( AbstractTrackingBackend *newTrackingBackend );

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

    VariablesManager *variablesManager() const;
    void setVariablesManager( VariablesManager *newVariablesManager );

    //! Should be called when the same project is reloaded (e.g. there was a change in QGIS project file)
    Q_INVOKABLE void projectReloaded();

    //! Returns CRS of the tracked geometry
    Q_INVOKABLE QgsCoordinateReferenceSystem crs() const;

    QDateTime startTime() const;

  public slots:
    void addPoint( const GeoPosition &position );

    void storeTrackedPath();

  signals:

    void layerIdChanged( QString layerId );

    void trackedGeometryChanged( QgsGeometry trackedGeometry );

    void trackingBackendChanged( AbstractTrackingBackend *trackingBackend );

    void qgsProjectChanged( QgsProject *qgsProject );

    void variablesManagerChanged( VariablesManager *variablesManager );

    void startTimeChanged( QDateTime startTime );

  private:
    void setLayerId( QString newLayerId );
    void setup();

    void cacheTrackedGeometry();
    void clearCache();

    std::unique_ptr<AbstractTrackingBackend> mTrackingBackend; // owned

    QString mLayerId;
    QgsProject *mQgsProject = nullptr; // not owned
    VariablesManager *mVariablesManager = nullptr; // not owned

    QgsGeometry mTrackedGeometry;

    QDateTime mTrackingStartTime;
    QgsFeature mTrackedFeature;
};

#endif // POSITIONTRACKINGMANAGER_H
