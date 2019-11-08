#ifndef IOSIMAGEPICKER_H
#define IOSIMAGEPICKER_H

#include <QObject>

//#include "iosinterface.h"

class IOSImagePicker : public QObject
{
    Q_OBJECT
public:
    explicit IOSImagePicker(QObject *parent = nullptr);

    Q_INVOKABLE void test();

    //Q_INVOKABLE int someMethod (void *objectiveCObject, void *aParameter);

signals:

public slots:
};

#endif // IOSIMAGEPICKER_H
