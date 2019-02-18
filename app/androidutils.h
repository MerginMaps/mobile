#ifndef ANDROIDUTILS_H
#define ANDROIDUTILS_H

#include <QObject>

class AndroidUtils: public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT )
public:
    explicit AndroidUtils( QObject* parent = nullptr );

    bool isAndroid() const;
    static void requirePermissions();
    static bool checkAndAcquirePermissions( const QString &permissionString );


public slots:
    void showToast( QString message );

private:
    bool mIsAndroid;

};

#endif // ANDROIDUTILS_H
