/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INPUTUTILS_H
#define INPUTUTILS_H

#define STR1(x)  #x
#define STR(x)  STR1(x)

#include <QObject>
#include <QtGlobal>
#include <QUuid>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QtPositioning/QGeoCoordinate>
#include <QModelIndex>

#include <limits>

#include "inputhelp.h"
#include "merginapi.h"
#include "androidutils.h"
#include "featurelayerpair.h"
#include "qgsquickmapsettings.h"
#include "positionkit.h"
#include "qgis.h"
#include "qgsexpressioncontextutils.h"
#include "qgsmessagelog.h"
#include "qgspoint.h"
#include "qgspointxy.h"
#include "qgsunittypes.h"
#include "qgsquickmapsettings.h"
#include "featurelayerpair.h"
#include "qgscoordinateformatter.h"


class QgsFeature;
class QgsVectorLayer;
class QgsCoordinateReferenceSystem;

class InputUtils: public QObject
{
    Q_OBJECT
  public:
    explicit InputUtils( QObject *parent = nullptr );
    ~InputUtils() override = default;

    Q_INVOKABLE static bool copyFile( const QString &srcPath, const QString &dstPath );

    Q_INVOKABLE QString getFileName( const QString &filePath );
    Q_INVOKABLE QString formatProjectName( const QString &fullProjectName );
    Q_INVOKABLE QString formatNumber( const double number, int precision = 1 );
    Q_INVOKABLE void setExtentToFeature( const FeatureLayerPair &pair, QgsQuickMapSettings *mapSettings, double panelOffsetRatio );

    // utility functions to extract information from map settings
    // (in theory this data should be directly available from .MapTransform
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
    Q_INVOKABLE QVector<double> extractGeometryCoordinates( const FeatureLayerPair &pair, QgsQuickMapSettings *mapSettings );

    /**
     * Renames a file located at a given path with a dateTime. Tend to be use to avoid name conflicts.
     * \param srcPath Absolute path to a file.
     * \param dateTime new name of a file. If is invalid, current dateTime is used.
     * \result Either absolute path of a rename file or empty string.
     */
    Q_INVOKABLE static QString renameWithDateTime( const QString &srcPath, const QDateTime &dateTime = QDateTime() );

    /**
     * Shows notification
     */
    Q_INVOKABLE void showNotification( const QString &message );

    /**
     * Returns speed from positionKit's QGeoPositionInfo.
     * \param PositionKit positionKit.
     * \result The ground speed, in meters/sec.
     */
    Q_INVOKABLE qreal groundSpeedFromSource( PositionKit *positionKit );

    /**
     * Converts bytes to  human readable size (e.g. 1GB, 500MB)
     */
    Q_INVOKABLE static QString bytesToHumanSize( double bytes );

    Q_INVOKABLE bool acquireCameraPermission();

    Q_INVOKABLE void quitApp();

    /**
     * Method copies all entries from given source path to destination path. If cannot copy a file for the first time,
     * removes it and tries again (overwrite a file). If failes again, skips the file, sets result to false and continue.
     * \param srcPath Source path
     * \param dstPath Destination path
     * \result True if operation was fully successful otherwise false.
     */
    static bool cpDir( const QString &srcPath, const QString &dstPath, bool onlyDiffable = false );

    // Returns size of directory in bytes, 0 if path does not exist
    static qint64 dirSize( const QString &path );

    static QString filesToString( QList<MerginFile> files );

    /** InputApp platform */
    static QString appPlatform();

    /**
     * Converts string in rational number format to double.
     * @param rationalValue String - expecting value in format "numerator/denominator" (e.g "123/100").
     * @return -1 if format is invalid, otherwise double value of given string.
     */
    static double convertRationalNumber( const QString &rationalValue );

    /**
     * Converts string coordinate in EXIF rational number format (rational64u[3] - degrees,minutes,seconds) to double.
     * @param rationalValue String - expecting value in format "12/1,3/1,456789/10000".
     * @return 0 if format is invalid, otherwise double for given string coordinate.
     */
    static double convertCoordinateString( const QString &rationalValue );

