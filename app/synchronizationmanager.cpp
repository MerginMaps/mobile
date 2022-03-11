/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "synchronizationmanager.h"
#include "merginuserauth.h"
#include "coreutils.h"

#include "qdebug.h"

SynchronizationManager::SynchronizationManager( MerginApi *backend, QObject *parent )
  : QObject( parent )
  , mBackend( backend )
{

}

void SynchronizationManager::syncProject( const Project &project, bool isAuthOptional )
{
  if ( !project.isMergin() || project.mergin->pending )
  {
    return;
  }

  if ( project.mergin->status == ProjectStatus::NoVersion || project.mergin->status == ProjectStatus::OutOfDate )
  {
    bool useAuth = false;

    // TODO: this entire auth condition tree should go to MerginApi - new method that is going to be called also for "listProjectsByName"
    if ( !isAuthOptional )
    {
      // auth is mandatory
      useAuth = true;
    }
    else
    {
      // we only want to include auth token when user is logged in.
      if ( mBackend->userAuth()->hasAuthData() )
      {
        // his token, however, might have already expired, so let's just refresh it
        if ( mBackend->userAuth()->tokenExpiration() < QDateTime::currentDateTimeUtc() )
        {
          // TODO: this needs to be a blocking call to "refresh token", not simple authorize
          mBackend->authorize( mBackend->userAuth()->username(), mBackend->userAuth()->password() );
        }
        useAuth = true;
      }
    }

    mBackend->updateProject( project.mergin->projectNamespace, project.mergin->projectName, useAuth );
  }
  else if ( project.mergin->status == ProjectStatus::Modified )
  {
    mBackend->uploadProject( project.mergin->projectNamespace, project.mergin->projectName );
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
      mBackend->updateCancel( projectFullname );
    }
    else
    {
      mBackend->uploadCancel( projectFullname );
    }
  }
}

bool SynchronizationManager::autosyncAllowed()
{
  return mAutosyncAllowed;
}

void SynchronizationManager::setAutosyncAllowed( bool allowed )
{
  if ( mAutosyncAllowed != allowed )
  {
    mAutosyncAllowed = allowed;
    emit autosyncAllowedChanged( allowed );
  }
}

void SynchronizationManager::activeProjectChanged( LocalProject activeProject )
{
  mActiveProject.local.reset( activeProject.clone() );
  mActiveProject.mergin.reset();

  if ( mAutosyncAllowed )
  {
    if ( activeProject.localVersion < 0 )
    {
      // not a mergin project
      return;
    }

    if ( !mBackend )
    {
      CoreUtils::log( QStringLiteral( "Synchronization Manager" ), QStringLiteral( "Manager does not have a valid MerginAPI reference" ) );
      return;
    }

    // acquire server information about an active project
    QString projectname = MerginApi::getFullProjectName( activeProject.projectNamespace, activeProject.projectName );

    QObject::connect( mBackend, &MerginApi::listProjectsByNameFinished, this, &SynchronizationManager::receivedServerInfo, Qt::UniqueConnection );

    mBackend->listProjectsByName( QStringList() << projectname );
  }
}

void SynchronizationManager::receivedServerInfo( const MerginProjectsList &merginProjects, Transactions, QString requestId )
{
  // find active project in merginProjects
  MerginProject finder;
  finder.projectName = mActiveProject.projectName();
  finder.projectNamespace = mActiveProject.projectNamespace();

  qDebug() << "Got an answer" << requestId;
  if ( merginProjects.contains( finder ) )
  {
    MerginProject activeProjectData = merginProjects.at( merginProjects.indexOf( finder ) );
    mActiveProject.mergin.reset( activeProjectData.clone() );

    qDebug() << QStringLiteral( "Acquired data for project" ) << mActiveProject.mergin->projectName;

    syncProject( mActiveProject );
  }
}
