/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "androidpositionprovider.h"
#include "coreutils.h"

#include "qgis.h"

#include <QCoreApplication>
#include <QLocationPermission>

#include <android/log.h>

#include <QTimeZone>


int AndroidPositionProvider::sLastInstanceId = 0;
QMap<int, AndroidPositionProvider *> AndroidPositionProvider::sInstances;


void jniOnPositionUpdated( JNIEnv *env, jclass clazz, jint instanceId, jobject locationObj, jobject gnssStatusObj )
{
  AndroidPositionProvider *inst = AndroidPositionProvider::sInstances[instanceId];
  if ( !inst )
  {
    __android_log_print( ANDROID_LOG_ERROR, "CPP", "[c++] unknown instance! %d", instanceId );
    return;
  }

  QJniObject location( locationObj );
  if ( !location.isValid() )
  {
    __android_log_print( ANDROID_LOG_ERROR, "CPP", "[c++] invalid location obj" );
    return;
  }

  const jdouble latitude = location.callMethod<jdouble>( "getLatitude" );
  const jdouble longitude = location.callMethod<jdouble>( "getLongitude" );
  const jlong timestamp = location.callMethod<jlong>( "getTime" );

  GeoPosition pos;
  pos.latitude = latitude;
  pos.longitude = longitude;
  pos.utcDateTime = QDateTime::fromMSecsSinceEpoch( timestamp, QTimeZone::UTC );

  if ( location.callMethod<jboolean>( "hasAltitude" ) )
  {
    const jdouble value = location.callMethod<jdouble>( "getAltitude" );
    if ( !qFuzzyIsNull( value ) )
      pos.elevation = value;
  }

  // TODO: we are getting ellipsoid elevation here. From API level 34 (Android 14),
  // there is AltitudeConverter() class in Java that can be used to add MSL altitude
  // to Location object. How to deal with this correctly? (we could also convert
  // to MSL (orthometric) altitude ourselves if we add geoid model to our APK

  // horizontal accuracy
  if ( location.callMethod<jboolean>( "hasAccuracy" ) )
  {
    const jfloat accuracy = location.callMethod<jfloat>( "getAccuracy" );
    if ( !qFuzzyIsNull( accuracy ) )
      pos.hacc = accuracy;
  }

  // vertical accuracy (available since API Level 26 (Android 8.0))
  if ( QNativeInterface::QAndroidApplication::sdkVersion() >= 26 )
  {
    if ( location.callMethod<jboolean>( "hasVerticalAccuracy" ) )
    {
      const jfloat accuracy = location.callMethod<jfloat>( "getVerticalAccuracyMeters" );
      if ( !qFuzzyIsNull( accuracy ) )
        pos.vacc = accuracy;
    }
  }

  // ground speed
  if ( location.callMethod<jboolean>( "hasSpeed" ) )
  {
    const jfloat speed = location.callMethod<jfloat>( "getSpeed" );
    if ( !qFuzzyIsNull( speed ) )
      pos.speed = speed * 3.6; // convert from m/s to km/h

    // could also use getSpeedAccuracyMetersPerSecond() since API level 26 (Android 8.0)
  }

  // bearing
  if ( location.callMethod<jboolean>( "hasBearing" ) )
  {
    const jfloat bearing = location.callMethod<jfloat>( "getBearing" );
    if ( !qFuzzyIsNull( bearing ) )
      pos.direction = bearing;

    // could also use getBearingAccuracyDegrees() since API level 26 (Android 8.0)
  }

  // could also use isMock() to detect if location is mocked
  // (may useful to check if 3rd party app is setting it for external GNSS receiver)

  // could also use getExtras() to get further details from mocked location
  // (the key/value pairs are vendor-specific, and could include things like DOP,
  // info about corrections, geoid undulation, receiver model)

  __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] pos %f %f", latitude, longitude );

  QJniObject gnssStatus( gnssStatusObj );
  if ( gnssStatus.isValid() )
  {
    int satellitesUsed = 0;
    const int satellitesCount = gnssStatus.callMethod<jint>( "getSatelliteCount" );
    for ( int i = 0; i < satellitesCount; ++i )
    {
      if ( gnssStatus.callMethod<jboolean>( "usedInFix", i ) )
        ++satellitesUsed;

      // we could get more info here (ID, azimuth, elevation, signal strength, ...)
      // but we are not using that anywhere
    }

    pos.satellitesVisible = satellitesCount;
    pos.satellitesUsed = satellitesUsed;
  }

  QMetaObject::invokeMethod( inst, "positionChanged",
                             Qt::AutoConnection, Q_ARG( GeoPosition, pos ) );

}


