/***************************************************************************
  ioshandler.h
  --------------------------------------
  Date                 : Nov 2019
  Copyright            : (C) 2019 by Viktor Sklencar
  Email                : viktor.sklencar@lutraconsulting.co.uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IOSHANDLER_H
#define IOSHANDLER_H

#include <QObject>
#include <QWidget>
#include <QVariantMap>
//#include "ios/iosimagepicker.h"

/**
 * The intermediate class between objective-c and cpp. It calls and forwards methods from both side
 * and therefore provides communication between them.
*/
class IOSHandler : public QObject
{
    Q_OBJECT
  public:
    explicit IOSHandler( QObject *parent = nullptr );
#ifdef Q_OS_IOS
    /**
     * Method calls IOSInterface::showImagePicker which invokes IOSViewDelegate and Image picker
     */
    Q_INVOKABLE static void showImagePicker( int sourceType, IOSHandler *handler );
#endif

    //! Instance used for invoking method from objective-c code
    static IOSHandler *instance();

  public slots:
    //! Middle step method between iOSInterface and iOSImagePicker
    void imagePickerFinished( bool successful, QVariantMap data );

  signals:
    //! Forwarded signal from iOSInterface's ImagePicker
    void forwardedImagePickerFinished( bool successful, QVariantMap data );
};

#endif // IOSHANDLER_H
