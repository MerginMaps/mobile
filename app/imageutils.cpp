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
#include "mmstyle.h"

#include <QFileInfo>
#include <QImage>
#include <QPainter>

#include <cmath>

#include <exiv2/exiv2.hpp>

bool ImageUtils::copyExifMetadata( const QString &sourceImage, const QString &targetImage )
{
  if ( !QFileInfo::exists( sourceImage ) || !QFileInfo::exists( targetImage ) )
    return false;

  try
  {
    const std::unique_ptr srcImage( Exiv2::ImageFactory::open( sourceImage.toStdString() ) );
    if ( !srcImage )
      return false;

    srcImage->readMetadata();
    const Exiv2::ExifData &exifData = srcImage->exifData();
    if ( exifData.empty() )
    {
      return true;
    }

    const std::unique_ptr dstImage( Exiv2::ImageFactory::open( targetImage.toStdString() ) );
    if ( !dstImage )
      return false;

    dstImage->setExifData( exifData );
    dstImage->writeMetadata();
    return true;
  }
  catch ( Exiv2::Error &error )
  {
    CoreUtils::log( "Copying EXIF", error.what() );
    return false;
  }
  catch ( ... )
  {
    CoreUtils::log( "Copying EXIF", QStringLiteral( "Failed to copy EXIF metadata" ) );
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
    bool metadataModified = false;

    Exiv2::ExifData &exifData = srcImage->exifData();
    if ( !exifData.empty() )
    {
      const auto exifIt = exifData.findKey( Exiv2::ExifKey( "Exif.Image.Orientation" ) );
      if ( exifIt != exifData.end() )
      {
        exifIt->setValue( "1" ); // 1 = no rotation
        metadataModified = true;
      }
    }

    Exiv2::XmpData &xmpData = srcImage->xmpData();
    if ( !xmpData.empty() )
    {
      const auto xmpIt = xmpData.findKey( Exiv2::XmpKey( "Xmp.tiff.Orientation" ) );
      if ( xmpIt != xmpData.end() )
      {
        xmpIt->setValue( "1" );
        metadataModified = true;
      }
    }

    // Only write to disk if we actually changed something
    if ( metadataModified )
    {
      srcImage->writeMetadata();
    }

    return true;
  }
  catch ( const Exiv2::Error &e )
  {
    CoreUtils::log( "Editing EXIF", QStringLiteral( "Failed to clear orientation: " ).arg( e.what() ) );
    return false;
  }
}

bool ImageUtils::createSignatureCanvas( const QString &path, int width, int height )
{
  if ( width <= 0 || height <= 0 )
    return false;

  QImage image( width, height, QImage::Format_RGB32 );
  image.fill( Qt::white );

  constexpr double guideLineBottomMargin = 0.15;
  constexpr double guideLineSideMargin = 0.1;
  const int guideLineY = static_cast<int>( height * ( 1 - guideLineBottomMargin ) );

  QPainter painter( &image );
  painter.setRenderHint( QPainter::Antialiasing );
  painter.setPen( QPen( MMStyle::mediumGreyColor(), 2 ) );
  painter.drawLine( QPointF( width * guideLineSideMargin, guideLineY ), QPointF( width * ( 1 - guideLineSideMargin ), guideLineY ) );
  painter.end();

  if ( !image.save( path, "PNG" ) )
  {
    CoreUtils::log( "Creating signature canvas", QStringLiteral( "Failed to save blank image to: %1" ).arg( path ) );
    return false;
  }

  return true;
}

namespace
{
  // Converts a decimal degrees value into the "deg/1 min/1 sec/1" rational format expected by GPS EXIF tags.
  std::string degreesToExifDms( double value )
  {
    value = std::fabs( value );
    const int degrees = static_cast<int>( value );
    value = ( value - degrees ) * 60.0;
    const int minutes = static_cast<int>( value );
    value = ( value - minutes ) * 60.0;
    const int seconds = static_cast<int>( value );

    return QStringLiteral( "%1/1 %2/1 %3/1" ).arg( degrees ).arg( minutes ).arg( seconds ).toStdString();
  }
}

bool ImageUtils::writeGpsAndTimestamp( const QString &path, const QDateTime &dateTimeUtc, bool hasPosition, double latitude, double longitude, double altitude )
{
  if ( !QFileInfo::exists( path ) )
    return false;

  try
  {
    const std::unique_ptr image( Exiv2::ImageFactory::open( path.toStdString() ) );
    if ( !image )
      return false;

    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();

    const std::string exifDateTime = dateTimeUtc.toString( QStringLiteral( "yyyy:MM:dd HH:mm:ss" ) ).toStdString();
    exifData["Exif.Image.DateTime"] = exifDateTime;
    exifData["Exif.Photo.DateTimeOriginal"] = exifDateTime;
    exifData["Exif.Photo.DateTimeDigitized"] = exifDateTime;

    if ( hasPosition )
    {
      exifData["Exif.GPSInfo.GPSVersionID"] = "2 2 0 0";
      exifData["Exif.GPSInfo.GPSMapDatum"] = "WGS-84";

      exifData["Exif.GPSInfo.GPSLatitude"] = degreesToExifDms( latitude );
      exifData["Exif.GPSInfo.GPSLatitudeRef"] = latitude >= 0.0 ? "N" : "S";

      exifData["Exif.GPSInfo.GPSLongitude"] = degreesToExifDms( longitude );
      exifData["Exif.GPSInfo.GPSLongitudeRef"] = longitude >= 0.0 ? "E" : "W";

      exifData["Exif.GPSInfo.GPSAltitude"] = QStringLiteral( "%1/1" ).arg( static_cast<int>( std::round( std::fabs( altitude ) ) ) ).toStdString();
      exifData["Exif.GPSInfo.GPSAltitudeRef"] = altitude < 0.0 ? "1" : "0";

      exifData["Exif.GPSInfo.GPSDateStamp"] = dateTimeUtc.toString( QStringLiteral( "yyyy:MM:dd" ) ).toStdString();
      exifData["Exif.GPSInfo.GPSTimeStamp"] = QStringLiteral( "%1/1 %2/1 %3/1" )
                                              .arg( dateTimeUtc.time().hour() )
                                              .arg( dateTimeUtc.time().minute() )
                                              .arg( dateTimeUtc.time().second() )
                                              .toStdString();
    }

    image->writeMetadata();
    return true;
  }
  catch ( const Exiv2::Error &e )
  {
    CoreUtils::log( "Writing EXIF", QStringLiteral( "Failed to write GPS/timestamp metadata: %1" ).arg( e.what() ) );
    return false;
  }
}
