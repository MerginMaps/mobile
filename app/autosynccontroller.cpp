/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "autosynccontroller.h"
#include "coreutils.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"

// 1 minute
constexpr int SYNC_INTERVAL = 60000;
// 10 seconds
constexpr int SYNC_CHECK_TIMEOUT = 10000;

AutosyncController::AutosyncController(
  QgsProject *openedQgsProject,
  QObject *parent
)
  : QObject( parent )
  , mQgsProject( openedQgsProject )
// set to current timestamp as we sync with project open
  , mLastUpdateTime( QDateTime::currentDateTime() )
{
  if ( !mQgsProject )
  {
    CoreUtils::log( QStringLiteral( "Autosync" ), QStringLiteral( "Received an invalid active project data" ) );
    return;
  }

  // Register for data change of project's vector layers
  const QMap<QString, QgsMapLayer *> layers = mQgsProject->mapLayers( true );
  for ( const QgsMapLayer *layer : layers )
  {
    const QgsVectorLayer *vecLayer = qobject_cast<const QgsVectorLayer *>( layer );
    if ( vecLayer )
    {
      if ( !vecLayer->readOnly() )
      {
        connect( vecLayer, &QgsVectorLayer::afterCommitChanges, this, [&]
        {
          mLastUpdateTime = QDateTime::currentDateTime();
          emit projectSyncRequested( SyncOptions::RequestOrigin::AutomaticRequest );
        } );
      }
    }
  }

  //every 10 seconds check if last sync was a 60 seconds or more ago and sync if it's true
  mTimer = std::make_unique<QTimer>( this );
  connect( mTimer.get(), &QTimer::timeout, this, [&]
  {
    if ( QDateTime::currentDateTime() - mLastUpdateTime >= std::chrono::milliseconds( SYNC_INTERVAL ) )
    {
      mLastUpdateTime = QDateTime::currentDateTime();
      emit projectSyncRequested( SyncOptions::RequestOrigin::AutomaticRequest );
    }
  } );
  mTimer->start( SYNC_CHECK_TIMEOUT );
}

void AutosyncController::updateLastUpdateTime()
{
  mLastUpdateTime = QDateTime::currentDateTime();
}

void AutosyncController::checkSyncRequiredAfterAppStateChange( const Qt::ApplicationState state )
{
  if ( state != Qt::ApplicationState::ApplicationActive )
  {
    mTimer->stop();
    return;
  }
  if ( !mTimer->isActive() )
  {
    mTimer->start();
  }

  const bool isLongerThanSyncInterval = QDateTime::currentDateTime() - mLastUpdateTime >= std::chrono::milliseconds( SYNC_INTERVAL );
  if ( isLongerThanSyncInterval )
  {
    mLastUpdateTime = QDateTime::currentDateTime();
    emit projectSyncRequested( SyncOptions::RequestOrigin::AutomaticRequest );
  }
}
