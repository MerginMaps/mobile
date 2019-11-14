#ifndef IOSIMAGEPICKER_H
#define IOSIMAGEPICKER_H

#include <QObject>
#include <QVariantMap>
#include "ios/ioshandler.h"


/**
 * The class suppose to be used in QML to invoke iOS image picker and postprocess the image if any has been choosen.
*/
class IOSImagePicker : public QObject
{
    Q_OBJECT
  public:
    explicit IOSImagePicker( QObject *parent = nullptr );
    //! Absolute path to the location where an image suppose to be copied according external widget
    Q_PROPERTY( QString targetDir READ targetDir WRITE setTargetDir NOTIFY targetDirChanged )

    Q_INVOKABLE void showImagePicker();

    QString targetDir() const;
    void setTargetDir( const QString &targetDir );

  signals:
    void targetDirChanged();
    void imageSaved( const QString &absoluteImagePath );

  public slots:
    //!
    void onImagePickerFinished( bool successful, QVariantMap data );

  private:
    QString mTargetDir;
    IOSHandler *mHandler;
};
#endif // IOSIMAGEPICKER_H
