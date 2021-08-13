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

ProjectStatus::Status ProjectStatus::projectStatus( const std::shared_ptr<Project> project )
{
  if ( !project || !project->isMergin() || !project->isLocal() ) // This is not a Mergin project or not downloaded project
    return ProjectStatus::NoVersion;

  // There was no sync yet
  if ( project->local->localVersion < 0 )
  {
    return ProjectStatus::NoVersion;
  }

  // Something has locally changed after last sync with server
  QString metadataFilePath = project->local->projectDir + "/" + MerginApi::sMetadataFile;
  QDateTime lastModified = CoreUtils::getLastModifiedFileDateTime( project->local->projectDir );
  QDateTime lastSync = QFileInfo( metadataFilePath ).lastModified().toUTC();
  MerginProjectMetadata meta = MerginProjectMetadata::fromCachedJson( metadataFilePath );
  int filesCount = CoreUtils::getProjectFilesCount( project->local->projectDir );
  if ( lastSync < lastModified || meta.files.count() != filesCount )
  {
    // When GPKG is opened, its header is updated and therefore lastModified timestamp is updated as well.
    // Double check if there is really something to upload
    QList<MerginFile> localFiles = MerginApi::getLocalProjectFiles( project->local->projectDir + "/" );
    MerginProjectMetadata serverProject = MerginProjectMetadata::fromCachedJson( metadataFilePath );
    ProjectDiff diff = MerginApi::compareProjectFiles( serverProject.files, serverProject.files, localFiles, project->local->projectDir );

    if ( !diff.localAdded.isEmpty() || !diff.localUpdated.isEmpty() )
      return ProjectStatus::Modified;
    else if ( !diff.localDeleted.isEmpty() )
    {
      // photos that are not downloaded due to a selective sync are marked as deleted
      // and thus would set the project state as modified. Therefore, we need to check
      // if there is any file from localDeleted that is not included in selectiveSync
      MerginConfig config = MerginConfig::fromFile( project->local->projectDir );

      for ( QString file : diff.localDeleted )
      {
        if ( !MerginApi::excludeFromSync( file, config ) )
          return ProjectStatus::Modified;
      }
    }
  }

  // Version is lower than latest one, last sync also before updated
  if ( project->local->localVersion < project->mergin->serverVersion )
  {
    return ProjectStatus::OutOfDate;
  }

  return ProjectStatus::UpToDate;
}
