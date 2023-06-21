/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "androidtrackingbackend.h"
#include "coreutils.h"

#include <QtCore/private/qandroidextras_p.h>

AndroidTrackingBackend *AndroidTrackingBackend::mInstance = nullptr;

static void notifyListenersPositionUpdated( JNIEnv *env, jobject /*this*/, jdouble longitude, jdouble latitude, jdouble altitude )
{
  AndroidTrackingBackend::instance()->update( longitude, latitude, altitude );
}

static void notifyListenersStatusUpdate( JNIEnv *env, jobject /*this*/, jstring message )
{
  CoreUtils::log( QStringLiteral( "Android Tracking Backend" ), env->GetStringUTFChars( message, 0 ) );
}

AndroidTrackingBackend::AndroidTrackingBackend( AbstractTrackingBackend::UpdateFrequency frequency, QObject *parent )
  : AbstractTrackingBackend{ frequency, AbstractTrackingBackend::SignalSlotSupport::NotSupported, parent }
{
  if ( !mInstance )
  {
    mInstance = this;
  }

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

  startForegroundService();
}

AndroidTrackingBackend::~AndroidTrackingBackend()
{
  // stop the foreground service
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  QAndroidIntent serviceIntent( activity.object(), "uk/co/lutraconsulting/PositionTrackingService" );

  activity.callMethod<jboolean>(
    "stopService",
    "(Landroid/content/Intent;)Z",
    serviceIntent.handle().object() );
}

void AndroidTrackingBackend::update( double longitude, double latitude, double altitude )
{
  GeoPosition position;

  position.longitude = longitude;
  position.latitude = latitude;
  position.elevation = altitude;
  notifyListeners( position );
}

void AndroidTrackingBackend::startForegroundService()
{
  // 0. register callbacks from java
  JNINativeMethod methods[] {{"notifyListenersPositionUpdated", "(DDD)V", reinterpret_cast<void *>( notifyListenersPositionUpdated )},
    {"notifyListenersStatusUpdate", "(Ljava/lang/String;)V", reinterpret_cast<void *>( notifyListenersStatusUpdate )}
  };
  QJniObject broadcastClass( "uk/co/lutraconsulting/PositionTrackingBroadcastMiddleware" );

  QJniEnvironment javaenv;
  jclass objectClass = javaenv->GetObjectClass( broadcastClass.object<jobject>() );

  javaenv->RegisterNatives( objectClass, methods, 2 );
  javaenv->DeleteLocalRef( objectClass );

  // 1. get context object
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );

  // 2. create intent with the foreground service
  QAndroidIntent serviceIntent( activity.object(), "uk/co/lutraconsulting/PositionTrackingService" );
  serviceIntent.putExtra( QStringLiteral( "uk.co.lutraconsulting.tracking.distanceInterval" ), mDistanceFilter );
  serviceIntent.putExtra( QStringLiteral( "uk.co.lutraconsulting.tracking.timeInterval" ), mUpdateInterval );

  // 3. build service with the intent
  QJniObject result = activity.callObjectMethod(
                        "startService",
                        "(Landroid/content/Intent;)Landroid/content/ComponentName;",
                        serviceIntent.handle().object() );


  // 4. register position updates from the service to broadcast
  QJniEnvironment env;
  jclass javaClass = env.findClass( "uk/co/lutraconsulting/PositionTrackingBroadcastMiddleware" );
  QJniObject classObject( javaClass );

  classObject.callMethod<void>( "registerServiceBroadcastReceiver",
                                "(Landroid/content/Context;)V",
                                QNativeInterface::QAndroidApplication::context() );
}
