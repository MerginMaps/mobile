/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ROUTINGCONTROLLER_H
#define ROUTINGCONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QDateTime>

#include "qgsgeometry.h"
#include "qgspoint.h"

class QgsProject;
class QNetworkAccessManager;
class QNetworkReply;
class InputMapSettings;
class PositionKit;

/**
 * Online routing with a Valhalla server: route from the GPS position (or any point) to a point
 * picked on the map, then turn-by-turn navigation following the GPS position.
 *
 * Enabled by the "routing" section of the field profile (see FieldProfile):
 *
 * "routing": {
 *   "url": "https://valhalla1.openstreetmap.de/route",   // optional, Valhalla /route endpoint
 *   "modes": [ { "label": "Car", "costing": "auto" },     // optional, Valhalla costing models
 *              { "label": "Motorbike", "costing": "motorcycle" },
 *              { "label": "Foot", "costing": "pedestrian" } ]
 * }
 *
 * Points are in WGS84, geometries for highlights are in the map CRS.
 */
class RoutingController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( InputMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )
    Q_PROPERTY( PositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )

    Q_PROPERTY( bool enabled READ enabled NOTIFY configurationChanged )
    Q_PROPERTY( QVariantList modes READ modes NOTIFY configurationChanged ) // [ { label, costing } ]
    Q_PROPERTY( int modeIndex READ modeIndex WRITE setModeIndex NOTIFY modeIndexChanged )

    // an empty point means "my location"
    Q_PROPERTY( bool fromIsMyLocation READ fromIsMyLocation NOTIFY pointsChanged )
    Q_PROPERTY( bool toIsMyLocation READ toIsMyLocation NOTIFY pointsChanged )
    Q_PROPERTY( QgsGeometry fromGeometry READ fromGeometry NOTIFY pointsChanged ) // map CRS
    Q_PROPERTY( QgsGeometry toGeometry READ toGeometry NOTIFY pointsChanged ) // map CRS

    Q_PROPERTY( bool busy READ busy NOTIFY busyChanged )
    Q_PROPERTY( bool hasRoute READ hasRoute NOTIFY routeChanged )
    Q_PROPERTY( QgsGeometry routeGeometry READ routeGeometry NOTIFY routeChanged ) // map CRS
    Q_PROPERTY( QString distanceText READ distanceText NOTIFY routeChanged )
    Q_PROPERTY( QString durationText READ durationText NOTIFY routeChanged )
    Q_PROPERTY( QString arrivalText READ arrivalText NOTIFY routeChanged )

    Q_PROPERTY( bool navigating READ navigating NOTIFY navigationChanged )
    Q_PROPERTY( bool arrived READ arrived NOTIFY navigationChanged )
    Q_PROPERTY( QString instruction READ instruction NOTIFY navigationChanged )
    Q_PROPERTY( QString distanceToInstructionText READ distanceToInstructionText NOTIFY navigationChanged )

  public:
    explicit RoutingController( QObject *parent = nullptr );

    //! Re-reads the routing configuration from the field profile of the project
    Q_INVOKABLE void reload();

    //! Starts planning a route from my location to the map point \a screenPoint
    Q_INVOKABLE void routeTo( const QPointF &screenPoint );

    //! Sets the start (\a from = true) or the destination to the map point \a screenPoint
    Q_INVOKABLE void setPointFromScreen( bool from, const QPointF &screenPoint );

    //! Sets the start (\a from = true) or the destination to my location
    Q_INVOKABLE void setPointToMyLocation( bool from );

    Q_INVOKABLE void swapPoints();

    //! Requests the route from the routing server
    Q_INVOKABLE void calculate();

    Q_INVOKABLE void startNavigation();
    Q_INVOKABLE void stopNavigation();

    //! Clears the route and the points
    Q_INVOKABLE void clear();

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *project );
    InputMapSettings *mapSettings() const;
    void setMapSettings( InputMapSettings *mapSettings );
    PositionKit *positionKit() const;
    void setPositionKit( PositionKit *positionKit );

    bool enabled() const;
    QVariantList modes() const;
    int modeIndex() const;
    void setModeIndex( int index );

    bool fromIsMyLocation() const;
    bool toIsMyLocation() const;
    QgsGeometry fromGeometry() const;
    QgsGeometry toGeometry() const;

    bool busy() const;
    bool hasRoute() const;
    QgsGeometry routeGeometry() const;
    QString distanceText() const;
    QString durationText() const;
    QString arrivalText() const;

    bool navigating() const;
    bool arrived() const;
    QString instruction() const;
    QString distanceToInstructionText() const;

    //! Decodes a Valhalla/Google encoded polyline with the given precision (6 for Valhalla) to lon/lat points
    static QVector<QgsPointXY> decodePolyline( const QString &encoded, int precision = 6 );

  signals:
    void qgsProjectChanged();
    void mapSettingsChanged();
    void positionKitChanged();
    void configurationChanged();
    void modeIndexChanged();
    void pointsChanged();
    void busyChanged();
    void routeChanged();
    void navigationChanged();
    void routingFailed( const QString &message );

  private slots:
    void onPositionChanged();

  private:
    struct Maneuver
    {
      QString instruction;
      int beginIndex = 0;
    };

    QgsPointXY myLocation( bool &ok ) const;
    QgsGeometry toMapGeometry( const QgsGeometry &wgs84Geometry ) const;
    QgsPointXY screenToWgs84( const QPointF &screenPoint ) const;
    void setBusy( bool busy );
    void clearRoute();
    void onRouteReply( QNetworkReply *reply );
    void updateNavigation( const QgsPointXY &position );
    double remainingTime() const;
    static QString formatDistance( double metres );
    static QString formatDuration( double seconds );

    QPointer<QgsProject> mProject;
    QPointer<InputMapSettings> mMapSettings;
    QPointer<PositionKit> mPositionKit;
    QNetworkAccessManager *mNetwork = nullptr;
    QPointer<QNetworkReply> mReply;

    QString mUrl;
    QVariantList mModes;
    int mModeIndex = 0;
    bool mEnabled = false;

    // WGS84, empty = my location
    QgsPointXY mFrom;
    QgsPointXY mTo;
    bool mFromIsMyLocation = true;
    bool mToIsMyLocation = false;

    bool mBusy = false;
    QVector<QgsPointXY> mRoute; // WGS84
    QVector<double> mRouteDistances; // cumulative metres along the route at each vertex
    QVector<Maneuver> mManeuvers;
    double mRouteLength = 0; // metres
    double mRouteTime = 0; // seconds

    bool mNavigating = false;
    bool mArrived = false;
    int mRouteIndex = 0; // nearest route vertex while navigating
    double mRemainingDistance = 0;
    QString mInstruction;
    double mDistanceToInstruction = 0;
    QDateTime mLastReroute;
};

#endif // ROUTINGCONTROLLER_H