AndroidPositionProvider::AndroidPositionProvider( bool fused, QObject *parent )
  : AbstractPositionProvider( fused ? QStringLiteral( "android_fused" ) : QStringLiteral( "android_gps" ),
                              QStringLiteral( "internal" ),
                              fused ? tr( "Android (fused)" ) : tr( "Android (gps)" ), parent )
  , mFused( fused )
  , mInstanceId( ++sLastInstanceId )
{
  __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] CONSTRUCT" );

  Q_ASSERT( !sInstances.contains( mInstanceId ) );
  sInstances[mInstanceId] = this;

  // register the native methods

  JNINativeMethod methods[]
  {
    {
      "jniOnPositionUpdated",
      "(ILandroid/location/Location;Landroid/location/GnssStatus;)V",
      reinterpret_cast<void *>( jniOnPositionUpdated )
    }
  };

  QJniEnvironment javaenv;

  javaenv.registerNativeMethods( "uk/co/lutraconsulting/MMAndroidPosition", methods, 1 );

  // create the Java object

  __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] create Java object" );

  jobject context = QNativeInterface::QAndroidApplication::context();

  mAndroidPos = QJniObject::callStaticObjectMethod( "uk/co/lutraconsulting/MMAndroidPosition", "createWithJniCallback",
                "(Landroid/content/Context;ZI)Luk/co/lutraconsulting/MMAndroidPosition;", context, mFused, mInstanceId );

  // Request permissions if needed

  QLocationPermission perm;
  perm.setAccuracy( QLocationPermission::Precise );
  if ( qApp->checkPermission( perm ) != Qt::PermissionStatus::Granted )
  {
    // if user previously completely denied location, the permissions request dialog
    // may not even show up and we get denied response again.
    __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] going to request permissions" );
    qApp->requestPermission( perm, [this]( const QPermission & p )
    {
      if ( p.status() == Qt::PermissionStatus::Granted )
      {
        __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] permissions granted!" );
        this->startUpdates();
      }
      else
      {
        __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] permissions denied :-(" );
        // User may have granted permission just for approximate location,
        // so it would be good to detect that and warn about that: approximate
        // location has intentionally only ~2km accuracy - too low for any data collection
        QLocationPermission permApprox;
        permApprox.setAccuracy( QLocationPermission::Approximate );
        if ( qApp->checkPermission( permApprox ) == Qt::PermissionStatus::Granted )
          this->setState( tr( "Approximate location only!" ), State::NoConnection );
        else
          this->setState( tr( "No location permissions" ), State::NoConnection );
      }
    } );
    return;
  }

  // TODO: this should not be needed?
  AndroidPositionProvider::startUpdates();
}

AndroidPositionProvider::~AndroidPositionProvider()
{
  __android_log_print( ANDROID_LOG_INFO, "CPP", "DESTRUCT" );

  Q_ASSERT( sInstances[mInstanceId] == this );
  sInstances.remove( mInstanceId );

}

void AndroidPositionProvider::startUpdates()
{
  __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] start updates" );

  jboolean res = mAndroidPos.callMethod<jboolean>( "start", "()Z" );

  __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] start updates res: %d", res );

  if ( !res )
  {
    QJniObject errMsgJni = mAndroidPos.callObjectMethod( "errorMessage", "()Ljava/lang/String;" );
    QString errMsg = errMsgJni.toString();
    __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] error: %s", errMsg.toUtf8().constData() );
    if ( errMsg == "MISSING_PERMISSIONS" )
      setState( tr( "No location permissions" ), State::NoConnection );
    else if ( errMsg == "FUSED_NOT_AVAILABLE" )
      setState( tr( "Fused location not available" ), State::NoConnection );
    else
      setState( errMsg, State::NoConnection );
    return;
  }

  setState( tr( "Waiting for fix..." ), State::Connected );
}

void AndroidPositionProvider::stopUpdates()
{
  __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] stop updates" );

  jboolean res = mAndroidPos.callMethod<jboolean>( "stop", "()Z" );

  __android_log_print( ANDROID_LOG_INFO, "CPP", "[c++] stop updates res: %d", res );

}

void AndroidPositionProvider::closeProvider()
{
  stopUpdates();

  mAndroidPos = QJniObject();
}
