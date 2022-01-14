/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mapposition.h"
#include "inpututils.h"

MapPosition::MapPosition( QObject *parent ) : QObject( parent )
{

}

PositionKit *MapPosition::positionKit() const
{
  return mPositionKit;
}

void MapPosition::setPositionKit( PositionKit *newPositionKit )
{
  if ( mPositionKit == newPositionKit )
    return;

  if ( mPositionKit )
    mPositionKit->disconnect();

  mPositionKit = newPositionKit;

  if ( mPositionKit )
    connect( mPositionKit, &PositionKit::positionChanged, this, &MapPosition::update );

  emit positionKitChanged( mPositionKit );
}

QgsQuickMapSettings *MapPosition::mapSettings() const
{
  return mMapSettings;
}

void MapPosition::setMapSettings( QgsQuickMapSettings *newMapSettings )
{
  if ( mMapSettings == newMapSettings )
    return;

  if ( mMapSettings )
  {
    mMapSettings->disconnect();
  }

  mMapSettings = newMapSettings;

  if ( mMapSettings )
  {
    connect( mMapSettings, &QgsQuickMapSettings::extentChanged, this, &MapPosition::update );
    connect( mMapSettings, &QgsQuickMapSettings::destinationCrsChanged, this, &MapPosition::update );
    connect( mMapSettings, &QgsQuickMapSettings::mapUnitsPerPixelChanged, this, &MapPosition::update );
    connect( mMapSettings, &QgsQuickMapSettings::visibleExtentChanged, this, &MapPosition::update );
    connect( mMapSettings, &QgsQuickMapSettings::outputSizeChanged, this, &MapPosition::update );
    connect( mMapSettings, &QgsQuickMapSettings::outputDpiChanged, this, &MapPosition::update );
  }

  emit mapSettingsChanged( mMapSettings );
}

QgsPoint MapPosition::mapPosition() const
{
  return mMapPosition;
}

QPointF MapPosition::screenPosition() const
{
  return mScreenPosition;
}

double MapPosition::screenAccuracy() const
{
  return mScreenAccuracy;
}

void MapPosition::update()
{
  // map position needs to go first
  recalculateMapPosition();

  recalculateScreenAccuracy();
  recalculateScreenPosition();
}

void MapPosition::recalculateMapPosition()
{
  QgsPoint newMapPosition;

  if ( mMapSettings && mPositionKit )
  {
    QgsPoint geoposition = mPositionKit->positionCoordinate();

    // During startup, GPS position might not be available so we do not transform empty points.
    if ( !geoposition.isEmpty() )
    {
      QgsPointXY srcPoint = QgsPointXY( geoposition.x(), geoposition.y() );
      QgsPointXY mapPositionXY = InputUtils::transformPoint(
                                         mPositionKit->positionCRS(),
                                         mMapSettings->destinationCrs(),
                                         mMapSettings->transformContext(),
                                         srcPoint
                                       );

      newMapPosition = QgsPoint( mapPositionXY );
      newMapPosition.addZValue( geoposition.z() );
    }
  }

  if ( mMapPosition != newMapPosition )
  {
    mMapPosition = newMapPosition;
    emit mapPositionChanged( mMapPosition );
  }
}

void MapPosition::recalculateScreenPosition()
{
  QPointF newScreenPosition;

  if ( mMapSettings && mPositionKit )
  {
    newScreenPosition = mapSettings()->coordinateToScreen( mMapPosition );
  }

  if ( mScreenPosition != newScreenPosition )
  {
    mScreenPosition = newScreenPosition;
    emit screenPositionChanged( mScreenPosition );
  }
}

void MapPosition::recalculateScreenAccuracy()
{
  double newScreenAccuracy = 2.0;

  if ( mMapSettings && mPositionKit )
  {
    double hacc = mPositionKit->horizontalAccuracy();
    if ( hacc > 0 )
    {
      double scpm = InputUtils::screenUnitsToMeters( mMapSettings, 1 );
      if ( scpm > 0 )
        newScreenAccuracy = 2 * ( hacc / scpm );
    }
  }

  if ( !qgsDoubleNear( mScreenAccuracy, newScreenAccuracy ) )
  {
    mScreenAccuracy = newScreenAccuracy;
    emit screenAccuracyChanged( mScreenAccuracy );
  }
}
