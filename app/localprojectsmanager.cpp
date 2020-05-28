/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "localprojectsmanager.h"

#include "merginapi.h"
#include "merginprojectmetadata.h"

#include <QDir>
#include <QDirIterator>

static QString findQgisProjectFile( const QString &projectDir, QString &err )
{
  QList<QString> foundProjectFiles;
  QDirIterator it( projectDir, QStringList() << QStringLiteral( "*.qgs" ) << QStringLiteral( "*.qgz" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    foundProjectFiles << it.filePath();
  }
  if ( foundProjectFiles.count() == 1 )
  {
    return foundProjectFiles.first();
  }
  else if ( foundProjectFiles.count() > 1 )
  {
    // error: multiple project files found
    err = QString( "ERR_MULTIPLE_PROJECTS" );
  }
  else
  {
    // no projects
    err = QString( "ERR_NO_PROJECTS" );
  }
  return QString();
}


LocalProjectsManager::LocalProjectsManager( const QString &dataDir )
  : mDataDir( dataDir )
{
  QStringList entryList = QDir( mDataDir ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );
  for ( QString folderName : entryList )
  {
    LocalProjectInfo info;
    QString err;
    info.projectDir = mDataDir + "/" + folderName;
    info.qgisProjectFilePath = findQgisProjectFile( info.projectDir, err );
    if ( !err.isEmpty() )
      info.setprojectFileError( err );

    MerginProjectMetadata metadata = MerginProjectMetadata::fromCachedJson( info.projectDir + "/" + MerginApi::sMetadataFile );
    if ( metadata.isValid() )
    {
      info.projectName = metadata.name;
      info.projectNamespace = metadata.projectNamespace;
      info.localVersion = metadata.version;
    }
    else
    {
      info.projectName = folderName;
    }

    mProjects << info;
  }
}

LocalProjectInfo LocalProjectsManager::projectFromDirectory( const QString &projectDir ) const
{
  for ( const LocalProjectInfo &info : mProjects )
  {
    if ( info.projectDir == projectDir )
      return info;
  }
  return LocalProjectInfo();
}

LocalProjectInfo LocalProjectsManager::projectFromMerginName( const QString &projectFullName ) const
{
  for ( const LocalProjectInfo &info : mProjects )
  {
    if ( MerginApi::getFullProjectName( info.projectNamespace, info.projectName ) == projectFullName )
      return info;
  }
  return LocalProjectInfo();
}

LocalProjectInfo LocalProjectsManager::projectFromMerginName( const QString &projectNamespace, const QString &projectName ) const
{
  return projectFromMerginName( MerginApi::getFullProjectName( projectNamespace, projectName ) );
}

bool LocalProjectsManager::hasMerginProject( const QString &projectFullName ) const
{
  return projectFromMerginName( projectFullName ).isValid();
}

bool LocalProjectsManager::hasMerginProject( const QString &projectNamespace, const QString &projectName ) const
{
  return hasMerginProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
}

void LocalProjectsManager::updateProjectStatus( const QString &projectDir )
{
  for ( LocalProjectInfo &info : mProjects )
  {
    if ( info.projectDir == projectDir )
    {
      updateProjectStatus( info );
      return;
    }
  }
  Q_ASSERT( false );  // should not happen
}

void LocalProjectsManager::addMerginProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName )
{
  LocalProjectInfo project;
  QString err;
  project.projectDir = projectDir;
  project.qgisProjectFilePath = findQgisProjectFile( projectDir, err );
  if ( !err.isEmpty() )
    project.setprojectFileError( err );
  project.projectNamespace = projectNamespace;
  project.projectName = projectName;
  // version info and status should be updated afterwards

  mProjects << project;
  emit localProjectAdded( projectDir );
}

void LocalProjectsManager::removeProject( const QString &projectDir )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      mProjects.removeAt( i );
      emit localProjectRemoved( projectDir );
      return;
    }
  }
}

void LocalProjectsManager::deleteProjectDirectory( const QString &projectDir )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      Q_ASSERT( !projectDir.isEmpty() && projectDir != "/" );
      QDir( projectDir ).removeRecursively();
      mProjects.removeAt( i );
      return;
    }
  }
}

void LocalProjectsManager::updateMerginLocalVersion( const QString &projectDir, int version )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      mProjects[i].localVersion = version;
      updateProjectStatus( mProjects[i] );
      return;
    }
  }
  Q_ASSERT( false );  // should not happen
}

void LocalProjectsManager::updateMerginServerVersion( const QString &projectDir, int version )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      mProjects[i].serverVersion = version;
      updateProjectStatus( mProjects[i] );
      return;
    }
  }
  Q_ASSERT( false );  // should not happen
}

static QDateTime _getLastModifiedFileDateTime( const QString &path )
{
  QDateTime lastModified;
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !MerginApi::isInIgnore( it.fileInfo() ) )
    {
      if ( it.fileInfo().lastModified() > lastModified )
      {
        lastModified = it.fileInfo().lastModified();
      }
    }
  }
  return lastModified.toUTC();
}

static int _getProjectFilesCount( const QString &path )
{
  int count = 0;
  QDirIterator it( path, QStringList() << QStringLiteral( "*" ), QDir::Files, QDirIterator::Subdirectories );
  while ( it.hasNext() )
  {
    it.next();
    if ( !MerginApi::isInIgnore( it.fileInfo() ) )
    {
      count++;
    }
  }
  return count;
}

ProjectStatus LocalProjectsManager::currentProjectStatus( const LocalProjectInfo &project )
{
  // There was no sync yet
  if ( project.localVersion < 0 )
  {
    return ProjectStatus::NoVersion;
  }

  //
  // TODO: this check for local modifications should be revisited
  //

  // Something has locally changed after last sync with server
  QString metadataFilePath = project.projectDir + "/" + MerginApi::sMetadataFile;
  QDateTime lastModified = _getLastModifiedFileDateTime( project.projectDir );
  QDateTime lastSync = QFileInfo( metadataFilePath ).lastModified();
  MerginProjectMetadata meta = MerginProjectMetadata::fromCachedJson( metadataFilePath );
  int filesCount = _getProjectFilesCount( project.projectDir );
  if ( lastSync < lastModified || meta.files.count() != filesCount )
  {
    return ProjectStatus::Modified;
  }

  // Version is lower than latest one, last sync also before updated
  if ( project.localVersion < project.serverVersion )
  {
    return ProjectStatus::OutOfDate;
  }

  return ProjectStatus::UpToDate;
}

void LocalProjectsManager::updateProjectStatus( LocalProjectInfo &project )
{
  ProjectStatus newStatus = currentProjectStatus( project );
  if ( newStatus != project.status )
  {
    project.status = newStatus;
    emit projectMetadataChanged( project.projectDir );
  }
}
