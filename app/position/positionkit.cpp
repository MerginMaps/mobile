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
#include "coreutils.h"

PositionKit::PositionKit( QObject *parent )
  : QObject( parent )
{
  // TODO: try to load last used provider
}

GpsInformation PositionKit::lastPosition() const
{
  return mLastPosition;
}

void PositionKit::startUpdates()
{
  if ( mPositionProvider )
  {
    mPositionProvider->startUpdates();
  }
}

void PositionKit::stopUpdates()
{
  if ( mPositionProvider )
  {
    mPositionProvider->stopUpdates();
  }
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

void PositionKit::setVerticalAccuracy( double vAccuracy )
{
  if ( !qgsDoubleNear( vAccuracy, mVerticalAccuracy ) )
  {
    mVerticalAccuracy = vAccuracy;
    emit verticalAccuracyChanged( mVerticalAccuracy );
  }
}

void PositionKit::setSpeed( double speed )
{
  if ( !qgsDoubleNear( speed, mSpeed ) )
  {
    mSpeed = speed;
    emit speedChanged( mSpeed );
  }
}

void PositionKit::setLastGPSRead( const QDateTime &timestamp )
{
  if ( mLastGPSRead != timestamp )
  {
    mLastGPSRead = timestamp;
    emit lastGPSReadChanged( mLastGPSRead );
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

  // recalculate projected/screen variables
  onMapSettingsUpdated();
}

void PositionKit::onMapSettingsUpdated()
{
  updateProjectedPosition();

  updateScreenAccuracy();
  updateScreenPosition();
}

void PositionKit::numberOfUsedSatellitesChanged( const QList<QGeoSatelliteInfo> &list )
{
  if ( list.count() != mUsedSatellitesCount )
  {
    mUsedSatellitesCount = list.count();
    emit usedSatellitesCountChanged( list.count() );
  }
}

void PositionKit::numberOfSatellitesInViewChanged( const QList<QGeoSatelliteInfo> &list )
{
  if ( list.count() != mSatellitesInViewCount )
  {
    mSatellitesInViewCount = list.count();
    emit satellitesInViewCountChanged( list.count() );
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

QPointF PositionKit::screenPosition() const
{
  return mScreenPosition;
}

double PositionKit::screenAccuracy() const
{
  return mScreenAccuracy;
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

double PositionKit::verticalAccuracy() const
{
  return mVerticalAccuracy;
}

double PositionKit::direction() const
{
  return mDirection;
}

double PositionKit::speed() const
{
  return mSpeed;
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

void PositionKit::setPositionProvider( AbstractPositionProvider *provider )
{
  // TODO: disconnect from signals of previous provider

  mPositionProvider.reset( provider );

  // TODO: connect to signals of new provider
}

const QDateTime &PositionKit::lastGPSRead() const
{
  return mLastGPSRead;
}

int PositionKit::satellitesInViewCount() const
{
  return mSatellitesInViewCount;
}

int PositionKit::usedSatellitesCount() const
{
  return mUsedSatellitesCount;
}
