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
#include "qgslayertreemodel.h"
#include "qgslayertreelayer.h"
#include "qgslayertreegroup.h"
#include "qgsmapthemecollection.h"

#include "activeproject.h"
#include "coreutils.h"

#ifdef ANDROID
#include "position/tracking/androidtrackingbroadcast.h"
#endif

const QString ActiveProject::LOADING_FLAG_FILE_PATH = QString( "%1/.input_loading_project" ).arg( QStandardPaths::standardLocations( QStandardPaths::TempLocation ).first() );
const int ActiveProject::LOADING_FLAG_FILE_EXPIRATION_MS = 5000;

ActiveProject::ActiveProject( AppSettings &appSettings
                              , ActiveLayer &activeLayer
                              , LocalProjectsManager &localProjectsManager
                              , QObject *parent ) :

  QObject( parent )
  , mAppSettings( appSettings )
  , mActiveLayer( activeLayer )
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

QgsProject *ActiveProject::qgsProject() const
{
  return mQgsProject;
}

LocalProject ActiveProject::localProject() const
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
  CoreUtils::log( QStringLiteral( "Project loading" ), filePath + " " + ( force ? "true" : "false" ) );

  // clear autosync
  setAutosyncEnabled( false );

  // clear position tracking broadcast listeners
#ifdef ANDROID
  disconnect( &AndroidTrackingBroadcast::getInstance() );
  AndroidTrackingBroadcast::unregisterBroadcast();
#endif

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
    if ( !res )
    {
      QString error = mQgsProject->error();
      CoreUtils::log( QStringLiteral( "Project loading" ), QStringLiteral( "Could not read project file: " ) + error );

      mLocalProject = LocalProject();
      if ( mMapSettings )
      {
        QList< QgsMapLayer * > layers;
        mMapSettings->setLayers( layers );
      }
      mQgsProject->clear();

      emit localProjectChanged( mLocalProject );
      emit projectReloaded( mQgsProject );

      emit projectReadingFailed( error );
      return res;
    }

    mLocalProject = mLocalProjectsManager.projectFromProjectFilePath( filePath );

    if ( !mLocalProject.isValid() )
    {
      CoreUtils::log( QStringLiteral( "Project load" ), QStringLiteral( "Could not find project in local projects: " ) + filePath );
    }

    QString role = MerginProjectMetadata::fromCachedJson( CoreUtils::getProjectMetadataPath( mLocalProject.projectDir ) ).role;
    setProjectRole( role );

    updateMapTheme();
    updateActiveLayer();
    updateMapSettingsLayers();

    emit localProjectChanged( mLocalProject );
    emit projectReloaded( mQgsProject );
    emit positionTrackingSupportedChanged();
    emit mapSketchesEnabledChanged();
  }

  bool foundErrorsInLoadedProject = validateProject();

  // Remove the loading flag file after a while, in case the app crashes not during load, but during the first renderings
  QTimer::singleShot( LOADING_FLAG_FILE_EXPIRATION_MS, this, []()
  {
    QFile::remove( ActiveProject::LOADING_FLAG_FILE_PATH );
    CoreUtils::log( QStringLiteral( "Project loading" ), QStringLiteral( "Removed project loading flag" ) );
  } );

  if ( !force )
  {
    emit loadingFinished();

    if ( foundErrorsInLoadedProject )
    {
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
    requestSync();
  }

  // in case tracking is running, we want to show the UI
#ifdef ANDROID
  if ( positionTrackingSupported() )
  {
    connect(
      &AndroidTrackingBroadcast::getInstance(),
      &AndroidTrackingBroadcast::aliveResponse,
      this,
      [this]( bool isAlive )
    {
      if ( isAlive )
      {
        emit startPositionTracking();
      }
    },
    Qt::SingleShotConnection
    );

    // note: order matters in the following calls
    AndroidTrackingBroadcast::registerBroadcast();
    AndroidTrackingBroadcast::sendAliveRequestAsync();
  }
#endif

  return res;
}

