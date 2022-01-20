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


#ifdef ANDROID
#include <QAndroidActivityResultReceiver>
#include <QAndroidJniObject>
#include <QBluetoothLocalDevice>
#endif
#include <QObject>

class AppSettings;

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

    // Copies legacy app folder INPUT from external storage to app specific folder based on several rules.
    // Returns true if migration has been done, false otherwise
    void handleLegacyFolderMigration( AppSettings *appsettings, bool demoProjectsCopiedThisRun );

    bool findLegacyFolder( QString &legacyFolderPath );
    void migrateLegacyProjects( const QString &from, const QString &to );

    void turnBluetoothOn();
    bool isBluetoothTurnedOn();

    /**
      * Starts ACTION_PICK activity which opens a gallery. If an image is selected,
      * handler of the activity emits imageSelected signal.
      * */
    Q_INVOKABLE void callImagePicker();
    Q_INVOKABLE void callCamera( const QString &targetPath );
#ifdef ANDROID
    const static int MEDIA_CODE = 101;
    const static int CAMERA_CODE = 102;
    const static int BLUETOOTH_CODE = 103;

    void handleActivityResult( int receiverRequestCode, int resultCode, const QAndroidJniObject &data ) override;
#endif

  signals:
    void imageSelected( QString imagePath );

    void migrationFinished( bool success );
    void migrationProgressed( int progress );
    void migrationStarted( int numOfProjectsToCopy );
    void notEnoughSpaceLeftToMigrate( QString neededSpace );

    void bluetoothEnabled( bool state );

  public slots:
    void showToast( QString message );

  private:
    bool mIsAndroid;

#ifdef ANDROID
    QBluetoothLocalDevice mBluetooth;
#endif
};

#endif // ANDROIDUTILS_H
