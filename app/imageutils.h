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

#include <QDateTime>
#include <QString>

class ImageUtils
{
  public:
    explicit ImageUtils( ) = default;
    ~ImageUtils() = default;

    /**
     * Copies EXIF metadata from sourceImage to targetImage.
     * \note Developers need to make sure the paths don't reference the same file, it will cause empty metadata
     */
    static bool copyExifMetadata( const QString &sourceImage, const QString &targetImage );

    /**
     * Writes a timestamp and, optionally, a GPS position into an image's EXIF metadata.
     * \return bool if successful
     */
    static bool writeGpsAndTimestamp( const QString &path, const QDateTime &dateTimeUtc, bool hasPosition, double latitude, double longitude, double altitude );

    /**
     * Creates the blank canvas a signature is drawn onto, with a baseline guide line near the bottom.
     */
    static bool createSignatureCanvas( const QString &path, int width, int height );

    /**
     * Rescales image to the given quality taking into account its orientation
     * and preserving EXIF metadata.
     */
    static bool rescale( const QString &path, int quality );

    /**
     * Removes orientation metadata, which is necessary when copying an image which is already rotated.
     * \param sourceImage path to image
     * \return bool if successful
     */
    static bool clearOrientationMetadata( const QString &sourceImage );
};

#endif // IMAGEUTILS_H
