/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputexpressionfunctions.h"
#include "math.h"
#include "ios/iosutils.h"

QVariant ReadExif::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
  if ( values.size() != 2 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
  QString exifTag( values.at( 1 ).toString() );
#ifdef ANDROID
  return AndroidUtils::readExif( filepath, exifTag );
#elif defined( Q_OS_IOS )
  return IosUtils::readExif( filepath, exifTag );
# else
  return QString();
#endif
}

QVariant ReadExifImgDirection::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
  if ( values.size() != 1 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
#ifdef ANDROID
  QString resultString = AndroidUtils::readExif( filepath, GPS_DIRECTION_TAG );
  if ( resultString.isEmpty() )
    return QVariant();

  double resultDouble = InputUtils::convertRationalNumber( resultString );
  if ( isnan( resultDouble ) )
    return QVariant();

  return QVariant( resultDouble );
#elif defined( Q_OS_IOS )
  QString result = IosUtils::readExif( filepath, GPS_DIRECTION_TAG );
  return QVariant( result.toDouble() );
# else
  return QString();
#endif
}

QVariant ReadExifLatitude::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
  if ( values.size() != 1 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
#ifdef ANDROID
  QString resultString = AndroidUtils::readExif( filepath, GPS_LAT_TAG );
  if ( resultString.isEmpty() )
    return QVariant();

  return QVariant( InputUtils::convertCoordinateString( resultString ) );
#elif defined( Q_OS_IOS )
  QString result = IosUtils::readExif( filepath, GPS_LAT_TAG );
  return QVariant( result.toDouble() );
# else
  return QString();
#endif
}

QVariant ReadExifLongitude::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
  if ( values.size() != 1 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
#ifdef ANDROID
  QString resultString = AndroidUtils::readExif( filepath, GPS_LON_TAG );
  if ( resultString.isEmpty() )
    return QVariant();

  return QVariant( InputUtils::convertCoordinateString( resultString ) );
#elif defined( Q_OS_IOS )
  QString result = IosUtils::readExif( filepath, GPS_LON_TAG ) ;
  return QVariant( result.toDouble() );
# else
  return QString();
#endif
}
