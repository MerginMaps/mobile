/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "compass.h"

Compass::Compass( QObject *parent ) : QObject( parent )
{
  mOrientationSensor = new QOrientationSensor( this );
  mCompass = new QCompass( this );

  mOrientationSensor->start();
  mCompass->start();

  QObject::connect( mOrientationSensor, &QOrientationSensor::readingChanged, this, &Compass::setUserOrientation );
}

qreal Compass::direction() const
{
  qreal newDirection = MIN_INVALID_DIRECTION;
  if ( mCompass && mCompass->reading() )
  {
    newDirection = mCompass->reading()->azimuth() + mCompass->userOrientation();
  }

  return newDirection;
}

QCompassReading *Compass::reading()
{
  if ( mCompass )
  {
    return mCompass->reading();
  }
  return nullptr;
}

void Compass::setUserOrientation()
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
