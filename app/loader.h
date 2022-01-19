/***************************************************************************
  app.h
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : peter.petrik@lutraconsulting.co.uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include "qgsproject.h"
#include "inpututils.h"
#include "mapthemesmodel.h"
#include "appsettings.h"
#include "activelayer.h"
#include "layersproxymodel.h"

class QgsQuickMapSettings;

class Loader: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QgsProject *project READ project NOTIFY projectChanged ) // never changes
    Q_PROPERTY( bool recording READ isRecording WRITE setRecording NOTIFY recordingChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    explicit Loader(
      MapThemesModel &mapThemeModel
      , AppSettings &appSettings
      , ActiveLayer &activeLayer
      , LayersProxyModel &recordingLayerPM
      , QObject *parent = nullptr );

    QgsProject *project();

    bool isRecording() const { return mRecording; }
    void setRecording( bool isRecording );

    Q_INVOKABLE bool load( const QString &filePath );
    Q_INVOKABLE void zoomToProject( QgsQuickMapSettings *mapSettings );
    Q_INVOKABLE QString loadIconFromLayer( QgsMapLayer *layer );
    Q_INVOKABLE QString loadIconFromFeature( QgsFeature feature );

    /**
     * Updates active map theme.
     * \param index Represents row number in the map theme model.
     */
    Q_INVOKABLE void setActiveMapTheme( int index );

    /**
     * setActiveLayer sets active layer from layer name
     */
    void setActiveLayerByName( QString layerName ) const;

    /**
     * setActiveLayer sets active layer from layer
     */
    Q_INVOKABLE void setActiveLayer( QgsMapLayer *layer ) const;

    //! A File on this path represents a project is loading and exists only during the process.
    static const QString LOADING_FLAG_FILE_PATH;

    /**
     * mapSettings method returns mapsettings pointer
     */
    QgsQuickMapSettings *mapSettings() const;

    /**
     * setMapSettings method sets mapSettings
     * Method also reloads the layer list
     */
    void setMapSettings( QgsQuickMapSettings *mapSettings );

    /**
     * setMapSettingsLayers reloads layer list from current project
     */
    void setMapSettingsLayers() const;

    /**
     * layerVisible returns boolean if input layer is visible within current project
     */
    bool layerVisible( QgsMapLayer *layer );

    /**
     * Return the QGIS log recorded during the loading phase of the project
     */
    Q_INVOKABLE QString projectLoadingLog() const;

  signals:
    void projectChanged();
    void projectReloaded( QgsProject *project );
    void projectWillBeReloaded( const QString &projectFile );

    void recordingChanged();

    void loadingStarted();
    void loadingFinished();

    void reportIssue( QString layerName, QString message );
    void loadingErrorFound();
    void qgisLogChanged();
    void setProjectIssuesHeader( QString text );

    void mapSettingsChanged();

  public slots:
    void appStateChanged( Qt::ApplicationState state );
    // Reloads project if current project path matches given path (its the same project)
    bool reloadProject( QString projectDir );
    void appAboutToQuit();

  private:
    QString iconFromGeometry( const QgsWkbTypes::GeometryType &geometry );


    QgsProject *mProject = nullptr;
    bool mRecording = false;

    MapThemesModel &mMapThemeModel;
    AppSettings &mAppSettings;
    ActiveLayer &mActiveLayer;
    LayersProxyModel &mRecordingLayerPM;
    QgsQuickMapSettings *mMapSettings = nullptr;

    QString mProjectLoadingLog;

    /**
    * Reloads project.
    * \param filePath Path to project file.
    * \param force If true, reloads mProject on given path withload loading signals - suppose to be called internally,
    * otherwise used only for loading a new projects (evoked by a user).
    */
    bool forceLoad( const QString &filePath, bool force );
};

#endif // LOADER_H
