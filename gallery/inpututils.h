#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <QObject>

class InputUtils: public QObject
{
    Q_OBJECT

  public:
    explicit InputUtils( QObject *parent = nullptr ) {}

    Q_INVOKABLE bool acquireCameraPermission() { return true; }
    Q_INVOKABLE static QString fieldType( const QObject &field ) { return "QDate"; }
    Q_INVOKABLE static QString dateTimeFieldFormat( const QString &fieldFormat ) { return "QDateTime"; }
    Q_INVOKABLE bool fileExists( const QString &path ) { return false; }
    Q_INVOKABLE static QString resolveTargetDir( const QString &homePath, const QVariantMap &config, const QString &pair, QString activeProject ) { return ""; }
    Q_INVOKABLE static QString resolvePrefixForRelativePath( int relativeStorageMode, const QString &homePath, const QString &targetDir ) { return ""; }
    Q_INVOKABLE static QString imageGalleryLocation() { return ""; }
    Q_INVOKABLE static QString getAbsolutePath( const QString &path, const QString &prefixPath ) { return ""; }

    Q_PROPERTY( bool isIos READ isIos CONSTANT )
    Q_PROPERTY( bool isAndroid READ isAndroid CONSTANT )

    bool isIos() { return false; }
    bool isAndroid() { return true; }

  signals:
    void imageSelected( QString imagePath, QString code );
};

#endif // INPUTUTILS_H
