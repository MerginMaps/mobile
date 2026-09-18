/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QTimer>

#include "coreutils.h"
#include "synchronizationmanager.h"

using namespace Qt::Literals;

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
    connect( mMerginApi, &MerginApi::syncTransactionFinished, this, &SynchronizationManager::onTransactionFinished );
    connect( mMerginApi, &MerginApi::networkErrorOccurred, this, &SynchronizationManager::onTransactionFailure );
    connect( mMerginApi, &MerginApi::projectCreated, this, &SynchronizationManager::onProjectCreated );
    connect( mMerginApi, &MerginApi::projectAttachedToMergin, this, &SynchronizationManager::onProjectAttachedToMergin );
    connect( mMerginApi, &MerginApi::syncProjectStatusChanged, this, &SynchronizationManager::onProjectSyncProgressChanged );
    connect( mMerginApi, &MerginApi::projectReloadNeededAfterSync, this, &SynchronizationManager::onProjectReloadNeededAfterSync );
    connect( mMerginApi, &MerginApi::projectAlreadyOnLatestVersion, this, [&]( const QString & projectFullName )
    {
      const SyncProcess &process = mSyncProcesses[projectFullName];
      if ( process.requestOrigin == SyncOptions::ManualRequest )
      {
        emit projectAlreadyOnLatestVersion( projectFullName );
      }
    } );
  }
}

void SynchronizationManager::syncProject( const Project &project, SyncOptions::Authorization auth, SyncOptions::Strategy strategy, const SyncOptions::RequestOrigin
    requestOrigin )
{
  if ( project.isLocal() )
  {
    syncProject( project.local, auth, strategy, requestOrigin );
    return;
  }

  CoreUtils::log( QStringLiteral( "Sync Manager" ), QStringLiteral( "Requested download of project %2" ).arg( project.mergin.projectName ) );

  // project is not local yet -> we download it for the first time
  mMerginApi->pullProject( project.mergin.projectNamespace, project.mergin.projectName );

  SyncProcess &process = mSyncProcesses[project.fullName()]; // gets or creates
  process.pending = true;
  process.strategy = strategy;
  process.requestOrigin = requestOrigin;

  emit syncStarted( project.fullName() );
}

