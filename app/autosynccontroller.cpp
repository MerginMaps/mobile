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
  , mLocalProject( openedProject )
  , mQgsProject( openedQgsProject )
{
  if ( !mQgsProject || !mLocalProject )
  {
    CoreUtils::log( QStringLiteral( "Autosync" ), QStringLiteral( "Received an invalid active project data" ) );
    return;
  }

  // Register for data change of project's vector layers
  for ( const QgsMapLayer *layer : mQgsProject->mapLayers( true ) )
  {
    const QgsVectorLayer *vecLayer = qobject_cast<const QgsVectorLayer *>( layer );
    if ( vecLayer )
    {
      QObject::connect( vecLayer, &QgsVectorLayer::afterCommitChanges, this, &AutosyncController::handleLocalChange );
    }
  }

  if ( !mLocalProject->isMergin() )
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
  if ( !mLocalProject )
    return;

  QString activeProjectName = MerginApi::getFullProjectName( mLocalProject->projectNamespace, mLocalProject->projectName );

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
  if ( !mLocalProject )
    return;

  QString activeProjectName = MerginApi::getFullProjectName( mLocalProject->projectNamespace, mLocalProject->projectName );

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
  if ( !mLocalProject )
    return;

  if ( !mLocalProject->isMergin() )
  {
    // still not a mergin project
    setSyncStatus( SyncStatus::NotAMerginProject );
    return;
  }

  setSyncStatus( SyncStatus::PendingChanges );
  emit foundProjectChanges( mLocalProject->projectNamespace, mLocalProject->projectName );
}

void AutosyncController::setSyncStatus( SyncStatus status )
{
  if ( mSyncStatus == status )
    return;

  mSyncStatus = status;
  emit syncStatusChanged( status );
}
