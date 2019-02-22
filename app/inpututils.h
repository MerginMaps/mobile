#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <QObject>

class InputUtils: public QObject
{
    Q_OBJECT
public:
    explicit InputUtils( QObject* parent = nullptr );
    ~InputUtils() = default;

    Q_INVOKABLE bool removeFile( const QString &filePath );
    Q_INVOKABLE bool cpFile( const QString &srcPath, const QString &dstPath );
};

#endif // INPUTUTILS_H
