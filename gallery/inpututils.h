#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <QObject>

class InputUtils: public QObject
{
    Q_OBJECT

  public:
    explicit InputUtils( QObject *parent = nullptr ) {};

    Q_INVOKABLE bool acquireCameraPermission() { return true; }
};

#endif // INPUTUTILS_H
