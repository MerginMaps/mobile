/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANDROIDUTILS_H
#define ANDROIDUTILS_H

#include "inputconfig.h"

#ifdef ANDROID
#include <QtCore/private/qandroidextras_p.h>
#include <QJniObject>
#include <QBluetoothLocalDevice>
#endif
#include <QObject>

class AndroidUtils: public QObject
#ifdef ANDROID
  , QAndroidActivityResultReceiver
#endif
{
    Q_OBJECT
    Q_PROPERTY( bool isAndroid READ isAndroid CONSTANT )

  public:
    explicit AndroidUtils( QObject *parent = nullptr );

    bool isAndroid() const;

    bool checkAndAcquirePermissions( const QString &permissionString );

    static QString externalStorageAppFolder();

    // Android 13+ needs permission for sending notifications
    static bool requestNotificationPermission();

    /**
     * Reads EXIF and returns value for given parameters.
     * @param filePath Absolute path to a file
     * @param tag EXIF string tag
     * @return String value of EXIF attribute for given parameters. Note that rational numbers are still in rational string format.
     */
    static QString readExif( const QString &filePath, const QString &tag );

    bool requestStoragePermission();
    bool requestCameraPermission();
    bool requestMediaLocationPermission();

    void turnBluetoothOn();
    bool isBluetoothTurnedOn();

    static void quitApp();

    Q_INVOKABLE QVector<int> getSafeArea();

    void hideSplashScreen();

    /**
      * Starts ACTION_PICK activity which opens a gallery. If an image is selected,
      * handler of the activity emits imageSelected signal.
      * The code parameter will be used in response (signal)
      */
    Q_INVOKABLE void callImagePicker( const QString &code = "" );
    Q_INVOKABLE void callCamera( const QString &targetPath, const QString &code = "" );

#ifdef ANDROID
    const static int MEDIA_CODE = 101;
    const static int CAMERA_CODE = 102;
    const static int BLUETOOTH_CODE = 103;

    const static int ANDROID_VERSION_13 = 13;

    void handleActivityResult( int receiverRequestCode, int resultCode, const QJniObject &data ) override;
#endif

  signals:
    void imageSelected( QString imagePath, QString code );
    void bluetoothEnabled( bool state );
    void notifyInfo( const QString &msg );
    void notifyError( const QString &msg );

  private:
    QString mLastCode;

#ifdef ANDROID
    QBluetoothLocalDevice mBluetooth;
#endif
};

#endif // ANDROIDUTILS_H
