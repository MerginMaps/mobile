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
#import "iosviewdelegate.h"

@interface IOSViewDelegate()

@end

@implementation IOSViewDelegate

-( id ) initWithHandler:( IOSImagePicker * )handler
{
  self = [super init];
  if ( self )
  {
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
