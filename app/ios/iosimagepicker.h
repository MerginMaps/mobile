/***************************************************************************
  iosimagepicker.h
  --------------------------------------
  Date                 : Nov 2019
  Copyright            : (C) 2019 by Viktor Sklencar
  Email                : viktor.sklencar@lutraconsulting.co.uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IOSIMAGEPICKER_H
#define IOSIMAGEPICKER_H

#include <QObject>
#include <QVariantMap>

/**
 * The class suppose to be used in QML to invoke iOS image picker and postprocess the image if any has been choosen.
*/
class IOSImagePicker : public QObject
{
    Q_OBJECT
  public:
    explicit IOSImagePicker( QObject *parent = nullptr );
    /**
    * Method suppose to be used in QML and calls IOSImagePicker::showImagePickerDirect which invokes IOSViewDelegate and image picker.
    * \param sourceMode - when 0 == Gallery, 1 == Camera.
    * \param targetDir - String representing directory path where captured photo suppose to be saved.
    */
    Q_INVOKABLE void showImagePicker( int sourceMode, const QString  &targetDir );

    QString targetDir() const;
    void setTargetDir( const QString &targetDir );

  signals:
    void targetDirChanged();
    void imageCaptured( const QString &absoluteImagePath );

  public slots:
    /**
     * Callback after succesfuly choosing an image - saves image at targetDir location.
     */
    void onImagePickerFinished( bool successful, const QVariantMap &data );

  private:
    QString mTargetDir;

    /**
     * Calls the objective-c function to show image picker.
     */
    void showImagePickerDirect( int sourceType, IOSImagePicker *handler );
};
#endif // IOSIMAGEPICKER_H
