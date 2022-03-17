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

AutosyncController::AutosyncController(
  LocalProject *openedProject,
  QgsProject *openedQgsProject,
  QObject *parent
)
  : QObject( parent )
  , mActiveProject( openedProject )
  , mActiveQgsProject( openedQgsProject )
{
  if ( !mActiveQgsProject || !mActiveProject )
  {
    CoreUtils::log( QStringLiteral( "Autosync" ), QStringLiteral( "Received an invalid active project data" ) );
    return;
  }

  // Register for data change of project's vector layers
  for ( const QgsMapLayer *layer : mActiveQgsProject->mapLayers( true ) )
  {
    const QgsVectorLayer *vecLayer = qobject_cast<const QgsVectorLayer *>( layer );
    if ( vecLayer )
    {
      QObject::connect( vecLayer, &QgsVectorLayer::afterCommitChanges, this, &AutosyncController::handleLocalChange );
    }
  }

  if ( mActiveProject->localVersion < 0 )
  {
    // this is not a mergin project
    setSyncStatus( SyncStatus::NotAMerginProject );
  }
}

AutosyncController::~AutosyncController() = default;

AutosyncController::SyncStatus AutosyncController::syncStatus()
{
  return mSyncStatus;
}

void AutosyncController::synchronizationProgressed( const QString &projectFullName, qreal progress )
{
  if ( !mActiveProject )
    return;

  QString activeProjectName = MerginApi::getFullProjectName( mActiveProject->projectNamespace, mActiveProject->projectName );

  if ( projectFullName == activeProjectName )
  {
    if ( progress >= 0 && progress < 1 )
    {
      setSyncStatus( SyncStatus::SyncInProgress );
    }
  }
}

void AutosyncController::synchronizationFinished( const QString &, const QString &projectFullName, bool successfully, int )
{
  if ( !mActiveProject )
    return;

  QString activeProjectName = MerginApi::getFullProjectName( mActiveProject->projectNamespace, mActiveProject->projectName );

  if ( projectFullName != activeProjectName )
    return;

  if ( successfully )
  {
    setSyncStatus( SyncStatus::Synced );
  }
  else
  {
    setSyncStatus( SyncStatus::SyncKeepsFailing );
  }
}

void AutosyncController::handleLocalChange()
{
  if ( !mActiveProject )
    return;

  if ( mActiveProject->localVersion < 0 )
  {
    // still not a mergin project
    setSyncStatus( SyncStatus::NotAMerginProject );
    return;
  }

  setSyncStatus( SyncStatus::PendingChanges );
  emit foundProjectChanges( mActiveProject->projectNamespace, mActiveProject->projectName );
}

void AutosyncController::setSyncStatus( SyncStatus status )
{
  if ( mSyncStatus == status )
    return;

  mSyncStatus = status;
  emit syncStatusChanged( status );
}
