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
#import <ImageIO/ImageIO.h>
#import "ios/iosinterface.h"
#include "iosviewdelegate.h"
#include "inpututils.h"
#include "imageutils.h"
#import <MobileCoreServices/MobileCoreServices.h>
#include "position/positionkit.h"
#include "compass.h"

#import <ImageIO/CGImageSource.h>
#import <ImageIO/CGImageProperties.h>
#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreLocation/CoreLocation.h>

@implementation IOSInterface

static UIImagePickerController *imagePickerController = nullptr;
static UIActivityIndicatorView *imagePickerIndicatorView = nullptr;

NSMutableDictionary *mGpsData = [[NSMutableDictionary alloc]init];

static NSObject *readExifAttribute( NSString *imagePath, NSString *tag )
{
  NSLog(@"%@",imagePath);
  NSData *data = [NSData dataWithContentsOfFile:imagePath];
  CGImageSourceRef source = CGImageSourceCreateWithData( ( __bridge CFDataRef )data, NULL );
  NSDictionary *metadata = [( NSDictionary * )CGImageSourceCopyPropertiesAtIndex( source, 0, NULL )autorelease];
  NSLog(@"%@",metadata);

  NSMutableDictionary *dict = nil;
  NSString *key = nil;
  if ( [tag hasPrefix:@"GPS"] )
  {
    key = [tag stringByReplacingOccurrencesOfString: @"GPS" withString:@""];
    dict = [metadata objectForKey:( NSString * )kCGImagePropertyGPSDictionary];
  }
  else
  {
    key = tag;
    dict = [metadata objectForKey:( NSString * )kCGImagePropertyExifDictionary];
  }
  NSLog(@"%@",key);
  NSLog(@"%@",dict);

  if ( !dict )
  {
    return nil;
  }

  NSObject *result = [dict objectForKey:( NSObject * )key];

  CFRelease( source );
  return result;
}

+( QString )handleCameraPhoto:( NSDictionary * )info :( NSString * )imagePath
{
  QString err;
  // 1. Get your image.
  UIImage *capturedImage = info[UIImagePickerControllerEditedImage];
  if ( !capturedImage )
  {
    capturedImage = info[UIImagePickerControllerOriginalImage];
  }

  // 2. Create your file URL.
  NSString *imagePathFixed = [NSString stringWithFormat:@"%@/%@", @"file://", imagePath];
  NSURL *outputURL = [NSURL URLWithString:[imagePathFixed stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]]];

  // 3. Get your metadata (includes the EXIF data) + extended with GPS EXIF data
  NSDictionary *metadata = [info objectForKey:UIImagePickerControllerMediaMetadata];
  NSMutableDictionary *mutableMetadata = [metadata mutableCopy];
  if ( [mGpsData count] != 0 )
    [mutableMetadata setObject:mGpsData forKey:( NSString * )kCGImagePropertyGPSDictionary];

  // 4. Set your compression quuality (0.0 to 1.0).
  [mutableMetadata setObject:@( 1.0 ) forKey:( __bridge NSString * )kCGImageDestinationLossyCompressionQuality];

  // 5. Create an image destination.
  CGImageDestinationRef imageDestination = CGImageDestinationCreateWithURL( ( __bridge CFURLRef )outputURL, kUTTypeJPEG, 1, NULL );
  if ( imageDestination == NULL )
  {
    err = "failed to create image destination.";
  }
  else
  {
    // 6. Save the image
    CGImageDestinationAddImage( imageDestination, capturedImage.CGImage, ( __bridge CFDictionaryRef )mutableMetadata );
    if ( CGImageDestinationFinalize( imageDestination ) == NO )
    {
      err = "failed to finalize the image.";
    }

    CFRelease( imageDestination );
  }

  return err;
}

static NSString *generateImagePath( NSString *targetDir )
{
  NSDateFormatter *dateformate = [[NSDateFormatter alloc]init];
  [dateformate setDateFormat: @"yyyyMMdd_HHmmss"];
  NSString *fileName = [dateformate stringFromDate:[NSDate date]];
  NSString *fileNameWithSuffix = [fileName stringByAppendingString:@".jpg"];
  NSString *imagePath = [targetDir stringByAppendingPathComponent:fileNameWithSuffix];
  return imagePath;
}

