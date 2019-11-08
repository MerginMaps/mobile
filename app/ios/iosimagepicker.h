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

signals:

public slots:
};

#endif // IOSIMAGEPICKER_H
