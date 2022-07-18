/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QDebug>
#include <QStandardPaths>
#include <QTimer>

#include "qgsvectorlayer.h"
#include "qgslayertree.h"
#include "qgslayertreelayer.h"
#include "qgslayertreegroup.h"
#include "qgsmapthemecollection.h"
#include "qgsquickmapsettings.h"
#include "qgsapplication.h"
#include "qgslogger.h"

#include "activeproject.h"
#include "inpututils.h"
#include "coreutils.h"
#include "autosynccontroller.h"

const QString ActiveProject::LOADING_FLAG_FILE_PATH = QString( "%1/.input_loading_project" ).arg( QStandardPaths::standardLocations( QStandardPaths::TempLocation ).first() );

ActiveProject::ActiveProject( MapThemesModel &mapThemeModel
                              , AppSettings &appSettings
                              , ActiveLayer &activeLayer
                              , LayersProxyModel &recordingLayerPM
                              , LocalProjectsManager &localProjectsManager
                              , QObject *parent ) :

  QObject( parent )
  , mMapThemeModel( mapThemeModel )
  , mAppSettings( appSettings )
  , mActiveLayer( activeLayer )
  , mRecordingLayerPM( recordingLayerPM )
  , mLocalProjectsManager( localProjectsManager )
  , mProjectLoadingLog( "" )
{
  // we used to have our own QgsProject instance, but unfortunately few pieces of qgis_core
  // still work with QgsProject::instance() singleton hardcoded (e.g. vector layer's feature
  // iterator uses it for virtual fields, causing minor bugs with expressions)
  // so for the time being let's just stick to using the singleton until qgis_core is completely fixed
  mQgsProject = QgsProject::instance();

  // listen to local project removal event to invalidate mProject
  QObject::connect(
    &mLocalProjectsManager,
    &LocalProjectsManager::aboutToRemoveLocalProject,
    this, [this]( const LocalProject & project )
  {
    if ( project.id() == mLocalProject.id() )
    {
      load( QLatin1String() );
    }
  } );

  // listen to metadata changes of opened LocalProject (e.g. local version update or namespace update)
  QObject::connect(
    &mLocalProjectsManager,
    &LocalProjectsManager::localProjectDataChanged,
    this, [this]( const LocalProject & project )
  {
    if ( project.projectDir == mLocalProject.projectDir )
    {
      mLocalProject = project;
      emit localProjectChanged( mLocalProject );
    }
  } );

  setAutosyncEnabled( mAppSettings.autosyncAllowed() );

  QObject::connect( &mAppSettings, &AppSettings::autosyncAllowedChanged, this, &ActiveProject::setAutosyncEnabled );
}

ActiveProject::~ActiveProject() = default;

QgsProject *ActiveProject::qgsProject()
{
  return mQgsProject;
}

LocalProject ActiveProject::localProject()
{
  return mLocalProject;
}

QString ActiveProject::projectFullName() const
{
  return mLocalProject.fullName();
}

bool ActiveProject::load( const QString &filePath )
{
  return forceLoad( filePath, false );
}

bool ActiveProject::forceLoad( const QString &filePath, bool force )
{
  CoreUtils::log( QStringLiteral( "Project loading" ), filePath + " " + force );

  // clear autosync
  setAutosyncEnabled( false );

  // Just clear project if empty
  if ( filePath.isEmpty() )
  {
    emit projectWillBeReloaded();

    whileBlocking( &mActiveLayer )->resetActiveLayer();
    mLocalProject = LocalProject();
    mQgsProject->clear();

    emit localProjectChanged( mLocalProject );
    emit projectReloaded( mQgsProject );

    return true;
  }

  if ( !force )
    emit loadingStarted();

  QFile flagFile( LOADING_FLAG_FILE_PATH );
  flagFile.open( QIODevice::WriteOnly );
  flagFile.close();

  mProjectLoadingLog.clear();

  QString logFilePath = CoreUtils::logFilename();
  qint64 alreadyAppendedCharsCount = 0;

  {
    QFile file( logFilePath );
    alreadyAppendedCharsCount = file.size();
  }

  // Give some time to other (GUI) processes before loading a project in the main thread
  QEventLoop loop;
  QTimer t;
  t.connect( &t, &QTimer::timeout, &loop, &QEventLoop::quit );
  t.start( 10 );
  loop.exec();

  bool res = true;
  if ( mQgsProject->fileName() != filePath || force )
  {
    emit projectWillBeReloaded();
    mActiveLayer.resetActiveLayer();
    res = mQgsProject->read( filePath );
    mMapThemeModel.reloadMapThemes( mQgsProject );
    mLocalProject = mLocalProjectsManager.projectFromProjectFilePath( filePath );

    if ( !mLocalProject.isValid() )
    {
      CoreUtils::log( QStringLiteral( "Project load" ), QStringLiteral( "Could not find project in local projects: " ) + filePath );
    }

    QgsVectorLayer *defaultLayer = mRecordingLayerPM.layerFromLayerName( mAppSettings.defaultLayer() );
    if ( defaultLayer )
      setActiveLayer( defaultLayer );
    else
      setActiveLayer( mRecordingLayerPM.firstUsableLayer() );

    setMapSettingsLayers();

    emit localProjectChanged( mLocalProject );
    emit projectReloaded( mQgsProject );
  }

  bool foundInvalidLayer = false;
  QStringList invalidLayers;
  for ( QgsMapLayer *layer : mQgsProject->mapLayers() )
  {
    if ( !layer->isValid() )
    {
      invalidLayers.append( layer->name() );
      foundInvalidLayer = true;
      emit reportIssue( layer->name(), layer->publicSource() );
    }
  }

  flagFile.remove();
  if ( !force )
  {
    emit loadingFinished();

    if ( foundInvalidLayer )
    {
      QString message = QStringLiteral( "WARNING: The following layers are invalid: %1" ).arg( invalidLayers.join( ", " ) );
      CoreUtils::log( "project loading", message );

      QFile file( logFilePath );
      if ( file.open( QIODevice::ReadOnly ) )
      {
        file.seek( alreadyAppendedCharsCount );
        QByteArray neededLogFileData = file.readAll();
        mProjectLoadingLog = QString::fromStdString( neededLogFileData.toStdString() );
        file.close();
      }
      emit qgisLogChanged();
      emit loadingErrorFound();
    }
  }

  if ( mAppSettings.autosyncAllowed() )
  {
    setAutosyncEnabled( true );
  }

  return res;
}

