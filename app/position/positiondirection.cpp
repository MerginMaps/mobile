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
  mTimer.setInterval( 200 );
  mTimer.start();

  QObject::connect( &mTimer, &QTimer::timeout, this, &PositionDirection::updateDirection );
}

void PositionDirection::updateDirection()
{
  qreal groundSpeed = -1;
  if ( mPositionKit == nullptr )
  {
    return;
  }

  if ( !std::isnan( mPositionKit->speed() ) )
  {
    groundSpeed = mPositionKit->speed();
  }

  qreal newDirection = Compass::MIN_INVALID_DIRECTION;
  if ( groundSpeed >= mSpeedLimit )
  {
    newDirection = mPositionKit->direction();
  }
  else if ( mCompass && mCompass->reading() )
  {
    newDirection = mCompass->direction();
  }
  if ( mDirection <= Compass::MIN_INVALID_DIRECTION && newDirection > Compass::MIN_INVALID_DIRECTION )
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

Compass *PositionDirection::compass() const
{
  return mCompass;
}

void PositionDirection::setCompass( Compass *compass )
{
  mCompass = compass;
  emit compassChanged();
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

PositionKit *PositionDirection::positionKit() const
{
  return mPositionKit;
}

void PositionDirection::setPositionKit( PositionKit *positionKit )
{
  mPositionKit = positionKit;
}
