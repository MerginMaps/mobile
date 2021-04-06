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

QString IOSImagePicker::readExif( const QString &filepath, const QString &tag )
{
#ifdef Q_OS_IOS
  return readExifDirect( filepath, tag );
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
    QImage image = data["image"].value<QImage>();
    QString imagePath = data["imagePath"].value<QString>();

    // Image is not saved yet, will be written to target location
    if ( imagePath.isNull() )
    {
      QString absoluteImagePath = QString( "%1/%2.jpg" ).arg( mTargetDir, QDateTime::currentDateTime().toString( QStringLiteral( "yyMMdd-hhmmss" ) ) );

      image.save( absoluteImagePath );
      QImageWriter writer;
      writer.setFileName( absoluteImagePath );
      if ( !writer.write( image ) )
      {
        qWarning() << QString( "Failed to save %1 : %2" ).arg( absoluteImagePath ).arg( writer.errorString() );
      }
      qDebug() << "Image saved to: " << absoluteImagePath;
      QUrl url = QUrl::fromLocalFile( absoluteImagePath );
      emit imageCaptured( url.toString() );
    }
    // Image has been already copied from a gallery
    else
    {
      emit imageCaptured( imagePath );
    }
  }
}
