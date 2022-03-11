/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "synchronizationmanager.h"

#include "qdebug.h"

SynchronizationManager::SynchronizationManager( QObject *parent ) : QObject( parent )
{

}

bool SynchronizationManager::autosyncAllowed()
{
  return mAutosyncAllowed;
}

void SynchronizationManager::setAutosyncAllowed( bool )
{

}

void SynchronizationManager::activeProjectChanged( LocalProject activeProject )
{
  qDebug() << "Active project changed to:" << activeProject.projectName;
}
