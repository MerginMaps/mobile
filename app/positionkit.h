/***************************************************************************
 positionkit.h
  --------------------------------------
  Date                 : Dec. 2017
  Copyright            : (C) 2017 Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONKIT_H
#define POSITIONKIT_H

#include <QObject>
#include <QtPositioning>

#include "qgspoint.h"

#include "qgsquickmapsettings.h"
#include "qgsquickcoordinatetransformer.h"

/**
 * \brief Convenient set of tools to read GPS position and accuracy.
 *
 * Also, if one can use use_simulated_location to specify simulated position.
 * Simulated position source generates random points in circles around the selected
 * point and radius. Real GPS position is not used in this mode.
 *
 * \note QML Type: PositionKit
 */
class PositionKit : public QObject
{
    Q_OBJECT

    /**
     * GPS position in WGS84 coords.
     *
     * This is a readonly property.
     */
    Q_PROPERTY( QgsPoint position READ position NOTIFY positionChanged )

    /**
     * GPS position in map coords.
     *
     * This is a readonly property.
     */
    Q_PROPERTY( QgsPoint projectedPosition READ projectedPosition NOTIFY projectedPositionChanged )

    /**
     * GPS position in device coords (pixels).
     *
     * This is a readonly property.
     */
    Q_PROPERTY( QPointF screenPosition READ screenPosition NOTIFY screenPositionChanged )

    /**
     * GPS position is available (position property is a valid number).
     *
     * This is a readonly property.
     */
    Q_PROPERTY( bool hasPosition READ hasPosition NOTIFY hasPositionChanged )

    /**
     * GPS horizontal accuracy in accuracyUnits, -1 if not available.
     *
     * This is a readonly property.
     */
    Q_PROPERTY( double accuracy READ accuracy NOTIFY accuracyChanged )

    /**
     * GPS vertical accuracy in accuracyUnits, -1 if not available.
     *
     * This is a readonly property.
     */
    Q_PROPERTY( double verticalAccuracy READ verticalAccuracy NOTIFY verticalAccuracyChanged )

    /**
     * Screen horizontal accuracy, 2 if not available or resolution is too small.
     *
     * This is a readonly property.
     */
    Q_PROPERTY( double screenAccuracy READ screenAccuracy NOTIFY screenAccuracyChanged )

    /**
     * GPS direction, bearing in degrees clockwise from north to direction of travel. -1 if not available
     *
     * This is a readonly property.
     */
    Q_PROPERTY( double direction READ direction NOTIFY directionChanged )

    /**
     * GPS calculated ground speed in km/h
     *
     * This is a readonly property.
     */
    Q_PROPERTY( double speed READ speed NOTIFY speedChanged )

    /**
     * Timestamp of last acquired GPS position
     *
     * This is a readonly property.
     */
    Q_PROPERTY( QDateTime lastGPSRead READ lastGPSRead NOTIFY lastGPSReadChanged )

    /**
     * GPS position and accuracy is simulated (not real from GPS sensor). Default FALSE (use real GPS)
     *
     * This is a readonly property. To change to simulated position, see PositionKit::simulatePositionLongLatRad
     */
    Q_PROPERTY( bool isSimulated READ isSimulated NOTIFY isSimulatedChanged )

    /**
     * Associated map settings. Should be initialized before the first use from mapcanvas map settings.
     *
     * This is a readonly property.
     */
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

    /**
     * Uses of GPS and simulated position and sets its parameters
     *
     * Vector containing longitude, latitude and radius (meters) of simulated position e.g. [-97.36, 36.93, 2]
     * If empty vector is assigned, GPS source will be used.
     *
     * From QML context, also functions useSimulatedLocation() or useGpsLocation() could be used instead
     */
    Q_PROPERTY( QVector<double> simulatePositionLongLatRad READ simulatePositionLongLatRad WRITE setSimulatePositionLongLatRad NOTIFY simulatePositionLongLatRadChanged )

    /**
     * Internal source of GPS location data.
     * Allows start/stop of its services or access properties.
     */
    Q_PROPERTY( QGeoPositionInfoSource *source READ source NOTIFY sourceChanged )

    /**
     * Internal source of GPS satellite data.
     * Allows start/stop of its services or access properties.
     * Source is not available in simulated mode.
     */
    Q_PROPERTY( QGeoSatelliteInfoSource *satelliteSource READ satelliteSource NOTIFY satelliteSourceChanged )

    // How many satellites is device using.
    Q_PROPERTY( int usedSatellitesCount READ usedSatellitesCount NOTIFY usedSatellitesCountChanged )

    // How many satellites are in view.
    Q_PROPERTY( int satellitesInViewCount READ satellitesInViewCount NOTIFY satellitesInViewCountChanged )

  public:
    //! Creates new position kit
    explicit PositionKit( QObject *parent = nullptr );

    //! \copydoc PositionKit::position
    bool hasPosition() const;

    //! \copydoc PositionKit::position
    QgsPoint position() const;

    //! \copydoc PositionKit::projectedPosition
    QgsPoint projectedPosition() const;

    //! \copydoc PositionKit::screenPosition
    QPointF screenPosition() const;

    //! \copydoc PositionKit::accuracy
    double accuracy() const;

    double verticalAccuracy() const;

    //! \copydoc PositionKit::screenAccuracy
    double screenAccuracy() const;

    /**
     * GPS horizontal accuracy units - meters (constant)
     */
    QgsUnitTypes::DistanceUnit accuracyUnits() const;

    //! \copydoc PositionKit::direction
    double direction() const;

