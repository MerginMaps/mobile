/***************************************************************************
  iosinterface.h
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

#ifndef IOSINTERFACE_H
#define IOSINTERFACE_H

#include <UIKit/UIKit.h>

#include <QVariantMap>
#include "iosimagepicker.h"

/**
 * The interface of objective-c methods.
*/
@interface IOSInterface : NSObject
+( void )showImagePicker:( int )sourceType : ( IOSImagePicker * )hander;
+( QString )handleCameraPhoto:( NSDictionary * )info:( NSString * )imagePath;
+( NSString * )readExif:( NSString * ) imageFileURL:( NSString * )tag;

extern NSMutableDictionary *mGpsData;
@end

#endif // IOSINTERFACE_H
