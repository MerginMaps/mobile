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

class IosUtils: public QObject
{
    Q_OBJECT
    Q_PROPERTY( bool isIos READ isIos CONSTANT )
    Q_PROPERTY( IOSImagePicker *mImagePicker READ imagePicker )

  public:
    explicit IosUtils( QObject *parent = nullptr );
    bool isIos() const;

    Q_INVOKABLE void callImagePicker( const QString &targetPath );
    Q_INVOKABLE void callCamera( const QString &targetPath );
    IOSImagePicker *imagePicker() const;
  signals:
    void imageCaptured( const QString &absoluteImagePath );
    void imageSelected( QString imagePath );
  private:

    IOSImagePicker *mImagePicker;
};

#endif // IOSUTILS_H
