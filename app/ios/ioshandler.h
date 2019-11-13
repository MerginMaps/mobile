#ifndef IOSHANDLER_H
#define IOSHANDLER_H

#include <QObject>
#include <QWidget>
#include <QVariantMap>

class IOSHandler : public QObject
{
    Q_OBJECT
public:
    explicit IOSHandler(QObject *parent = nullptr);

    Q_INVOKABLE static void showImagePicker();

    static IOSHandler* instance();

public slots:
    void imagePickerChoosen(QString name , QVariantMap data);

signals:
    void imagePickerFinished(QString name , QVariantMap data);
};

#endif // IOSHANDLER_H