    /** Formats coordinates from point into degrees format with minutes, seconds and hemisphere */
    Q_INVOKABLE static QString degreesString( const QgsPoint &point );

    //! Creates and registers custom expression functions to Input, so they can be used in default value definitions.
    static void registerInputExpressionFunctions();

    /**
     * @brief Creates formatted string of difference for given tMin and tMax datetimes (in minutes, hours, ... ago).
     * Note, that tMin < tMax, otherwise arguments are invalid.
     * @param tMin Datetime in the past
     * @param tMax Datetime after tMin, currentDateTime by default.
     * @return Formatted string
     */
    static QString formatDateTimeDiff( const QDateTime &tMin, const QDateTime &tMax = QDateTime::currentDateTimeUtc() );

    /**
      * Creates crs from epsg code in QML
      */
    Q_INVOKABLE static QgsCoordinateReferenceSystem coordinateReferenceSystemFromEpsgId( long epsg );

    /**
      * Creates QgsPointXY in QML
      */
    Q_INVOKABLE static QgsPointXY pointXY( double x, double y );

    /**
      * Creates QgsPoint in QML
      */
    Q_INVOKABLE static QgsPoint point( double x, double y, double z = std::numeric_limits<double>::quiet_NaN(), double m = std::numeric_limits<double>::quiet_NaN() );

    /**
      * Converts QGeoCoordinate to QgsPoint
      */
    Q_INVOKABLE static QgsPoint coordinateToPoint( const QGeoCoordinate &coor );

    /**
      * Transforms point between different crs from QML
      */
    Q_INVOKABLE static QgsPointXY transformPoint( const QgsCoordinateReferenceSystem &srcCrs,
        const QgsCoordinateReferenceSystem &destCrs,
        const QgsCoordinateTransformContext &context,
        const QgsPointXY &srcPoint );

    /**
      * Calculates the distance in meter representing baseLengthPixels pixels on the screen based on the current map settings.
      */
    Q_INVOKABLE static double screenUnitsToMeters( QgsQuickMapSettings *mapSettings, int baseLengthPixels );

    /**
      * Returns whether file on path exists
      */
    Q_INVOKABLE static bool fileExists( const QString &path );

    /**
     * Returns working path to load the image from QML.
     * @param path Path of an image - either relative or absolute
     * @param homePath Project path
     * @param config Field widget's config
     * @param pair FeatureLayerPair - needed for expression evaluation
     * @param activeProject QgsProject - needed for expression evaluation
     * @return Path to the image
     */
    Q_INVOKABLE static QString resolvePath( const QString &path, const QString &homePath, const QVariantMap &config, const FeatureLayerPair &pair, QgsProject *activeProject );

    /**
     * This evaluates the "default path" with the following order:
     * 1. evaluate default path expression if defined,
     * 2. use default path value if not empty,
     * 3. use project home folder
     */
    Q_INVOKABLE static QString resolveTargetDir( const QString &homePath, const QVariantMap &config, const FeatureLayerPair &pair, QgsProject *activeProject );


    /**
     * Function used for resolving path of an image for a field with ExternalResource widget type.
     * Returns prefix which has to be added to the field's value to obtain working path to load the image from QML.
     * @param relativeStorageMode: 0 - Relative path disabled; 1 - Relative path to project;
     * 2 - Relative path to defaultRoot defined in the config - Default path field in the widget configuration form
     * @param homePath Project path
     * @param targetDir Default path in the widget configuration
     * @return Returns either homePath, targetDir or empty QString according relativeStorageMode that is configurable in field widget's config
     */
    Q_INVOKABLE static QString resolvePrefixForRelativePath( int relativeStorageMode, const QString &homePath, const QString &targetDir );


    /**
     * Returns absolute path of the file for given path and its prefix. If prefixPath is empty,
     * returns given path.
     * \param path (Relative) path to file,
     * \param prefixPath Empty or prefix for given path to abtain absolute path.
     */
    Q_INVOKABLE static QString getAbsolutePath( const QString &path, const QString &prefixPath );

