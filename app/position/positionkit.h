/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONKIT_H
#define POSITIONKIT_H

#include "position/providers/abstractpositionprovider.h"

#include "qgspoint.h"
#include "qgscoordinatereferencesystem.h"
#include <QObject>
#include <QtQml/qqmlregistration.h>

class AppSettings;

/**
 * PositionKit is a core component that feeds application with position data it receives from position provider.
 * PositionKit contains a static function to construct new provider which can then be assigned to positionProvider property.
 * It is possible to either read all data about position at once (via position property) or read separate data via its
 * corresponding property.
 */
class PositionKit : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY( double latitude READ latitude NOTIFY latitudeChanged )
    Q_PROPERTY( double longitude READ longitude NOTIFY longitudeChanged )

    Q_PROPERTY( double altitude READ altitude NOTIFY altitudeChanged )
    Q_PROPERTY( double geoidSeparation READ geoidSeparation NOTIFY geoidSeparationChanged )

    // auxiliary property providing QgsPoint for lat/long/alt instead of separate properties
    Q_PROPERTY( QgsPoint positionCoordinate READ positionCoordinate NOTIFY positionCoordinateChanged )
    Q_PROPERTY( bool hasPosition READ hasPosition NOTIFY hasPositionChanged )

    // accuracy in metres
    Q_PROPERTY( double horizontalAccuracy READ horizontalAccuracy NOTIFY horizontalAccuracyChanged )
    Q_PROPERTY( double verticalAccuracy READ verticalAccuracy NOTIFY verticalAccuracyChanged )

    // GPS direction, bearing in degrees clockwise from north to direction of travel
    Q_PROPERTY( double direction READ direction NOTIFY directionChanged )

    // magnetic variation between true and magnetic north, in degrees
    Q_PROPERTY( double magneticVariation READ magneticVariation NOTIFY magneticVariationChanged )

    // speed in km/h
    Q_PROPERTY( double speed READ speed NOTIFY speedChanged )
    Q_PROPERTY( double verticalSpeed READ verticalSpeed NOTIFY verticalSpeedChanged )

    // Timestamp of last acquired GPS position
    Q_PROPERTY( QDateTime lastRead READ lastRead NOTIFY lastReadChanged )

    // Number of visible/used GPS satellites
    Q_PROPERTY( int satellitesUsed READ satellitesUsed NOTIFY satellitesUsedChanged )
    Q_PROPERTY( int satellitesVisible READ satellitesVisible NOTIFY satellitesVisibleChanged )

    // GPS satellites advanced data (Horizontal dilution of precision)
    Q_PROPERTY( double hdop READ hdop NOTIFY hdopChanged )
    Q_PROPERTY( double vdop READ vdop NOTIFY vdopChanged )
    Q_PROPERTY( double pdop READ pdop NOTIFY pdopChanged )

    // GPS fix status
    Q_PROPERTY( QString fix READ fix NOTIFY fixChanged )

    // Auxiliary property
    Q_PROPERTY( GeoPosition position READ position NOTIFY positionChanged )

    // Provider of position data
    Q_PROPERTY( AbstractPositionProvider *positionProvider READ positionProvider WRITE setPositionProvider NOTIFY positionProviderChanged )
    Q_PROPERTY( bool isMockPosition READ isMockPosition NOTIFY isMockPositionChanged )

    Q_PROPERTY( AppSettings *appSettings READ appSettings WRITE setAppSettings NOTIFY appSettingsChanged )
    Q_PROPERTY( double antennaHeight READ antennaHeight NOTIFY antennaHeightChanged )

  public:
    //! Creates new position kit
    explicit PositionKit( QObject *parent = nullptr );

    Q_INVOKABLE void startUpdates();
    Q_INVOKABLE void stopUpdates();

    double latitude() const;
    double longitude() const;
    double altitude() const;
    double geoidSeparation() const;
    QgsPoint positionCoordinate() const;
    bool hasPosition() const;

    double horizontalAccuracy() const;
    double verticalAccuracy() const;

    double direction() const;
    double magneticVariation() const;

    double speed() const;
    double verticalSpeed() const;

    const QDateTime &lastRead() const;

    int satellitesUsed() const;
    int satellitesVisible() const;

    QString fix() const;

    const GeoPosition &position() const;
    bool isMockPosition() const;

    AbstractPositionProvider *positionProvider() const;
    void setPositionProvider( AbstractPositionProvider *newPositionProvider );

    double hdop() const;
    double vdop() const;
    double pdop() const;

    // Coordinate reference system - WGS84 (EPSG:4326)
    static QgsCoordinateReferenceSystem positionCrs2D();
    // Coordinate reference system - WGS84 + ellipsoid height (EPSG:4979)
    static QgsCoordinateReferenceSystem positionCrs3DEllipsoidHeight();
    // Coordinate reference system of position - WGS84 + geoid height - egm96_15 (EPSG:9707)
    static QgsCoordinateReferenceSystem positionCrs3D();

    Q_INVOKABLE static AbstractPositionProvider *constructProvider( const QString &type, const QString &id, const QString &name = QString() );
    Q_INVOKABLE static AbstractPositionProvider *constructActiveProvider( AppSettings *appsettings );

    AppSettings *appSettings() const;
    void setAppSettings( AppSettings *appSettings );

    double antennaHeight() const;

  signals:
    void latitudeChanged( double );
    void longitudeChanged( double );
    void altitudeChanged( double );
    void geoidSeparationChanged( double );
    void positionCoordinateChanged( QgsPoint );
    void hasPositionChanged( bool );

    void horizontalAccuracyChanged( double );
    void verticalAccuracyChanged( double );

    void directionChanged( double );
    void magneticVariationChanged( double );

    void speedChanged( double );
    void verticalSpeedChanged( double );

    void lastReadChanged( const QDateTime & );

    void satellitesUsedChanged( int );
    void satellitesVisibleChanged( int );
    void hdopChanged( double );
    void vdopChanged( double );
    void pdopChanged( double );

    void fixChanged( const QString & );

    void positionProviderChanged( AbstractPositionProvider *provider );

    void positionChanged( const GeoPosition & );
    void isMockPositionChanged( bool );

    void appSettingsChanged();

    void antennaHeightChanged();

  public slots:
    void parsePositionUpdate( const GeoPosition &newPosition );

    // stop updates when application is minimized
    void appStateChanged( Qt::ApplicationState state );

  private:
    GeoPosition mPosition;
    bool mHasPosition = false;
    std::unique_ptr<AbstractPositionProvider> mPositionProvider;
    AppSettings *mAppSettings = nullptr; // not owned
};

#endif // POSITIONKIT_H
