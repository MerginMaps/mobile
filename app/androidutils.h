#ifndef ANDROIDUTILS_H
#define ANDROIDUTILS_H


#ifdef ANDROID
#include <QAndroidActivityResultReceiver>
#include <QAndroidJniObject>
#endif
#include <QObject>

class AndroidUtils: public QObject
        #ifdef ANDROID
        , QAndroidActivityResultReceiver
        #endif
{
    Q_OBJECT
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT )

public:
    explicit AndroidUtils( QObject* parent = nullptr );

    bool isAndroid() const;
    static void requirePermissions();
    static bool checkAndAcquirePermissions( const QString &permissionString );

    Q_INVOKABLE void callImagePicker();
#ifdef ANDROID
    void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data);

#endif

signals:
    void imageSelected(QString imagePath);

public slots:
    void showToast( QString message );

private:
    bool mIsAndroid;

};

#endif // ANDROIDUTILS_H