static NSMutableDictionary *getGPSData( PositionKit *positionKit, Compass *compass )
{
  NSMutableDictionary *gpsDict = [[NSMutableDictionary alloc]init];
  if ( positionKit )
  {
    if ( positionKit->hasPosition() )
    {
      @try
      {
        const QgsPoint position = positionKit->positionCoordinate();
        [gpsDict setValue:[NSNumber numberWithFloat:position.x()] forKey:( NSString * )kCGImagePropertyGPSLongitude];
        [gpsDict setValue:[NSNumber numberWithFloat:position.y()] forKey:( NSString * )kCGImagePropertyGPSLatitude];
        [gpsDict setValue:position.x() < 0.0 ? @"W" : @"E" forKey : ( NSString * )kCGImagePropertyGPSLongitudeRef];
        [gpsDict setValue:position.y() < 0.0 ? @"S" : @"N" forKey : ( NSString * )kCGImagePropertyGPSLatitudeRef];
        [gpsDict setValue:[NSNumber numberWithFloat:position.z()] forKey:( NSString * )kCGImagePropertyGPSAltitude];
        [gpsDict setValue:[NSNumber numberWithShort:position.z() < 0.0 ? 1 : 0] forKey:( NSString * )kCGImagePropertyGPSAltitudeRef];
      }
      @catch ( NSException *exception )
      {
        qWarning() << "An exception occures during extracting GPS info: " << exception.reason;
      }
    }
    else
    {
      qWarning( "no position in position kit, no GPS EXIF" );
    }
  }
  else
  {
    qWarning( "invalid position kit, no GPS EXIF" );
  }

  if ( compass )
  {
    [gpsDict setValue:[NSNumber numberWithFloat:compass->direction()] forKey:( NSString * )kCGImagePropertyGPSImgDirection];
    [gpsDict setValue:@"T" forKey:( NSString * )kCGImagePropertyGPSImgDirectionRef];
  }
  else
  {
    qWarning( "invalid compass, no GPS Direction" );
  }

  return gpsDict;
}

+( void )showImagePicker:( int )sourceType : ( IOSImagePicker * )handler
{
  UIApplication *app = [UIApplication sharedApplication];

  if ( app.windows.count <= 0 )
  {
    return;
  }

  UIWindow *rootWindow = app.windows[0];
  UIViewController *rootViewController = rootWindow.rootViewController;

  if ( ![UIImagePickerController isSourceTypeAvailable:( UIImagePickerControllerSourceType ) sourceType] )
  {
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
  }
  else
  {
    UIImagePickerController *picker = [[UIImagePickerController alloc] init];
    imagePickerController = picker;
    picker.sourceType = ( UIImagePickerControllerSourceType ) sourceType;
    static IOSViewDelegate *delegate = nullptr;
    delegate = [[IOSViewDelegate alloc] initWithHandler:handler];

    [[NSNotificationCenter defaultCenter] addObserverForName:@"_UIImagePickerControllerUserDidCaptureItem" object:nil queue:nil usingBlock: ^ ( NSNotification * _Nonnull notification )
    {
      Q_UNUSED( notification )
      // Fetch GPS data when an image is captured
      mGpsData = getGPSData( delegate->handler->positionKit(), delegate->handler->compass() );
    }];

    // Confirm event
    delegate->imagePickerControllerDidFinishPickingMediaWithInfo = ^( UIImagePickerController * picker, NSDictionary * info )
    {
      if ( delegate->processingPicture )
      {
        qWarning() << "Image Picker: Already processing other photo (imagePickerControllerDidFinishPickingMediaWithInfo)";
        return;
      }
      delegate->processingPicture = YES;

      NSString *imagePath = generateImagePath( delegate->handler->targetDir().toNSString() );
      QString err;

      bool isCameraPhoto = picker.sourceType == UIImagePickerControllerSourceType::UIImagePickerControllerSourceTypeCamera;
      if ( isCameraPhoto )
      {
        // Camera handling
        err = [IOSInterface handleCameraPhoto:info:imagePath];
      }
      else
      {
        // Gallery handling
        // Copy an image with metadata from imageURL to targetPath
        NSURL *infoImageUrl = info[UIImagePickerControllerImageURL];
        if ( !InputUtils::copyFile( QString::fromNSString( infoImageUrl.absoluteString ), QString::fromNSString( imagePath ) ) )
        {
          err = QStringLiteral( "Copying image from a gallery failed." );
        }
        infoImageUrl = nil;
      }

      [picker dismissViewControllerAnimated:YES completion:nil];
      if ( delegate->handler )
      {
        QVariantMap data;
        QString imagePathData( [imagePath UTF8String] );
        data["imagePath"] = imagePathData;
        data["error"] = err;
        QMetaObject::invokeMethod( delegate->handler, "onImagePickerFinished", Qt::DirectConnection,
                                   Q_ARG( bool, err.isEmpty() ),
                                   Q_ARG( const QVariantMap, data ) );
      }
    };


    // Cancel event
    delegate->imagePickerControllerDidCancel = ^( UIImagePickerController * picker )
    {
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

+ ( NSString * ) readExif:( NSString * ) imageFileURL : ( NSString * )tag
{
  NSObject *result = readExifAttribute( imageFileURL, tag );
  if ( !result )
  {
    return nil;
  }

  if ( [result class] == [NSString class] )
  {
    return ( NSString * ) result;
  }
  else if ( [result class] == [NSNumber class] )
  {
    NSString *stringResult = [( NSNumber * )result stringValue];
    return stringResult;
  }
  else if ( [result class] == [NSDecimalNumber class] )
  {
    NSString *stringResult = [( NSDecimalNumber * )result stringValue];
    return stringResult;
  }
  else
  {
    NSString *stringResult = [NSString stringWithFormat:@"%@", result];
    return stringResult;
  }
}

@end
