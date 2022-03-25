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

SynchronizationManager::SynchronizationManager(
  MerginApi *merginApi,
  QObject *parent
)
  : QObject( parent )
  , mMerginApi( merginApi )
{
  if ( mMerginApi )
  {
    QObject::connect( mMerginApi, &MerginApi::pushCanceled, this, &SynchronizationManager::onProjectSyncCanceled );
    QObject::connect( mMerginApi, &MerginApi::syncProjectFinished, this, &SynchronizationManager::onProjectSyncFinished );
    QObject::connect( mMerginApi, &MerginApi::syncProjectStatusChanged, this, &SynchronizationManager::onProjectSyncProgressChanged );
    QObject::connect( mMerginApi, &MerginApi::networkErrorOccurred, this, &SynchronizationManager::onProjectSyncFailure );
  }
}

SynchronizationManager::~SynchronizationManager() = default;

void SynchronizationManager::syncProject( const Project &project, bool withAuth )
{
  if ( project.isLocal() )
  {
    syncProject( project.local, withAuth );
    return;
  }

  // project is not local yet -> we download it for the first time
  mMerginApi->pullProject( project.mergin.projectNamespace, project.mergin.projectName, withAuth );
}

void SynchronizationManager::syncProject( const LocalProject &project, bool withAuth )
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

  QString projectFullName = MerginApi::getFullProjectName( project.projectNamespace, project.projectName );

  if ( mSyncProcesses.contains( projectFullName ) )
  {
    SyncProcess &process = mSyncProcesses[projectFullName];
    if ( process.pending )
    {
      return; // this project is currently syncing
    }
    else if ( process.awaitsRetry )
    {
      process.awaitsRetry = false;
    }
  }

  bool syncHasStarted = false;

  if ( ProjectStatus::hasLocalChanges( project ) )
  {
    syncHasStarted = mMerginApi->pushProject( project.projectNamespace, project.projectName );
  }
  else
  {
    syncHasStarted = mMerginApi->pullProject( project.projectNamespace, project.projectName, withAuth );
  }

  if ( syncHasStarted )
  {
    SyncProcess &process = mSyncProcesses[projectFullName]; // gets or creates
    process.pending = true;

    emit syncStarted( projectFullName );
  }
}

void SynchronizationManager::stopProjectSync( const QString &projectFullname )
{
  if ( mSyncProcesses.contains( projectFullname ) )
  {
    Transactions syncTransactions = mMerginApi->transactions();

    if ( syncTransactions.contains( projectFullname ) )
    {
      TransactionStatus &transaction = syncTransactions[projectFullname];

      if ( transaction.type == TransactionStatus::Pull )
      {
        mMerginApi->cancelPull( projectFullname );
      }
      else
      {
        mMerginApi->cancelPush( projectFullname );
      }
    }
  }
}

void SynchronizationManager::migrateProjectToMergin( const QString &projectName )
{
  QString projectNamespace = mMerginApi->merginUserName();
  QString fullProjectName = MerginApi::getFullProjectName( projectNamespace, projectName );

  if ( !mSyncProcesses.contains( fullProjectName ) )
  {
    mMerginApi->migrateProjectToMergin( projectName );
    emit syncStarted( fullProjectName );
  }
}

qreal SynchronizationManager::syncProgress( const QString &projectFullName ) const
{
  if ( mSyncProcesses.contains( projectFullName ) )
  {
    return mSyncProcesses.value( projectFullName ).progress;
  }

  return -1;
}

bool SynchronizationManager::hasPendingSync( const QString &projectFullName ) const
{
  if ( mSyncProcesses.contains( projectFullName ) )
  {
    return mSyncProcesses.value( projectFullName ).pending;
  }

  return false;
}

QList<QString> SynchronizationManager::pendingProjects() const
{
  return mSyncProcesses.keys();
}

void SynchronizationManager::onProjectSyncCanceled( const QString &projectFullName, bool withError )
{
  Q_UNUSED( withError )

  if ( mSyncProcesses.contains( projectFullName ) )
  {
    mSyncProcesses.remove( projectFullName );
    emit syncCancelled( projectFullName );
  }
}

void SynchronizationManager::onProjectSyncFinished( const QString &projectFullName, bool successfully, int version )
{
  if ( mSyncProcesses.contains( projectFullName ) )
  {
    SyncProcess &process = mSyncProcesses[projectFullName];

    if ( !successfully && process.awaitsRetry )
    {
      // sync has failed, but we will try to sync again
      process.pending = false;
      process.progress = -1;
    }
    else // successfully or we won't try again
    {
      mSyncProcesses.remove( projectFullName );
    }

    emit syncFinished( projectFullName, successfully, version );
  }
}

void SynchronizationManager::onProjectSyncProgressChanged( const QString &projectFullName, qreal progress )
{
  if ( mSyncProcesses.contains( projectFullName ) )
  {
    mSyncProcesses[projectFullName].progress = progress;
    emit syncProgressChanged( projectFullName, progress );
  }
}

void SynchronizationManager::onProjectSyncFailure(
  const QString &message,
  const QString &topic,
  QNetworkReply::NetworkError networkError,
  int errorCode,
  const QString &projectFullName )
{
  if ( projectFullName.isEmpty() )
  {
    return; // network error outside of sync
  }

  if ( !mSyncProcesses.contains( projectFullName ) )
  {
    return;
  }

  SyncProcess &process = mSyncProcesses[projectFullName];

  SynchronizationError::ErrorType error = SynchronizationError::errorType( errorCode, message, topic, networkError );

  emit syncError( projectFullName, error, message );

  // We currently retry only once
  if ( process.retriesCount == 0 )
  {
    if ( SynchronizationError::isWorthOfRetry( error ) )
    {
      process.retriesCount = process.retriesCount + 1;
      process.awaitsRetry = true;

      QTimer::singleShot( mSyncRetryIntervalSeconds, this, [this, projectFullName]()
      {
        LocalProject project = mMerginApi->getLocalProject( projectFullName );
        syncProject( project );
      } );
    }
  }

  if ( error == process.lastSyncError )
  {
    mSyncProcesses.remove( projectFullName );
    emit syncFinished( projectFullName, false, -1 );

    return;
  }

  process.lastSyncError = error;
}