    double speed() const;

    //! \copydoc PositionKit::isSimulated
    bool isSimulated() const;

    //! \copydoc PositionKit::mapSettings
    void setMapSettings( QgsQuickMapSettings *mapSettings );

    //! \copydoc PositionKit::mapSettings
    QgsQuickMapSettings *mapSettings() const;

    //! \copydoc PositionKit::simulatePositionLongLatRad
    QVector<double> simulatePositionLongLatRad() const;

    //! \copydoc PositionKit::simulatePositionLongLatRad
    void setSimulatePositionLongLatRad( const QVector<double> &simulatePositionLongLatRad );

    /**
     * Returns pointer to the internal QGeoPositionInfoSource object used to receive GPS location.
     * \note The returned pointer is only valid until sourceChanged() signal is emitted
     */
    QGeoPositionInfoSource *source() const;

    /**
     * Returns last known QGeoPositionInfo of the source.
     */
    QGeoPositionInfo lastKnownPosition() const;

    /**
     * Coordinate reference system of position - WGS84 (constant)
     */
    Q_INVOKABLE QgsCoordinateReferenceSystem positionCRS() const;

    /**
     * Use simulated GPS source.
     *
     * Simulated GPS source emulates point on circle around defined point in specified radius
     *
     * We do not want to have the origin point as property
     * We basically want to set it once based on project/map cente and keep
     * it that way regardless of mapsettings change (e.g. zoom etc)
     *
     * \param longitude longitude of the centre of the emulated points
     * \param latitude latitude of the centre of the emulated points
     * \param radius distance of emulated points from the centre (in degrees WSG84)
     */
    Q_INVOKABLE void useSimulatedLocation( double longitude, double latitude, double radius );

    // Starts receiving updates from GPS sources
    void startUpdates();

    // Stops receiving updates from GPS sources
    void stopUpdates();

    /**
     * Use real GPS source (not simulated)
     */
    Q_INVOKABLE void useGpsLocation();

    const QDateTime &lastGPSRead() const;

    QGeoSatelliteInfoSource *satelliteSource() const;

    int satellitesInViewCount() const;

    int usedSatellitesCount() const;

  signals:
    //! \copydoc PositionKit::position
    void positionChanged();

    //! \copydoc PositionKit::projectedPosition
    void projectedPositionChanged();

    //! \copydoc PositionKit::screenPosition
    void screenPositionChanged();

    //! hasPosition changed
    void hasPositionChanged();

    //! \copydoc PositionKit::accuracy
    double accuracyChanged() const;

    double verticalAccuracyChanged( double verticalAccuracy );

    //! \copydoc PositionKit::screenAccuracy
    double screenAccuracyChanged() const;

    //! \copydoc PositionKit::accuracyUnits
    Q_INVOKABLE QString accuracyUnitsChanged() const;

    //! \copydoc PositionKit::direction
    double directionChanged() const;

    double speedChanged( double speed );

    //! \copydoc PositionKit::isSimulated
    void isSimulatedChanged();

    //! \copydoc PositionKit::mapSettings
    void mapSettingsChanged();

    //! \copydoc PositionKit::simulatePositionLongLatRad
    void simulatePositionLongLatRadChanged( QVector<double> simulatePositionLongLatRad );

    //! Emitted when the internal source of GPS location data has been replaced.
    void sourceChanged();

    void lastGPSReadChanged( const QDateTime &lastread );

    void satelliteSourceChanged();

    void satellitesInViewCountChanged( int );

    void usedSatellitesCountChanged( int );

  private slots:
    void onPositionUpdated( const QGeoPositionInfo &info );
    void onMapSettingsUpdated();
    void onUpdateTimeout();
    void onSimulatePositionLongLatRadChanged( QVector<double> simulatePositionLongLatRad );

    void numberOfUsedSatellitesChanged( const QList<QGeoSatelliteInfo> &list );
    void numberOfSatellitesInViewChanged( const QList<QGeoSatelliteInfo> &list );

  private:
    void replacePositionSource( QGeoPositionInfoSource *source );
    void replaceSatelliteSource( QGeoSatelliteInfoSource *satelliteSource );
    QString calculateStatusLabel();
    double calculateScreenAccuracy();
    void updateProjectedPosition();
    void updateScreenPosition();
    void updateScreenAccuracy();

    void setProjectedPosition( const QgsPoint &projectedPosition );
    void setVerticalAccuracy( double vaccuracy );
    void setSpeed( double speedInMS );
    void setLastGPSRead( const QDateTime &timestamp );

    QGeoPositionInfoSource *gpsSource();
    QGeoSatelliteInfoSource *gpsSatellitesSource();
    QGeoPositionInfoSource *simulatedSource( double longitude, double latitude, double radius );

    QgsPoint mPosition;
    QgsPoint mProjectedPosition;
    QPointF mScreenPosition;
    double mAccuracy = -1;
    double mVerticalAccuracy = -1;
    double mScreenAccuracy = 2;
    double mDirection = -1;
    double mSpeed = -1;
    bool mHasPosition = false;
    bool mIsSimulated = false;
    QDateTime mLastGPSRead;
    QVector<double> mSimulatePositionLongLatRad;
    std::unique_ptr<QGeoPositionInfoSource> mSource;
    std::unique_ptr<QGeoSatelliteInfoSource> mSatelliteSource;
    int mSatellitesInViewCount = 0;
    int mUsedSatellitesCount = 0;

    QgsQuickMapSettings *mMapSettings = nullptr; // not owned
};

#endif // POSITIONKIT_H
