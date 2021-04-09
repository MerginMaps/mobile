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

QString MerginProject::id() const
{
  return MerginApi::getFullProjectName( projectNamespace, projectName );
}

ProjectStatus::Status ProjectStatus::projectStatus( const std::shared_ptr<Project> project )
{
  if ( !project || !project->isMergin() || !project->isLocal() ) // This is not a Mergin project or not downloaded project
    return ProjectStatus::NoVersion;

  // There was no sync yet
  if ( project->local->localVersion < 0 )
  {
    return ProjectStatus::NoVersion;
  }

  //
  // TODO: this check for local modifications should be revisited
  //

  // Something has locally changed after last sync with server
  QString metadataFilePath = project->local->projectDir + "/" + MerginApi::sMetadataFile;
  QDateTime lastModified = CoreUtils::getLastModifiedFileDateTime( project->local->projectDir );
  QDateTime lastSync = QFileInfo( metadataFilePath ).lastModified();
  MerginProjectMetadata meta = MerginProjectMetadata::fromCachedJson( metadataFilePath );
  int filesCount = CoreUtils::getProjectFilesCount( project->local->projectDir );
  if ( lastSync < lastModified || meta.files.count() != filesCount )
  {
    return ProjectStatus::Modified;
  }

  // Version is lower than latest one, last sync also before updated
  if ( project->local->localVersion < project->mergin->serverVersion )
  {
    return ProjectStatus::OutOfDate;
  }

  return ProjectStatus::UpToDate;
}
