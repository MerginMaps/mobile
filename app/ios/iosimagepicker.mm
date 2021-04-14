/***************************************************************************
  iosimagepicker.mm
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

#include <QCoreApplication>
#include <UIKit/UIKit.h>
#include <QPointer>
#include <QtCore>
#include <QImage>

#include "iosinterface.h"
#include "iosimagepicker.h"

void IOSImagePicker::showImagePickerDirect( IOSImagePicker *handler )
{
  [IOSInterface showImagePicker:UIImagePickerControllerSourceType::UIImagePickerControllerSourceTypePhotoLibrary:handler];
}

void IOSImagePicker::callCameraDirect( IOSImagePicker *handler )
{
  [IOSInterface showImagePicker:UIImagePickerControllerSourceType::UIImagePickerControllerSourceTypeCamera:handler];
}

QString IOSImagePicker::readExifDirect( const QString &filepath, const QString &tag )
{
  NSString *result = [IOSInterface readExif:filepath.toNSString():tag.toNSString()];
  if ( result )
    return QString::fromNSString( result );
  else
    return QString();
}
