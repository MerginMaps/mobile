/***************************************************************************
  iosutils.h
  --------------------------------------
  Date                 : Sept 2019
  Copyright            : (C) 2019 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IOSUTILS_H
#define IOSUTILS_H

#include <QObject>

#include "iosimagepicker.h"
#include "position/positionkit.h"
#include "compass.h"

class IosUtils: public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool isIos READ isIos CONSTANT )
    Q_PROPERTY( IOSImagePicker *mImagePicker READ imagePicker )
    Q_PROPERTY( PositionKit *positionKit MEMBER mPositionKit NOTIFY positionKitChanged )
    Q_PROPERTY( Compass *compass MEMBER mCompass NOTIFY compassChanged )

  public:
    explicit IosUtils( QObject *parent = nullptr );
    bool isIos() const;

    Q_INVOKABLE void callImagePicker( const QString &targetPath, const QString &code = "" );
    Q_INVOKABLE void callCamera( const QString &targetPath, const QString &code = "" );
    IOSImagePicker *imagePicker() const;
    static QString readExif( const QString &filepath, const QString &tag );

    Q_INVOKABLE QVector<int> getSafeArea();

    Q_INVOKABLE static bool openFile( const QString &filePath );

    static Q_INVOKABLE QString getManufacturer();
    static Q_INVOKABLE QString getDeviceModel();

#ifdef Q_OS_IOS
    static Q_INVOKABLE void vibrate();
#endif

  signals:
    void imageSelected( const QString &imagePath, const QString &code );
    void notifyError( const QString &message );
    void positionKitChanged();
    void compassChanged();

  private:
    IOSImagePicker *mImagePicker = nullptr;
    PositionKit *mPositionKit = nullptr;
    Compass *mCompass = nullptr;

    QString mLastCode;
    /**
     * Calls the objective-c function to disable idle timer to prevent screen from sleeping.
     */
    void setIdleTimerDisabled();

    QVector<int> getSafeAreaImpl();

    static QString getManufacturerImpl();
    static QString getDeviceModelImpl();
    static bool openFileImpl( const QString &filePath );
};

#endif // IOSUTILS_H
