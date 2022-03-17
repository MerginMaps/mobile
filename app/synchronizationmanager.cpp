/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "synchronizationmanager.h"
#include "activeprojectmanager.h"

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
    QObject::connect( mMerginApi, &MerginApi::syncProjectStatusChanged, this, &SynchronizationManager::syncProjectStatusChanged );
  }

  QObject::connect( this, &SynchronizationManager::autosyncAllowedChanged, this, [this]( bool allowed )
  {
    if ( allowed )
    {
      this->setupAutosync();
    }
    else
    {
      this->clearAutosync();
    }
  } );
}

SynchronizationManager::~SynchronizationManager() = default;

void SynchronizationManager::syncProject( const Project &project, bool withAuth )
{
  if ( !project.isMergin() || project.mergin->pending )
  {
    return;
  }

  if ( project.mergin->status == ProjectStatus::NoVersion || project.mergin->status == ProjectStatus::OutOfDate )
  {
    mMerginApi->pullProject( project.mergin->projectNamespace, project.mergin->projectName, withAuth );
  }
  else if ( project.mergin->status == ProjectStatus::Modified )
  {
    mMerginApi->pushProject( project.mergin->projectNamespace, project.mergin->projectName );
  }
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

void SynchronizationManager::setActiveProjectManager( ActiveProjectManager *activeProjectManager )
{
  mActiveProjectManager = activeProjectManager;
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

  emit autosyncAllowedChanged( allowed );
}

AutosyncController *SynchronizationManager::autosyncController() const
{
  return mAutosyncController.get();
}

void SynchronizationManager::setupAutosync()
{
  if ( mAutosyncAllowed )
  {
    if ( mActiveProjectManager )
    {
      // When project is going to be changed - destroy autosync controller
      QObject::connect(
        mActiveProjectManager,
        &ActiveProjectManager::projectWillBeReloaded,
        this,
        &SynchronizationManager::clearAutosyncController
      );

      // After new project is loaded - instantiate new autosync controller
      QObject::connect(
        mActiveProjectManager,
        &ActiveProjectManager::projectReloaded,
        this,
        &SynchronizationManager::setupAutosyncController
      );

      // Let's start the autosync right now if there is already a loaded project
      if ( !mActiveProjectManager->qgsProject()->homePath().isEmpty() )
      {
        setupAutosyncController();
      }
    }
  }
}

void SynchronizationManager::clearAutosync()
{
  if ( !mAutosyncAllowed )
  {
    if ( mActiveProjectManager )
    {
      QObject::disconnect( mActiveProjectManager );
    }

    clearAutosyncController();
  }
}

void SynchronizationManager::setupAutosyncController()
{
  if ( !mAutosyncAllowed || !mMerginApi || !mActiveProjectManager )
  {
    return;
  }

  if ( mAutosyncController )
  {
    clearAutosyncController();
  }

  if ( !mActiveProjectManager->project() )
  {
    // if there is an invalid project loaded / no project is loaded
    return clearAutosyncController();
  }

  mAutosyncController.reset( new AutosyncController(
                               mActiveProjectManager->project(),
                               mActiveProjectManager->qgsProject()
                             )
                           );

  QObject::connect( mAutosyncController.get(), &AutosyncController::foundProjectChanges, this, [this]( const QString & projectNamespace, const QString & projectName )
  {
    this->mMerginApi->pushProject( projectNamespace, projectName );
  } );

  // Let the controller listen to sync changes from backend
  QObject::connect( mMerginApi, &MerginApi::syncProjectFinished, mAutosyncController.get(), &AutosyncController::synchronizationFinished );
  QObject::connect( mMerginApi, &MerginApi::syncProjectStatusChanged, mAutosyncController.get(), &AutosyncController::synchronizationProgressed );
}

void SynchronizationManager::clearAutosyncController()
{
  if ( !mAutosyncController )
  {
    // controller is already null
    return;
  }

  mAutosyncController->disconnect();

  QObject::disconnect( mAutosyncController.get() );

  mAutosyncController.reset();
}
