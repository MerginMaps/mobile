/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "androidutils.h"

#ifdef ANDROID
#include <QtCore/private/qandroidextras_p.h>
#include <QCoreApplication>
#include <QJniObject>
#include <QJniEnvironment>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

#include "coreutils.h"
#endif

AndroidUtils::AndroidUtils( QObject *parent ): QObject( parent )
{
}

bool AndroidUtils::isAndroid() const
{
#ifdef ANDROID
  return true;
#else
  return false;
#endif
}

bool AndroidUtils::checkAndAcquirePermissions( const QString &permissionString )
{
#ifdef ANDROID
  auto r = QtAndroidPrivate::checkPermission( permissionString ).result();
  if ( r == QtAndroidPrivate::Denied )
  {
    r = QtAndroidPrivate::requestPermission( permissionString ).result();
    if ( r == QtAndroidPrivate::Denied )
    {
      return false;
    }
  }
#else
  Q_UNUSED( permissionString )
#endif
  return true;
}

QString AndroidUtils::externalStorageAppFolder()
{
#ifdef ANDROID
  // AppDataLocation returns two paths, first is internal app storage and the second is external storage
  QStringList paths = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation );
  if ( paths.size() > 1 )
  {
    return paths.at( 1 );
  }
  else
  {
    CoreUtils::log( "StorageException", "Path from QStandardPaths do not include external storage!! Using path: " + paths.at( 0 ) );
    return paths.at( 0 );
  }
#endif

  return QString();
}

bool AndroidUtils::requestNotificationPermission()
{
#ifdef ANDROID
  double buildVersion = QSysInfo::productVersion().toDouble();

  // POST_NOTIFICATIONS permission is available from Android 13+
  if ( buildVersion < ANDROID_VERSION_13 )
  {
    return true;
  }

  QString notificationPermission = QStringLiteral( "android.permission.POST_NOTIFICATIONS" );

  auto r = QtAndroidPrivate::checkPermission( notificationPermission ).result();
  if ( r == QtAndroidPrivate::Authorized )
  {
    return true;
  }

  r = QtAndroidPrivate::requestPermission( notificationPermission ).result();
  if ( r == QtAndroidPrivate::Authorized )
  {
    return true;
  }
#endif
  return false;
}

QString AndroidUtils::readExif( const QString &filePath, const QString &tag )
{
#ifdef ANDROID
  QJniObject jFilePath = QJniObject::fromString( filePath );
  QJniObject jTag = QJniObject::fromString( tag );
  QJniObject attribute = QJniObject::callStaticObjectMethod( "uk.co.lutraconsulting.EXIFUtils",
                         "getEXIFAttribute",
                         "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                         jFilePath.object<jstring>(),
                         jTag.object<jstring>() );
  return attribute.toString();
#else
  Q_UNUSED( filePath )
  Q_UNUSED( tag )
  return QString();
#endif
}

void AndroidUtils::turnBluetoothOn()
{
#ifdef ANDROID
  if ( !isBluetoothTurnedOn() )
  {
    QJniObject ACTION_BT = QJniObject::getStaticObjectField(
                             "android/bluetooth/BluetoothAdapter",
                             "ACTION_REQUEST_ENABLE",
                             "Ljava/lang/String;"
                           );

    QJniObject intent(
      "android/content/Intent",
      "(Ljava/lang/String;)V",
      ACTION_BT.object()
    );

    if ( ACTION_BT.isValid() && intent.isValid() )
    {
      QtAndroidPrivate::startActivity( intent.object<jobject>(), BLUETOOTH_CODE, this );
    }
  }
#endif
}

bool AndroidUtils::isBluetoothTurnedOn()
{
#ifdef ANDROID
  return mBluetooth.hostMode() != QBluetoothLocalDevice::HostPoweredOff;
#else
  return true;
#endif
}

void AndroidUtils::quitApp()
{
#ifdef ANDROID
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  activity.callMethod<void>( "quitGracefully", "()V" );

  // If quitGracefully failed or this device is not of specified manufacturer, let's exit via QT
  QCoreApplication::quit();
#endif
}

QString AndroidUtils::getManufacturer()
{
  QString manufacturer = "";
#ifdef ANDROID
  QJniObject activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  manufacturer = activity.callObjectMethod( "getManufacturer", "()Ljava/lang/String;" ).toString().toUpper();
#endif
  return manufacturer;
}

QString AndroidUtils::getDeviceModel()
{
  QString deviceModel = "";
#ifdef ANDROID
  QJniObject activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  deviceModel = activity.callObjectMethod( "getDeviceModel", "()Ljava/lang/String;" ).toString();
#endif
  return deviceModel;
}