void SynchronizationManager::syncProject( const LocalProject &project, SyncOptions::Authorization auth, SyncOptions::Strategy strategy, const SyncOptions::
    RequestOrigin requestOrigin )
{
  if ( !project.isValid() )
  {
    return;
  }

  CoreUtils::log( QStringLiteral( "Sync Manager" ), QStringLiteral( "Requested %1 sync of project %2" ).arg( requestOrigin == SyncOptions::ManualRequest ? "manual" : "automatic" ).arg( project.projectName ) );

  if ( !project.hasMerginMetadata() )
  {
    if ( requestOrigin == SyncOptions::ManualRequest )
    {
      emit syncError( project.id(), SynchronizationError::NotAMerginProject );
    }
    return;
  }

  const QString projectFullName = MerginApi::getFullProjectName( project.projectNamespace, project.projectName );

  if ( mSyncProcesses.contains( projectFullName ) )
  {
    SyncProcess &process = mSyncProcesses[projectFullName];

    if ( process.pending )
    {
      return; // this project is currently syncing
    }

    if ( process.awaitsRetry )
    {
      process.awaitsRetry = false;
    }
  }

  mMerginApi->pullProject( project.projectNamespace, project.projectName );

  SyncProcess &process = mSyncProcesses[projectFullName]; // gets or creates
  process.pending = true;
  process.strategy = strategy;
  process.requestOrigin = requestOrigin;

  emit syncStarted( projectFullName );
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
  if ( !mSyncProcesses.contains( projectName ) )
  {
    bool hasStarted = false;

    if ( mMerginApi->serverType() == MerginServerType::OLD )
    {
      hasStarted = mMerginApi->createProject( mMerginApi->userInfo()->username(), projectName );
    }
    else
    {
      hasStarted = mMerginApi->createProject( mMerginApi->userInfo()->activeWorkspaceName(), projectName );
    }

    if ( hasStarted )
    {
      SyncProcess &process = mSyncProcesses[projectName]; // creates new entry
      process.pending = true;

      emit syncStarted( projectName );
    }
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

void SynchronizationManager::onTransactionFinished( const QString &finishedProjectFullName, const bool successful, const int version,
    const TransactionStatus::TransactionType finishedTransactionType )
{
  // dangling transaction - ignore
  if ( !mSyncProcesses.contains( finishedProjectFullName ) )
  {
    return;
  }

  if ( !successful )
  {
    onProjectSyncFinished( finishedProjectFullName, false, version );
    return;
  }

  if ( finishedTransactionType == TransactionStatus::Pull )
  {
    QString projectNamespace, projectName;
    MerginApi::extractProjectName( finishedProjectFullName, projectNamespace, projectName );
    mMerginApi->pushProject( projectNamespace, projectName, false );
    return;
  }

  // a push just finished - go for another pull if there are still local changes to sync
  if ( mMerginApi->hasLocalProjectChanges( finishedProjectFullName ) )
  {
    QString projectNamespace, projectName;
    MerginApi::extractProjectName( finishedProjectFullName, projectNamespace, projectName );
    mMerginApi->pullProject( projectNamespace, projectName );
    return;
  }

  onProjectSyncFinished( finishedProjectFullName, true, version );
  emit projectDataChanged( finishedProjectFullName );
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
    bool reloadNeeded = process.reloadProject;

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

    emit syncFinished( projectFullName, successfully, version, reloadNeeded );
  }
}

void SynchronizationManager::onProjectSyncProgressChanged( const QString &projectFullName, const qreal progress )
{
  if ( mSyncProcesses.contains( projectFullName ) )
  {
    mSyncProcesses[projectFullName].progress = progress;
    emit syncProgressChanged( projectFullName, progress );
  }
  else if ( progress >= 0 )
  {
    //
    // Synchronization was not started via sync manager,
    // let's add it to the manager here.
    // This is most probably useful only for tests, where we
    // normally run sync from MerginApi directly
    //
    SyncProcess &process = mSyncProcesses[projectFullName];
    process.pending = true;
    process.progress = progress;
    emit syncStarted( projectFullName );
    emit syncProgressChanged( projectFullName, progress );
  }

}

void SynchronizationManager::onProjectCreated( const QString &projectFullName, bool result )
{
  // 'projectFullName' is in the format "namespace/projectName" and 'mSyncProcess' stores
  // projects that were not previously uploaded to the server in the format "projectName".
  QString projectNamespace, projectName;
  MerginApi::extractProjectName( projectFullName, projectNamespace, projectName );

  if ( !result && mSyncProcesses.contains( projectName ) )
  {
    mSyncProcesses.remove( projectName );
  }
}

void SynchronizationManager::onTransactionFailure(
  const QString &message,
  const QString &topic,
  const int errorCode,
  const QString &projectFullName,
  const QString &serverErrorCode )
{
  if ( projectFullName.isEmpty() )
  {
    return; // network error outside of sync
  }

  if ( !mSyncProcesses.contains( projectFullName ) )
  {
    return;
  }

  Q_UNUSED( topic );

  SyncProcess &process = mSyncProcesses[projectFullName];

  const SynchronizationError::ErrorType error = SynchronizationError::errorType( errorCode, message, serverErrorCode );

  const bool eligibleForRetry = process.retriesCount < MAXIMUM_RETRY_COUNT && !SynchronizationError::isPermanent( error );

  if ( process.requestOrigin == SyncOptions::ManualRequest )
  {
    emit syncError( projectFullName, error, eligibleForRetry, message );
  }

  if ( eligibleForRetry )
  {
    process.retriesCount = process.retriesCount++;
    process.awaitsRetry = true;

    QTimer::singleShot( mSyncRetryIntervalSeconds, this, [this, projectFullName]
    {
      const LocalProject project = mMerginApi->getLocalProject( projectFullName );
      syncProject( project );
    } );
  }
  else
  {
    mSyncProcesses.remove( projectFullName );
    emit syncFinished( projectFullName, false, -1, false );
  }
}

void SynchronizationManager::onProjectAttachedToMergin( const QString &projectFullName, const QString &previousName )
{
  if ( mSyncProcesses.contains( previousName ) )
  {
    SyncProcess process = mSyncProcesses.value( previousName );
    mSyncProcesses.remove( previousName );
    mSyncProcesses.insert( projectFullName, process );
  }
}

void SynchronizationManager::onProjectReloadNeededAfterSync( const QString &projectFullName )
{
  if ( mSyncProcesses.contains( projectFullName ) )
  {
    mSyncProcesses[projectFullName].reloadProject = true;
  }

}
