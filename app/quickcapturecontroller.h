/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUICKCAPTURECONTROLLER_H
#define QUICKCAPTURECONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QVariantList>
#include <QVariantMap>
#include <QStringList>
#include <QMap>

class QgsProject;
class QgsVectorLayer;
class VariablesManager;

/**
 * Quick capture lets field users save a point at their GPS position with a single tap,
 * without opening the feature form. Field values come from the layer's default value
 * expressions (e.g. now(), @mm_username, @position_horizontal_accuracy), so the whole
 * behaviour is configured in the QGIS project.
 *
 * Configuration is read from the "quickCapture" section of the field profile (see FieldProfile).
 * It is synced with the project like any other file, so changing it changes the app behaviour on all devices:
 *
 * {
 *   "version": 1,
 *   "quickCapture": {
 *     "layer": "observations",              // layer name or id, must be a point layer
 *     "typeField": "obs_type",              // field set by the chosen quick capture type
 *     "types": [ { "label": "Inaccessible road", "value": "inaccessible_road" } ],
 *                                           // optional, defaults to the typeField's value map
 *     "maxHorizontalAccuracy": 50,          // optional, refuse capture when GPS accuracy is worse (metres)
 *     "media": {                            // optional, enables the media button
 *       "typeValue": "field_note",          // optional, type field value of media points
 *       "photoField": "photo",              // each field is optional, only configured media are offered
 *       "audioField": "audio",
 *       "videoField": "video"
 *     }
 *   }
 * }
 */
class QuickCaptureController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )
    Q_PROPERTY( VariablesManager *variablesManager READ variablesManager WRITE setVariablesManager NOTIFY variablesManagerChanged )

    Q_PROPERTY( bool enabled READ enabled NOTIFY configurationChanged )
    Q_PROPERTY( QVariantList types READ types NOTIFY configurationChanged )
    Q_PROPERTY( QStringList mediaKinds READ mediaKinds NOTIFY configurationChanged ) // subset of "photo", "audio", "video"

  public:
    explicit QuickCaptureController( QObject *parent = nullptr );

    //! Re-reads the configuration from the field profile of the project
    Q_INVOKABLE void reload();

    //! Saves a point at the current GPS position with the type field set to \a typeValue
    Q_INVOKABLE bool capturePoint( const QVariant &typeValue );

    //! Returns the folder where the media file of \a kind ("photo", "audio" or "video") should be stored
    Q_INVOKABLE QString mediaTargetDir( const QString &kind ) const;

    /**
     * Saves a point at the current GPS position with the media file at \a mediaPath attached.
     * The file is copied to the media target folder if it is not there yet.
     */
    Q_INVOKABLE bool captureMedia( const QString &kind, const QString &mediaPath );

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *project );

    VariablesManager *variablesManager() const;
    void setVariablesManager( VariablesManager *variablesManager );

    bool enabled() const;
    QVariantList types() const;
    QStringList mediaKinds() const;

  signals:
    void qgsProjectChanged();
    void variablesManagerChanged();
    void configurationChanged();

    void captureSucceeded( const QString &message );
    void captureFailed( const QString &message );

  private:
    QgsVectorLayer *captureLayer() const;
    QVariantMap fieldConfig( const QString &fieldName ) const;
    QString typeLabel( const QVariant &typeValue ) const;
    bool saveFeature( const QVariantMap &attributes, const QString &successMessage );
    void clearConfiguration();

    QPointer<QgsProject> mProject;
    QPointer<VariablesManager> mVariablesManager;

    QString mLayerId;
    QString mTypeField;
    QVariantList mTypes;
    double mMaxHorizontalAccuracy = -1;
    QMap<QString, QString> mMediaFields; // media kind -> attachment field name
    QVariant mMediaTypeValue;
};

#endif // QUICKCAPTURECONTROLLER_H
