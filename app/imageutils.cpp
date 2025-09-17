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

    srcImage->readMetadata();
    Exiv2::ExifData &exifData = srcImage->exifData();
    if ( exifData.empty() )
    {
      return true;
    }

    dstImage->setExifData( exifData );
    dstImage->writeMetadata();
    return true;
  }
  catch ( ... )
  {
    CoreUtils::log( "copying EXIF", QStringLiteral( "Failed to copy EXIF metadata" ) );
    return false;
  }
}

bool ImageUtils::rescale( const QString &path, int quality )
{

  QImage sourceImage( path );
  bool isPortrait = sourceImage.height() > sourceImage.width();
  int size = isPortrait ? sourceImage.width() : sourceImage.height();

  int newSize = size;
  switch ( quality )
  {
    case 0: // original quality, no rescaling needed
    {
      break;
    }
    case 1: // high quality, output image size ~2-4 Mb
    {
      newSize = 3000;
      break;
    }
    case 2: // medium quality, output image size ~1-2 Mb
    {
      newSize = 1500;
      break;
    }
    case 3: // low quality, output image size ~0.5 Mb
    {
      newSize = 1000;
      break;
    }
  }

  // if image width or height (depending on the orientation) is smaller
  // than new size we keep original image
  if ( size <= newSize )
  {
    return true;
  }

  // rescale
  QImage rescaledImage;
  if ( isPortrait )
  {
    rescaledImage = sourceImage.scaledToWidth( newSize, Qt::SmoothTransformation );
  }
  else
  {
    rescaledImage = sourceImage.scaledToHeight( newSize, Qt::SmoothTransformation );
  }

  if ( rescaledImage.isNull() )
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "Failed to rescale %1" ).arg( path ) );
    return false;
  }

  QFileInfo fi( path );
  QString newPath = QStringLiteral( "%1/%2_rescaled.%3" ).arg( fi.path(), fi.baseName(), fi.completeSuffix() );

  if ( !rescaledImage.save( newPath ) )
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "Failed to save rescaled image" ) );
    return false;
  }

  // copy EXIF from source image to rescaled image
  if ( !copyExifMetadata( path, newPath ) )
  {
    CoreUtils::log( "rescaling image", QStringLiteral( "Failed to copy EXIF metadata from original image" ) );
    return false;
  }

  // remove original file and rename rescaled version
  if ( QFile::remove( path ) )
  {
    if ( QFile::rename( newPath, path ) )
    {
      return true;
    }
  }

  CoreUtils::log( "rescaling image", QStringLiteral( "Can not replace original file with rescaled version" ) );
  return false;
}

bool ImageUtils::clearOrientationMetadata( const QString &sourceImage )
{
  if ( !QFileInfo::exists( sourceImage ) )
    return false;

  try
  {
    const std::unique_ptr srcImage( Exiv2::ImageFactory::open( sourceImage.toStdString() ) );
    if ( !srcImage )
      return false;

    srcImage->readMetadata();
    Exiv2::ExifData &exifData = srcImage->exifData();
    if ( exifData.empty() )
    {
      return true;
    }

    const auto iterator = exifData.findKey( Exiv2::ExifKey( "Exif.Image.Orientation" ) );
    if ( iterator != exifData.end() )
    {
      exifData.erase( iterator );
    }
    srcImage->setExifData( exifData );
    srcImage->writeMetadata();
    return true;
  }
  catch ( ... )
  {
    CoreUtils::log( "copying EXIF", QStringLiteral( "Failed to copy EXIF metadata" ) );
    return false;
  }
}
