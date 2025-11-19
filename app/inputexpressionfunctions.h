/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INPUTEXPRESSIONFUNCTIONS_H
#define INPUTEXPRESSIONFUNCTIONS_H

#include <QObject>

#include "qvariant.h"
#include "qgsexpression.h"
#include "qgsexpressionfunction.h"

#include "androidutils.h"
#include "inpututils.h"
#include "ios/iosimagepicker.h"

const static QString GPS_DIRECTION_TAG = "GPSImgDirection";
const static QString GPS_LON_TAG = "GPSLongitude";
const static QString GPS_LAT_TAG = "GPSLatitude";


//! Expression function for reading EXIF attribute
class ReadExif : public QgsScopedExpressionFunction
{
  public:
    ReadExif()
      : QgsScopedExpressionFunction( QStringLiteral( "read_exif" ), QgsExpressionFunction::ParameterList()
                                     << QgsExpressionFunction::Parameter( QStringLiteral( "file" ) )
                                     << QgsExpressionFunction::Parameter( QStringLiteral( "exif_tag" ) ),
                                     QStringLiteral( "Custom" ) ) {}
    /**
     * Custom expression function to read EXIF metadata. It is platform dependent and currently supported only on Android.
     * Example field definition: read_exif('<ABSOLUTE_PATH_TO_IMAGE>', '<EXIF_TAG_STRING>')
     * @param values - suppose to contain 2 parameters:
     *  - file: Absolute path of an image that exif attribute will be read from,
     *  - exif_tag: The officially supported EXIF TAG name.
     * @return Formatted attribute value for given parameters, if parameters are not valid, returns empty string
     */
    QVariant func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * ) override;
    QgsScopedExpressionFunction *clone() const override { return new ReadExif();}
};


//! Expression function for reading EXIF Image Direction
class ReadExifImgDirection : public QgsScopedExpressionFunction
{
  public:
    ReadExifImgDirection()
      : QgsScopedExpressionFunction( QStringLiteral( "read_exif_img_direction" ), QgsExpressionFunction::ParameterList()
                                     << QgsExpressionFunction::Parameter( QStringLiteral( "file" ) ),
                                     QStringLiteral( "Custom" ) ) {}
    /**
     * @param values - contain one parameter:
     *  - file: Absolute path of an image that exif attribute will be read from,
     * @return EXIF value for GPS Image Direction
     */
    QVariant func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * ) override;
    QgsScopedExpressionFunction *clone() const override { return new ReadExifImgDirection();}
};


//! Expression function for reading EXIF Latitude
class ReadExifLatitude : public QgsScopedExpressionFunction
{
  public:
    ReadExifLatitude()
      : QgsScopedExpressionFunction( QStringLiteral( "read_exif_latitude" ), QgsExpressionFunction::ParameterList()
                                     << QgsExpressionFunction::Parameter( QStringLiteral( "file" ) ),
                                     QStringLiteral( "Custom" ) ) {}
    /**
     * @param values - contain one parameter:
     *  - file: Absolute path of an image that exif attribute will be read from,
     * @return EXIF value for GPS Latitude
     */
    QVariant func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * ) override;
    QgsScopedExpressionFunction *clone() const override { return new ReadExifLatitude();}
};


//! Expression function for reading EXIF Longitude
class ReadExifLongitude : public QgsScopedExpressionFunction
{
  public:
    ReadExifLongitude()
      : QgsScopedExpressionFunction( QStringLiteral( "read_exif_longitude" ), QgsExpressionFunction::ParameterList()
                                     << QgsExpressionFunction::Parameter( QStringLiteral( "file" ) ),
                                     QStringLiteral( "Custom" ) ) {}
    /**
     * @param values - contain one parameter:
     *  - file: Absolute path of an image that exif attribute will be read from,
     * @return EXIF value for GPS Longitude
     */
    QVariant func( const QVariantList &values, const QgsExpressionContext *, QgsExpression *, const QgsExpressionNodeFunction * ) override;
    QgsScopedExpressionFunction *clone() const override { return new ReadExifLongitude();}
};

#endif // INPUTEXPRESSIONFUNCTIONS_H
