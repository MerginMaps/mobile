/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
    return;
  }

  QString projectFullName = MerginApi::getFullProjectName( project.projectNamespace, project.projectName );

  if ( mSyncProcesses.contains( projectFullName ) )
  {
    return; // there is already an active sync
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
    SyncProcess sync;
    sync.pending = true;

    mSyncProcesses.insert( projectFullName, sync );

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
  }

  emit syncCancelled( projectFullName );
}

void SynchronizationManager::onProjectSyncFinished( const QString &projectDir, const QString &projectFullName, bool successfully, int version )
{
  Q_UNUSED( projectDir )

  if ( mSyncProcesses.contains( projectFullName ) )
  {
    mSyncProcesses.remove( projectFullName );
  }

  emit syncFinished( projectFullName, successfully, version );
}

void SynchronizationManager::onProjectSyncProgressChanged( const QString &projectFullName, qreal progress )
{
  if ( mSyncProcesses.contains( projectFullName ) )
  {
    mSyncProcesses[projectFullName].progress = progress;
  }

  emit syncProgressChanged( projectFullName, progress );
}
