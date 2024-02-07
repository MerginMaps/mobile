/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "project.h"

QString LocalProject::id() const
{
  return fullName();
}

QString LocalProject::fullName() const
{
  if ( !projectName.isEmpty() && !projectNamespace.isEmpty() )
    return QString( "/getFullProjectName/%1/%2" ).arg( projectNamespace, projectName );

  if ( projectDir.isEmpty() )
    return QString();

  QDir dir( projectDir );
  return dir.dirName();
}

QString MerginProject::id() const
{
  return QString( "/getFullProjectName/%1/%2" ).arg( projectNamespace, projectName );
}

ProjectStatus::Status ProjectStatus::projectStatus( const Project &project )
{
  if ( !project.isMergin() || !project.isLocal() ) // This is not a Mergin project or not downloaded project
    return ProjectStatus::NoVersion;

  // There was no sync yet
  if ( !project.local.hasMerginMetadata() )
  {
    return ProjectStatus::NoVersion;
  }

  // Version is lower than latest one, last sync also before updated
  if ( project.local.localVersion < project.mergin.serverVersion )
  {
    return ProjectStatus::NeedsSync;
  }

  if ( ProjectStatus::hasLocalChanges( project.local ) )
  {
    return ProjectStatus::NeedsSync;
  }

  return ProjectStatus::UpToDate;
}

bool ProjectStatus::hasLocalChanges( const LocalProject &project )
{
  return false;
}
