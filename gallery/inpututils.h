#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <QObject>

class InputUtils: public QObject
{
    Q_OBJECT

  public:
    explicit InputUtils( QObject *parent = nullptr ) {};

    Q_INVOKABLE bool acquireCameraPermission() { return true; }
    Q_INVOKABLE static QString fieldType( const QObject &field ) { return "QDate"; };
    Q_INVOKABLE static QString dateTimeFieldFormat( const QString &fieldFormat ) { return "QDateTime"; };
};

#endif // INPUTUTILS_H
