/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "routingcontroller.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QLocale>

#include <cmath>
#include <limits>

#include "coreutils.h"
#include "fieldprofile.h"
#include "inputmapsettings.h"
#include "position/positionkit.h"

#include "qgsproject.h"
#include "qgscoordinatetransform.h"
#include "qgsdistancearea.h"
#include "qgslinestring.h"
#include "qgsexception.h"

static const QString DEFAULT_URL = QStringLiteral( "https://valhalla1.openstreetmap.de/route" );
// distance from the route (metres) after which the route is calculated again from the current position
static const double OFF_ROUTE_DISTANCE = 60;
static const double ARRIVAL_DISTANCE = 30;
static const int REROUTE_INTERVAL_SECS = 20;

RoutingController::RoutingController( QObject *parent )
  : QObject( parent )
  , mNetwork( new QNetworkAccessManager( this ) )
{
}

void RoutingController::reload()
{
  clear();
  const QVariantMap config = FieldProfile::read( mProject ).value( QStringLiteral( "routing" ) ).toMap();
  mEnabled = !config.isEmpty();
  mUrl = config.value( QStringLiteral( "url" ), DEFAULT_URL ).toString();
  mModes = config.value( QStringLiteral( "modes" ) ).toList();
  if ( mModes.isEmpty() )
  {
    mModes = {
      QVariantMap( { { QStringLiteral( "label" ), tr( "Car" ) }, { QStringLiteral( "costing" ), QStringLiteral( "auto" ) } } ),
      QVariantMap( { { QStringLiteral( "label" ), tr( "Motorbike" ) }, { QStringLiteral( "costing" ), QStringLiteral( "motorcycle" ) } } ),
      QVariantMap( { { QStringLiteral( "label" ), tr( "Foot" ) }, { QStringLiteral( "costing" ), QStringLiteral( "pedestrian" ) } } ),
    };
  }
  mModeIndex = 0;
  emit configurationChanged();
  emit modeIndexChanged();
}

void RoutingController::routeTo( const QPointF &screenPoint )
{
  clearRoute();
  mFromIsMyLocation = true;
  mToIsMyLocation = false;
  mTo = screenToWgs84( screenPoint );
  emit pointsChanged();
  calculate();
}

void RoutingController::setPointFromScreen( bool from, const QPointF &screenPoint )
{
  if ( from )
  {
    mFrom = screenToWgs84( screenPoint );
    mFromIsMyLocation = false;
  }
  else
  {
    mTo = screenToWgs84( screenPoint );
    mToIsMyLocation = false;
  }
  emit pointsChanged();
  calculate();
}

void RoutingController::setPointToMyLocation( bool from )
{
  if ( from )
    mFromIsMyLocation = true;
  else
    mToIsMyLocation = true;
  emit pointsChanged();
  calculate();
}

void RoutingController::swapPoints()
{
  std::swap( mFrom, mTo );
  std::swap( mFromIsMyLocation, mToIsMyLocation );
  emit pointsChanged();
  calculate();
}

void RoutingController::calculate()
{
  QgsPointXY from = mFrom;
  QgsPointXY to = mTo;
  if ( mFromIsMyLocation || mToIsMyLocation )
  {
    bool ok = false;
    const QgsPointXY location = myLocation( ok );
    if ( !ok )
    {
      emit routingFailed( tr( "GPS position is not available" ) );
      return;
    }
    if ( mFromIsMyLocation )
      from = location;
    if ( mToIsMyLocation )
      to = location;
  }

  if ( mReply )
    mReply->abort();

  const QString costing = mModes.value( mModeIndex ).toMap().value( QStringLiteral( "costing" ) ).toString();
  QJsonObject body;
  body[QStringLiteral( "locations" )] = QJsonArray(
  {
    QJsonObject( { { QStringLiteral( "lat" ), from.y() }, { QStringLiteral( "lon" ), from.x() } } ),
    QJsonObject( { { QStringLiteral( "lat" ), to.y() }, { QStringLiteral( "lon" ), to.x() } } )
  } );
  body[QStringLiteral( "costing" )] = costing;
  body[QStringLiteral( "directions_options" )] = QJsonObject(
  {
    { QStringLiteral( "units" ), QStringLiteral( "kilometers" ) },
    { QStringLiteral( "language" ), QLocale().name().replace( '_', '-' ) }
  } );

  QNetworkRequest request( ( QUrl( mUrl ) ) );
  request.setHeader( QNetworkRequest::ContentTypeHeader, QStringLiteral( "application/json" ) );
  setBusy( true );
  mReply = mNetwork->post( request, QJsonDocument( body ).toJson( QJsonDocument::Compact ) );
  QNetworkReply *reply = mReply;
  connect( reply, &QNetworkReply::finished, this, [this, reply]() { onRouteReply( reply ); } );
}

