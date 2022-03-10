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

#include "loader.h"
#include "inpututils.h"
#include "coreutils.h"

const QString Loader::LOADING_FLAG_FILE_PATH = QString( "%1/.input_loading_project" ).arg( QStandardPaths::standardLocations( QStandardPaths::TempLocation ).first() );

Loader::Loader( MapThemesModel &mapThemeModel
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
}

QgsProject *Loader::qgsProject()
{
  return mQgsProject;
}

LocalProject Loader::project()
{
  return mProject;
}

bool Loader::load( const QString &filePath )
{
  return forceLoad( filePath, false );
}

bool Loader::forceLoad( const QString &filePath, bool force )
{
  CoreUtils::log( QStringLiteral( "Project loading" ), filePath + " " + force );

  // Just clear project if empty
  if ( filePath.isEmpty() )
  {
    emit projectWillBeReloaded();
    mQgsProject->clear();
    whileBlocking( &mActiveLayer )->resetActiveLayer();
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
    res = mQgsProject->read( filePath );
    mActiveLayer.resetActiveLayer();
    mMapThemeModel.reloadMapThemes( mQgsProject );
    mProject = mLocalProjectsManager.projectFromProjectFilePath( filePath );

    if ( !mProject.isValid() )
    {
      CoreUtils::log( QStringLiteral( "Project load" ), QStringLiteral( "Could not find project in local projects: " ) + filePath );
    }

    QgsVectorLayer *defaultLayer = mRecordingLayerPM.layerFromLayerName( mAppSettings.defaultLayer() );
    if ( defaultLayer )
      setActiveLayer( defaultLayer );
    else
      setActiveLayer( mRecordingLayerPM.firstUsableLayer() );

    setMapSettingsLayers();

    emit projectReloaded( mQgsProject );
    emit projectChanged( mProject );
  }

  bool foundInvalidLayer = false;
  QStringList invalidLayers;
  for ( QgsMapLayer *layer : mQgsProject->mapLayers().values() )
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
  return res;
}

bool Loader::reloadProject( QString projectDir )
{
  if ( mQgsProject->homePath() == projectDir )
  {
    return forceLoad( mQgsProject->fileName(), true );
  }
  return false;
}

void Loader::setMapSettings( QgsQuickMapSettings *mapSettings )
{
  if ( mMapSettings == mapSettings )
    return;

  mMapSettings = mapSettings;
  setMapSettingsLayers();

  emit mapSettingsChanged();
}

void Loader::setMapSettingsLayers() const
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

QgsQuickMapSettings *Loader::mapSettings() const
{
  return mMapSettings;
}

bool Loader::layerVisible( QgsMapLayer *layer )
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

QString Loader::projectLoadingLog() const
{
  return mProjectLoadingLog;
}

void Loader::setActiveMapTheme( int index )
{
  QString name = mMapThemeModel.setActiveThemeIndex( index );

  // if active layer is no longer visible
  if ( !layerVisible( mActiveLayer.layer() ) )
    setActiveLayer( mRecordingLayerPM.firstUsableLayer() );

  setMapSettingsLayers();
}

void Loader::setActiveLayerByName( QString layerName ) const
{
  if ( !layerName.isEmpty() )
  {
    QList<QgsMapLayer *> layersByName = QgsProject::instance()->mapLayersByName( layerName );

    if ( !layersByName.isEmpty() )
    {
      return setActiveLayer( layersByName.at( 0 ) );
    }
  }

  setActiveLayer( nullptr );
}

void Loader::setActiveLayer( QgsMapLayer *layer ) const
{
  if ( !layer || !layer->isValid() )
    mActiveLayer.resetActiveLayer();
  else
  {
    mActiveLayer.setActiveLayer( layer );
    mAppSettings.setDefaultLayer( mActiveLayer.layerName() );
  }
}
