/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef ACTIVEPROJECT_H
#define ACTIVEPROJECT_H

#include <QObject>

#include "qgsproject.h"

#include "inpututils.h"
#include "mapthemesmodel.h"
#include "appsettings.h"
#include "activelayer.h"
#include "layersproxymodel.h"
#include "localprojectsmanager.h"

class QgsQuickMapSettings;
class AutosyncController;

/**
 * \brief The ActiveProject class can load a QGIS project and holds its data.
 */
class ActiveProject: public QObject
{
    Q_OBJECT
    Q_PROPERTY( LocalProject localProject READ localProject NOTIFY localProjectChanged ) // LocalProject instance of active project, changes when project is loaded
    Q_PROPERTY( QgsProject *qgsProject READ qgsProject NOTIFY qgsProjectChanged ) // QgsProject instance of active project, never changes
    Q_PROPERTY( AutosyncController *autosyncController READ autosyncController NOTIFY autosyncControllerChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    explicit ActiveProject(
      MapThemesModel &mapThemeModel
      , AppSettings &appSettings
      , ActiveLayer &activeLayer
      , LayersProxyModel &recordingLayerPM
      , LocalProjectsManager &localProjectsManager
      , QObject *parent = nullptr );

    virtual ~ActiveProject();

    //! Returns active project's QgsProject instance to do QGIS API magic
    QgsProject *qgsProject();

    //! Returns Input related info about active project
    LocalProject localProject();

    /**
     * Loads a .qgz/.qgs project file specified by filePath.
     * \param filePath Path to project file.
     */
    Q_INVOKABLE bool load( const QString &filePath );

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

    /**
     * mapSettings method returns mapsettings pointer
     */
    QgsQuickMapSettings *mapSettings() const;

    //! Returns autosyncController instance if autosync is allowed, otherwise returns nullptr
    AutosyncController *autosyncController() const;

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

    //! A File on this path represents that project is loading and exists only during the process.
    static const QString LOADING_FLAG_FILE_PATH;

  signals:
    void qgsProjectChanged();
    void localProjectChanged( LocalProject project );

    void projectWillBeReloaded();
    void projectReloaded( QgsProject *project );
    void loadingStarted();
    void loadingFinished();

    void reportIssue( QString layerName, QString message );
    void loadingErrorFound();
    void qgisLogChanged();

    void autosyncControllerChanged( AutosyncController *controller );

    void mapSettingsChanged();

    void syncActiveProject( const LocalProject &project );

  public slots:
    // Reloads project if current project path matches given path (its the same project)
    bool reloadProject( QString projectDir );

    void setAutosyncEnabled( bool enabled );

  private:

    QgsProject *mQgsProject = nullptr;
    LocalProject mLocalProject;

    MapThemesModel &mMapThemeModel;
    AppSettings &mAppSettings;
    ActiveLayer &mActiveLayer;
    LayersProxyModel &mRecordingLayerPM;
    LocalProjectsManager &mLocalProjectsManager;
    QgsQuickMapSettings *mMapSettings = nullptr;

    std::unique_ptr<AutosyncController> mAutosyncController;

    QString mProjectLoadingLog;

    /**
    * Reloads project.
    * \param filePath Path to project file.
    * \param force If true, reloads mProject on given path withload loading signals - suppose to be called internally,
    * otherwise used only for loading a new projects (evoked by a user).
    */
    bool forceLoad( const QString &filePath, bool force );
};

#endif // ACTIVEPROJECT_H
