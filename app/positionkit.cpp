/***************************************************************************
  positionkit.cpp
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

#include "positionkit.h"
#include "inpututils.h"
#include "simulatedpositionsource.h"

PositionKit::PositionKit( QObject *parent )
  : QObject( parent )
{
  connect( this,
           &PositionKit::simulatePositionLongLatRadChanged,
           this,
           &PositionKit::onSimulatePositionLongLatRadChanged );

  useGpsLocation();
}

QGeoPositionInfoSource *PositionKit::gpsSource()
{
  // this should give us "true" position source
  // on Linux it comes from Geoclue library
  std::unique_ptr<QGeoPositionInfoSource> source( QGeoPositionInfoSource::createDefaultSource( nullptr ) );
  if ( ( !source ) || ( source->error() != QGeoPositionInfoSource::NoError ) )
  {
    QgsMessageLog::logMessage( QStringLiteral( "%1 (%2)" )
                               .arg( tr( "Unable to create default GPS Position Source" ) )
                               .arg( QString::number( ( long )source->error() ) )
                               , QStringLiteral( "Input" )
                               , Qgis::Warning );
    return nullptr;
  }
  else
  {
    return source.release();
  }
}

QGeoPositionInfoSource  *PositionKit::simulatedSource( double longitude, double latitude, double radius )
{
  return new SimulatedPositionSource( this, longitude, latitude, radius );
}

QGeoPositionInfoSource *PositionKit::source() const
{
  return mSource.get();
}

QGeoPositionInfo PositionKit::lastKnownPosition() const
{
  if ( source() )
    return source()->lastKnownPosition();
  else
    return QGeoPositionInfo();
}

void PositionKit::useSimulatedLocation( double longitude, double latitude, double radius )
{
  std::unique_ptr<QGeoPositionInfoSource> source( simulatedSource( longitude, latitude, radius ) );
  mIsSimulated = true;
  replacePositionSource( source.release() );
}

void PositionKit::updateScreenPosition()
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

void PositionKit::updateScreenAccuracy()
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

void PositionKit::setProjectedPosition( const QgsPoint &projectedPosition )
{
  if ( projectedPosition != mProjectedPosition )
  {
    mProjectedPosition = projectedPosition;
    emit projectedPositionChanged();
  }
}

void PositionKit::useGpsLocation()
{
  QGeoPositionInfoSource *source = gpsSource();
  mIsSimulated = false;
  replacePositionSource( source );
}

void PositionKit::replacePositionSource( QGeoPositionInfoSource *source )
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
    connect( mSource.get(), &QGeoPositionInfoSource::positionUpdated, this, &PositionKit::onPositionUpdated );
    connect( mSource.get(), &QGeoPositionInfoSource::updateTimeout, this,  &PositionKit::onUpdateTimeout );

    mSource->startUpdates();

    QgsDebugMsg( QStringLiteral( "Position source changed: %1" ).arg( mSource->sourceName() ) );
  }
}

QgsQuickMapSettings *PositionKit::mapSettings() const
{
  return mMapSettings;
}

void PositionKit::updateProjectedPosition()
{
  if ( !mMapSettings )
    return;

  // During startup, GPS position might not be available so we do not transform empty points.
  if ( mPosition.isEmpty() )
  {
    setProjectedPosition( QgsPoint() );
  }
  else
  {
    QgsPointXY srcPoint = QgsPointXY( mPosition.x(), mPosition.y() );
    QgsPointXY projectedPositionXY = InputUtils::transformPoint(
                                       positionCRS(),
                                       mMapSettings->destinationCrs(),
                                       mMapSettings->transformContext(),
                                       srcPoint
                                     );

    QgsPoint projectedPosition( projectedPositionXY );
    projectedPosition.addZValue( mPosition.z() );

    setProjectedPosition( projectedPosition );
  }
}

void PositionKit::onPositionUpdated( const QGeoPositionInfo &info )
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

  if ( position != mPosition )
  {
    mPosition = position;
    emit positionChanged();
  }
  // calculate accuracy
  double accuracy;
  if ( info.hasAttribute( QGeoPositionInfo::HorizontalAccuracy ) )
    accuracy = info.attribute( QGeoPositionInfo::HorizontalAccuracy );
  else
    accuracy = -1;
  if ( !qgsDoubleNear( accuracy, mAccuracy ) )
  {
    mAccuracy = accuracy;
    emit accuracyChanged();
  }

  // calculate direction
  double direction;
  if ( info.hasAttribute( QGeoPositionInfo::Direction ) )
    direction = info.attribute( QGeoPositionInfo::Direction );
  else
    direction = -1;
  if ( !qgsDoubleNear( direction, mDirection ) )
  {
    mDirection = direction;
    emit directionChanged();
  }

  // recalculate projected/screen variables
  onMapSettingsUpdated();
}

void PositionKit::onMapSettingsUpdated()
{
  updateProjectedPosition();

  updateScreenAccuracy();
  updateScreenPosition();
}

void PositionKit::onSimulatePositionLongLatRadChanged( QVector<double> simulatePositionLongLatRad )
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

double PositionKit::calculateScreenAccuracy()
{
  if ( !mMapSettings )
    return 2.0;

  if ( accuracy() > 0 )
  {
    double scpm = InputUtils::screenUnitsToMeters( mMapSettings, 1 );
    if ( scpm > 0 )
      return 2 * ( accuracy() / scpm );
    else
      return 2.0;
  }
  return 2.0;
}

void PositionKit::onUpdateTimeout()
{
  if ( mHasPosition )
  {
    mHasPosition = false;
    emit hasPositionChanged();
  }
}

QPointF PositionKit::screenPosition() const
{
  return mScreenPosition;
}

double PositionKit::screenAccuracy() const
{
  return mScreenAccuracy;
}

QVector<double> PositionKit::simulatePositionLongLatRad() const
{
  return mSimulatePositionLongLatRad;
}

void PositionKit::setSimulatePositionLongLatRad( const QVector<double> &simulatePositionLongLatRad )
{
  mSimulatePositionLongLatRad = simulatePositionLongLatRad;
  emit simulatePositionLongLatRadChanged( simulatePositionLongLatRad );
}

QgsCoordinateReferenceSystem PositionKit::positionCRS() const
{
  return QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
}

QgsPoint PositionKit::projectedPosition() const
{
  return mProjectedPosition;
}

bool PositionKit::hasPosition() const
{
  return mHasPosition;
}

QgsPoint PositionKit::position() const
{
  return mPosition;
}

double PositionKit::accuracy() const
{
  return mAccuracy;
}

QgsUnitTypes::DistanceUnit PositionKit::accuracyUnits() const
{
  return QgsUnitTypes::DistanceMeters;
}

double PositionKit::direction() const
{
  return mDirection;
}

bool PositionKit::isSimulated() const
{
  return mIsSimulated;
}

void PositionKit::setMapSettings( QgsQuickMapSettings *mapSettings )
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
    connect( mMapSettings, &QgsQuickMapSettings::extentChanged, this, &PositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, &PositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::mapUnitsPerPixelChanged, this, &PositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, &PositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::outputSizeChanged, this, &PositionKit::onMapSettingsUpdated );
    connect( mMapSettings, &QgsQuickMapSettings::outputDpiChanged, this, &PositionKit::onMapSettingsUpdated );
  }

  emit mapSettingsChanged();
}