    /**
     * Returns relative path of the file to given prefixPath. If prefixPath does not match a path parameter,
     * returns an empty string. If a path starts with "file://", this prefix is ignored.
     * \param path Absolute path to file
     * \param prefixPath
     */
    Q_INVOKABLE static QString getRelativePath( const QString &path, const QString &prefixPath );

    /**
      * Log message in QgsMessageLog
      */
    Q_INVOKABLE static void logMessage( const QString &message,
                                        const QString &tag = QString( "" ),
                                        Qgis::MessageLevel level = Qgis::Warning );

    /**
      * FeatureLayerPair factory for tuple of QgsFeature and QgsVectorLayer used in QgsQUick library.
      * \param feature QgsFeature linked to new Feature instance.
      * \param layer QgsVectorLayer which the feature belongs to, optional.
      */
    Q_INVOKABLE static FeatureLayerPair featureFactory( const QgsFeature &feature, QgsVectorLayer *layer = nullptr );

    /**
      * Returns QUrl to image from library's /images folder.
      */
    Q_INVOKABLE static const QUrl getThemeIcon( const QString &name );

    /**
      * Returns url to field editor component for a feature form.
      * If the widgetName does not match any supported widget, text edit is returned.
      * \param widgetName name of the attribute field widget
      * \param config map coming from QGIS describing this field
      * \param field qgsfield instance of this field
      */
    Q_INVOKABLE static const QUrl getEditorComponentSource( const QString &widgetName, const QVariantMap &config = QVariantMap(), const QgsField &field = QgsField() );

    /**
     * \copydoc QgsCoordinateFormatter::format()
     */
    Q_INVOKABLE static QString formatPoint(
      const QgsPoint &point,
      QgsCoordinateFormatter::Format format = QgsCoordinateFormatter::FormatPair,
      int decimals = 3,
      QgsCoordinateFormatter::FormatFlags flags = QgsCoordinateFormatter::FlagDegreesUseStringSuffix );

    /**
      * Converts distance to human readable distance
      *
      * This is useful for scalebar texts or output of the GPS accuracy
      *
      * The resulting units are determined automatically,
      * based on requested system of measurement.
      * e.g. 1222.234 m is converted to 1.2 km
      *
      * \param distance distance in units
      * \param units units of dist
      * \param decimals decimal to use
      * \param destSystem system of measurement of the result
      * \returns string represetation of dist in desired destSystem. For distance less than 0, 0 is returned.
      */
    Q_INVOKABLE static QString formatDistance( double distance,
        QgsUnitTypes::DistanceUnit units,
        int decimals,
        QgsUnitTypes::SystemOfMeasurement destSystem = QgsUnitTypes::MetricSystem );

    /**
      * Deletes file from a given path.
      *
      * \param filePath Absolute path to file
      * \returns bool TRUE, if removal was successful, otherwise FALSE.
      */
    Q_INVOKABLE static bool removeFile( const QString &filePath );

    /**
      * Converts distance to human readable distance in destination system of measurement
      *
      * \sa Utils::formatDistance()
      *
      * \param srcDistance distance in units
      * \param srcUnits units of dist
      * \param destSystem system of measurement of the result
      * \param destDistance output: distance if desired system of measurement
      * \param destUnits output: unit of destDistance
      */
    static void humanReadableDistance( double srcDistance,
                                       QgsUnitTypes::DistanceUnit srcUnits,
                                       QgsUnitTypes::SystemOfMeasurement destSystem,
                                       double &destDistance,
                                       QgsUnitTypes::DistanceUnit &destUnits );

    //! Returns a string with information about screen size and resolution - useful for debugging
    QString dumpScreenInfo() const;

