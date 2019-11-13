#pragma once

#include <QCoreApplication>
#include <UIKit/UIKit.h>
#include <QPointer>
#include <QtCore>
#include <QImage>

#include "iosinterface.h"
#include "ioshandler.h"
#include "iosviewdelegate.h"

void IOSHandler::showImagePicker()
{
    //Objective C code calling.....
    IOSInterface *obj=[[IOSInterface alloc]init]; //Allocating the new object for the objective C   class we created
    [obj showImagePicker];   //Calling the function we defined
}

// TODO catch invokedMethod from interface, even better in imagepicker
