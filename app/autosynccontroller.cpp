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
  LocalProject openedProject,
  QgsProject *openedQgsProject,
  MerginApi *backend,
  QObject *parent
)
  : QObject( parent )
  , mActiveQgsProject( openedQgsProject )
  , mBackend( backend )
{
  mActiveProject.reset( new Project() );
  mActiveProject->local.reset( openedProject.clone() );

  // Register for data change of project's vector layers
  for ( const QgsMapLayer *layer : mActiveQgsProject->mapLayers( true ) )
  {
    const QgsVectorLayer *vecLayer = qobject_cast<const QgsVectorLayer *>( layer );
    if ( vecLayer )
    {
      QObject::connect( vecLayer, &QgsVectorLayer::afterCommitChanges, this, &AutosyncController::handleLocalChange );
    }
  }

  // We need to send ListProjectsByNameAPI to find out the state of
  // the active project on server and if we have rights to access it
  QObject::connect( mBackend, &MerginApi::listProjectsByNameFinished, this, &AutosyncController::receivedServerInfo, Qt::UniqueConnection );
  if ( mActiveProject->local->localVersion >= 0 )
  {
    mLastRequestId = mBackend->listProjectsByName( QStringList() << mActiveProject->projectFullName() );
  }
  else
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
  if ( projectFullName == mActiveProject->projectFullName() )
  {
    if ( progress >= 0 && progress < 1 )
    {
      setSyncStatus( SyncStatus::SyncInProgress );
    }
  }
}

void AutosyncController::synchronizationFinished( const QString &, const QString &projectFullName, bool successfully, int )
{
  if ( projectFullName != mActiveProject->projectFullName() )
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

void AutosyncController::receivedServerInfo( const MerginProjectsList &merginProjects, Transactions, QString requestId )
{
  if ( mLastRequestId != requestId )
  {
    return;
  }

  // find active project in merginProjects
  MerginProject finder;
  finder.projectName = mActiveProject->projectName();
  finder.projectNamespace = mActiveProject->projectNamespace();

  if ( merginProjects.contains( finder ) )
  {
    MerginProject activeProjectData = merginProjects.at( merginProjects.indexOf( finder ) );
    mActiveProject->mergin.reset( activeProjectData.clone() );

    mActiveProject->mergin->status = ProjectStatus::projectStatus( mActiveProject.get() );

    QString projectError = mActiveProject->mergin->remoteError;
    if ( !projectError.isEmpty() )
    {
      CoreUtils::log( QStringLiteral( "Autosync" ), QStringLiteral( "Can not start syncing current project, error:" ) + projectError );
      return;
    }

    emit syncProject( mActiveProject.get() );
  }
}

void AutosyncController::handleLocalChange()
{
  if ( mActiveProject->local->localVersion < 0 )
  {
    // still not a mergin project
    setSyncStatus( SyncStatus::NotAMerginProject );
    return;
  }

  setSyncStatus( SyncStatus::PendingChanges );
  emit syncProject( mActiveProject.get() );
}

void AutosyncController::setSyncStatus( SyncStatus status )
{
  if ( mSyncStatus == status )
    return;

  mSyncStatus = status;
  emit syncStatusChanged( status );
}
