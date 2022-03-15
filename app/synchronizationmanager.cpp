/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "synchronizationmanager.h"

SynchronizationManager::SynchronizationManager( MerginApi *backend, QObject *parent )
  : QObject( parent )
  , mAutosyncController( nullptr )
  , mBackend( backend )
{
  QObject::connect( mBackend, &MerginApi::syncProjectFinished, this, &SynchronizationManager::syncProjectFinished );
  QObject::connect( mBackend, &MerginApi::syncProjectStatusChanged, this, &SynchronizationManager::syncProjectStatusChanged );
}

SynchronizationManager::~SynchronizationManager()
{

}

void SynchronizationManager::syncProject( const Project &project, bool withAuth )
{
  if ( !project.isMergin() || project.mergin->pending )
  {
    return;
  }

  if ( project.mergin->status == ProjectStatus::NoVersion || project.mergin->status == ProjectStatus::OutOfDate )
  {
    mBackend->pullProject( project.mergin->projectNamespace, project.mergin->projectName, withAuth );
  }
  else if ( project.mergin->status == ProjectStatus::Modified )
  {
    mBackend->pushProject( project.mergin->projectNamespace, project.mergin->projectName );
  }
}

void SynchronizationManager::stopProjectSync( const QString &projectFullname )
{
  Transactions t = mBackend->transactions();

  if ( t.contains( projectFullname ) )
  {
    TransactionStatus transaction = t.value( projectFullname );

    if ( transaction.type == TransactionStatus::Pull )
    {
      mBackend->cancelPull( projectFullname );
    }
    else
    {
      mBackend->cancelPush( projectFullname );
    }
  }
}

bool SynchronizationManager::autosyncAllowed() const
{
  return mAutosyncAllowed;
}

void SynchronizationManager::setAutosyncAllowed( bool allowed )
{
  if ( mAutosyncAllowed == allowed )
    return;

  mAutosyncAllowed = allowed;

  if ( mAutosyncAllowed )
  {
    // In future, instantiate AutosyncController
  }
  else
  {
    // In future, delete AutosyncController
  }

  emit autosyncAllowedChanged( allowed );
}

AutosyncController *SynchronizationManager::autosyncController() const
{
  return mAutosyncController.get();
}