QVector<int> AndroidUtils::getSafeArea()
{
  QVector<int> ret;

#ifdef ANDROID
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  auto safeArrayStringObj = activity.callMethod<jintArray>( "getSafeArea", "()Ljava/lang/String;" );

  if ( safeArrayStringObj.isValid() )
  {
    QString safeArrayString = safeArrayStringObj.toString();

    QStringList stringParts = safeArrayString.split( "," );
    if ( stringParts.length() != 4 )
    {
      CoreUtils::log( "SafeArea", "Android returned malformed string from getSafeArea method" );
      return ret;
    }

    int top = stringParts[0].toInt(); // top inset
    int right = stringParts[1].toInt(); // right inset
    int bottom = stringParts[2].toInt(); // bottom inset
    int left = stringParts[3].toInt(); // left inset

    ret << top << right << bottom << left;
    return ret;
  }
  else
  {
    CoreUtils::log( "SafeArea", "Android returned null from getSafeArea method" );
    return ret;
  }
#endif
  return ret;
}

void AndroidUtils::hideSplashScreen()
{
#ifdef ANDROID
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  activity.callMethod<void>( "hideSplashScreen", "()V" );
#endif
}

bool AndroidUtils::openFile( const QString &filePath )
{
  bool result = false;
#ifdef ANDROID
  auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
  QJniObject jFilePath = QJniObject::fromString( filePath );
  result = activity.callMethod<jboolean>( "openFile", "(Ljava/lang/String;)Z", jFilePath.object<jstring>() );
#endif
  return result;
}

bool AndroidUtils::requestStoragePermission()
{
#ifdef ANDROID
  const double buildVersion = QSysInfo::productVersion().toDouble();

  //
  // Android SDK 33 has a new set of permissions when reading external storage.
  // See https://developer.android.com/reference/android/Manifest.permission#READ_EXTERNAL_STORAGE
  //

  if ( buildVersion < ANDROID_VERSION_13 && !checkAndAcquirePermissions( QStringLiteral( "android.permission.READ_EXTERNAL_STORAGE" ) ) )
  {
    auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
    const jboolean res = activity.callMethod<jboolean>( "shouldShowRequestPermissionRationale", "(Ljava/lang/String;)Z", QJniObject::fromString( "android.permission.WRITE_EXTERNAL_STORAGE" ).object() );
    if ( !res )
    {
      // permanently denied permission, user needs to go to settings to allow permission
      emit notifyInfo( tr( "Storage permission is permanently denied, please allow it in settings in order to load pictures from gallery" ) );
    }
    else
    {
      emit notifyInfo( tr( "Mergin Maps needs a storage permission in order to load pictures from gallery" ) );
    }
    return false;
  }
#endif
  return true;
}

bool AndroidUtils::requestCameraPermission()
{
#ifdef ANDROID
  if ( checkAndAcquirePermissions( "android.permission.CAMERA" ) == false )
  {
    auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
    jboolean res = activity.callMethod<jboolean>( "shouldShowRequestPermissionRationale", "(Ljava/lang/String;)Z", QJniObject::fromString( "android.permission.CAMERA" ).object() );
    if ( !res )
    {
      // permanently denied permission, user needs to go to settings to allow permission
      emit notifyInfo( tr( "Camera permission is permanently denied, please allow it in settings" ) );
    }
    else
    {
      emit notifyInfo( tr( "We need a camera permission in order to take a photo" ) );
    }
    return false;
  }
#endif
  return true;
}

bool AndroidUtils::requestMediaLocationPermission()
{
#ifdef ANDROID
  const double buildVersion = QSysInfo::productVersion().toDouble();
  // ACCESS_MEDIA_LOCATION is a runtime permission without UI dialog (User do not need to click anything to grant it, it is granted automatically)
  if ( buildVersion < ANDROID_VERSION_13 )
  {
    return checkAndAcquirePermissions( "android.permission.ACCESS_MEDIA_LOCATION" );
  }
#endif
  return true;
}

void AndroidUtils::callImagePicker( const QString &code )
{
#ifdef ANDROID

  if ( !requestStoragePermission() )
  {
    return;
  }

  mLastCode = code;

  // request media location permission to be able to read EXIF metadata from gallery image (only necessary for android < 14)
  // it is not a mandatory permission, so continue even if it is rejected
  requestMediaLocationPermission();

  const QJniObject ACTION_GET_CONTENT = QJniObject::getStaticObjectField( "android/content/Intent", "ACTION_GET_CONTENT", "Ljava/lang/String;" );

  QJniObject intent = QJniObject( "android/content/Intent", "(Ljava/lang/String;)V", ACTION_GET_CONTENT.object<jstring>() );

  if ( ACTION_GET_CONTENT.isValid() && intent.isValid() )
  {
    intent = intent.callObjectMethod( "setType", "(Ljava/lang/String;)Landroid/content/Intent;", QJniObject::fromString( "image/*" ).object<jstring>() );
    QtAndroidPrivate::startActivity( intent.object<jobject>(), MEDIA_CODE, this ); // this as receiver
  }
#endif
}

