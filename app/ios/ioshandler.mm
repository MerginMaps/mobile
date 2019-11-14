#pragma once

#include <QCoreApplication>
#include <UIKit/UIKit.h>
#include <QPointer>
#include <QtCore>
#include <QImage>

#include "iosinterface.h"
#include "ioshandler.h"

void IOSHandler::showImagePicker()
{
    IOSInterface *obj=[[IOSInterface alloc]init];
    [obj showImagePicker];
}

