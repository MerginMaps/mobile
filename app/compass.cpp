/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "compass.h"

#include <QGuiApplication>
#include <QRotationSensor>
#include <QtMath>
#include <QScreen>

#include <iterator>

// smoothing time constant of the azimuth (in milliseconds), higher is smoother but slower
static constexpr qreal SMOOTHING_TIME_MS = 300;

static constexpr qreal FULL_CIRCLE_DEG = 360;
static constexpr qreal HALF_CIRCLE_DEG = 180;

// screen orientations, each a quarter turn from the previous one
static constexpr Qt::ScreenOrientation QUARTER_TURNS[] = { Qt::PortraitOrientation, Qt::LandscapeOrientation, Qt::InvertedPortraitOrientation, Qt::InvertedLandscapeOrientation };
static constexpr int QUARTER_TURNS_COUNT = static_cast<int>( std::size( QUARTER_TURNS ) );
static constexpr int HALF_TURN = QUARTER_TURNS_COUNT / 2;
static constexpr int THREE_QUARTER_TURNS = QUARTER_TURNS_COUNT - 1;

// user orientations of the compass (in degrees)
static constexpr int USER_ORIENTATION_NATIVE = 0;
static constexpr int USER_ORIENTATION_RIGHT_UP = 90;
static constexpr int USER_ORIENTATION_UPSIDE_DOWN = 180;
static constexpr int USER_ORIENTATION_LEFT_UP = 270;

Compass::Compass( QObject *parent ) : QObject( parent )
{
  mOrientationSensor = new QOrientationSensor( this );
  mCompass = new QCompass( this );

  mOrientationSensor->start();
  mCompass->start();

  QObject::connect( mOrientationSensor, &QOrientationSensor::readingChanged, this, &Compass::setUserOrientation );

#ifdef ANDROID
  // the raw Android azimuth is noisy (iOS filters it), smoothed for when the fused rotation sensor is not used
  mSmoothing = true;
#endif
  QObject::connect( mCompass, &QCompass::readingChanged, this, &Compass::onReadingChanged );

  if ( const QScreen *screen = QGuiApplication::primaryScreen() )
  {
    QObject::connect( screen, &QScreen::orientationChanged, this, &Compass::setUserOrientation );
  }
}

qreal Compass::direction() const
{
  qreal azimuth = MIN_INVALID_DIRECTION;
  if ( mRotationSensor && mHasRotationReading && mRotationSensor->reading() )
    azimuth = azimuthFromRotation( mRotationSensor->reading()->z() );
  else if ( mSmoothing && mHasSmoothedAzimuth )
    azimuth = smoothedAzimuth();
  else if ( mCompass->reading() )
    azimuth = mCompass->reading()->azimuth();
  else
    return MIN_INVALID_DIRECTION;

  return normalizedDirection( azimuth + mCompass->userOrientation() );
}

bool Compass::rotationSensorEnabled() const
{
  return mRotationSensorEnabled;
}

void Compass::setRotationSensorEnabled( bool enabled )
{
  if ( mRotationSensorEnabled == enabled )
    return;

  mRotationSensorEnabled = enabled;

#ifdef ANDROID
  if ( enabled )
  {
    mRotationSensor = new QRotationSensor( this );
    if ( mRotationSensor->connectToBackend() )
    {
      QObject::connect( mRotationSensor, &QRotationSensor::readingChanged, this, [this] { mHasRotationReading = true; } );
      mRotationSensor->start();
    }
    else
    {
      delete mRotationSensor;
      mRotationSensor = nullptr;
    }
  }
  else if ( mRotationSensor )
  {
    // back to the smoothed compass, it starts again from its next reading
    mRotationSensor->stop();
    delete mRotationSensor;
    mRotationSensor = nullptr;
    mHasRotationReading = false;
    mHasSmoothedAzimuth = false;
  }
#endif

  emit rotationSensorEnabledChanged();
}

void Compass::onReadingChanged()
{
  // not needed while the fused rotation sensor works
  if ( !mSmoothing || mHasRotationReading || !mCompass->reading() )
    return;

  addAzimuth( mCompass->reading()->azimuth(), mSmoothingTimer.isValid() ? mSmoothingTimer.elapsed() : 0 );
  mSmoothingTimer.start();
}

