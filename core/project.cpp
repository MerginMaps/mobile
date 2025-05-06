/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "project.h"
#include "merginapi.h"
#include "coreutils.h"
#include "../app/inpututils.h"

QString LocalProject::id() const
{
  // if project directory or project name doesn't exist return empty string
  if ( projectDir.isEmpty() || projectName.isEmpty() )
    return {};

  return projectId;
}

QString LocalProject::fullName() const
{
  if ( !projectName.isEmpty() && !projectNamespace.isEmpty() )
    return CoreUtils::getFullProjectName( projectNamespace, projectName );

  if ( projectDir.isEmpty() )
    return {};

  const QDir dir( projectDir );
  return dir.dirName();
}

QString LocalProject::generateProjectId()
{
  return CoreUtils::uuidWithoutBraces( QUuid::createUuid() );
}

QString MerginProject::id() const
{
  return projectId;
}

QString MerginProject::fullName() const
{
  if ( !projectName.isEmpty() && !projectNamespace.isEmpty() )
    return CoreUtils::getFullProjectName( projectNamespace, projectName );

  return {};
}

ProjectStatus::Status ProjectStatus::projectStatus( const Project &project, const bool supportsSelectiveSync )
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

  if ( ProjectStatus::hasLocalChanges( project.local, supportsSelectiveSync ) )
  {
    return ProjectStatus::NeedsSync;
  }

  return ProjectStatus::UpToDate;
}

bool ProjectStatus::hasLocalChanges( const LocalProject &project, const bool supportsSelectiveSync )
{
  const QString metadataFilePath = project.projectDir + "/" + MerginApi::sMetadataFile;

  // If the project does not have metadata file, there are local changes
  if ( !QFile::exists( metadataFilePath ) )
  {
    return true;
  }

  return MerginApi::hasLocalProjectChanges( project.projectDir, supportsSelectiveSync );
}
