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
#include <QJniObject>
#include <QJniEnvironment>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QStorageInfo>

#include <QtConcurrent>
#include <QFuture>

#include "coreutils.h"
#include "inpututils.h"
#include "appsettings.h"
#endif

AndroidUtils::AndroidUtils( QObject *parent ): QObject( parent )
{
}

void AndroidUtils::showToast( QString message )
{
#ifdef ANDROID
  QNativeInterface::QAndroidApplication::runOnAndroidMainThread( [message]()
  {
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject javaString = QJniObject::fromString( message );
    QJniObject toast = QJniObject::callStaticObjectMethod(
                         "android.widget.Toast",
                         "makeText",
                         "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
                         activity.object(),
                         javaString.object<jstring>(),
                         jint( 1 ) );
    toast.callMethod<void>( "show" );
  } ).waitForFinished();
#else
  Q_UNUSED( message )
#endif
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

bool AndroidUtils::findLegacyFolder( QString &legacyFolderPath )
{
#ifdef ANDROID
  legacyFolderPath = QStringLiteral();
  QString dataPathRaw = QStringLiteral( "INPUT" );

  QFileInfo extDir( "/sdcard/" );
  if ( extDir.isDir() && extDir.isWritable() )
  {
    // seems that this directory transposes to the latter one in case there is no sdcard attached
    dataPathRaw = extDir.path() + "/" + dataPathRaw;
    QDir d( dataPathRaw );
    if ( d.exists() )
    {
      legacyFolderPath = dataPathRaw;
      return true;
    }
  }
  else
  {
    CoreUtils::log( "$$: Migration", "Ext path " + extDir.path() + " is not readable!" );

    QStringList split = QDir::homePath().split( "/" ); // something like /data/user/0/uk.co.lutraconsulting/files

    QFileInfo usrDir( "/storage/emulated/" + split[2] + "/" );
    dataPathRaw = usrDir.path() + "/" + dataPathRaw;

    if ( usrDir.isDir() && usrDir.isWritable() )
    {
      QDir d( dataPathRaw );
      if ( d.exists() )
      {
        legacyFolderPath = dataPathRaw;
        return true;
      }
    }
  }
#else
  Q_UNUSED( legacyFolderPath );
#endif
  return false;
}

void AndroidUtils::migrateLegacyProjects( const QString &legacyFolder, const QString &scopedStorageFolder )
{
#ifdef ANDROID
  QString legacyFolderProjects = legacyFolder + QStringLiteral( "/projects" );
  QString scopedStorageProjects = scopedStorageFolder + QStringLiteral( "/projects" );

  CoreUtils::log( "LegacyFolderMigration", "Starting copy from:" + legacyFolderProjects +  " to:" +  scopedStorageProjects );

  QDir legacyProjectsDir( legacyFolderProjects );
  QStringList projectsToCopy = legacyProjectsDir.entryList( QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden );

  int cnt = 0;

  emit migrationStarted( projectsToCopy.count() );
  emit migrationProgressed( cnt );

  for ( const QString &project : projectsToCopy )
  {
    bool copyResult = InputUtils::cpDir( legacyFolderProjects + "/" + project, scopedStorageProjects + "/" + project );
    if ( !copyResult )
    {
      CoreUtils::log( "LegacyFolderMigration", "Could not copy project" + project + "!" );
      emit migrationFinished( false );
      return;
    }

    emit migrationProgressed( ++cnt );
  }

  // Final step: rename legacy folder to indicate that projects were successfully migrated
  QDir legacyFolderDir( legacyFolder );
  bool renameSuccessful = legacyFolderDir.rename( legacyFolder, legacyFolder + QStringLiteral( "_migrated" ) );
  if ( !renameSuccessful )
  {
    CoreUtils::log( "LegacyFolderMigration", "Rename not successful, but data are copied" );
    // Even though the folder is not renamed, it is copied, hence do not copy it again in future
    emit migrationFinished( true );
    return;
  }

  emit migrationFinished( true );
#else
  Q_UNUSED( legacyFolder );
  Q_UNUSED( scopedStorageFolder );
#endif
}

void AndroidUtils::handleLegacyFolderMigration( AppSettings *appsettings, bool demoProjectsCopiedThisRun )
{
#ifdef ANDROID
  // Step 1: already migrated? - do not copy
  if ( appsettings->legacyFolderMigrated() )
  {
    CoreUtils::log( "LegacyFolderMigration", "Ignoring legacy folder logic, already migrated!" );
    return;
  }

  // Step 2: is this first run of application (after install or reset of all data)? - do not copy
  if ( demoProjectsCopiedThisRun )
  {
    CoreUtils::log( "LegacyFolderMigration", "Ignoring legacy folder logic, just installed or removed app data!" );
    appsettings->setLegacyFolderMigrated( true );
    return;
  }

  // Step 3: make sure we have a WRITE permission to storage
  // this check should not be that important since previous app versions could not run without this permission - all updated
  // versions will thus have it granted. Anyways..
  QFuture<QtAndroidPrivate::PermissionResult> res = QtAndroidPrivate::checkPermission( "android.permission.WRITE_EXTERNAL_STORAGE" );
  res.waitForFinished();
  if ( res.result() != QtAndroidPrivate::PermissionResult::Authorized )
  {
    if ( !checkAndAcquirePermissions( "android.permission.WRITE_EXTERNAL_STORAGE" ) )
    {
      showToast( tr( "Without storage permission you will not be able to access previous projects" ) );
      CoreUtils::log( "LegacyFolderMigration", "Storage permission not granted after rationale, leaving!" );
      return;
    }
    CoreUtils::log( "LegacyFolderMigration", "Storage permission granted after rationale, continuing!" );
  }

  // Step 4: check existence of legacy folder
  QString legacyFolderPath;
  bool containsLegacyFolder = findLegacyFolder( legacyFolderPath );

  if ( !containsLegacyFolder )
  {
    appsettings->setLegacyFolderMigrated( true );
    CoreUtils::log( "LegacyFolderMigration", "Could not find legacy folder" );
    return;
  }

  // Step 5: is there enough space to copy the folder?
  // https://doc.qt.io/qt-5/qstorageinfo.html
  QDir legacyDir( legacyFolderPath );
  QStorageInfo storage( legacyDir );

  qint64 freeSpace = storage.bytesAvailable();
  qint64 neededSpace = InputUtils::dirSize( legacyFolderPath );

  if ( freeSpace < neededSpace )
  {
    emit notEnoughSpaceLeftToMigrate( InputUtils::bytesToHumanSize( neededSpace ) );
    CoreUtils::log( "LegacyFolderMigration", "Device does not have enough space to copy the folder, needed: " + \
                    InputUtils::bytesToHumanSize( neededSpace ) + \
                    " free space: " + InputUtils::bytesToHumanSize( freeSpace ) );
    return;
  }

  //  Step 6: finally, let's copy the projects folder, project after project
  QtConcurrent::run( &AndroidUtils::migrateLegacyProjects, this, legacyFolderPath, externalStorageAppFolder() );

  CoreUtils::log( "LegacyFolderMigration", "Data migration has been sent to other thread!" );
#else
  Q_UNUSED( appsettings );
  Q_UNUSED( demoProjectsCopiedThisRun );
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

bool AndroidUtils::requestStoragePermission()
{
#ifdef ANDROID

  if ( !checkAndAcquirePermissions( "android.permission.READ_EXTERNAL_STORAGE" ) )
  {
    auto activity = QJniObject( QNativeInterface::QAndroidApplication::context() );
    jboolean res = activity.callMethod<jboolean>( "shouldShowRequestPermissionRationale", "(Ljava/lang/String;)Z", "android.permission.WRITE_EXTERNAL_STORAGE" );
    if ( !res )
    {
      // permanently denied permission, user needs to go to settings to allow permission
      showToast( tr( "Storage permission is permanently denied, please allow it in settings in order to load pictures from gallery" ) );
    }
    else
    {
      showToast( tr( "Input needs a storage permission in order to load pictures from gallery" ) );
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
      showToast( tr( "Camera permission is permanently denied, please allow it in settings" ) );
    }
    else
    {
      showToast( tr( "We need a camera permission in order to take a photo" ) );
    }
    return false;
  }
#endif
  return true;
}

bool AndroidUtils::requestMediaLocationPermission()
{
#ifdef ANDROID
  // ACCESS_MEDIA_LOCATION is a runtime permission without UI dialog (User do not need to click anything to grant it, it is granted automatically)
  return checkAndAcquirePermissions( "android.permission.ACCESS_MEDIA_LOCATION" );
#endif
  return true;
}

void AndroidUtils::callImagePicker()
{
#ifdef ANDROID

  if ( !requestStoragePermission() )
  {
    return;
  }

  // request media location permission to be able to read EXIF metadata from gallery image
  // it is not a mandatory permission, so continue even if it is rejected
  requestMediaLocationPermission();

  QJniObject ACTION_PICK = QJniObject::getStaticObjectField( "android/content/Intent", "ACTION_PICK", "Ljava/lang/String;" );
  QJniObject EXTERNAL_CONTENT_URI = QJniObject::getStaticObjectField( "android/provider/MediaStore$Images$Media", "EXTERNAL_CONTENT_URI", "Landroid/net/Uri;" );

  QJniObject intent = QJniObject( "android/content/Intent", "(Ljava/lang/String;Landroid/net/Uri;)V", ACTION_PICK.object<jstring>(), EXTERNAL_CONTENT_URI.object<jobject>() );

  if ( ACTION_PICK.isValid() && intent.isValid() )
  {
    intent.callObjectMethod( "setType", "(Ljava/lang/String;)Landroid/content/Intent;", QJniObject::fromString( "image/*" ).object<jstring>() );
    QtAndroidPrivate::startActivity( intent.object<jobject>(), MEDIA_CODE, this ); // this as receiver
  }
#endif
}

void AndroidUtils::callCamera( const QString &targetPath )
{
#ifdef ANDROID
  if ( !requestCameraPermission() )
  {
    return;
  }

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
    showToast( errorMsg );
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
    emit imageSelected( selectedImagePath );
  }
  else if ( receiverRequestCode == CAMERA_CODE && resultCode == RESULT_OK )
  {
    QJniObject RESULT_STRING = QJniObject::fromString( QStringLiteral( "__RESULT__" ) );
    QJniObject absolutePathJNI = data.callObjectMethod( "getStringExtra", "(Ljava/lang/String;)Ljava/lang/String;", RESULT_STRING.object<jstring>() );
    QString absolutePath = absolutePathJNI.toString();

    QString selectedImagePath = "file://" + absolutePath;

    emit imageSelected( absolutePath );
  }
  else
  {
    QString msg( "Something went wrong with media store activity" );
    qDebug() << msg;
    showToast( msg );
  }

}
#endif
