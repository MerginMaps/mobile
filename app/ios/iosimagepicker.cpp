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

#include <QDebug>
#include <QImage>
#include <QImageWriter>
#include <QUrl>

IOSImagePicker::IOSImagePicker( QObject *parent ) : QObject( parent )
{
}

void IOSImagePicker::showImagePicker( int sourceType, const QString  &targetDir )
{
#ifdef Q_OS_IOS
  setTargetDir( targetDir );
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
