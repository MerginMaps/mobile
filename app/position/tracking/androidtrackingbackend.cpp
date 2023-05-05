/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "androidtrackingbackend.h"

#include <QtCore/private/qandroidextras_p.h>

AndroidTrackingBackend *AndroidTrackingBackend::mInstance = nullptr;

static void notifyListenersFromJava( JNIEnv *env, jobject /*this*/, jdouble longitude, jdouble latitude, jdouble altitude )
{
  qDebug() << "Qt printing the data:" << longitude << latitude;

  AndroidTrackingBackend::instance()->update( longitude, latitude, altitude );
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
  // 0. register callback from java
  JNINativeMethod methods[] {{"notifyListenersFromJava", "(DDD)V", reinterpret_cast<void *>( notifyListenersFromJava )}};
  QJniObject broadcastClass( "uk/co/lutraconsulting/PositionTrackingBroadcastMiddleware" );

  QJniEnvironment javaenv;
  jclass objectClass = javaenv->GetObjectClass( broadcastClass.object<jobject>() );

  javaenv->RegisterNatives( objectClass, methods, 1 );
  javaenv->DeleteLocalRef( objectClass );

  // 1. get context object
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );

  // 2. create intent with the foreground service
  QAndroidIntent serviceIntent( activity.object(), "uk/co/lutraconsulting/PositionTrackingService" );

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
