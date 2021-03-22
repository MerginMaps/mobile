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
#include "inpututils.h"

#include <QDir>
#include <QDirIterator>

LocalProjectsManager::LocalProjectsManager( const QString &dataDir )
  : mDataDir( dataDir )
{
  reloadDataDir();
}

void LocalProjectsManager::reloadDataDir() // TODO: maybe add function to reload one specific project
{
  mProjects.clear();
  QStringList entryList = QDir( mDataDir ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );
  for ( const QString &folderName : entryList )
  {
    LocalProject_future info;
    info.projectDir = mDataDir + "/" + folderName;
    info.qgisProjectFilePath = findQgisProjectFile( info.projectDir, info.projectError );

    MerginProjectMetadata metadata = MerginProjectMetadata::fromCachedJson( info.projectDir + "/" + MerginApi::sMetadataFile );
    if ( metadata.isValid() )
    {
      info.projectName = metadata.name;
      info.projectNamespace = metadata.projectNamespace;
      info.localVersion = metadata.version;
    }
//    else
//    {
//      info.projectName = folderName;
//    }

    mProjects << info;
  }

  qDebug() << "LocalProjectsManager: found" << mProjects.size() << "projects";
}

LocalProject_future LocalProjectsManager::projectFromDirectory( const QString &projectDir ) const
{
  for ( const LocalProject_future &info : mProjects )
  {
    if ( info.projectDir == projectDir )
      return info;
  }
  return LocalProject_future();
}

LocalProject_future LocalProjectsManager::projectFromProjectFilePath( const QString &projectFilePath ) const
{
  for ( const LocalProject_future &info : mProjects )
  {
    if ( info.qgisProjectFilePath == projectFilePath )
      return info;
  }
  return LocalProject_future();
}

LocalProject_future LocalProjectsManager::projectFromMerginName( const QString &projectFullName ) const
{
  for ( const LocalProject_future &info : mProjects )
  {
    if ( info.id() == projectFullName )
      return info;
  }
  return LocalProject_future();
}

LocalProject_future LocalProjectsManager::projectFromMerginName( const QString &projectNamespace, const QString &projectName ) const
{
  return projectFromMerginName( MerginApi::getFullProjectName( projectNamespace, projectName ) );
}

//bool LocalProjectsManager::hasMerginProject( const QString &projectFullName ) const
//{
//  return projectFromMerginName( projectFullName ).isValid();
//}

//bool LocalProjectsManager::hasMerginProject( const QString &projectNamespace, const QString &projectName ) const
//{
//  return hasMerginProject( MerginApi::getFullProjectName( projectNamespace, projectName ) );
//}

//void LocalProjectsManager::updateProjectStatus( const QString &projectDir )
//{
//  for ( LocalProject_future &info : mProjects )
//  {
//    if ( info.projectDir == projectDir )
//    {
//      updateProjectStatus( info );
//      return;
//    }
//  }
//  Q_ASSERT( false );  // should not happen
//}

void LocalProjectsManager::addLocalProject( const QString &projectDir, const QString &projectName )
{
  addProject( projectDir, QString(), projectName );
}

void LocalProjectsManager::addMerginProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName )
{
  addProject( projectDir, projectNamespace, projectName );
}

void LocalProjectsManager::removeLocalProject( const QString &projectDir )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      InputUtils::removeDir( mProjects[i].projectDir );
      mProjects.removeAt( i );

      emit localProjectRemoved( projectDir );
      return;
    }
  }
}

//void LocalProjectsManager::removeMerginInfo( const QString &projectFullName )
//{
//  for ( int i = 0; i < mProjects.count(); ++i )
//  {
//    if ( mProjects[i].id() == projectFullName )
//    {
//      mProjects[i].localVersion = -1;
//      mProjects[i].projectNamespace.clear();
//      InputUtils::removeDir( mProjects[i].projectDir + "/.mergin" );

//      emit localProjectDataChanged( mProjects[i].projectDir );
//      return;
//    }
//  }
//}

