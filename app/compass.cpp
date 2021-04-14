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
  qreal newDirection = -180.1; // TODO @vsklencar
  if ( mCompass->reading() )
  {
    newDirection = mCompass->reading()->azimuth() + mCompass->userOrientation();
  }

  return newDirection;
}

QCompassReading *Compass::reading()
{
  return mCompass->reading();
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
