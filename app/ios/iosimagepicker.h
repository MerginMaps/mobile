#ifndef IOSIMAGEPICKER_H
#define IOSIMAGEPICKER_H

#include <QObject>
#include <QVariantMap>

#include "ios/ioshandler.h"

class IOSImagePicker : public QObject
{
    Q_OBJECT
  public:
    explicit IOSImagePicker( QObject *parent = nullptr );

    Q_PROPERTY( QString targetDir READ targetDir WRITE setTargetDir NOTIFY targetDirChanged )
    Q_PROPERTY( IOSHandler *handler READ handler WRITE setHandler NOTIFY handlerChange )

    Q_INVOKABLE void showImagePicker();

    QString targetDir() const;
    void setTargetDir( const QString &targetDir );

    IOSHandler *handler() const;
    void setHandler( IOSHandler *handler );

  signals:
    void targetDirChanged();
    void handlerChange();
    void imageSaved( const QString &absoluteImagePath );

  public slots:
    void onImagePickerFinished( bool successful, QVariantMap data );

  private:
    QString mTargetDir;
    IOSHandler *mHandler;
};

#endif // IOSIMAGEPICKER_H
