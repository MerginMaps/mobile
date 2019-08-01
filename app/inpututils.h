#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#include <QObject>
#include "merginapi.h"
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
    Q_INVOKABLE QString formatProjectName( const QString &fullProjectName );

    Q_INVOKABLE void setExtentToFeature( const QgsQuickFeatureLayerPair &pair, QgsQuickMapSettings *mapSettings, double panelOffsetRatio );

    // utility functions to extract information from map settings
    // (in theory this data should be directly available from QgsQuick.MapTransform
    // but they are not currently, so this is a workaround we need for display of markers)

    Q_INVOKABLE double mapSettingsScale( QgsQuickMapSettings *ms );
    Q_INVOKABLE double mapSettingsOffsetX( QgsQuickMapSettings *ms );
    Q_INVOKABLE double mapSettingsOffsetY( QgsQuickMapSettings *ms );

    /**
     * Extract geometry coordinates from the given feature.
     *
     * The output can encode also multi-part geometries or even geometry collections.
     * We pass a single array out of the function, so this is the encoding of coordinates:
     * - point:  <value 0> <x> <y>
     * - linestring: <value 1> <number of points> <x1> <y1> ... <xn> <yn>
     * - polygon: <value 2> <number of points> <x1> <y1> ... <xn> <yn>
     *
     * The output is a chain of sub-geometries. Polygon's holes (interior rings) are treated just
     * like exterior ring because when we create a singla path, Qt automatically detects which rings
     * are holes by using even-odd fill rule.
     *
     * If the layer's CRS is not the same as map CRS, the geometry will be first transformed to map CRS.
     */
    Q_INVOKABLE QVector<double> extractGeometryCoordinates( const QgsQuickFeatureLayerPair &pair, QgsQuickMapSettings *mapSettings );

    /**
     * Renames a file located at a given path with given prefix and current timestamp. Prefix is omitted of empty.
     * \param srcPath Absolute path to a file.
     * \param prefix String that is used as a prefix for a new unique name.
     * \param dateTime Used as a part of a new name.
     * \result Either absolute path of a rename file or empty string.
     */
    Q_INVOKABLE static QString renameWithUniqueName( const QString &srcPath, const QString &prefix, const QDateTime &dateTime );

    static bool cpDir( const QString &srcPath, const QString &dstPath );

    static void log( const QString &msg, const QString &info = QStringLiteral() );

    static void setLogFilename( const QString &value );

    static QString filesToString( QList<MerginFile> files );

  private:

    static QString sLogFile;

    static void appendLog( const QByteArray &data, const QString &path );
};

#endif // INPUTUTILS_H