void Compass::addAzimuth( qreal azimuth, qint64 elapsed )
{
  // exponential smoothing weighted by the time since the previous azimuth (the first one is taken as is)
  const qreal weight = mHasSmoothedAzimuth ? 1 - std::exp( -elapsed / SMOOTHING_TIME_MS ) : 1;
  mHasSmoothedAzimuth = true;

  const qreal radians = qDegreesToRadians( azimuth );
  mSmoothedX += weight * ( std::cos( radians ) - mSmoothedX );
  mSmoothedY += weight * ( std::sin( radians ) - mSmoothedY );
}

qreal Compass::smoothedAzimuth() const
{
  return qRadiansToDegrees( std::atan2( mSmoothedY, mSmoothedX ) );
}

qreal Compass::azimuthFromRotation( qreal rotationZ )
{
  // the rotation is counter-clockwise, the azimuth is clockwise
  qreal azimuth = std::fmod( -rotationZ, FULL_CIRCLE_DEG );
  if ( azimuth > HALF_CIRCLE_DEG )
    azimuth -= FULL_CIRCLE_DEG;
  else if ( azimuth <= -HALF_CIRCLE_DEG )
    azimuth += FULL_CIRCLE_DEG;
  return azimuth;
}

qreal Compass::normalizedDirection( qreal direction )
{
  qreal normalized = std::fmod( direction, FULL_CIRCLE_DEG );
  if ( normalized < 0 )
    normalized += FULL_CIRCLE_DEG;
  // a tiny negative direction would become exactly 360
  if ( normalized >= FULL_CIRCLE_DEG )
    normalized -= FULL_CIRCLE_DEG;
  return normalized;
}

void Compass::setUserOrientation()
{
  const QScreen *screen = QGuiApplication::primaryScreen();
  const QOrientationReading *reading = mOrientationSensor->reading();

  mCompass->setUserOrientation( userOrientation(
                                  screen ? screen->orientation() : Qt::PortraitOrientation,
                                  screen ? screen->nativeOrientation() : Qt::PortraitOrientation,
                                  reading ? reading->orientation() : QOrientationReading::Undefined,
                                  mCompass->userOrientation() ) );
}

// number of quarter turns from portrait
static int quarterTurns( Qt::ScreenOrientation orientation )
{
  for ( int i = 0; i < QUARTER_TURNS_COUNT; ++i )
  {
    if ( QUARTER_TURNS[i] == orientation )
      return i;
  }
  return 0; // primary orientation
}

int Compass::userOrientation( Qt::ScreenOrientation screenOrientation, Qt::ScreenOrientation nativeOrientation,
                              QOrientationReading::Orientation deviceOrientation, int currentUserOrientation )
{
  // The UI orientation decides: the physical one changes also when the UI does not follow it (rotation lock,
  // quick moves), which would turn the direction by a quarter or a half turn.
  const int turns = ( quarterTurns( screenOrientation ) - quarterTurns( nativeOrientation ) + QUARTER_TURNS_COUNT ) % QUARTER_TURNS_COUNT;

  if ( turns == 0 )
    return USER_ORIENTATION_NATIVE;

  if ( turns == HALF_TURN )
    return USER_ORIENTATION_UPSIDE_DOWN;

  // a quarter turn from native, the sensor tells which side
  if ( deviceOrientation == QOrientationReading::RightUp )
    return USER_ORIENTATION_RIGHT_UP;

  if ( deviceOrientation == QOrientationReading::LeftUp )
    return USER_ORIENTATION_LEFT_UP;

  // unknown (e.g. lying flat), keep the previous value
  if ( currentUserOrientation == USER_ORIENTATION_RIGHT_UP || currentUserOrientation == USER_ORIENTATION_LEFT_UP )
    return currentUserOrientation;

  return turns == THREE_QUARTER_TURNS ? USER_ORIENTATION_RIGHT_UP : USER_ORIENTATION_LEFT_UP;
}
