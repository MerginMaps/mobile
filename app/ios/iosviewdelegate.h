/***************************************************************************
  iosviewdelegate.h
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

#ifndef IOSVIEWDELEGATE_H
#define IOSVIEWDELEGATE_H

#include <UIKit/UIKit.h>
#include <PhotosUI/PhotosUI.h>
#import <Photos/Photos.h>

#include "iosimagepicker.h"
/**
 * View controller for iOSImagePicker
*/
@interface IOSViewDelegate : NSObject<UIImagePickerControllerDelegate,
UINavigationControllerDelegate>
{
  @public

  IOSImagePicker *handler;
  BOOL processingPicture;

  void ( ^ imagePickerControllerDidFinishPickingMediaWithInfo )( UIImagePickerController * picker, NSDictionary * info );
  void ( ^ imagePickerControllerDidCancel )( UIImagePickerController * picker );
}
- ( id ) initWithHandler:( IOSImagePicker * )handler;
@end

@interface IOSGalleryViewController : NSObject<PHPickerViewControllerDelegate>
- (void)openPHPicker;
@end

#endif // IOSVIEWDELEGATE_H
