/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "imageutils.h"

#include "coreutils.h"

#include <QFile>
#include <QFileInfo>
#include <QImage>

#include <exiv2/exiv2.hpp>

bool ImageUtils::copyExifMetadata( const QString &sourceImage, const QString &targetImage )
{
  CoreUtils::log( "copy EXIF", QStringLiteral( "Copy EXIF from %1 to %2" ).arg( sourceImage, targetImage ) );

  if ( !QFileInfo::exists( sourceImage ) || !QFileInfo::exists( targetImage ) )
    return false;

  try
  {
    std::unique_ptr< Exiv2::Image > srcImage( Exiv2::ImageFactory::open( sourceImage.toStdString() ) );
    if ( !srcImage )
      return false;

    std::unique_ptr< Exiv2::Image > dstImage( Exiv2::ImageFactory::open( targetImage.toStdString() ) );
    if ( !dstImage )
      return false;

    CoreUtils::log( "copy EXIF", QStringLiteral( "Read EXIF from source image" ) );
    srcImage->readMetadata();
    Exiv2::ExifData &exifData = srcImage->exifData();
    if ( exifData.empty() )
    {
      return true;
    }

    CoreUtils::log( "copy EXIF", QStringLiteral( "Write EXIF to target image" ) );
    dstImage->setExifData( exifData );
    dstImage->writeMetadata();
    CoreUtils::log( "copy EXIF", QStringLiteral( "Save target image metadata" ) );
    return true;
  }
  catch ( ... )
  {
    CoreUtils::log( "copy EXIF", QStringLiteral( "FAILED to copy EXIF metadata" ) );
    return false;
  }
}

bool ImageUtils::rescale( const QString &path, int quality )
{
  CoreUtils::log( "rescaling image", QStringLiteral( "Rescale %1 with quality %2" ).arg( path ).arg( quality ) );

  QImage sourceImage( path );
  bool isPortrait = sourceImage.height() > sourceImage.width();
  int size = isPortrait ? sourceImage.width() : sourceImage.height();
  CoreUtils::log( "rescaling image", QStringLiteral( "Portrait orientation: %1" ).arg( isPortrait ) );
  CoreUtils::log( "rescaling image", QStringLiteral( "Original size: %1" ).arg( size ) );

  int newSize = size;
  switch ( quality )
  {
    case 0: // original quality, no rescaling needed
    {
      break;
    }
    case 1: // high quality, output image size ~5Mb
    {
      newSize = 3000;
      break;
    }
    case 2: // medium quality, output image size ~3Mb
    {
      newSize = 1500;
      break;
    }
    case 3: // low quality, output image size ~1Mb
    {
      newSize = 800;
      break;
    }
  }

  // if image width or height (depending on the orientation) is smaller
  // than new size we keep original image
  if ( size <= newSize )
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "Original size smaller than new size. No rescaling needed" ) );
    return true;
  }

  // rescale
  QImage rescaledImage;
  if ( isPortrait )
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "Rescale to width" ) );
    rescaledImage = sourceImage.scaledToWidth( newSize, Qt::SmoothTransformation );
  }
  else
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "Rescale to height" ) );
    rescaledImage = sourceImage.scaledToHeight( newSize, Qt::SmoothTransformation );
  }

  if ( rescaledImage.isNull() )
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "FAILED to rescale" ) );
    return false;
  }

  QFileInfo fi( path );
  QString newPath = QStringLiteral( "%1/%2_rescaled.%3" ).arg( fi.path(), fi.baseName(), fi.completeSuffix() );

  if ( !rescaledImage.save( newPath ) )
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "FAILED to save rescaled image" ) );
    return false;
  }

  // copy EXIF from source image to rescaled image
  copyExifMetadata( path, newPath );

  // remove original file and rename rescaled version
  if ( QFile::remove( path ) )
  {
    if ( QFile::rename( newPath, path ) )
    {
      return true;
    }
  }

  CoreUtils::log( "rescaling image", QStringLiteral( "FAILED to replace original file with rescaled version" ) );
  return false;
}
