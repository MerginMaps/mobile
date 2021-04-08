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

QVariant ReadExif::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
#ifdef ANDROID
  if ( values.size() != 2 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
  QString exifTag( values.at( 1 ).toString() );
  return AndroidUtils::getExifInfo( filepath, exifTag );
# else
  return QString();
#endif
}

QVariant ReadExifImgDirection::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
#ifdef ANDROID
  if ( values.size() != 1 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
  QString resultString = AndroidUtils::getExifInfo( filepath, GPS_DIRECTION_TAG );
  if ( resultString.isEmpty() )
    return QVariant();

  double resultDouble = InputUtils::convertRationalNumber( resultString );
  if ( isnan( resultDouble ) )
    return QVariant();

  return QVariant( resultDouble );
# else
  return QString();
#endif
}

QVariant ReadExifLatitude::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
#ifdef ANDROID
  if ( values.size() != 1 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
  QString resultString = AndroidUtils::getExifInfo( filepath, GPS_LAT_TAG );
  if ( resultString.isEmpty() )
    return QVariant();

  return QVariant( InputUtils::convertCoordinateString( resultString ) );
# else
  return QString();
#endif
}

QVariant ReadExifLongitude::func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * )
{
#ifdef ANDROID
  if ( values.size() != 1 ) return QVariant();

  QString filepath( values.at( 0 ).toString() );
  QString resultString = AndroidUtils::getExifInfo( filepath, GPS_LON_TAG );
  if ( resultString.isEmpty() )
    return QVariant();

  return QVariant( InputUtils::convertCoordinateString( resultString ) );
# else
  return QString();
#endif
}
