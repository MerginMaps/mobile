/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QTimer>

#include "synchronizationmanager.h"

#include "synchronizationerror.h"

SynchronizationManager::SynchronizationManager(
  MerginApi *merginApi,
  QObject *parent
)
  : QObject( parent )
  , mMerginApi( merginApi )
{
  if ( mMerginApi )
  {
    connect( mMerginApi, &MerginApi::pushCanceled, this, &SynchronizationManager::onProjectSyncCanceled );
    connect( mMerginApi, &MerginApi::syncProjectFinished, this, &SynchronizationManager::onProjectSyncFinished );
    connect( mMerginApi, &MerginApi::networkErrorOccurred, this, &SynchronizationManager::onProjectSyncFailure );
    connect( mMerginApi, &MerginApi::projectCreated, this, &SynchronizationManager::onProjectCreated );
    connect( mMerginApi, &MerginApi::syncProjectStatusChanged, this, &SynchronizationManager::onProjectSyncProgressChanged );
    connect( mMerginApi, &MerginApi::projectReloadNeededAfterSync, this, &SynchronizationManager::onProjectReloadNeededAfterSync );
  }
}

SynchronizationManager::~SynchronizationManager() = default;

void SynchronizationManager::syncProject( const Project &project, const SyncOptions::Authorization auth, const SyncOptions::Strategy strategy )
{
  if ( project.isLocal() )
  {
    syncProject( project.local, auth, strategy );
    return;
  }

  // project is not local yet -> we download it for the first time
  const bool syncHasStarted = mMerginApi->pullProject( project.fullName(), project.id(), auth == SyncOptions::Authorized );

  if ( syncHasStarted )
  {
    SyncProcess &process = mSyncProcesses[project.id()]; // gets or creates
    process.pending = true;
    process.strategy = strategy;

    emit syncStarted( project.id() );
  }
}

void SynchronizationManager::syncProject( const LocalProject &project, const SyncOptions::Authorization auth, const SyncOptions::Strategy strategy )
{
  if ( !project.isValid() )
  {
    return;
  }

  if ( !project.hasMerginMetadata() )
  {
    emit syncError( project.id(), SynchronizationError::NotAMerginProject );
    return;
  }

  if ( mSyncProcesses.contains( project.id() ) )
  {
    SyncProcess &process = mSyncProcesses[project.id()];
    if ( process.pending )
    {
      return; // this project is currently syncing
    }
    if ( process.awaitsRetry )
    {
      process.awaitsRetry = false;
    }
  }

  bool syncHasStarted = false;

  if ( ProjectStatus::hasLocalChanges( project, mMerginApi->supportsSelectiveSync() ) )
  {
    syncHasStarted = mMerginApi->pushProject( project.fullName(), project.id() );
  }
  else
  {
    syncHasStarted = mMerginApi->pullProject( project.fullName(), project.id(), auth == SyncOptions::Authorized );
  }

  if ( syncHasStarted )
  {
    SyncProcess &process = mSyncProcesses[project.id()]; // gets or creates
    process.pending = true;
    process.strategy = strategy;

    emit syncStarted( project.id() );
  }
}

void SynchronizationManager::stopProjectSync( const QString &projectId ) const
{
  if ( mSyncProcesses.contains( projectId ) )
  {
    Transactions syncTransactions = mMerginApi->transactions();

    if ( syncTransactions.contains( projectId ) )
    {
      const TransactionStatus &transaction = syncTransactions[projectId];

      if ( transaction.type == TransactionStatus::Pull )
      {
        mMerginApi->cancelPull( projectId );
      }
      else
      {
        mMerginApi->cancelPush( projectId );
      }
    }
  }
}

void SynchronizationManager::migrateProjectToMergin( const QString &projectName, const QString &projectId )
{
  if ( !mSyncProcesses.contains( projectId ) )
  {
    bool hasStarted = false;

    if ( mMerginApi->serverType() == MerginServerType::OLD )
    {
      hasStarted = mMerginApi->createProject( mMerginApi->userInfo()->username(), projectName, projectId );
    }
    else
    {
      hasStarted = mMerginApi->createProject( mMerginApi->userInfo()->activeWorkspaceName(), projectName, projectId );
    }

    if ( hasStarted )
    {
      SyncProcess &process = mSyncProcesses[projectId]; // creates new entry
      process.pending = true;

      emit syncStarted( projectId );
    }
  }
}

