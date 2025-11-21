/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trackingmanager.h"

#include "qgslinestring.h"

#include "positionkit.h"
#include "coreutils.h"

#include <QDir>
#include <QStandardPaths>

// backend implementations
#ifdef ANDROID
#include "androidtrackingbackend.h"
#elif defined( Q_OS_IOS )
#include "iostrackingbackend.h"
#else
#include "internaltrackingbackend.h"
#endif

TrackingManager::TrackingManager( QObject *parent )
  : QObject( parent ),
    mFileOffset( 0 ),
    mFileLock()
{
  connect( &mElapsedTimeTextTimer, &QTimer::timeout, this, &TrackingManager::elapsedTimeTextChanged );
}

bool TrackingManager::startTracking( QString dataPath, TrackingUtils::UpdateFrequency updateFrequency, PositionKit *positionKit )
{
  if ( mIsRunning )
  {
    qInfo() << "Position tracking is already running";
    return false;
  }

  buildBackend( updateFrequency, positionKit );

  if ( !mBackend.get() )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Could not build tracking backend!" ) );
    return false;
  }

  QgsLineString *line = new QgsLineString();
  mGeometry.set( line );

  mIsRunning = true;
  mStartTime = QDateTime::currentDateTime(); // TODO: shall we read this start time from somewhere else? If the app gets closed in the meantime, we lose this information... -> maybe we should read it from the first point

  Q_UNUSED( dataPath ); // TODO: use the project data path!
  QDir appData( QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ) );
  QString trackingFilePath = appData.absoluteFilePath( QStringLiteral( "tracking_data.txt" ) );

  mFile.setFileName( trackingFilePath );

  connect( mBackend.get(), &AbstractTrackingBackend::positionUpdated, this, &TrackingManager::onPositionUpdated );

  emit geometryChanged();
  emit isRunningChanged();
  emit startTimeChanged();
  emit elapsedTimeTextChanged();

  mElapsedTimeTextTimer.start( 1000 );
  return true;
}

bool TrackingManager::stopTracking()
{
  if ( mBackend )
  {
    mBackend.get()->disconnect( this );
  }

  mBackend.reset( nullptr );

  mFile.close();
  mIsRunning = false;

  emit isRunningChanged();

  return true;
}

void TrackingManager::reset()
{
  stopTracking();

  mFile.close();
  mFile.setFileName( "" );
  mFileOffset = 0;

  mStartTime = QDateTime();
  mGeometry = QgsGeometry();

  emit geometryChanged();
  emit startTimeChanged();
  emit elapsedTimeTextChanged();
}

bool TrackingManager::isRunning() const
{
  return mIsRunning;
}

QgsGeometry TrackingManager::geometry() const
{
  return mGeometry;
}

QDateTime TrackingManager::startTime() const
{
  return mStartTime;
}

QString TrackingManager::elapsedTimeText() const
{
  if ( !mIsRunning )
    return QString();

  qint64 totalSecs = mStartTime.secsTo( QDateTime::currentDateTime() );
  int hours = totalSecs / 3600;
  totalSecs = totalSecs % 3600;
  int minutes = totalSecs / 60;
  int seconds = totalSecs % 60;

  return QString( "%1:%2:%3" ).arg( hours, 2, 10, QChar( '0' ) ).arg( minutes, 2, 10, QChar( '0' ) ).arg( seconds, 2, 10, QChar( '0' ) );
}

void TrackingManager::onPositionUpdated()
{
  if ( !mIsRunning ) // TODO: could this happen on Android??
  {
    // something unexpected happened ...
    CoreUtils::log(
      QStringLiteral( "Position tracking" ),
      QStringLiteral( "Received position update, but we are not tracking" )
    );
    return;
  }

  QString newData;

  qDebug() << "--> R: Using file lock" << &mFileLock;

  // QReadLocker unlocks the lock once it goes out of scope
  {
    QReadLocker readLocker( &mFileLock );

    // let's read the tracking file
    if ( !mFile.isOpen() )
    {
      if ( !mFile.open( QFile::ReadOnly ) )
      {
        qCritical() << "Error, could not open tracking file for reading!";
        return;
      }
    }

    mFile.seek( mFileOffset );

    newData = mFile.readAll();

    mFileOffset = mFile.pos();
  }

  if ( !newData.isEmpty() )
  {
    // append data to the track
    TrackingUtils::parseAndAppendPositionUpdates( newData, mGeometry );

    emit geometryChanged();
  }
}

void TrackingManager::buildBackend( TrackingUtils::UpdateFrequency updateFrequency, PositionKit *positionKit )
{
  AbstractTrackingBackend *positionBackend = nullptr;

  // TODO: pass in the file path!

#ifdef ANDROID
  positionBackend = new AndroidTrackingBackend( updateFrequency, mFileLock );
#elif defined( Q_OS_IOS )
  positionBackend = new IOSTrackingBackend( updateFrequency, mFileLock );
#else
  // desktop
  if ( positionKit )
  {
    positionBackend = new InternalTrackingBackend( &mFileLock, positionKit, updateFrequency );
  }
  else
  {
    CoreUtils::log(
      QStringLiteral( "Position tracking" ),
      QStringLiteral( "Requested tracking on desktop, but no position kit provided" )
    );
  }
#endif

  mBackend.reset( positionBackend );
}
