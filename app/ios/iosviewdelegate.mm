/***************************************************************************
  iosviewdelegate.mm
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
#include <QPointer>
#import "iosviewdelegate.h"
#import "coreutils.h"

@implementation IOSViewDelegate

-( id ) initWithHandler:( IOSImagePicker * )handler
{
  self = [super init];
  if ( self )
  {
    self->processingPicture = NO;
    self->handler = handler;
  }
  return self;
}

- ( void )imagePickerController:( UIImagePickerController * )picker didFinishPickingMediaWithInfo:( NSDictionary * )info
{
  if ( imagePickerControllerDidFinishPickingMediaWithInfo )
  {
    imagePickerControllerDidFinishPickingMediaWithInfo( picker, info );
  }
}

- ( void )imagePickerControllerDidCancel:( UIImagePickerController * )picker
{
  if ( imagePickerControllerDidCancel )
  {
    imagePickerControllerDidCancel( picker );
  }
}

@end

@implementation IOSGalleryPickerDelegate
{
  QPointer<IOSImagePicker> _handler;
}

- ( instancetype ) initWithHandler:( IOSImagePicker * )handler
{
  self = [super init];
  if ( self )
  {
    _handler = handler;
  }
  return self;
}

- ( void )picker:( PHPickerViewController * )picker didFinishPicking:( NSArray<PHPickerResult *> * )results
{
  [picker dismissViewControllerAnimated:YES completion:nil];

  if ( results.count == 0 )
  {
    return; // user cancelled
  }

  if ( !_handler )
  {
    return;
  }

  PHPickerResult *result = results.firstObject;

  NSDateFormatter *df = [[NSDateFormatter alloc] init];
  [df setDateFormat:@"yyyyMMdd_HHmmss"];
  NSString *fileName = [[df stringFromDate:[NSDate date]] stringByAppendingString:@".jpg"];
  NSString *imagePath = [_handler->targetDir().toNSString() stringByAppendingPathComponent:fileName];

  [result.itemProvider loadDataRepresentationForTypeIdentifier:@"public.jpeg"
   completionHandler: ^ ( NSData * data, NSError * error )
  {
    BOOL writeSuccess = data && !error && [data writeToFile:imagePath atomically:YES];
    if ( !writeSuccess )
    {
      CoreUtils::log( "iOS photo picker", QStringLiteral( "Gallery Picker: failed to write image data to %1" ).arg( QString::fromNSString( imagePath ) ) );
    }

    dispatch_async( dispatch_get_main_queue(), ^
    {
      if ( _handler )
      {
        QVariantMap resultData;
        resultData["imagePath"] = QString::fromNSString( imagePath );
        if ( !writeSuccess )
        {
          resultData["error"] = QStringLiteral( "Copying image from gallery failed." );
        }
        QMetaObject::invokeMethod( _handler, "onImagePickerFinished", Qt::DirectConnection,
                                   Q_ARG( bool, writeSuccess ),
                                   Q_ARG( const QVariantMap, resultData ) );
      }
    } );
  }];
}

@end