    /**
     * Creates a cache for a value relation field.
     * This can be used to keep the value map in the local memory
     * if doing multiple lookups in a loop.
     * \param config The widget configuration
     * \param formFeature The feature currently being edited with current attribute values
     * \return A kvp list of values for the widget
     */
    Q_INVOKABLE static QVariantMap createValueRelationCache( const QVariantMap &config, const QgsFeature &formFeature = QgsFeature() );

    /**
     * Evaluates expression.
     * \param pair Used to define a context scope.
     * \param activeProject Used to define a context scope.
     * \param expression
     * \return Evaluated expression
     */
    Q_INVOKABLE static QString evaluateExpression( const FeatureLayerPair &pair, QgsProject *activeProject, const QString &expression );

    /**
     * Selects features in a layer
     * This method is required since QML cannot perform the conversion of a feature ID to a QgsFeatureId (i.e. a qint64)
     * \param layer the vector layer
     * \param fids the list of feature IDs
     * \param behavior the selection behavior
     */
    Q_INVOKABLE static void selectFeaturesInLayer( QgsVectorLayer *layer, const QList<int> &fids, QgsVectorLayer::SelectBehavior behavior = QgsVectorLayer::SetSelection );


    /**
    * Returns the QVariant typeName of a \a field.
    * This is a stable identifier (compared to the provider field name).
    * \param field QgsField
    */
    Q_INVOKABLE static QString fieldType( const QgsField &field );


    /**
    * Returns field format's name for given string representing field format defined in QgsDateTimeFieldFormatter.
    * \param fieldFormat string representing formats from QgsDateTimeFieldFormatter.
    */
    Q_INVOKABLE static QString dateTimeFieldFormat( const QString &fieldFormat );

    /**
     * \brief invalidIndex returns invalid index
     */
    Q_INVOKABLE static QModelIndex invalidIndex();

    /**
     * Returns if provided Id is valid ( >= 0 )
     */
    Q_INVOKABLE static bool isFeatureIdValid( qint64 featureId );

    /**
     * \brief setupMapSettings sets visible layers and transform context for map settings based on project
     * \return map settings with layers and transform context set
     */
    Q_INVOKABLE static QgsQuickMapSettings *setupMapSettings( QgsProject *project, QgsQuickMapSettings *settings );

    /**
     * Returns widget setup according the field type - supports only basic types.
     * Note that external widget cannot be guessed from type since its the very same as text.
     * @param field QgsField
     * @return QgsEditorWidgetSetup for given field.
     */
    static const QgsEditorWidgetSetup getEditorWidgetSetup( const QgsField &field );
    static const QgsEditorWidgetSetup getEditorWidgetSetup( const QgsField &field, const QString &widgetType, const QVariantMap &additionalArgs = QVariantMap() );

    // Returns geometry type in form that qml understands
    Q_INVOKABLE static QString geometryFromLayer( QgsVectorLayer *layer );

  signals:
    Q_INVOKABLE void showNotificationRequested( const QString &message );

  public slots:
    void onQgsLogMessageReceived( const QString &message, const QString &tag, Qgis::MessageLevel level );

  private:

    static void formatToMetricDistance( double srcDistance,
                                        QgsUnitTypes::DistanceUnit srcUnits,
                                        double &destDistance,
                                        QgsUnitTypes::DistanceUnit &destUnits );

    static void formatToImperialDistance( double srcDistance,
                                          QgsUnitTypes::DistanceUnit srcUnits,
                                          double &destDistance,
                                          QgsUnitTypes::DistanceUnit &destUnits );

    static void formatToUSCSDistance( double srcDistance,
                                      QgsUnitTypes::DistanceUnit srcUnits,
                                      double &destDistance,
                                      QgsUnitTypes::DistanceUnit &destUnits );

    // on iOS the names from gallery pickers are like
    // file:assets-library://asset/asset.PNG%3Fid=A53AB989-6354-433A-9CB9-958179B7C14D&ext=PNG
    // we need to change it to something more readable
    static QString sanitizeName( const QString &path );

    static double ratherZeroThanNaN( double d );
    std::unique_ptr<AndroidUtils> mAndroidUtils;
};

#endif // INPUTUTILS_H
