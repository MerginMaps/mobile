/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "autosynccontroller.h"

AutosyncController::AutosyncController( QObject *parent ) : QObject( parent )
{
  // raise NotImplemented :)
}

AutosyncController::~AutosyncController()
{
  // raise NotImplemented :)
}

AutosyncController::SyncStatus AutosyncController::syncStatus()
{
  return mSyncStatus;
}

void AutosyncController::setActiveProject( Project )
{
  // raise NotImplemented :)
}

void AutosyncController::setActiveQgsProject( QgsProject * )
{
  // raise NotImplemented :)
}

void AutosyncController::handleSyncFinished()
{
  // raise NotImplemented :)
}
