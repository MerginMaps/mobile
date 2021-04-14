/***************************************************************************
  iosimagepicker.cpp
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

#include "iosimagepicker.h"
#include "qdatetime.h"
#include "positiondirection.h"

#include <QDebug>
#include <QImage>
#include <QImageWriter>
#include <QUrl>

IOSImagePicker::IOSImagePicker( QObject *parent ) : QObject( parent )
{
}

void IOSImagePicker::showImagePicker( int sourceType, const QString  &targetDir, QgsQuickPositionKit *positionKit, PositionDirection *compass )
{
#ifdef Q_OS_IOS
  setTargetDir( targetDir );
  setPositionKit( positionKit );
  setCompass( compass );
  showImagePickerDirect( sourceType, this );
#else
  Q_UNUSED( sourceType )
  Q_UNUSED( targetDir )
#endif
}

QString IOSImagePicker::targetDir() const
{
  return mTargetDir;
}

void IOSImagePicker::setTargetDir( const QString &targetDir )
{
  mTargetDir = targetDir;
  emit targetDirChanged();
}

void IOSImagePicker::setPositionKit( QgsQuickPositionKit *positionKit )
{
  mPositionKit = positionKit;
  emit positionKitChanged();
}

void IOSImagePicker::onImagePickerFinished( bool successful, const QVariantMap &data )
{
  if ( successful )
  {
    QString imagePath = data["imagePath"].value<QString>();
    emit imageCaptured( imagePath );
  }
  else
  {
    qWarning() << QString( "Failed with err" ).arg( data["error"].value<QString>() );
  }
}

PositionDirection *IOSImagePicker::compass() const
{
  return mCompass;
}

void IOSImagePicker::setCompass( PositionDirection *compass )
{
  mCompass = compass;
  emit compassChanged();
}

QgsQuickPositionKit *IOSImagePicker::positionKit() const
{
  return mPositionKit;
}
