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

    /**
      * Starts ACTION_PICK activity which opens a gallery. If an image is selected,
      * handler of the activity emits imageSelected signal.
      * */
    Q_INVOKABLE void callImagePicker();
    Q_INVOKABLE void callCamera( const QString &targetPath );

    Q_INVOKABLE void installQRCodeScanner();
    Q_INVOKABLE void scanQRCode();

#ifdef ANDROID
    const static int MEDIA_CODE = 101;
    const static int CAMERA_CODE = 102;
    const static int BLUETOOTH_CODE = 103;
    const static int INSTALL_QR_SCANNER_CODE = 104;
    const static int QR_SCAN_CODE = 105;

    const static int ANDROID_VERSION_13 = 13;

    void handleActivityResult( int receiverRequestCode, int resultCode, const QJniObject &data ) override;
#endif

  signals:
    void imageSelected( QString imagePath );

    void bluetoothEnabled( bool state );

    void qrScanFinished( QString scanValue );
    void qrScanAborted();
    void qrScannerMissing();

  public slots:
    void showToast( QString message );

  private:

#ifdef ANDROID
    QBluetoothLocalDevice mBluetooth;
#endif
};

#endif // ANDROIDUTILS_H
