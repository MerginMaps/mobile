/***************************************************************************
  iosinterface.mm
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

#include <QtCore>
#include <QImage>
#import "ios/iosinterface.h"
#include "iosviewdelegate.h"

@implementation IOSInterface

static UIImagePickerController* imagePickerController = nullptr;
static UIActivityIndicatorView* imagePickerIndicatorView = nullptr;

static QString fromNSUrl(NSURL* url) {
    return QString::fromNSString([url absoluteString]);
}

static QImage fromUIImage(UIImage* image) {
    QImage::Format format = QImage::Format_RGB32;

    CGColorSpaceRef colorSpace = CGImageGetColorSpace(image.CGImage);
    CGFloat width = image.size.width;
    CGFloat height = image.size.height;

    int orientation = [image imageOrientation];
    int degree = 0;

    switch (orientation) {
    case UIImageOrientationLeft:
        degree = -90;
        break;
    case UIImageOrientationDown: // Down
        degree = 180;
        break;
    case UIImageOrientationRight:
        degree = 90;
        break;
    }

    if (degree == 90 || degree == -90)  {
        CGFloat tmp = width;
        width = height;
        height = tmp;
    }

    QSize size(width,height);

    QImage result = QImage(size,format);

    CGContextRef contextRef = CGBitmapContextCreate(result.bits(),                 // Pointer to  data
                                                   width,                       // Width of bitmap
                                                   height,                       // Height of bitmap
                                                   8,                          // Bits per component
                                                   result.bytesPerLine(),              // Bytes per row
                                                   colorSpace,                 // Colorspace
                                                   kCGImageAlphaNoneSkipFirst |
                                                   kCGBitmapByteOrder32Little); // Bitmap info flags

    CGContextDrawImage(contextRef, CGRectMake(0, 0, width, height), image.CGImage);
    CGContextRelease(contextRef);

    if (degree != 0) {
        QTransform myTransform;
        myTransform.rotate(degree);
        result = result.transformed(myTransform,Qt::SmoothTransformation);
    }

    return result;
}

-(void)showImagePicker:(int)sourceType:(IOSImagePicker*)handler
{
    UIApplication* app = [UIApplication sharedApplication];

    if (app.windows.count <= 0) {
        return;
    }

    UIWindow* rootWindow = app.windows[0];
    UIViewController* rootViewController = rootWindow.rootViewController;

  if (![UIImagePickerController isSourceTypeAvailable:(UIImagePickerControllerSourceType) sourceType]) {


    NSString *alertTitle = @"Image picker";
    NSString *alertMessage = @"The functionality is not available";
    NSString *alertOkButtonText = @"Ok";

    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:alertTitle
                                                                             message:alertMessage
                                                                      preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *actionOk = [UIAlertAction actionWithTitle:alertOkButtonText
                                                       style:UIAlertActionStyleDefault
                                                     handler:nil]; //You can use a block here to handle a press on this button
    [alertController addAction:actionOk];
    [rootViewController presentViewController:alertController animated:YES completion:nil];
} else
{
    UIImagePickerController *picker = [[UIImagePickerController alloc] init];
    imagePickerController = picker;
    picker.sourceType = (UIImagePickerControllerSourceType) sourceType;
    static IOSViewDelegate *delegate = nullptr;
    delegate = [[IOSViewDelegate alloc] initWithHandler:handler];

    // Confirm event
    delegate->imagePickerControllerDidFinishPickingMediaWithInfo = ^(UIImagePickerController *picker, NSDictionary* info) {
        Q_UNUSED(picker)

        UIImage *chosenImage = info[UIImagePickerControllerEditedImage];
        if (!chosenImage) {
            chosenImage = info[UIImagePickerControllerOriginalImage];
        }

        if (chosenImage) {
            QImage image = fromUIImage(chosenImage);
            QVariantMap data;
            data["image"] = image;

            if (delegate->handler) {

            QMetaObject::invokeMethod(delegate->handler,"onImagePickerFinished",Qt::DirectConnection,
                                      Q_ARG(bool, true),
                                      Q_ARG(const QVariantMap, data));
            }


        }

        delegate = nil;
        [picker dismissViewControllerAnimated:YES completion:nil];
        
    };


    // Cancel event
    delegate->imagePickerControllerDidCancel = ^(UIImagePickerController *picker) {
        qWarning() << "Image Picker: Cancel event (imagePickerControllerDidCancel)";
        [picker dismissViewControllerAnimated:YES completion:nil];
    };
    
    picker.delegate = delegate;
    imagePickerIndicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    imagePickerIndicatorView.center = picker.view.center;
    [picker.view addSubview:imagePickerIndicatorView];

    [rootViewController presentViewController:picker animated:YES completion:nil];
    
  }
}
@end
