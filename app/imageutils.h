/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QString>
#include <QImage>
#include <QVideoFrame>

#include "inputconfig.h"

class ImageUtils
{
  public:
    explicit ImageUtils( ) = default;
    ~ImageUtils() = default;

    /**
     * Copies EXIF metadata from sourceImage to targetImage.
     */
    static bool copyExifMetadata( const QString &sourceImage, const QString &targetImage );

    /**
     * Rescales image to the given quality taking into account its orientation
     * and preserving EXIF metadata.
     */
    static bool rescale( const QString &path, int quality );

    /**
     * Rescales given image in memory and returns scaled down copy
     */
    static QImage rescale( const QImage &image, int quality );

    /**
     * Converts QVideoFrame to image and rescales it down to minimum resolution
     */
    static QImage rescaledImage( const QVideoFrame &frame );
};

#endif // IMAGEUTILS_H
