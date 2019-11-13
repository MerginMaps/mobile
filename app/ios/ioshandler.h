#ifndef IOSHANDLER_H
#define IOSHANDLER_H

#include <QObject>
#include <QWidget>
#include <QVariantMap>

class IOSHandler : public QObject
{
    Q_OBJECT
  public:
    explicit IOSHandler( QObject *parent = nullptr );

    Q_INVOKABLE static void showImagePicker();

    static IOSHandler *instance();

  public slots:
    //! Middle step method between iOSInterface and iOSImagePicker
    void imagePickerFinished( bool successful, QVariantMap data );

  signals:
    //! Forwarded signal from iOSInterface's ImagePicker
    void forwardedImagePickerFinished( bool successful, QVariantMap data );
};

#endif // IOSHANDLER_H
