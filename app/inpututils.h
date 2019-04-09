#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <QObject>
#include "qgsquickfeaturelayerpair.h"
#include "qgsquickmapsettings.h"

class InputUtils: public QObject
{
    Q_OBJECT
  public:
    explicit InputUtils( QObject *parent = nullptr );
    ~InputUtils() = default;

    Q_INVOKABLE bool removeFile( const QString &filePath );
    Q_INVOKABLE bool copyFile( const QString &srcPath, const QString &dstPath );
    Q_INVOKABLE QString getFileName( const QString &filePath );

    Q_INVOKABLE void setExtentToFeature( const QgsQuickFeatureLayerPair &pair, QgsQuickMapSettings *mapSettings, double panelOffsetRatio );
};

#endif // INPUTUTILS_H
