/***************************************************************************
  qgsquickpositionkit.cpp
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

#include <memory>

#include "qgis.h"
#include "qgslogger.h"
#include "qgsmessagelog.h"

#include "qgsquickpositionkit.h"
#include "qgsquickutils.h"
#include "qgsquicksimulatedpositionsource.h"

QgsQuickPositionKit::QgsQuickPositionKit( QObject *parent )
  : QObject( parent )
{
  connect( this,
           &QgsQuickPositionKit::simulatePositionLongLatRadChanged,
           this,
           &QgsQuickPositionKit::onSimulatePositionLongLatRadChanged );

  useGpsLocation();
}

QGeoPositionInfoSource  *QgsQuickPositionKit::gpsSource()
{
  // this should give us "true" position source
  // on Linux it comes from Geoclue library
  std::unique_ptr<QGeoPositionInfoSource> source( QGeoPositionInfoSource::createDefaultSource( nullptr ) );
  if ( ( !source ) || ( source->error() != QGeoPositionInfoSource::NoError ) )
  {
    QgsMessageLog::logMessage( QStringLiteral( "%1 (%2)" )
                               .arg( tr( "Unable to create default GPS Position Source" ) )
                               .arg( QString::number( ( long )source->error() ) )
                               , QStringLiteral( "QgsQuick" )
                               , Qgis::Warning );
    return nullptr;
  }
  else
  {
    return source.release();
  }
}

QGeoPositionInfoSource  *QgsQuickPositionKit::simulatedSource( double longitude, double latitude, double radius )
{
  return new QgsQuickSimulatedPositionSource( this, longitude, latitude, radius );
}

PositionInfo QgsQuickPositionKit::lastPositionInfo() const
{
  return mLastPositionInfo;
}

void QgsQuickPositionKit::setLastPositionInfo( const PositionInfo &lastPositionInfo )
{
  mLastPositionInfo = lastPositionInfo;
  emit lastPositionInfoChanged();
}

QGeoPositionInfoSource *QgsQuickPositionKit::source() const
{
  return mSource.get();
}

void QgsQuickPositionKit::useSimulatedLocation( double longitude, double latitude, double radius )
{
  std::unique_ptr<QGeoPositionInfoSource> source( simulatedSource( longitude, latitude, radius ) );
  mIsSimulated = true;
  replacePositionSource( source.release() );
}

void QgsQuickPositionKit::updateScreenPosition()
{
  if ( !mMapSettings )
    return;

  QPointF screenPosition = mapSettings()->coordinateToScreen( projectedPosition() );
  if ( screenPosition != mScreenPosition )
  {
    mScreenPosition = screenPosition;
    emit screenPositionChanged();
  }
}

void QgsQuickPositionKit::updateScreenAccuracy()
{
  if ( !mMapSettings )
    return;

  double screenAccuracy = calculateScreenAccuracy();
  if ( !qgsDoubleNear( screenAccuracy, mScreenAccuracy ) )
  {
    mScreenAccuracy = screenAccuracy;
    emit screenAccuracyChanged();
  }
}

void QgsQuickPositionKit::useGpsLocation()
{
  QGeoPositionInfoSource *source = gpsSource();
  mIsSimulated = false;
  replacePositionSource( source );
}

void QgsQuickPositionKit::replacePositionSource( QGeoPositionInfoSource *source )
{
  if ( mSource.get() == source )
    return;

  if ( mSource )
  {
    mSource->disconnect();
  }

  mSource.reset( source );
  emit sourceChanged();

  if ( mSource )
  {
    connect( mSource.get(), &QGeoPositionInfoSource::positionUpdated, this, &QgsQuickPositionKit::onPositionUpdated );
    connect( mSource.get(), &QGeoPositionInfoSource::updateTimeout, this,  &QgsQuickPositionKit::onUpdateTimeout );

    mSource->startUpdates();

    QgsDebugMsg( QStringLiteral( "Position source changed: %1" ).arg( mSource->sourceName() ) );
  }
}

QgsQuickMapSettings *QgsQuickPositionKit::mapSettings() const
{
  return mMapSettings;
}

void QgsQuickPositionKit::updateProjectedPosition()
{
  if ( !mMapSettings )
    return;

  QgsPointXY srcPoint = QgsPointXY( mLastPositionInfo.position.x(), mLastPositionInfo.position.y() );
  QgsPointXY projectedPositionXY = QgsQuickUtils::transformPoint(
                                     positionCRS(),
                                     mMapSettings->destinationCrs(),
                                     mMapSettings->transformContext(),
                                     srcPoint
                                   );

  QgsPoint projectedPosition( projectedPositionXY );
  projectedPosition.addZValue( mLastPositionInfo.position.z() );

  if ( projectedPosition != mProjectedPosition )
  {
    mProjectedPosition = projectedPosition;
    emit projectedPositionChanged();
  }
}

double QgsQuickPositionKit::getAttribute( const QGeoPositionInfo &info, QGeoPositionInfo::Attribute attribute )
{
  double value = std::numeric_limits< double >::quiet_NaN();
  if ( info.hasAttribute( attribute ) )
    value = info.attribute( attribute );
  return value;
}

void QgsQuickPositionKit::onPositionUpdated( const QGeoPositionInfo &info )
{
  bool hasPosition = info.coordinate().isValid();
  if ( hasPosition != mHasPosition )
  {
    mHasPosition = hasPosition;
    emit hasPositionChanged();
  }

  // Calculate position
  QgsPoint position = QgsPoint(
                        info.coordinate().longitude(),
                        info.coordinate().latitude(),
                        info.coordinate().altitude() ); // can be NaN

  if ( position != this->position() )
  {
    mLastPositionInfo.position = position;
    mLastPositionInfo.longitude = info.coordinate().longitude();
    mLastPositionInfo.latitude = info.coordinate().latitude();
    mLastPositionInfo.altitude = info.coordinate().altitude();
    emit positionChanged();
  }
  // calculate accuracy
  double accuracy = getAttribute( info, QGeoPositionInfo::HorizontalAccuracy );
  if ( !qgsDoubleNear( accuracy, this->accuracy() ) )
  {
    mLastPositionInfo.horizontalAccuracy = accuracy;
    emit accuracyChanged();
  }

  double verticalAccuracy = getAttribute( info, QGeoPositionInfo::VerticalAccuracy );
  if ( !qgsDoubleNear( verticalAccuracy, mLastPositionInfo.verticalAccuracy ) )
  {
    mLastPositionInfo.verticalAccuracy = verticalAccuracy;
  }

  double verticalSpeed = getAttribute( info, QGeoPositionInfo::VerticalSpeed );
  if ( !qgsDoubleNear( verticalSpeed, mLastPositionInfo.verticalSpeed ) )
  {
    mLastPositionInfo.verticalSpeed = verticalSpeed;
  }

  double groundSpeed = getAttribute( info, QGeoPositionInfo::GroundSpeed );
  if ( !qgsDoubleNear( groundSpeed, mLastPositionInfo.groundSpeed ) )
  {
    mLastPositionInfo.groundSpeed = groundSpeed;
  }

  // calculate direction
  double direction = getAttribute( info, QGeoPositionInfo::Direction );
  if ( !qgsDoubleNear( direction, this->direction() ) )
  {
    mLastPositionInfo.direction = direction;
    emit directionChanged();
  }

  emit lastPositionInfoChanged();

  // recalculate projected/screen variables
  onMapSettingsUpdated();
}

void QgsQuickPositionKit::onMapSettingsUpdated()
{
  updateProjectedPosition();

  updateScreenAccuracy();
  updateScreenPosition();
}

void QgsQuickPositionKit::onSimulatePositionLongLatRadChanged( QVector<double> simulatePositionLongLatRad )
{
  if ( simulatePositionLongLatRad.size() > 2 )
  {
    double longitude = simulatePositionLongLatRad[0];
    double latitude = simulatePositionLongLatRad[1];
    double radius = simulatePositionLongLatRad[2];
    QgsDebugMsg( QStringLiteral( "Use simulated position around longlat: %1, %2, %3" ).arg( longitude ).arg( latitude ).arg( radius ) );
    useSimulatedLocation( longitude, latitude, radius );
  }
  else if ( mIsSimulated )
  {
    QgsDebugMsg( QStringLiteral( "Switching from simulated to GPS location" ) );
    useGpsLocation();
  }
}

double QgsQuickPositionKit::calculateScreenAccuracy()
{
  if ( !mMapSettings )
    return 2.0;

  if ( accuracy() > 0 )
  {
    double scpm = QgsQuickUtils::screenUnitsToMeters( mMapSettings, 1 );
    if ( scpm > 0 )
      return 2 * ( accuracy() / scpm );
    else
      return 2.0;
  }
  return 2.0;
}

void QgsQuickPositionKit::onUpdateTimeout()
{
  if ( mHasPosition )
  {
    mHasPosition = false;
    emit hasPositionChanged();
  }
}

QPointF QgsQuickPositionKit::screenPosition() const
{
  return mScreenPosition;
}

double QgsQuickPositionKit::screenAccuracy() const
{
  return mScreenAccuracy;
}

QVector<double> QgsQuickPositionKit::simulatePositionLongLatRad() const
{
  return mSimulatePositionLongLatRad;
}

void QgsQuickPositionKit::setSimulatePositionLongLatRad( const QVector<double> &simulatePositionLongLatRad )
{
  mSimulatePositionLongLatRad = simulatePositionLongLatRad;
  emit simulatePositionLongLatRadChanged( simulatePositionLongLatRad );
}

QgsCoordinateReferenceSystem QgsQuickPositionKit::positionCRS() const
{
  return QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
}

QgsPoint QgsQuickPositionKit::projectedPosition() const
{
  return mProjectedPosition;
}

bool QgsQuickPositionKit::hasPosition() const
{
  return mHasPosition;
}

QgsPoint QgsQuickPositionKit::position() const
{
  return mLastPositionInfo.position;
}

double QgsQuickPositionKit::accuracy() const
{
  return mLastPositionInfo.horizontalAccuracy;
}

QgsUnitTypes::DistanceUnit QgsQuickPositionKit::accuracyUnits() const
{
  return QgsUnitTypes::DistanceMeters;
}

double QgsQuickPositionKit::direction() const
{
  return mLastPositionInfo.direction;
}

bool QgsQuickPositionKit::isSimulated() const
{
  return mIsSimulated;
}

void QgsQuickPositionKit::setMapSettings( QgsQuickMapSettings *mapSettings )
{
  if ( mMapSettings == mapSettings )
    return;

  if ( mMapSettings )
  {
    mMapSettings->disconnect();
  }

  mMapSettings = mapSettings;

  if ( mMapSettings )
  {
    connect( mMapSettings, &QgsQuickMapSettings::extentChanged, this, &QgsQuickPositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, &QgsQuickPositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::mapUnitsPerPixelChanged, this, &QgsQuickPositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, &QgsQuickPositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::outputSizeChanged, this, &QgsQuickPositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::outputDpiChanged, this, &QgsQuickPositionKit::onMapSettingsUpdated );
  }

  emit mapSettingsChanged();
}
