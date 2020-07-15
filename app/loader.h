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
#include "qgsquickutils.h"
#include "qgsquickpositionkit.h"
#include "mapthemesmodel.h"
#include "appsettings.h"
#include "activelayer.h"
#include "layersproxymodel.h"

class Loader: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QgsProject *project READ project NOTIFY projectChanged ) // never changes
    Q_PROPERTY( QgsQuickPositionKit *positionKit READ positionKit WRITE setPositionKit NOTIFY positionKitChanged )
    Q_PROPERTY( bool recording READ isRecording WRITE setRecording NOTIFY recordingChanged )
    Q_PROPERTY( QgsQuickMapSettings *mapSettings READ mapSettings WRITE setMapSettings NOTIFY mapSettingsChanged )

  public:
    explicit Loader(
      MapThemesModel &mapThemeModel
      , AppSettings &appSettings
      , ActiveLayer &activeLayer
      , LayersProxyModel &layersProxyModel
      , QObject *parent = nullptr );

    QgsProject *project();

    QgsQuickPositionKit *positionKit() const { return mPositionKit; }
    void setPositionKit( QgsQuickPositionKit *kit );

    bool isRecording() const { return mRecording; }
    void setRecording( bool isRecording );

    Q_INVOKABLE bool load( const QString &filePath );
    Q_INVOKABLE void zoomToProject( QgsQuickMapSettings *mapSettings );
    Q_INVOKABLE QString featureTitle( QgsQuickFeatureLayerPair pair );
    Q_INVOKABLE QString mapTipHtml( QgsQuickFeatureLayerPair pair );
    Q_INVOKABLE QString mapTipType( QgsQuickFeatureLayerPair pair );
    Q_INVOKABLE QString mapTipImage( QgsQuickFeatureLayerPair pair );
    Q_INVOKABLE QStringList mapTipFields( QgsQuickFeatureLayerPair pair );
    Q_INVOKABLE QString loadIconFromLayer( QgsMapLayer *layer );

    /**
     * Updates active map theme.
     * \param index Represents row number in the map theme model.
     */
    Q_INVOKABLE void setActiveMapTheme( int index );

    //! A File on this path represents a project is loading and exists only during the process.
    static const QString LOADING_FLAG_FILE_PATH;

    //! Gets map settings
    QgsQuickMapSettings *mapSettings() const;

    //! Sets map settings and loads the layer list from previously assigned map settings
    void setMapSettings( QgsQuickMapSettings *mapSettings );

    void setMapSettingsLayers() const;

    void setActiveLayerFromName( QString layerName ) const;

  signals:
    void projectChanged();
    void projectReloaded();

    void positionKitChanged();
    void recordingChanged();

    void loadingStarted();
    void loadingFinished();

    void mapSettingsChanged();

  public slots:
    void appStateChanged( Qt::ApplicationState state );
    // Reloads project if current project path matches given path (its the same project)
    bool reloadProject( QString projectDir );

  private:
    QList<QgsExpressionContextScope *> globalProjectLayerScopes( QgsMapLayer *layer );
    QgsProject *mProject = nullptr;
    QgsQuickPositionKit *mPositionKit = nullptr;
    bool mRecording = false;

    MapThemesModel &mMapThemeModel;
    AppSettings &mAppSettings;
    ActiveLayer &mActiveLayer;
    LayersProxyModel &mLayersProxyModel;
    QgsQuickMapSettings *mMapSettings = nullptr;

    /**
    * Reloads project.
    * \param filePath Path to project file.
    * \param force If true, reloads mProject on given path withload loading signals - suppose to be called internally,
    * otherwise used only for loading a new projects (evoked by a user).
    */
    bool forceLoad( const QString &filePath, bool force );
};

#endif // LOADER_H