qreal SynchronizationManager::syncProgress( const QString &projectId ) const
{
  if ( mSyncProcesses.contains( projectId ) )
  {
    return mSyncProcesses.value( projectId ).progress;
  }

  return -1;
}

bool SynchronizationManager::hasPendingSync( const QString &projectId ) const
{
  if ( mSyncProcesses.contains( projectId ) )
  {
    return mSyncProcesses.value( projectId ).pending;
  }

  return false;
}

QList<QString> SynchronizationManager::pendingProjects() const
{
  return mSyncProcesses.keys();
}

void SynchronizationManager::onProjectSyncCanceled( const QString &projectId )
{
  if ( mSyncProcesses.contains( projectId ) )
  {
    mSyncProcesses.remove( projectId );
    emit syncCancelled( projectId );
  }
}

void SynchronizationManager::onProjectSyncFinished( const QString &projectId, const bool successfully, const int version )
{
  if ( mSyncProcesses.contains( projectId ) )
  {
    SyncProcess &process = mSyncProcesses[projectId];
    const bool reloadNeeded = process.reloadProject;

    if ( !successfully && process.awaitsRetry )
    {
      // sync has failed, but we will try to sync again
      process.pending = false;
      process.progress = -1;
    }
    else // successfully or we won't try again
    {
      mSyncProcesses.remove( projectId );
    }

    emit syncFinished( projectId, successfully, version, reloadNeeded );
  }
}

void SynchronizationManager::onProjectSyncProgressChanged( const QString &projectId, const qreal progress )
{
  if ( mSyncProcesses.contains( projectId ) )
  {
    mSyncProcesses[projectId].progress = progress;
    emit syncProgressChanged( projectId, progress );
  }
  else if ( progress >= 0 )
  {
    //
    // Synchronization was not started via sync manager,
    // let's add it to the manager here.
    // This is most probably useful only for tests, where we
    // normally run sync from MerginApi directly
    //
    SyncProcess &process = mSyncProcesses[projectId];
    process.pending = true;
    process.progress = progress;
    emit syncStarted( projectId );
    emit syncProgressChanged( projectId, progress );
  }

}

void SynchronizationManager::onProjectCreated( const QString &projectId, const bool result )
{
  if ( !result && mSyncProcesses.contains( projectId ) )
  {
    mSyncProcesses.remove( projectId );
  }
}

void SynchronizationManager::onProjectSyncFailure(
  const QString &message,
  const int httpCode,
  const QString &projectId )
{
  if ( projectId.isEmpty() )
  {
    return; // network error outside of sync
  }

  if ( !mSyncProcesses.contains( projectId ) )
  {
    return;
  }

  SyncProcess &process = mSyncProcesses[projectId];

  const SynchronizationError::ErrorType error = SynchronizationError::errorType( httpCode, message );

  // We only retry twice
  const bool eligibleForRetry = process.strategy == SyncOptions::Retry &&
                                process.retriesCount < 2 &&
                                !SynchronizationError::isPermanent( error );

  emit syncError( projectId, error, eligibleForRetry, message );

  if ( eligibleForRetry )
  {
    process.retriesCount = process.retriesCount + 1;
    process.awaitsRetry = true;

    QTimer::singleShot( mSyncRetryIntervalSeconds, this, [this, projectId]
    {
      const LocalProject project = mMerginApi->getLocalProject( projectId );
      syncProject( project );
    } );
  }
  else
  {
    mSyncProcesses.remove( projectId );
    emit syncFinished( projectId, false, -1, false );
  }
}

void SynchronizationManager::onProjectReloadNeededAfterSync( const QString &projectId )
{
  if ( mSyncProcesses.contains( projectId ) )
  {
    mSyncProcesses[projectId].reloadProject = true;
  }

}
