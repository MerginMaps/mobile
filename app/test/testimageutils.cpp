/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testimageutils.h"
#include "testutils.h"
#include "imageutils.h"

#include <QTemporaryDir>
#include <QImage>
#include <exiv2/exiv2.hpp>

void TestImageUtils::init()
{
}

void TestImageUtils::cleanup()
{
}

void TestImageUtils::testRescale()
{
  const QTemporaryDir dir;
  const QString testPhotoName = QStringLiteral( "photo.jpg" );
  QFile::copy( TestUtils::testDataDir() + '/' + testPhotoName, dir.filePath( testPhotoName ) );

  QVERIFY( ImageUtils::rescale( dir.filePath( testPhotoName ), 3 ) );

  const QImage img( dir.filePath( testPhotoName ) );
  QCOMPARE( img.height(), 1000 );

  // check EXIF tags
  const std::unique_ptr image( Exiv2::ImageFactory::open( dir.filePath( testPhotoName ).toStdString() ) );

  image->readMetadata();
  Exiv2::ExifData &exifData = image->exifData();
  QVERIFY( !exifData.empty() );

  const Exiv2::ExifData::iterator itElevVal = exifData.findKey( Exiv2::ExifKey( "Exif.GPSInfo.GPSAltitude" ) );
  const Exiv2::Rational rational = itElevVal->value().toRational( 0 );
  const double val = static_cast< double >( rational.first ) / rational.second;
  QCOMPARE( val, 133 );
}

void TestImageUtils::testClearOrientationMetadata()
{
  const QTemporaryDir dir;
  const QString testPhotoName = QStringLiteral( "photo.jpg" );
  QFile::copy( TestUtils::testDataDir() + '/' + testPhotoName, dir.filePath( testPhotoName ) );

  // Set XMP orientation so the XMP code path is checked as well
  {
    const std::unique_ptr setupImage( Exiv2::ImageFactory::open( dir.filePath( testPhotoName ).toStdString() ) );
    setupImage->readMetadata();
    Exiv2::XmpData &xmpSetup = setupImage->xmpData();
    xmpSetup["Xmp.tiff.Orientation"] = uint16_t( 6 ); // 6 = 90° CW rotation
    setupImage->setXmpData( xmpSetup );
    setupImage->writeMetadata();
  }

  QVERIFY( ImageUtils::clearOrientationMetadata( dir.filePath( testPhotoName ) ) );

  const std::unique_ptr image( Exiv2::ImageFactory::open( dir.filePath( testPhotoName ).toStdString() ) );
  image->readMetadata();
  Exiv2::ExifData &exifData = image->exifData();
  QVERIFY( !exifData.empty() );

  const auto exifIterator = exifData.findKey( Exiv2::ExifKey( "Exif.Image.Orientation" ) );
  QVERIFY( exifIterator != exifData.end() ); // key should be preserved, just reset to 1
  QCOMPARE( QString::fromStdString( exifIterator->toString() ), QStringLiteral( "1" ) );

  Exiv2::XmpData &xmpData = image->xmpData();
  const auto xmpIterator = xmpData.findKey( Exiv2::XmpKey( "Xmp.tiff.Orientation" ) );
  QVERIFY( xmpIterator != xmpData.end() ); // key should be preserved, just reset to 1
  QCOMPARE( QString::fromStdString( xmpIterator->toString() ), QStringLiteral( "1" ) );
}
