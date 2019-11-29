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

void IOSImagePicker::showImagePickerDirect(int sourceType, IOSImagePicker* handler)
{
    IOSInterface *obj=[[IOSInterface alloc]init];
    [obj showImagePicker:sourceType:handler];
}
