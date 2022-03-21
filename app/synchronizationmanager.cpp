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
    QObject::connect( mMerginApi, &MerginApi::syncProjectFinished, this, &SynchronizationManager::syncProjectFinished );
    QObject::connect( mMerginApi, &MerginApi::syncProjectStatusChanged, this, &SynchronizationManager::syncProjectProgressChanged );

    QObject::connect( mMerginApi, &MerginApi::listProjectsByNameFinished, this, &SynchronizationManager::receivedRemoteProjectInfo );
  }
}

SynchronizationManager::~SynchronizationManager() = default;

void SynchronizationManager::syncProject( const Project &project, bool withAuth )
{
  if ( project.isLocal() )
  {
    // sync it as regular local project
    syncProject( project.local, withAuth );
    return;
  }

  // project does not have local struct yet -> we download it for the first time

  mMerginApi->pullProject( project.mergin.projectNamespace, project.mergin.projectName, withAuth );
  // TODO: send getProjectInfo / listProjectsByName to find out if there actually is change on server!
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

  if ( mOngoingSyncs.contains( projectFullName ) )
  {
    return; // there is already an active sync
  }

  if ( ProjectStatus::hasLocalChanges( project ) )
  {
    mMerginApi->pushProject( project.projectNamespace, project.projectName );
  }
  else
  {
    // no local changes, let's see if there are changes on server side
    mMerginApi->pullProject( project.projectNamespace, project.projectName, withAuth );
  }

  // TODO: insert a new entry to syncs hashmap
//  SyncTransation &syncEntry = mOngoingSyncs[projectFullName];

  // TODO: start listening on project sync changes and emit them further
}

void SynchronizationManager::stopProjectSync( const QString &projectFullname )
{
  Transactions t = mMerginApi->transactions();

  if ( t.contains( projectFullname ) )
  {
    TransactionStatus &transaction = t[projectFullname];

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

void SynchronizationManager::receivedRemoteProjectInfo( const MerginProjectsList &merginProjects, Transactions, QString requestId )
{
}
