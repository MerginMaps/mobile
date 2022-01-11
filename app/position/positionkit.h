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

#include "position/abstractpositionprovider.h"

#include <QObject>
#include <QtPositioning>

#include "qgspoint.h"
#include "qgsquickmapsettings.h"
#include "qgscoordinatetransform.h"
#include "qgsquickcoordinatetransformer.h"

/**
 * TODO: Add documentation
 */
class PositionKit : public QObject
{
    Q_OBJECT

    // GPS position in WGS84 coords.
    Q_PROPERTY( QgsPoint position READ position NOTIFY positionChanged )

    // GPS position is available (position property is a valid number)
    Q_PROPERTY( bool hasPosition READ hasPosition NOTIFY hasPositionChanged )

    // GPS horizontal accuracy in metres
    Q_PROPERTY( double accuracy READ accuracy NOTIFY accuracyChanged )

    // GPS vertical accuracy in metres
    Q_PROPERTY( double verticalAccuracy READ verticalAccuracy NOTIFY verticalAccuracyChanged )

    // GPS direction, bearing in degrees clockwise from north to direction of travel
    Q_PROPERTY( double direction READ direction NOTIFY directionChanged )

    // Ground speed in km/h
    Q_PROPERTY( double speed READ speed NOTIFY speedChanged )

    // Timestamp of last acquired GPS position
    Q_PROPERTY( QDateTime lastGPSRead READ lastGPSRead NOTIFY lastGPSReadChanged )

    // Number of used satellites to acquire position
    Q_PROPERTY( int usedSatellitesCount READ usedSatellitesCount NOTIFY usedSatellitesCountChanged )

    // Number of visible satellites
    Q_PROPERTY( int satellitesInViewCount READ satellitesInViewCount NOTIFY satellitesInViewCountChanged )

    // Provider of position data
    Q_PROPERTY( AbstractPositionProvider *positionProvider READ positionProvider WRITE setPositionProvider NOTIFY positionProviderChanged )

    //
    // ---- ADDITIONAL PROPERTIES WHEN MAP SETTINGS ARE PROVIDED ----
    //

    // GPS position in map coords
    Q_PROPERTY( QgsPoint projectedPosition READ projectedPosition NOTIFY projectedPositionChanged )

    // GPS position in device coords (pixels)
    Q_PROPERTY( QPointF screenPosition READ screenPosition NOTIFY screenPositionChanged )

    // Screen horizontal accuracy, 2 if not available or resolution is too small
    Q_PROPERTY( double screenAccuracy READ screenAccuracy NOTIFY screenAccuracyChanged )

    // Optional property, if provided, properties projectedPosition, screenPosition and screenAccuracy will be calculated
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

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

    //! \copydoc PositionKit::direction
    double direction() const;

    double speed() const;

    //! \copydoc PositionKit::mapSettings
    void setMapSettings( QgsQuickMapSettings *mapSettings );

    void setPositionProvider( AbstractPositionProvider *provider );

    AbstractPositionProvider *positionProvider();

    //! \copydoc PositionKit::mapSettings
    QgsQuickMapSettings *mapSettings() const;

    /**
     * Returns last known QGeoPositionInfo of the source.
     */
    GpsInformation lastPosition() const;

    /**
     * Coordinate reference system of position - WGS84 (constant)
     */
    Q_INVOKABLE QgsCoordinateReferenceSystem positionCRS() const;

    // Starts receiving updates from GPS sources
    void startUpdates();

    // Stops receiving updates from GPS sources
    void stopUpdates();

    const QDateTime &lastGPSRead() const;

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

    //! \copydoc PositionKit::direction
    double directionChanged() const;

    double speedChanged( double speed );

    //! \copydoc PositionKit::mapSettings
    void mapSettingsChanged();

    //! Emitted when the internal source of GPS location data has been replaced.
    void positionProviderChanged( AbstractPositionProvider *provider );

    void lastGPSReadChanged( const QDateTime &lastread );

    void satellitesInViewCountChanged( int );

    void usedSatellitesCountChanged( int );

  private slots:
    void onPositionUpdated( const QGeoPositionInfo &info );

    // ?
    void onMapSettingsUpdated();
    void onUpdateTimeout();

    void numberOfUsedSatellitesChanged( const QList<QGeoSatelliteInfo> &list );
    void numberOfSatellitesInViewChanged( const QList<QGeoSatelliteInfo> &list );

  private:
    // ?
    double calculateScreenAccuracy();

    void updateProjectedPosition();
    void updateScreenPosition();
    void updateScreenAccuracy();

    // TODO: add setter for each property
    void setProjectedPosition( const QgsPoint &projectedPosition );
    void setVerticalAccuracy( double vaccuracy );
    void setSpeed( double speedInMS );
    void setLastGPSRead( const QDateTime &timestamp );

    std::unique_ptr<AbstractPositionProvider> mPositionProvider; //owned

    GpsInformation mLastPosition;

    QgsPoint mPosition;
    QgsPoint mProjectedPosition;
    QPointF mScreenPosition;
    double mAccuracy = -1;
    double mVerticalAccuracy = -1;
    double mScreenAccuracy = 2;
    double mDirection = -1;
    double mSpeed = -1;
    bool mHasPosition = false;
    QDateTime mLastGPSRead;
    int mSatellitesInViewCount = -1;
    int mUsedSatellitesCount = -1;

    QgsQuickMapSettings *mMapSettings = nullptr; // not owned
};

#endif // POSITIONKIT_H