bool ActiveProject::reloadProject( QString projectDir )
{
  if ( mQgsProject->homePath() == projectDir )
  {
    // store current project extent
    QgsRectangle extent;
    if ( mMapSettings )
      extent = mMapSettings->extent();

    bool result = forceLoad( mQgsProject->fileName(), true );

    // restore extent
    if ( mMapSettings && !extent.isNull() )
      mMapSettings->setExtent( extent );

    return result;
  }
  return false;
}

void ActiveProject::setAutosyncEnabled( bool enabled )
{
  if ( enabled )
  {
    if ( mAutosyncController )
    {
      mAutosyncController->disconnect();
      mAutosyncController.reset();
    }

    mAutosyncController = std::make_unique<AutosyncController>( mQgsProject );

    connect( mAutosyncController.get(), &AutosyncController::projectChangeDetected, this, &ActiveProject::requestSync );
  }
  else
  {
    if ( mAutosyncController )
    {
      mAutosyncController->disconnect();
    }
    mAutosyncController.reset();
  }
}

void ActiveProject::requestSync()
{
  emit syncActiveProject( mLocalProject );
}

void ActiveProject::setMapSettings( QgsQuickMapSettings *mapSettings )
{
  if ( mMapSettings == mapSettings )
    return;

  mMapSettings = mapSettings;
  setMapSettingsLayers();

  emit mapSettingsChanged();
}

void ActiveProject::setMapSettingsLayers() const
{
  if ( !mQgsProject || !mMapSettings ) return;

  QgsLayerTree *root = mQgsProject->layerTreeRoot();

  // Get list of all visible and valid layers in the project
  QList< QgsMapLayer * > allLayers;
  foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
  {
    if ( nodeLayer->isVisible() )
    {
      QgsMapLayer *layer = nodeLayer->layer();
      if ( layer && layer->isValid() )
      {
        allLayers << layer;
      }
    }
  }

  mMapSettings->setLayers( allLayers );
  mMapSettings->setTransformContext( mQgsProject->transformContext() );
}

QgsQuickMapSettings *ActiveProject::mapSettings() const
{
  return mMapSettings;
}

AutosyncController *ActiveProject::autosyncController() const
{
  if ( mAutosyncController )
  {
    return mAutosyncController.get();
  }

  return nullptr;
}

bool ActiveProject::layerVisible( QgsMapLayer *layer )
{
  if ( !layer ) return false;

  // check if active layer is visible in current map theme too
  QgsLayerTree *root = QgsProject::instance()->layerTreeRoot();
  foreach ( QgsLayerTreeLayer *nodeLayer, root->findLayers() )
  {
    if ( nodeLayer->isVisible() )
    {
      QgsMapLayer *nLayer = nodeLayer->layer();
      if ( nLayer && nLayer->isValid() && nLayer->id() == layer->id() )
      {
        return true;
      }
    }
  }
  return false;
}

QString ActiveProject::projectLoadingLog() const
{
  return mProjectLoadingLog;
}

void ActiveProject::setActiveMapTheme( int index )
{
  QString name = mMapThemeModel.setActiveThemeIndex( index );

  // if active layer is no longer visible
  if ( !layerVisible( mActiveLayer.layer() ) )
    setActiveLayer( mRecordingLayerPM.firstUsableLayer() );

  setMapSettingsLayers();
}

void ActiveProject::setActiveLayer( QgsMapLayer *layer ) const
{
  if ( !layer || !layer->isValid() )
    mActiveLayer.resetActiveLayer();
  else
  {
    mActiveLayer.setActiveLayer( layer );
    mAppSettings.setDefaultLayer( mActiveLayer.layerName() );
  }
}