void RoutingController::onRouteReply( QNetworkReply *reply )
{
  reply->deleteLater();
  if ( reply != mReply )
    return; // a newer request replaced this one

  setBusy( false );
  const QByteArray data = reply->readAll();
  const QJsonObject json = QJsonDocument::fromJson( data ).object();

  if ( reply->error() != QNetworkReply::NoError || !json.contains( QStringLiteral( "trip" ) ) )
  {
    if ( reply->error() == QNetworkReply::OperationCanceledError )
      return;
    QString message = json.value( QStringLiteral( "error" ) ).toString();
    if ( message.isEmpty() )
      message = reply->errorString();
    CoreUtils::log( QStringLiteral( "Routing" ), QStringLiteral( "Routing failed: %1" ).arg( message ) );
    emit routingFailed( tr( "Could not calculate the route: %1" ).arg( message ) );
    return;
  }

  const QJsonObject trip = json.value( QStringLiteral( "trip" ) ).toObject();
  const QJsonObject leg = trip.value( QStringLiteral( "legs" ) ).toArray().first().toObject();

  mRoute = decodePolyline( leg.value( QStringLiteral( "shape" ) ).toString() );
  mRouteDistances.clear();
  QgsDistanceArea distanceArea;
  distanceArea.setEllipsoid( QStringLiteral( "WGS84" ) );
  distanceArea.setSourceCrs( QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:4326" ) ), QgsCoordinateTransformContext() );
  double total = 0;
  for ( int i = 0; i < mRoute.size(); ++i )
  {
    if ( i > 0 )
      total += distanceArea.measureLine( mRoute[i - 1], mRoute[i] );
    mRouteDistances << total;
  }

  mManeuvers.clear();
  for ( const QJsonValue &value : leg.value( QStringLiteral( "maneuvers" ) ).toArray() )
  {
    const QJsonObject maneuver = value.toObject();
    mManeuvers << Maneuver { maneuver.value( QStringLiteral( "instruction" ) ).toString(),
                             maneuver.value( QStringLiteral( "begin_shape_index" ) ).toInt() };
  }

  const QJsonObject summary = trip.value( QStringLiteral( "summary" ) ).toObject();
  mRouteLength = summary.value( QStringLiteral( "length" ) ).toDouble() * 1000;
  mRouteTime = summary.value( QStringLiteral( "time" ) ).toDouble();
  mRouteIndex = 0;
  mRemainingDistance = mRouteLength;
  emit routeChanged();

  if ( mNavigating )
  {
    bool ok = false;
    const QgsPointXY location = myLocation( ok );
    if ( ok )
      updateNavigation( location );
  }
}

void RoutingController::startNavigation()
{
  if ( !hasRoute() )
    return;

  // navigation always starts from the current position
  if ( !mFromIsMyLocation )
  {
    mFromIsMyLocation = true;
    emit pointsChanged();
    calculate();
  }
  mNavigating = true;
  mArrived = false;
  bool ok = false;
  const QgsPointXY location = myLocation( ok );
  if ( ok )
    updateNavigation( location );
  emit navigationChanged();
}

void RoutingController::stopNavigation()
{
  mNavigating = false;
  mArrived = false;
  emit navigationChanged();
}

void RoutingController::clear()
{
  if ( mReply )
    mReply->abort();
  setBusy( false );
  mNavigating = false;
  mArrived = false;
  mFromIsMyLocation = true;
  mToIsMyLocation = false;
  mFrom = QgsPointXY();
  mTo = QgsPointXY();
  clearRoute();
  emit pointsChanged();
  emit navigationChanged();
}

void RoutingController::clearRoute()
{
  mRoute.clear();
  mRouteDistances.clear();
  mManeuvers.clear();
  mRouteLength = 0;
  mRouteTime = 0;
  emit routeChanged();
}

void RoutingController::onPositionChanged()
{
  if ( !mNavigating )
    return;

  bool ok = false;
  const QgsPointXY location = myLocation( ok );
  if ( ok )
    updateNavigation( location );
}

void RoutingController::updateNavigation( const QgsPointXY &position )
{
  if ( mRoute.size() < 2 )
    return;

  QgsDistanceArea distanceArea;
  distanceArea.setEllipsoid( QStringLiteral( "WGS84" ) );
  distanceArea.setSourceCrs( QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:4326" ) ), QgsCoordinateTransformContext() );

  // nearest route vertex, searching ahead of the last one so the position does not jump back on loops
  int nearest = mRouteIndex;
  double nearestDistance = std::numeric_limits<double>::max();
  const int searchStart = std::max( 0, mRouteIndex - 5 );
  for ( int i = searchStart; i < mRoute.size(); ++i )
  {
    const double d = distanceArea.measureLine( position, mRoute[i] );
    if ( d < nearestDistance )
    {
      nearestDistance = d;
      nearest = i;
    }
  }
  mRouteIndex = nearest;
  mRemainingDistance = std::max( 0.0, mRouteLength - mRouteDistances.value( nearest ) );

  if ( distanceArea.measureLine( position, mRoute.last() ) < ARRIVAL_DISTANCE )
  {
    mArrived = true;
    mInstruction = tr( "You have arrived" );
    mDistanceToInstruction = 0;
    emit navigationChanged();
    emit routeChanged();
    return;
  }

  // off the route: calculate it again from here, but not too often
  if ( nearestDistance > OFF_ROUTE_DISTANCE && !mBusy &&
       ( !mLastReroute.isValid() || mLastReroute.secsTo( QDateTime::currentDateTime() ) > REROUTE_INTERVAL_SECS ) )
  {
    mLastReroute = QDateTime::currentDateTime();
    mFromIsMyLocation = true;
    calculate();
  }

  // the next maneuver ahead of the current position
  mInstruction.clear();
  mDistanceToInstruction = 0;
  for ( const Maneuver &maneuver : std::as_const( mManeuvers ) )
  {
    if ( maneuver.beginIndex > nearest )
    {
      mInstruction = maneuver.instruction;
      mDistanceToInstruction = mRouteDistances.value( maneuver.beginIndex ) - mRouteDistances.value( nearest );
      break;
    }
  }
  if ( mInstruction.isEmpty() && !mManeuvers.isEmpty() )
  {
    mInstruction = mManeuvers.last().instruction;
    mDistanceToInstruction = mRemainingDistance;
  }

  emit navigationChanged();
  emit routeChanged();
}

QgsPointXY RoutingController::myLocation( bool &ok ) const
{
  ok = mPositionKit && mPositionKit->hasPosition() && !mPositionKit->positionCoordinate().isEmpty();
  if ( !ok )
    return QgsPointXY();
  const QgsPoint position = mPositionKit->positionCoordinate();
  return QgsPointXY( position.x(), position.y() );
}

QgsPointXY RoutingController::screenToWgs84( const QPointF &screenPoint ) const
{
  if ( !mMapSettings )
    return QgsPointXY();

  const QgsPoint mapPoint = mMapSettings->screenToCoordinate( screenPoint );
  const QgsCoordinateTransform transform( mMapSettings->destinationCrs(), QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:4326" ) ),
                                          mMapSettings->transformContext() );
  try
  {
    return transform.transform( QgsPointXY( mapPoint.x(), mapPoint.y() ) );
  }
  catch ( QgsCsException & )
  {
    return QgsPointXY();
  }
}

QgsGeometry RoutingController::toMapGeometry( const QgsGeometry &wgs84Geometry ) const
{
  if ( !mMapSettings || wgs84Geometry.isNull() )
    return QgsGeometry();

  QgsGeometry geometry = wgs84Geometry;
  const QgsCoordinateTransform transform( QgsCoordinateReferenceSystem( QStringLiteral( "EPSG:4326" ) ), mMapSettings->destinationCrs(),
                                          mMapSettings->transformContext() );
  try
  {
    geometry.transform( transform );
  }
  catch ( QgsCsException & )
  {
    return QgsGeometry();
  }
  return geometry;
}

QVector<QgsPointXY> RoutingController::decodePolyline( const QString &encoded, int precision )
{
  QVector<QgsPointXY> points;
  const double factor = std::pow( 10.0, precision );
  int index = 0;
  qint64 lat = 0;
  qint64 lon = 0;
  const QByteArray bytes = encoded.toLatin1();

  auto next = [&]( qint64 & value ) -> bool
  {
    qint64 result = 0;
    int shift = 0;
    int byte = 0;
    do
    {
      if ( index >= bytes.size() )
        return false;
      byte = bytes.at( index++ ) - 63;
      result |= static_cast<qint64>( byte & 0x1f ) << shift;
      shift += 5;
    }
    while ( byte >= 0x20 );
    value += ( result & 1 ) ? ~( result >> 1 ) : ( result >> 1 );
    return true;
  };

  while ( index < bytes.size() )
  {
    if ( !next( lat ) || !next( lon ) )
      break;
    points << QgsPointXY( lon / factor, lat / factor );
  }
  return points;
}

QString RoutingController::formatDistance( double metres )
{
  if ( metres < 1000 )
    return tr( "%1 m" ).arg( std::round( metres / 10 ) * 10, 0, 'f', 0 );
  return tr( "%1 km" ).arg( metres / 1000, 0, 'f', metres < 10000 ? 1 : 0 );
}

QString RoutingController::formatDuration( double seconds )
{
  const int minutes = static_cast<int>( std::round( seconds / 60 ) );
  if ( minutes < 60 )
    return tr( "%1 min" ).arg( std::max( minutes, 1 ) );
  return tr( "%1 h %2 min" ).arg( minutes / 60 ).arg( minutes % 60 );
}

void RoutingController::setBusy( bool busy )
{
  if ( mBusy == busy )
    return;
  mBusy = busy;
  emit busyChanged();
}

QgsProject *RoutingController::qgsProject() const
{
  return mProject;
}

void RoutingController::setQgsProject( QgsProject *project )
{
  if ( mProject == project )
    return;
  mProject = project;
  emit qgsProjectChanged();
  reload();
}

InputMapSettings *RoutingController::mapSettings() const
{
  return mMapSettings;
}

void RoutingController::setMapSettings( InputMapSettings *mapSettings )
{
  if ( mMapSettings == mapSettings )
    return;
  mMapSettings = mapSettings;
  emit mapSettingsChanged();
  emit pointsChanged();
  emit routeChanged();
}

PositionKit *RoutingController::positionKit() const
{
  return mPositionKit;
}

void RoutingController::setPositionKit( PositionKit *positionKit )
{
  if ( mPositionKit == positionKit )
    return;
  if ( mPositionKit )
    disconnect( mPositionKit, nullptr, this, nullptr );
  mPositionKit = positionKit;
  if ( mPositionKit )
    connect( mPositionKit, &PositionKit::positionChanged, this, &RoutingController::onPositionChanged );
  emit positionKitChanged();
}

bool RoutingController::enabled() const
{
  return mEnabled;
}

QVariantList RoutingController::modes() const
{
  return mModes;
}

int RoutingController::modeIndex() const
{
  return mModeIndex;
}

void RoutingController::setModeIndex( int index )
{
  if ( mModeIndex == index || index < 0 || index >= mModes.size() )
    return;
  mModeIndex = index;
  emit modeIndexChanged();
  if ( hasRoute() || mBusy )
    calculate();
}

bool RoutingController::fromIsMyLocation() const
{
  return mFromIsMyLocation;
}

bool RoutingController::toIsMyLocation() const
{
  return mToIsMyLocation;
}

QgsGeometry RoutingController::fromGeometry() const
{
  if ( mFromIsMyLocation )
    return QgsGeometry();
  return toMapGeometry( QgsGeometry::fromPointXY( mFrom ) );
}

QgsGeometry RoutingController::toGeometry() const
{
  if ( mToIsMyLocation || mTo.isEmpty() )
    return QgsGeometry();
  return toMapGeometry( QgsGeometry::fromPointXY( mTo ) );
}

bool RoutingController::busy() const
{
  return mBusy;
}

bool RoutingController::hasRoute() const
{
  return mRoute.size() > 1;
}

QgsGeometry RoutingController::routeGeometry() const
{
  if ( !hasRoute() )
    return QgsGeometry();

  // while navigating only the part ahead is shown
  const int start = mNavigating ? std::min( mRouteIndex, static_cast<int>( mRoute.size() ) - 2 ) : 0;
  QgsPolylineXY line;
  for ( int i = start; i < mRoute.size(); ++i )
    line << mRoute[i];
  return toMapGeometry( QgsGeometry::fromPolylineXY( line ) );
}

QString RoutingController::distanceText() const
{
  return hasRoute() ? formatDistance( mNavigating ? mRemainingDistance : mRouteLength ) : QString();
}

QString RoutingController::durationText() const
{
  if ( !hasRoute() )
    return QString();
  return formatDuration( remainingTime() );
}

QString RoutingController::arrivalText() const
{
  if ( !hasRoute() )
    return QString();
  return QLocale().toString( QTime::currentTime().addSecs( static_cast<int>( remainingTime() ) ), QLocale::ShortFormat );
}

double RoutingController::remainingTime() const
{
  // while navigating the time is scaled by the remaining part of the route
  if ( mNavigating && mRouteLength > 0 )
    return mRouteTime * mRemainingDistance / mRouteLength;
  return mRouteTime;
}

bool RoutingController::navigating() const
{
  return mNavigating;
}

bool RoutingController::arrived() const
{
  return mArrived;
}

QString RoutingController::instruction() const
{
  return mInstruction;
}

QString RoutingController::distanceToInstructionText() const
{
  return mArrived ? QString() : formatDistance( mDistanceToInstruction );
}
