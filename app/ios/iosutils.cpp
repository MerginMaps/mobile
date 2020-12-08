/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "iosutils.h"

IosUtils::IosUtils( QObject *parent ): QObject( parent )
{
#ifdef Q_OS_IOS
  setIdleTimerDisabled();
#endif
  mImagePicker = new IOSImagePicker();
  QObject::connect( mImagePicker, &IOSImagePicker::imageCaptured, this, &IosUtils::imageSelected );
}

bool IosUtils::isIos() const
{
#ifdef Q_OS_IOS
  return true;
#else
  return false;
#endif
}

bool IosUtils::hasLocationPermission()
{
#ifdef Q_OS_IOS
  return hasLocationPermissionImpl();
#else
  return true;
#endif
}

bool IosUtils::acquireLocationPermission()
{
#ifdef Q_OS_IOS
  return acquireLocationPermissionImpl();
#else
  return true;
#endif
}

void IosUtils::callImagePicker( const QString &targetPath )
{
  mImagePicker->showImagePicker( 0, targetPath );
}

void IosUtils::callCamera( const QString &targetPath )
{
  mImagePicker->showImagePicker( 1, targetPath );
}

IOSImagePicker *IosUtils::imagePicker() const
{
  return mImagePicker;
}
