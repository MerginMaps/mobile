#ifndef IOSHANDLER_H
#define IOSHANDLER_H

#include <QObject>
#include <QWidget>

class IOSHandler : public QObject
{
    Q_OBJECT
public:
    explicit IOSHandler(QObject *parent = nullptr);

    static void objectiveC_Call();

//    static IOSHandler* instance();

//    static IOSHandler* mInstance;

signals:

public slots:
};

#endif // IOSHANDLER_H
