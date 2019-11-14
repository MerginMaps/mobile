#ifndef IOSHANDLER_H
#define IOSHANDLER_H

#include <QObject>
#include <QWidget>
#include <QVariantMap>

/**
 * The intermediate class between objective-c and cpp. It calls and forwards methods from both side
 * and therefore provides communication between them.
*/
class IOSHandler : public QObject
{
    Q_OBJECT
  public:
    explicit IOSHandler( QObject *parent = nullptr );
#ifdef MOBILE_OS
    /* Method calls IOSInterface::showImagePicker which invokes IOSViewDelegate and Image picker
     *
     */
    Q_INVOKABLE static void showImagePicker();
#endif

    //! Instance used for invoking method from objective-c code
    static IOSHandler *instance();

  public slots:
    //! Middle step method between iOSInterface and iOSImagePicker
    void imagePickerFinished( bool successful, QVariantMap data );

  signals:
    //! Forwarded signal from iOSInterface's ImagePicker
    void forwardedImagePickerFinished( bool successful, QVariantMap data );
};

#endif // IOSHANDLER_H
