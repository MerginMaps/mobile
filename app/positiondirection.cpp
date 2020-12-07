/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positiondirection.h"

PositionDirection::PositionDirection( QObject *parent ) : QObject( parent )
{
  mOrientationSensor = new QOrientationSensor( this );
  mCompass = new QCompass( this );

  mOrientationSensor->start();
  mCompass->start();
  mTimer.setInterval( 200 );
  mTimer.start();

  QObject::connect( &mTimer, &QTimer::timeout, this, &PositionDirection::updateDirection );
  QObject::connect( mOrientationSensor, &QOrientationSensor::readingChanged, this, &PositionDirection::setUserOrientation );
}

void PositionDirection::updateDirection()
{
  qreal groundSpeed = -1;
  if ( mPositionKit == nullptr )
  {
    return;
  }

  if ( mPositionKit->lastKnownPosition().isValid() )
  {
    groundSpeed = mPositionKit->lastKnownPosition().attribute( QGeoPositionInfo::Attribute::GroundSpeed );
  }

  qreal newDirection = MIN_INVALID_DIRECTION;
  if ( groundSpeed >= mSpeedLimit )
  {
    newDirection = mPositionKit->direction();
  }
  else if ( mCompass->reading() )
  {
    newDirection = mCompass->reading()->azimuth() + mCompass->userOrientation();
  }

  if ( mDirection <= MIN_INVALID_DIRECTION && newDirection > MIN_INVALID_DIRECTION )
  {
    mDirection = newDirection;
    setHasDirection( true );
    emit directionChanged();
    return;
  }

  qreal delta = angleBetween( mDirection, newDirection );
  if ( delta > mUpdateMinAngleDelta )
  {
    mDirection = newDirection;
    setHasDirection( true );
    emit directionChanged();
  }

}

void PositionDirection::setUserOrientation()
{
  if ( mOrientationSensor->reading()->orientation() == QOrientationReading::Orientation::TopUp )
  {
    mCompass->setUserOrientation( 0 );
  }
  else if ( mOrientationSensor->reading()->orientation() == QOrientationReading::Orientation::TopDown )
  {
    mCompass->setUserOrientation( 180 );
  }
  else if ( mOrientationSensor->reading()->orientation() == QOrientationReading::Orientation::RightUp )
  {
    mCompass->setUserOrientation( 90 );
  }
  else if ( mOrientationSensor->reading()->orientation() == QOrientationReading::Orientation::LeftUp )
  {
    mCompass->setUserOrientation( 270 );
  }
}

bool PositionDirection::hasDirection() const
{
  return mHasDirection;
}

void PositionDirection::setHasDirection( bool hasDirection )
{
  mHasDirection = hasDirection;
  emit hasDirectionChanged();
}

qreal PositionDirection::angleBetween( qreal d1, qreal d2 )
{
  return abs( 180 - abs( abs( d1 - d2 ) - 180 ) );
}

qreal PositionDirection::direction() const
{
  return mDirection;
}

QgsQuickPositionKit *PositionDirection::positionKit() const
{
  return mPositionKit;
}

void PositionDirection::setPositionKit( QgsQuickPositionKit *positionKit )
{
  mPositionKit = positionKit;
}
