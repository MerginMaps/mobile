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

QString LocalProject::id() const
{
  if ( !projectName.isEmpty() && !projectNamespace.isEmpty() )
    return MerginApi::getFullProjectName( projectNamespace, projectName );

  QDir dir( projectDir );
  return dir.dirName();
}

LocalProject *LocalProject::clone() const
{
  LocalProject *me = new LocalProject();
  me->projectName = projectName;
  me->projectNamespace = projectNamespace;
  me->projectDir = projectDir;
  me->projectError = projectError;
  me->qgisProjectFilePath = qgisProjectFilePath;
  me->localVersion = localVersion;
  return me;
}

QString MerginProject::id() const
{
  return MerginApi::getFullProjectName( projectNamespace, projectName );
}

MerginProject *MerginProject::clone() const
{
  MerginProject *me = new MerginProject();
  me->projectName = projectName;
  me->projectNamespace = projectNamespace;
  me->serverUpdated = serverUpdated;
  me->serverVersion = serverVersion;
  me->pending = pending;
  me->progress = progress;
  me->status = status;
  me->remoteError = remoteError;
  return me;
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

  if ( ProjectStatus::hasLocalChanges( project.local ) )
  {
    return ProjectStatus::Modified;
  }

  // Version is lower than latest one, last sync also before updated
  if ( project.local.localVersion < project.mergin.serverVersion )
  {
    return ProjectStatus::OutOfDate;
  }

  return ProjectStatus::UpToDate;
}

bool ProjectStatus::hasLocalChanges( const LocalProject &project )
{
  QString metadataFilePath = project.projectDir + "/" + MerginApi::sMetadataFile;

  if ( !QFile::exists( metadataFilePath ) )
  {
    // If the project does not have metadata file, there are local changes
    return true;
  }

  // Check if something has locally changed after last sync with server
  QDateTime lastModified = CoreUtils::getLastModifiedFileDateTime( project.projectDir );

  QDateTime lastSync = QFileInfo( metadataFilePath ).lastModified().toUTC();
  MerginProjectMetadata metadata = MerginProjectMetadata::fromCachedJson( metadataFilePath );

  int filesCount = CoreUtils::getProjectFilesCount( project.projectDir );

  if ( lastSync < lastModified || metadata.files.count() != filesCount )
  {
    // When GPKG is opened, its header is updated and therefore lastModified timestamp is updated as well.
    // Double check if something has really changed
    ProjectDiff diff = MerginApi::localProjectChanges( project.projectDir );

    if ( !diff.localAdded.isEmpty() || !diff.localUpdated.isEmpty() || !diff.localDeleted.isEmpty() )
      return true;
  }

  return false;
}
