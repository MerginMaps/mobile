/***************************************************************************
  ioshandler.mm
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

#pragma once

#include <QCoreApplication>
#include <UIKit/UIKit.h>
#include <QPointer>
#include <QtCore>
#include <QImage>

#include "iosinterface.h"
#include "ioshandler.h"

void IOSHandler::showImagePicker(int sourceType, IOSHandler* handler)
{
    IOSInterface *obj=[[IOSInterface alloc]init];
    [obj showImagePicker:sourceType:handler];
}