void LocalProjectsManager::updateLocalVersion( const QString &projectDir, int version )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      mProjects[i].localVersion = version;

      emit localProjectDataChanged( mProjects[i].projectDir );
      return;
    }
  }
  Q_ASSERT( false );  // should not happen
}

//void LocalProjectsManager::updateMerginServerVersion( const QString &projectDir, int version )
//{
//  for ( int i = 0; i < mProjects.count(); ++i )
//  {
//    if ( mProjects[i].projectDir == projectDir )
//    {
//      mProjects[i].serverVersion = version;
//      updateProjectStatus( mProjects[i] );
//      return;
//    }
//  }
//  Q_ASSERT( false );  // should not happen
//}

//void LocalProjectsManager::updateProjectErrors( const QString &projectDir, const QString &errMsg )
//{
//  for ( int i = 0; i < mProjects.count(); ++i )
//  {
//    if ( mProjects[i].projectDir == projectDir )
//    {
//      // Effects only local project list, no need to send projectMetadataChanged
//      mProjects[i].qgisProjectError = errMsg;
//      return;
//    }
//  }
//}

void LocalProjectsManager::updateNamespace( const QString &projectDir, const QString &projectNamespace )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      mProjects[i].projectNamespace = projectNamespace;

      emit localProjectDataChanged( mProjects[i].projectDir );
      return;
    }
  }
}

QString LocalProjectsManager::findQgisProjectFile( const QString &projectDir, QString &err )
{
  if ( QFile::exists( InputUtils::downloadInProgressFilePath( projectDir ) ) )
  {
    // if this is a mergin project and file indicating download in progress is still there
    // download failed or copying from .temp to project dir failed (app was probably closed meanwhile)

    err = tr( "Download failed, remove and retry" );
    return QString();
  }

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
    err = tr( "Found multiple QGIS project files" );
  }
  else if ( foundProjectFiles.count() < 1 )
  {
    // no projects
    err = tr( "Failed to find a QGIS project file" );
  }

  return QString();
}

void LocalProjectsManager::addProject(const QString &projectDir, const QString &projectNamespace, const QString &projectName)
{
  LocalProject_future project;
  project.projectDir = projectDir;
  project.qgisProjectFilePath = findQgisProjectFile( projectDir, project.projectError );
  project.projectName = projectName;
  project.projectNamespace = projectNamespace;

  mProjects << project;
  emit localProjectAdded( projectDir );
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

ProjectStatus_future LocalProjectsManager::currentProjectStatus( const Project_future &project )
{
  if ( !project.isMergin() || !project.isLocal() ) // This is not a Mergin project or not downloaded project
    return ProjectStatus_future::_NoVersion;

  // There was no sync yet
  if ( project.local->localVersion < 0 )
  {
    return ProjectStatus_future::_NoVersion;
  }

  //
  // TODO: this check for local modifications should be revisited
  //

  // Something has locally changed after last sync with server
  QString metadataFilePath = project.local->projectDir + "/" + MerginApi::sMetadataFile;
  QDateTime lastModified = _getLastModifiedFileDateTime( project.local->projectDir );
  QDateTime lastSync = QFileInfo( metadataFilePath ).lastModified();
  MerginProjectMetadata meta = MerginProjectMetadata::fromCachedJson( metadataFilePath );
  int filesCount = _getProjectFilesCount( project.local->projectDir );
  if ( lastSync < lastModified || meta.files.count() != filesCount )
  {
    return ProjectStatus_future::_Modified;
  }

  // Version is lower than latest one, last sync also before updated
  if ( project.local->localVersion < project.mergin->serverVersion )
  {
    return ProjectStatus_future::_OutOfDate;
  }

  return ProjectStatus_future::_UpToDate;
}

//void LocalProjectsManager::updateProjectStatus( LocalProject_future &project )
//{
//  ProjectStatus newStatus = currentProjectStatus( project );
//  if ( newStatus != project.status )
//  {
//    project.status = newStatus;
//    emit projectMetadataChanged( project.projectDir );
//  }
//}