bool ActiveProject::validateProject()
{
  Q_ASSERT( mQgsProject );

  bool errorsFound = false;

  // A. Per project validations
  // A.1. Project CRS
  if ( !mQgsProject->crs().isValid() )
  {
    errorsFound = true;
    CoreUtils::log( QStringLiteral( "Project load" ), QStringLiteral( "Invalid canvas CRS" ) );
    emit reportIssue( tr( "General" ), tr( "Project has invalid CRS assigned. Map and tools have undefined behaviour!" ) );
  }

  // B. Per-Layer validations
  QMap<QString, QgsMapLayer *> projectLayers = mQgsProject->mapLayers();
  for ( QgsMapLayer *layer : projectLayers )
  {
    // B.1. Layer Validity
    if ( !layer->isValid() )
    {
      errorsFound = true;
      CoreUtils::log( QStringLiteral( "Project load" ), QStringLiteral( "Invalid layer %1" ).arg( layer->name() ) );
      emit reportIssue( tr( "Layer" ) + ": " + layer->name(), tr( "Unable to load source " ) + ": " + layer->publicSource() );
    }
    else
    {
      // B.2. Layer CRS
      if ( layer->isSpatial() && !layer->crs().isValid() )
      {
        errorsFound = true;
        CoreUtils::log( QStringLiteral( "Project load" ), QStringLiteral( "Invalid layer CRS %1" ).arg( layer->name() ) );
        emit reportIssue( tr( "Layer" ) + ": " + layer->name(), tr( "Layer has invalid CRS assigned. Recording tools have undefined behaviour." ) );
      }
    }
  }

  return errorsFound;
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
    connect( mAutosyncController.get(), &AutosyncController::projectSyncRequired, this, &ActiveProject::requestSync );
    connect( this, &ActiveProject::appStateChanged, mAutosyncController.get(), &AutosyncController::checkSyncRequiredAfterAppStateChange );
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

void ActiveProject::setMapSettings( InputMapSettings *mapSettings )
{
  if ( mMapSettings == mapSettings )
    return;

  mMapSettings = mapSettings;
  updateMapSettingsLayers();

  emit mapSettingsChanged();
}

void ActiveProject::updateMapSettingsLayers() const
{
  if ( !mQgsProject || !mMapSettings ) return;

  QList<QgsMapLayer *> visibleLayers = getVisibleLayers();
  mMapSettings->setLayers( visibleLayers );
  mMapSettings->setTransformContext( mQgsProject->transformContext() );
}

InputMapSettings *ActiveProject::mapSettings() const
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

QString ActiveProject::projectLoadingLog() const
{
  return mProjectLoadingLog;
}

void ActiveProject::updateMapTheme()
{
  if ( !mQgsProject )
  {
    return;
  }

  QgsLayerTree *root = mQgsProject->layerTreeRoot();
  QgsMapThemeCollection *collection = mQgsProject->mapThemeCollection();

  if ( !root || !collection )
  {
    return;
  }

  QgsLayerTreeModel model( root );

  QString themeCandidateName;
  QStringList mapThemes = collection->mapThemes();

  QgsMapThemeCollection::MapThemeRecord themeCandidate = collection->createThemeFromCurrentState( root, &model );

  for ( const QString &themeName : mapThemes )
  {
    QgsMapThemeCollection::MapThemeRecord themeIterator = collection->mapThemeState( themeName );

    if ( themeCandidate == themeIterator )
    {
      themeCandidateName = themeName;
      break;
    }
  }

  setMapTheme( themeCandidateName );
}

void ActiveProject::setMapTheme( const QString &themeName )
{
  if ( mMapTheme == themeName )
  {
    return;
  }

  if ( !mQgsProject )
  {
    return;
  }

  mMapTheme = themeName;

  if ( !mMapTheme.isEmpty() )
  {
    QgsLayerTree *root = mQgsProject->layerTreeRoot();
    QgsMapThemeCollection *collection = mQgsProject->mapThemeCollection();

    if ( !root || !collection )
    {
      return;
    }

    QgsLayerTreeModel model( root );
    collection->applyTheme( mMapTheme, root, &model );
  }

  emit mapThemeChanged( mMapTheme );

  updateActiveLayer();
  updateMapSettingsLayers();
}

void ActiveProject::updateActiveLayer()
{
  QList< QgsMapLayer * > visibleLayers = getVisibleLayers();

  if ( visibleLayers.isEmpty() )
  {
    setActiveLayer( nullptr );
    return;
  }

  if ( !visibleLayers.contains( mActiveLayer.layer() ) )
  {
    QgsMapLayer *defaultLayer = InputUtils::mapLayerFromName( mAppSettings.defaultLayer(), mQgsProject );

    if ( !recordingAllowed( defaultLayer ) )
    {
      for ( QgsMapLayer *layer : visibleLayers )
      {
        if ( recordingAllowed( layer ) )
        {
          defaultLayer = layer;
          break;
        }
      }
    }

    setActiveLayer( defaultLayer );
  }
}

bool ActiveProject::isProjectLoaded() const
{
  return mQgsProject && !mQgsProject->fileName().isEmpty();
}

void ActiveProject::setActiveLayer( QgsMapLayer *layer ) const
{
  if ( !layer || !layer->isValid() )
  {
    mActiveLayer.resetActiveLayer();
  }
  else
  {
    mActiveLayer.setActiveLayer( layer );
    mAppSettings.setDefaultLayer( mActiveLayer.layerName() );
  }
}

void ActiveProject::switchLayerTreeNodeVisibility( QgsLayerTreeNode *node )
{
  if ( !node )
    return;

  node->setItemVisibilityChecked( !node->isVisible() );

  updateMapTheme();
  updateActiveLayer();
  updateMapSettingsLayers();
}

const QString &ActiveProject::mapTheme() const
{
  return mMapTheme;
}

bool ActiveProject::positionTrackingSupported() const
{
  if ( !isProjectLoaded() )
  {
    return false;
  }

  return mQgsProject->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/Enabled" ), false );
}

bool ActiveProject::projectHasRecordingLayers() const
{
  if ( !mQgsProject )
    return false;

  const QMap<QString, QgsMapLayer *> layers = mQgsProject->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    if ( recordingAllowed( it.value() ) )
      return true;
  }

  return false;
}

