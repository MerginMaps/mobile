/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "position/tracking/androidtrackingbroadcast.h"
#include "coreutils.h"

#include <QtCore/private/qandroidextras_p.h>

bool AndroidTrackingBroadcast::registerBroadcastPrivate()
{
  if ( !mBroadcastReceiver.isValid() )
  {
    CoreUtils::log( QStringLiteral( "Android Tracking Broadcast" ), QStringLiteral( "Invalid broadcast receiver, aborting...!" ) );

    return false;
  }

  if ( mBroadcastIsRegistered )
  {
    return true; // already registered
  }

  JNINativeMethod methods[]
  {
    {
      "notifyListenersPositionUpdated",
      "()V",
      reinterpret_cast<void *>( AndroidTrackingBroadcast::notifyListenersPositionUpdated )
    },
    {
      "notifyListenersStatusUpdate",
      "(Ljava/lang/String;)V",
      reinterpret_cast<void *>( AndroidTrackingBroadcast::notifyListenersStatusUpdate )
    },
    {
      "notifyListenersAliveResponse",
      "(Z)V",
      reinterpret_cast<void *>( AndroidTrackingBroadcast::notifyListenersAliveResponse )
    }
  };

  QJniEnvironment javaenv;

  jclass objectClass = javaenv->GetObjectClass( mBroadcastReceiver.object<jobject>() );

  javaenv->RegisterNatives( objectClass, methods, 3 );
  javaenv->DeleteLocalRef( objectClass );

  mBroadcastReceiver.callMethod<void>(
    "registerBroadcastReceiver",
    "(Landroid/content/Context;)V",
    QNativeInterface::QAndroidApplication::context()
  );

  mBroadcastIsRegistered = true;

  return true;
}

bool AndroidTrackingBroadcast::unregisterBroadcastPrivate()
{
  if ( !mBroadcastReceiver.isValid() )
  {
    CoreUtils::log( QStringLiteral( "Android Tracking Broadcast" ), QStringLiteral( "Can not unregister invalid broadcast" ) );
    return false;
  }

  if ( !mBroadcastIsRegistered )
  {
    return true; // already unregistered
  }

  mBroadcastReceiver.callMethod<void>( "unregisterBroadcastReceiver",
                                       "(Landroid/content/Context;)V",
                                       QNativeInterface::QAndroidApplication::context() );

  // unregister the natives
  QJniEnvironment javaenv;
  jclass objectClass = javaenv->GetObjectClass( mBroadcastReceiver.object<jobject>() );

  javaenv->UnregisterNatives( objectClass );
  javaenv->DeleteLocalRef( objectClass );

  mBroadcastIsRegistered = false;

  return true;
}

void AndroidTrackingBroadcast::sendAliveRequestAsyncPrivate()
{
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  QAndroidIntent serviceIntent( activity.object(), "uk/co/lutraconsulting/PositionTrackingService" );

  serviceIntent.putExtra( QStringLiteral( "uk.co.lutraconsulting.tracking.alive" ), true );

  QJniObject result = activity.callObjectMethod(
                        "startService",
                        "(Landroid/content/Intent;)Landroid/content/ComponentName;",
                        serviceIntent.handle().object() );
}
