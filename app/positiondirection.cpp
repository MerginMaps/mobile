#include "positiondirection.h"

PositionDirection::PositionDirection( QObject *parent ) : QObject( parent )
{
#ifdef MOBILE_OS
  mOrientationSensor = new QOrientationSensor( this );
  mCompass = new QCompass( this );

  mOrientationSensor->start();
  mCompass->start();
  mTimer.setInterval( 200 );
  mTimer.start();

  QObject::connect( &mTimer, &QTimer::timeout, this, &PositionDirection::updateDirection );
  QObject::connect( mOrientationSensor, &QOrientationSensor::readingChanged, this, &PositionDirection::setUserOrientation );
#endif
}

#ifdef MOBILE_OS
void PositionDirection::updateDirection()
{
  qreal groundSpeed = -1;
  if ( mPositionKit == nullptr )
  {
    qDebug() << "NULL PTE POSITION KIT";
    return;
  }

  if ( mPositionKit->source()->lastKnownPosition().isValid() )
  {
    groundSpeed = mPositionKit->source()->lastKnownPosition().attribute( QGeoPositionInfo::Attribute::GroundSpeed );
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
    emit directionChanged();
    return;
  }

  qreal delta = angleBetween( mDirection, newDirection );
  if ( delta > mDirectionTrahsold )
  {
    mDirection = newDirection;
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

qreal PositionDirection::angleBetween( qreal d1, qreal d2 )
{
  return 180 - abs( abs( d1 - d2 ) - 180 );
}
#endif

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

qreal PositionDirection::minInvalidDirection() const
{
  return MIN_INVALID_DIRECTION;
}
