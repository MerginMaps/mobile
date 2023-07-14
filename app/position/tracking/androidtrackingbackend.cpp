/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "androidtrackingbackend.h"
#include "androidtrackingcallback.h"
#include "coreutils.h"

#include <QtCore/private/qandroidextras_p.h>

AndroidTrackingBackend::AndroidTrackingBackend( AbstractTrackingBackend::UpdateFrequency frequency, QObject *parent )
  : AbstractTrackingBackend{ frequency, AbstractTrackingBackend::SignalSlotSupport::Supported, parent }
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

  mBroadcastReceiver = QJniObject( "uk/co/lutraconsulting/PositionTrackingBroadcastMiddleware" );

  setupForegroundUpdates();
}

AndroidTrackingBackend::~AndroidTrackingBackend()
{
  AndroidTrackingCallback &trackingCallback = AndroidTrackingCallback::getInstance();
  trackingCallback.setBackend( nullptr );

  mBroadcastReceiver.callMethod<void>( "unregisterBroadcastReceiver",
                                       "(Landroid/content/Context;)V",
                                       QNativeInterface::QAndroidApplication::context() );

  // unregister the natives
  QJniEnvironment javaenv;
  jclass objectClass = javaenv->GetObjectClass( mBroadcastReceiver.object<jobject>() );

  javaenv->UnregisterNatives( objectClass );
  javaenv->DeleteLocalRef( objectClass );

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
  QStringList positions = fileData.split( '\n', Qt::SkipEmptyParts );

  if ( positions.isEmpty() )
  {
    return;
  }

  QList<QgsPoint> parsedUpdates;

  for ( int ix = 0; ix < positions.size(); ix++ )
  {
    QStringList coordinates = positions[ix].split( ' ', Qt::SkipEmptyParts );

    if ( coordinates.size() != 4 )
    {
      continue;
    }

    QgsPoint geop;
    geop.setX( coordinates[0].toDouble() ); // long
    geop.setY( coordinates[1].toDouble() ); // lat
    geop.setZ( coordinates[2].toDouble() ); // alt
    geop.setM( coordinates[3].toDouble() ); // time in secs
    parsedUpdates << geop;
  }

  if ( parsedUpdates.size() > 0 )
  {
    emit multiplePositionChanges( parsedUpdates );
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
  if ( !mBroadcastReceiver.isValid() )
  {
    CoreUtils::log( QStringLiteral( "Android Tracking Backend" ), QStringLiteral( "Invalid broadcast receiver, aborting...!" ) );

    emit errorOccured( tr( "There was an error and tracking could not start, please contact support" ) );
    emit abort();

    return;
  }

  // prepare callbacks from Java
  AndroidTrackingCallback &trackingCallback = AndroidTrackingCallback::getInstance();
  trackingCallback.setBackend( this );

  JNINativeMethod methods[]
  {
    {
      "notifyListenersPositionUpdated",
      "()V",
      reinterpret_cast<void *>( AndroidTrackingCallback::notifyListenersPositionUpdated )
    },
    {
      "notifyListenersStatusUpdate",
      "(Ljava/lang/String;)V",
      reinterpret_cast<void *>( AndroidTrackingCallback::notifyListenersStatusUpdate )
    }
  };

  QJniEnvironment javaenv;

  jclass objectClass = javaenv->GetObjectClass( mBroadcastReceiver.object<jobject>() );

  javaenv->RegisterNatives( objectClass, methods, 2 );
  javaenv->DeleteLocalRef( objectClass );

  // build the foreground service
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  QAndroidIntent serviceIntent( activity.object(), "uk/co/lutraconsulting/PositionTrackingService" );

  serviceIntent.putExtra( QStringLiteral( "uk.co.lutraconsulting.tracking.distanceInterval" ), mDistanceFilter );
  serviceIntent.putExtra( QStringLiteral( "uk.co.lutraconsulting.tracking.timeInterval" ), mUpdateInterval );

  QJniObject result = activity.callObjectMethod(
                        "startService",
                        "(Landroid/content/Intent;)Landroid/content/ComponentName;",
                        serviceIntent.handle().object() );


  // register position updates from the service to broadcast
  mBroadcastReceiver.callMethod<void>(
    "registerBroadcastReceiver",
    "(Landroid/content/Context;)V",
    QNativeInterface::QAndroidApplication::context()
  );

  // find the file for position updates
  auto path = activity.callMethod<jstring>( "homePath", "()Ljava/lang/String;" );
  QString pathString = path.toString();

  mTrackingFile.setFileName( pathString + "/" + TRACKING_FILE_NAME );
  if ( !mTrackingFile.open( QFile::ReadOnly ) )
  {
    CoreUtils::log(
      QStringLiteral( "Android Tracking Backend" ),
      QStringLiteral( "Tracking file could not be opened for reading: %1" ).arg( pathString )
    );

    emit errorOccured( tr( "There was an error and tracking could not start, please contact support" ) );
    emit abort();
  }

  // go see if something's left from previous run!
  // (in future we could show a dialogue when project is opened and something is in the file)
  sourceUpdatedPosition();
}
