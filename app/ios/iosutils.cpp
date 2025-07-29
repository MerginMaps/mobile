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
  QObject::connect( mImagePicker, &IOSImagePicker::imageCaptured, this, [this]( const QString & absoluteImagePath )
  {
    emit imageSelected( absoluteImagePath, mLastCode );
  } );
  QObject::connect( mImagePicker, &IOSImagePicker::notifyError, this, &IosUtils::notifyError );
}

bool IosUtils::isIos() const
{
#ifdef Q_OS_IOS
  return true;
#else
  return false;
#endif
}

void IosUtils::callImagePicker( const QString &targetPath, const QString &code )
{
  mLastCode = code;
  mImagePicker->showImagePicker( targetPath );
}

void IosUtils::callCamera( const QString &targetPath, const QString &code )
{
  mLastCode = code;
  mImagePicker->callCamera( targetPath, mPositionKit, mCompass );
}

IOSImagePicker *IosUtils::imagePicker() const
{
  return mImagePicker;
}

QString IosUtils::readExif( const QString &filepath, const QString &tag )
{
#ifdef Q_OS_IOS
  return IOSImagePicker::readExifDirect( filepath, tag );
#else
  Q_UNUSED( filepath )
  Q_UNUSED( tag )
  return QString();
#endif
}

QVector<int> IosUtils::getSafeArea()
{
#ifdef Q_OS_IOS
  return getSafeAreaImpl();
#endif
  return QVector<int>();
}

QString IosUtils::getManufacturer()
{
#ifdef Q_OS_IOS
  return getManufacturerImpl();
#endif
  return "";
}

QString IosUtils::getDeviceModel()
{
#ifdef Q_OS_IOS
  return getDeviceModelImpl();
#endif
  return "";
}

bool IosUtils::openFile( const QString &filePath )
{
#ifdef Q_OS_IOS
  return openFileImpl( filePath );
#else
  return false;
#endif
}
