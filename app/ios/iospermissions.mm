/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QDebug>
#include "iosutils.h"
#import <CoreLocation/CoreLocation.h>

// See https://github.com/qt/qtlocation/blob/cec9762f3c553a2870b32cfb3ba645bdcaaef671/src/plugins/position/corelocation/qgeopositioninfosource_cl.mm#L141
bool IosUtils::hasLocationPermissionImpl()
{
  if ( [CLLocationManager locationServicesEnabled] )
  {
    // Location Services Are Enabled
    switch ( [CLLocationManager authorizationStatus] )
    {
      case kCLAuthorizationStatusNotDetermined:
        qDebug() << "LocationPermissions: User has not yet made a choice with regards to this application";
        return false;
      case kCLAuthorizationStatusRestricted:
        qDebug() << "LocationPermissions: This application is not authorized to use location services";
        // Due to active restrictions on location services, the user cannot change
        // this status, and may not have personally denied authorization
        return false;
      case kCLAuthorizationStatusDenied:
        qDebug() << "LocationPermissions: User has explicitly denied authorization for this application, or location services are disabled in Settings";
        return false;
      case kCLAuthorizationStatusAuthorizedAlways:
        // This app is authorized to start location services at any time.
        return true;
#ifndef Q_OS_MACOS
      case kCLAuthorizationStatusAuthorizedWhenInUse:
        // This app is authorized to start most location services while running in the foreground.
        return true;
#endif
      default:
        qDebug() << "LocationPermissions: Unknown state";
        return false;
    }
  }
  else
  {
    qDebug() << "LocationPermissions: Disabled in System Settings";
    return false;
  }
}

bool IosUtils::acquireLocationPermissionImpl()
{
  CLLocationManager *locationManager = [[CLLocationManager alloc] init];
  [locationManager requestWhenInUseAuthorization];
  [locationManager release];
  return hasLocationPermissionImpl();
}