void AndroidUtils::callCamera( const QString &targetPath, const QString &code )
{
#ifdef ANDROID
  if ( !requestCameraPermission() )
  {
    return;
  }

  mLastCode = code;

  // request media location permission to be able to read EXIF metadata from captured image
  // it is not a mandatory permission, so continue even if it is rejected
  requestMediaLocationPermission();

  const QString IMAGE_CAPTURE_ACTION = QString( "android.media.action.IMAGE_CAPTURE" );

  QJniObject activity = QJniObject::fromString( QStringLiteral( "uk.co.lutraconsulting.CameraActivity" ) );
  QJniObject intent = QJniObject( "android/content/Intent", "(Ljava/lang/String;)V", activity.object<jstring>() );

  QJniObject packageName = QJniObject::fromString( QStringLiteral( "uk.co.lutraconsulting" ) );
  QJniObject className = QJniObject::fromString( QStringLiteral( "uk.co.lutraconsulting.CameraActivity" ) );

  intent.callObjectMethod( "setClassName", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;", packageName.object<jstring>(), className.object<jstring>() );

  QJniObject extra = QJniObject::fromString( "targetPath" );
  QJniObject my_prefix = QJniObject::fromString( targetPath );

  intent.callObjectMethod( "putExtra",
                           "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                           extra.object<jstring>(),
                           my_prefix.object<jstring>() );

  if ( intent.isValid() )
  {
    QtAndroidPrivate::startActivity( intent.object<jobject>(), CAMERA_CODE, this );
  }
#else
  Q_UNUSED( targetPath )
#endif
}

#ifdef ANDROID
void AndroidUtils::handleActivityResult( int receiverRequestCode, int resultCode, const QJniObject &data )
{
  jint RESULT_OK = QJniObject::getStaticField<jint>( "android/app/Activity", "RESULT_OK" );
  jint RESULT_CANCELED = QJniObject::getStaticField<jint>( "android/app/Activity", "RESULT_CANCELED" );

  if ( receiverRequestCode == BLUETOOTH_CODE )
  {
    if ( resultCode == RESULT_OK )
    {
      emit bluetoothEnabled( true );
    }
    else
    {
      emit bluetoothEnabled( false );
    }

    return;
  }

  if ( resultCode == RESULT_CANCELED )
  {
    QJniObject RESULT_STRING = QJniObject::fromString( QStringLiteral( "__RESULT__" ) );
    // User has triggered cancel, result has no data.
    if ( !data.isValid() )
    {
      return;
    }

    QJniObject errorJNI = data.callObjectMethod( "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;", RESULT_STRING.object<jstring>() );
    // Internal cancelation due to an error
    QString errorMsg = errorJNI.toString();
    emit notifyError( errorMsg );
    return;
  }

  if ( receiverRequestCode == MEDIA_CODE && resultCode == RESULT_OK )
  {
    QJniObject uri = data.callObjectMethod( "getData", "()Landroid/net/Uri;" );
    QJniObject mediaStore = QJniObject::getStaticObjectField( "android/provider/MediaStore$MediaColumns", "DATA", "Ljava/lang/String;" );
    QJniEnvironment env;
    jobjectArray projection = ( jobjectArray )env->NewObjectArray( 1, env->FindClass( "java/lang/String" ), NULL );
    jobject projectionDataAndroid = env->NewStringUTF( mediaStore.toString().toStdString().c_str() );
    env->SetObjectArrayElement( projection, 0, projectionDataAndroid );
    auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
    QJniObject contentResolver = activity.callObjectMethod( "getContentResolver", "()Landroid/content/ContentResolver;" );
    QJniObject cursor = contentResolver.callObjectMethod( "query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", uri.object<jobject>(), projection, NULL, NULL, NULL );
    jint columnIndex = cursor.callMethod<jint>( "getColumnIndex", "(Ljava/lang/String;)I", mediaStore.object<jstring>() );
    cursor.callMethod<jboolean>( "moveToFirst", "()Z" );
    QJniObject result = cursor.callObjectMethod( "getString", "(I)Ljava/lang/String;", columnIndex );
    QString selectedImagePath = "file://" + result.toString();
    emit imageSelected( selectedImagePath, mLastCode );
  }
  else if ( receiverRequestCode == CAMERA_CODE && resultCode == RESULT_OK )
  {
    QJniObject RESULT_STRING = QJniObject::fromString( QStringLiteral( "__RESULT__" ) );
    QJniObject absolutePathJNI = data.callObjectMethod( "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;", RESULT_STRING.object<jstring>() );
    QString absolutePath = absolutePathJNI.toString();

    QString selectedImagePath = "file://" + absolutePath;

    emit imageSelected( absolutePath, mLastCode );
  }
  else
  {
    QString msg( "Something went wrong with media store activity" );
    qDebug() << msg;
    emit notifyError( msg );
  }

}
#endif