QString ActiveProject::projectRole() const
{
  return mProjectRole;
}

void ActiveProject::setProjectRole( const QString &role )
{
  if ( mProjectRole != role )
  {
    mProjectRole = role;

    emit projectRoleChanged();
  }
}

bool ActiveProject::recordingAllowed( QgsMapLayer *layer ) const
{
  if ( !layer )
    return false;

  //there is a bug in QgsMapLayerProxyModel::layerMatchesFilters, but having
  //just Qgis::LayerFilter::WritableLayer should be enough when fixed
  if ( layer->readOnly() )
    return false;

  return QgsMapLayerProxyModel::layerMatchesFilters( layer, Qgis::LayerFilter::HasGeometry | Qgis::LayerFilter::WritableLayer ) && layer->id() != positionTrackingLayerId() && layer->id() != mapSketchesLayerId();
}

QString ActiveProject::positionTrackingLayerId() const
{
  if ( !mQgsProject )
    return QString();

  return mQgsProject->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), QString() );
}

QList<QgsMapLayer *> ActiveProject::getVisibleLayers() const
{
  if ( !mQgsProject )
    return QList<QgsMapLayer *>();

  QgsLayerTree *root = mQgsProject->layerTreeRoot();

  if ( !root )
    return QList<QgsMapLayer *>();

  // Get list of all visible valid layers in the project
  QList<QgsMapLayer *> visibleLayers;
  const QList<QgsMapLayer *> layers = root->layerOrder();

  for ( QgsMapLayer *layer : layers )
  {
    if ( InputUtils::isLayerVisible( layer, mQgsProject ) )
    {
      visibleLayers << layer;
    }
  }

  return visibleLayers;
}

bool ActiveProject::mapSketchesEnabled() const
{
  if ( !isProjectLoaded() )
  {
    return false;
  }

  return mQgsProject->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "MapSketching/Enabled" ), false );
}

QString ActiveProject::mapSketchesLayerId() const
{
  if ( !mQgsProject )
    return {};

  return mQgsProject->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "MapSketching/Layer" ), QString() );
}

bool ActiveProject::photoSketchingEnabled() const
{
  if ( !isProjectLoaded() )
  {
    return false;
  }

  return mQgsProject->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PhotoSketching/Enabled" ), false );
}
