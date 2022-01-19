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

#include "loader.h"
#include "inpututils.h"
#include "coreutils.h"
#include "qgsvectorlayer.h"
#include "qgslayertree.h"
#include "qgslayertreelayer.h"
#include "qgslayertreegroup.h"
#include "qgsmapthemecollection.h"
#include "qgsquickmapsettings.h"
#include <qgsapplication.h>
#include <qgslogger.h>

#if VERSION_INT >= 30500
// this header only exists in QGIS >= 3.6
#include "qgsexpressioncontextutils.h"
#endif
#include <QDebug>

#include <qgsprojectviewsettings.h>
#include <QStandardPaths>
#include <QTimer>

const QString Loader::LOADING_FLAG_FILE_PATH = QString( "%1/.input_loading_project" ).arg( QStandardPaths::standardLocations( QStandardPaths::TempLocation ).first() );

Loader::Loader( MapThemesModel &mapThemeModel
                , AppSettings &appSettings
                , ActiveLayer &activeLayer
                , LayersProxyModel &recordingLayerPM
                , QObject *parent ) :

  QObject( parent )
  , mMapThemeModel( mapThemeModel )
  , mAppSettings( appSettings )
  , mActiveLayer( activeLayer )
  , mRecordingLayerPM( recordingLayerPM )
  , mProjectLoadingLog( "" )
{
  // we used to have our own QgsProject instance, but unfortunately few pieces of qgis_core
  // still work with QgsProject::instance() singleton hardcoded (e.g. vector layer's feature
  // iterator uses it for virtual fields, causing minor bugs with expressions)
  // so for the time being let's just stick to using the singleton until qgis_core is completely fixed
  mProject = QgsProject::instance();
}

QgsProject *Loader::project()
{
  return mProject;
}

void Loader::setRecording( bool isRecordingOn )
{
  if ( mRecording != isRecordingOn )
  {
    mRecording = isRecordingOn;
    emit recordingChanged();
  }
}

bool Loader::load( const QString &filePath )
{
  return forceLoad( filePath, false );
}

bool Loader::forceLoad( const QString &filePath, bool force )
{
  qDebug() << "Loading " << filePath << force;
  // Just clear project if empty
  if ( filePath.isEmpty() )
  {
    emit projectWillBeReloaded( QString() );
    mProject->clear();
    whileBlocking( &mActiveLayer )->resetActiveLayer();
    emit projectReloaded( mProject );
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
  if ( mProject->fileName() != filePath || force )
  {
    emit projectWillBeReloaded( filePath );
    res = mProject->read( filePath );
    mActiveLayer.resetActiveLayer();
    mMapThemeModel.reloadMapThemes( mProject );

    QgsVectorLayer *defaultLayer = mRecordingLayerPM.layerFromLayerName( mAppSettings.defaultLayer() );
    if ( defaultLayer )
      setActiveLayer( defaultLayer );
    else
      setActiveLayer( mRecordingLayerPM.firstUsableLayer() );

    setMapSettingsLayers();

    emit projectReloaded( mProject );
  }

  bool foundInvalidLayer = false;
  QStringList invalidLayers;
  for ( QgsMapLayer *layer : mProject->mapLayers().values() )
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
      emit setProjectIssuesHeader( "The following layers failed loading:" );
      emit loadingErrorFound();
    }
  }
  return res;
}

bool Loader::reloadProject( QString projectDir )
{
  if ( mProject->homePath() == projectDir )
  {
    return forceLoad( mProject->fileName(), true );
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
  if ( !mProject || !mMapSettings ) return;

  QgsLayerTree *root = mProject->layerTreeRoot();

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
  mMapSettings->setTransformContext( mProject->transformContext() );
}

QgsQuickMapSettings *Loader::mapSettings() const
{
  return mMapSettings;
}

void Loader::zoomToProject( QgsQuickMapSettings *mapSettings )
{
  if ( !mapSettings )
  {
    qDebug() << "Cannot zoom to extent, mapSettings is not defined";
    return;
  }
  QgsRectangle extent;

  QgsProjectViewSettings *viewSettings = mProject->viewSettings();
  extent = viewSettings->presetFullExtent();
  if ( extent.isNull() )
  {
    bool hasWMS;
    QStringList WMSExtent = mProject->readListEntry( "WMSExtent", QStringLiteral( "/" ), QStringList(), &hasWMS );

    if ( hasWMS && ( WMSExtent.length() == 4 ) )
    {
      extent.set( WMSExtent[0].toDouble(), WMSExtent[1].toDouble(), WMSExtent[2].toDouble(), WMSExtent[3].toDouble() );
    }
    else // set layers extent
    {
      const QVector<QgsMapLayer *> layers = mProject->layers<QgsMapLayer *>();
      for ( const QgsMapLayer *layer : layers )
      {
        QgsRectangle layerExtent = mapSettings->mapSettings().layerExtentToOutputExtent( layer, layer->extent() );
        extent.combineExtentWith( layerExtent );
      }
    }
  }

  if ( extent.isEmpty() )
  {
    extent.grow( mProject->crs().isGeographic() ? 0.01 : 1000.0 );
  }
  extent.scale( 1.05 );
  mapSettings->setExtent( extent );
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

void Loader::appStateChanged( Qt::ApplicationState state )
{
  QString msg;

  // Instatiate QDebug with QString to redirect output to string
  // It is used to convert enum to string
  QDebug logHelper( &msg );

  logHelper << "Application changed state to: " << state;
  CoreUtils::log( "Input", msg );
}

void Loader::appAboutToQuit()
{
  CoreUtils::log( "Input", "Application has quit" );
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

QString Loader::loadIconFromLayer( QgsMapLayer *layer )
{
  if ( !layer )
    return QString();

  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );

  if ( vectorLayer )
  {
    QgsWkbTypes::GeometryType geometry = vectorLayer->geometryType();
    return iconFromGeometry( geometry );
  }
  else
    return QString( "qrc:/mIconRasterLayer.svg" );
}

QString Loader::loadIconFromFeature( QgsFeature feature )
{
  return iconFromGeometry( feature.geometry().type() );
}

QString Loader::iconFromGeometry( const QgsWkbTypes::GeometryType &geometry )
{
  switch ( geometry )
  {
    case QgsWkbTypes::GeometryType::PointGeometry: return QString( "qrc:/mIconPointLayer.svg" );
    case QgsWkbTypes::GeometryType::LineGeometry: return QString( "qrc:/mIconLineLayer.svg" );
    case QgsWkbTypes::GeometryType::PolygonGeometry: return QString( "qrc:/mIconPolygonLayer.svg" );
    default: return QString( "qrc:/mIconTableLayer.svg" );
  }
}
