/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "androidtrackingbackend.h"
#include "androidtrackingbroadcast.h"
#include "coreutils.h"
#include "inpututils.h"

#include <QtCore/private/qandroidextras_p.h>

AndroidTrackingBackend::AndroidTrackingBackend(
  AbstractTrackingBackend::UpdateFrequency frequency,
  QObject *parent )
  : AbstractTrackingBackend(
      frequency,
      AbstractTrackingBackend::SignalSlotSupport::Supported,
      AbstractTrackingBackend::TrackingMethod::UpdatesThroughFile,
      parent
    )
{
  switch ( frequency )
  {
    case AbstractTrackingBackend::Often:
      mDistanceFilter = 1;
      mUpdateInterval = 1000;
      break;

    case AbstractTrackingBackend::Normal:
      mDistanceFilter = 5;
      mUpdateInterval = 3000;
      break;

    case AbstractTrackingBackend::Occasional:
      mDistanceFilter = 20;
      mUpdateInterval = 10000;
      break;
  }

  setupForegroundUpdates();
}

AndroidTrackingBackend::~AndroidTrackingBackend()
{
  disconnect( &AndroidTrackingBroadcast::getInstance() );

  AndroidTrackingBroadcast::unregisterBroadcast();

  // stop the foreground service
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  QAndroidIntent serviceIntent( activity.object(), "uk/co/lutraconsulting/PositionTrackingService" );

  activity.callMethod<jboolean>(
    "stopService",
    "(Landroid/content/Intent;)Z",
    serviceIntent.handle().object() );

  if ( mTrackingFile.isOpen() )
  {
    mTrackingFile.close();
  }

  if ( !mTrackingFile.remove() )
  {
    qDebug() << "Tracking file could not be removed";
  }
}

QList<QgsPoint> AndroidTrackingBackend::getAllUpdates()
{
  QList<QgsPoint> allUpdates;

  if ( mTrackingFile.isOpen() )
  {
    if ( !mTrackingFile.seek( 0 ) )
    {
      qDebug() << "Unknown error when rewinding to the beginning of the tracking file";
      return allUpdates;
    }
  }
  else
  {
    if ( !mTrackingFile.open( QFile::ReadOnly ) )
    {
      CoreUtils::log(
        QStringLiteral( "Android Tracking Backend" ),
        QStringLiteral( "Tracking file could not be opened for reading: %1" ).arg( mTrackingFile.fileName() )
      );

      return allUpdates;
    }
  }

  QString fileData = QString( mTrackingFile.readAll() );

  return InputUtils::parsePositionUpdates( fileData );
}

void AndroidTrackingBackend::sourceUpdatedPosition()
{
  if ( !mTrackingFile.isOpen() )
  {
    if ( !mTrackingFile.open( QFile::ReadOnly ) )
    {
      CoreUtils::log(
        QStringLiteral( "Android Tracking Backend" ),
        QStringLiteral( "Tracking file could not be opened for reading: %1" ).arg( mTrackingFile.fileName() )
      );

      emit errorOccured( tr( "There was an error and tracking could not start, please contact support" ) );
      emit abort();

      return;
    }
  }

  QString fileData = QString( mTrackingFile.readAll() );
  QList<QgsPoint> parsedUpdates = InputUtils::parsePositionUpdates( fileData );

  if ( parsedUpdates.size() > 1 )
  {
    emit multiplePositionChanges( parsedUpdates );
  }
  else if ( parsedUpdates.size() == 1 )
  {
    emit positionChanged( parsedUpdates[0] );
  }
}

void AndroidTrackingBackend::sourceUpdatedState( const QString &statusMessage )
{
  if ( statusMessage.startsWith( QStringLiteral( "ERROR" ), Qt::CaseSensitive ) )
  {
    CoreUtils::log( QStringLiteral( "Android Tracking Backend" ), statusMessage );

    if ( statusMessage.contains( QStringLiteral( "#UNSUPPORTED" ), Qt::CaseSensitive ) )
    {
      emit errorOccured( tr( "Your device does not support tracking, available from Android 8.0" ) );
      emit abort();
    }
    else if ( statusMessage.contains( QStringLiteral( "#PERMISSIONS" ), Qt::CaseSensitive ) )
    {
      emit errorOccured( tr( "Please enable location permission before starting tracking" ) );
      emit abort();
    }
    else if ( statusMessage.contains( QStringLiteral( "#GPS_UNAVAILABLE" ), Qt::CaseSensitive ) )
    {
      emit errorOccured( tr( "Please enable location services on your device before starting tracking" ) );
      emit abort();
    }
    else if ( statusMessage.contains( QStringLiteral( "#GENERAL" ), Qt::CaseSensitive ) )
    {
      emit errorOccured( tr( "There was an error and tracking could not start, please contact support" ) );
      emit abort();
    }
  }
  else
  {
    qDebug() << "Position Tracking:" << statusMessage; // just dev logs
  }
}

void AndroidTrackingBackend::setupForegroundUpdates()
{
  if ( !AndroidTrackingBroadcast::registerBroadcast() )
  {
    emit errorOccured( tr( "There was an error and tracking could not start, please contact support" ) );
    emit abort();

    return;
  }

  connect(
    &AndroidTrackingBroadcast::getInstance(),
    &AndroidTrackingBroadcast::positionUpdated,
    this,
    &AndroidTrackingBackend::sourceUpdatedPosition
  );

  connect(
    &AndroidTrackingBroadcast::getInstance(),
    &AndroidTrackingBroadcast::statusChanged,
    this,
    &AndroidTrackingBackend::sourceUpdatedState
  );

  // TODO: you need to check if this is starting tracking or just resuming
  // It should not be an issue to start this if it is already running, but let's double check

  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  QAndroidIntent serviceIntent( activity.object(), "uk/co/lutraconsulting/PositionTrackingService" );

  serviceIntent.putExtra( QStringLiteral( "uk.co.lutraconsulting.tracking.distanceInterval" ), mDistanceFilter );
  serviceIntent.putExtra( QStringLiteral( "uk.co.lutraconsulting.tracking.timeInterval" ), mUpdateInterval );

  QJniObject result = activity.callObjectMethod(
                        "startService",
                        "(Landroid/content/Intent;)Landroid/content/ComponentName;",
                        serviceIntent.handle().object() );

  // find the file for position updates
  auto path = activity.callMethod<jstring>( "homePath", "()Ljava/lang/String;" );
  QString pathString = path.toString();

  mTrackingFile.setFileName( pathString + "/" + TRACKING_FILE_NAME );

  // go see if something's left from previous run!
  // (in future we could show a dialogue when project is opened and something is in the file)
  sourceUpdatedPosition();
}
