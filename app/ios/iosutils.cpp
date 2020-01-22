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
  mImagePicker = new IOSImagePicker();
  QObject::connect( mImagePicker, &IOSImagePicker::imageCaptured, this, &IosUtils::imageCaptured );
}

bool IosUtils::isIos() const
{
#ifdef Q_OS_IOS
  return true;
#else
  return false;
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
