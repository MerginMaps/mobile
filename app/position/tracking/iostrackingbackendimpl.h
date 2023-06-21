/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IOSTRACKINGBACKENDIMPL_H
#define IOSTRACKINGBACKENDIMPL_H

#import <CoreFoundation/CoreFoundation.h>
#import <CoreLocation/CoreLocation.h>

#import "iostrackingbackend.h"

@interface IOSTrackingBackendImpl : NSObject<CLLocationManagerDelegate>
{
  IOSTrackingBackend *mObserver;
  CLLocationManager *mManager;
}

-( id )initWithObserver:( IOSTrackingBackend * )iosObserver;
-( NSString * )setup:( double )distanceFilter;

@end

#endif // IOSTRACKINGBACKENDIMPL_H
