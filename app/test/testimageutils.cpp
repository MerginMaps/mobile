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

#include <QtTest/QtTest>

void TestImageUtils::init()
{
}

void TestImageUtils::cleanup()
{
}

void TestImageUtils::testRescale()
{
  QTemporaryDir dir;
  QString testPhotoName = QStringLiteral( "photo.jpg" );
  QFile::copy( TestUtils::testDataDir() + '/' + testPhotoName, dir.filePath( testPhotoName ) );

  QVERIFY( ImageUtils::rescale( dir.filePath( testPhotoName ), 3 ) );

  QImage img( dir.filePath( testPhotoName ) );
  QCOMPARE( img.height(), 1000 );

  // check EXIF tags
  std::unique_ptr< Exiv2::Image > image( Exiv2::ImageFactory::open( dir.filePath( testPhotoName ).toStdString() ) );

  image->readMetadata();
  Exiv2::ExifData &exifData = image->exifData();
  QVERIFY( !exifData.empty() );

  const Exiv2::ExifData::iterator itElevVal = exifData.findKey( Exiv2::ExifKey( "Exif.GPSInfo.GPSAltitude" ) );
  const Exiv2::Rational rational = itElevVal->value().toRational( 0 );
  double val = static_cast< double >( rational.first ) / rational.second;
  QCOMPARE( val, 133 );
}
