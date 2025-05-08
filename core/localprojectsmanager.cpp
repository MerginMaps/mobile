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
#include "coreutils.h"

#include <QDir>
#include <QDirIterator>

LocalProjectsManager::LocalProjectsManager( const QString &dataDir )
  : mDataDir( dataDir )
{
  reloadDataDir();
}

void LocalProjectsManager::reloadDataDir()
{
  mProjects.clear();
  QStringList entryList = QDir( mDataDir ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );
  for ( const QString &folderName : entryList )
  {
    LocalProject info;
    info.projectDir = mDataDir + "/" + folderName;
    info.qgisProjectFilePath = findQgisProjectFile( info.projectDir, info.projectError );

    MerginProjectMetadata metadata = MerginProjectMetadata::fromCachedJson( info.projectDir + "/" + MerginApi::sMetadataFile );
    if ( metadata.isValid() )
    {
      info.projectName = metadata.name;
      info.projectNamespace = metadata.projectNamespace;
      info.localVersion = metadata.version;
      info.projectId = metadata.projectId;
    }
    else
    {
      info.projectName = folderName;
      info.projectId = LocalProject::generateProjectId();
    }

    mProjects.insert( info.projectId, info );
  }

  const QString msg = QString( "Found %1 local projects in %2" ).arg( mProjects.size() ).arg( mDataDir );
  CoreUtils::log( "Local projects", msg );
  emit dataDirReloaded();
}

LocalProject LocalProjectsManager::projectFromDirectory( const QString &projectDir ) const
{
  for ( const LocalProject &info : mProjects.values() )
  {
    if ( info.projectDir == projectDir )
      return info;
  }
  return {};
}

LocalProject LocalProjectsManager::projectFromProjectFilePath( const QString &projectFilePath ) const
{
  for ( const LocalProject &info : mProjects.values() )
  {
    if ( info.qgisProjectFilePath == projectFilePath )
      return info;
  }
  return {};
}

LocalProject LocalProjectsManager::projectFromProjectId( const QString &projectId ) const
{
  if ( mProjects.contains( projectId ) )
  {
    return mProjects.value( projectId );
  }
  return {};
}

void LocalProjectsManager::addLocalProject( const QString &projectDir, const QString &projectName )
{
  addProject( projectDir, QString(), projectName, LocalProject::generateProjectId() );
}

void LocalProjectsManager::addMerginProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName, const QString &projectId )
{
  addProject( projectDir, projectNamespace, projectName, projectId );
}

void LocalProjectsManager::removeLocalProject( const QString &projectId )
{
  if ( mProjects.contains( projectId ) )
  {
    const LocalProject project = mProjects.value( projectId );
    emit aboutToRemoveLocalProject( project );

    CoreUtils::removeDir( project.projectDir );
    mProjects.remove( projectId );
  }
}

bool LocalProjectsManager::projectIsValid( const QString &path ) const
{
  for ( LocalProject &project : mProjects.values() )
  {
    if ( project.qgisProjectFilePath == path )
    {
      return project.projectError.isEmpty();
    }
  }
  return false;
}

QString LocalProjectsManager::projectId( const QString &path ) const
{
  for ( LocalProject &project : mProjects.values() )
  {
    if ( project.qgisProjectFilePath == path )
    {
      return project.id();
    }
  }
  return {};
}

QString LocalProjectsManager::projectName( const QString &projectId ) const
{
  const LocalProject project = projectFromProjectId( projectId );

  if ( project.isValid() )
  {
    return CoreUtils::getFullProjectName( project.projectNamespace, project.projectName );
  }

  return {};
}

QString LocalProjectsManager::projectChanges( const QString &projectId ) const
{
  const LocalProject project = projectFromProjectId( projectId );

  if ( project.isValid() )
  {
    return MerginApi::localProjectChanges( project.projectDir ).dump();
  }

  return {};
}

void LocalProjectsManager::updateLocalVersion( const QString &projectId, const int version )
{
  if ( mProjects.contains( projectId ) )
  {
    LocalProject project = mProjects.value( projectId );
    project.localVersion = version;
    emit localProjectDataChanged( project );
  }
}

void LocalProjectsManager::updateNamespace( const QString &projectId, const QString &projectNamespace )
{
  if ( mProjects.contains( projectId ) )
  {
    LocalProject project = mProjects.value( projectId );
    project.projectNamespace = projectNamespace;
    emit localProjectDataChanged( project );
  }
}

QString LocalProjectsManager::findQgisProjectFile( const QString &projectDir, QString &err )
{
  if ( QFile::exists( CoreUtils::downloadInProgressFilePath( projectDir ) ) )
  {
    // if this is a mergin project and file indicating download in progress is still there
    // download failed or copying from .temp to project dir failed (app was probably closed meanwhile)

    err = tr( "Download failed, remove and retry" );
    return {};
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

  return {};
}

void LocalProjectsManager::updateProjectId( const QString &oldProjectId, const QString &newProjectId )
{
  if ( mProjects.contains( oldProjectId ) )
  {
    // updating values just in LocalProjectsManager is not enough we also update ProjectsModel and ActiveProject
    LocalProject project = mProjects.value( oldProjectId );
    emit aboutToRemoveLocalProject( project );
    mProjects.remove( oldProjectId );
    project.projectId = newProjectId;
    mProjects.insert( newProjectId, project );
    emit localProjectAdded( project );
    emit localProjectDataChanged( project );
  }
}

void LocalProjectsManager::addProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName, const QString &projectId )
{
  LocalProject project;
  project.projectDir = projectDir;
  project.qgisProjectFilePath = findQgisProjectFile( projectDir, project.projectError );
  project.projectName = projectName;
  project.projectNamespace = projectNamespace;
  project.projectId = projectId;

  mProjects.insert( projectId, project );
  emit localProjectAdded( project );
}
