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
#include <QSettings>

// Local (never synced to server) override for a project's display name, keyed by its
// stable projectDir. Stored outside of the Mergin metadata cache since that file is
// wholesale overwritten with the server's response on every sync (see MerginApi::writeData).
static const QString sLocalProjectNameSettingsGroup = QStringLiteral( "localProjectNames" );

LocalProjectsManager::LocalProjectsManager( const QString &dataDir )
  : mDataDir( dataDir )
{
  reloadDataDir();
}

void LocalProjectsManager::reloadDataDir()
{
  mProjects.clear();

  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );

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
    }
    else
    {
      info.projectName = folderName;
    }

    // A local rename overrides whatever name we just resolved above.
    const QString customName = settings.value( sLocalProjectNameSettingsGroup + "/" + info.projectDir ).toString();
    if ( !customName.isEmpty() )
    {
      info.projectName = customName;
    }

    mProjects << info;
  }

  settings.endGroup();

  QString msg = QString( "Found %1 local projects in %2" ).arg( mProjects.size() ).arg( mDataDir );
  CoreUtils::log( "Local projects", msg );
  emit dataDirReloaded();
}

LocalProject LocalProjectsManager::projectFromDirectory( const QString &projectDir ) const
{
  for ( const LocalProject &info : mProjects )
  {
    if ( info.projectDir == projectDir )
      return info;
  }
  return LocalProject();
}

LocalProject LocalProjectsManager::projectFromProjectFilePath( const QString &projectFilePath ) const
{
  for ( const LocalProject &info : mProjects )
  {
    if ( info.qgisProjectFilePath == projectFilePath )
      return info;
  }
  return LocalProject();
}

LocalProject LocalProjectsManager::projectFromProjectId( const QString &projectId ) const
{
  for ( const LocalProject &info : mProjects )
  {
    if ( info.id() == projectId )
      return info;
  }
  return LocalProject();
}

LocalProject LocalProjectsManager::projectFromMerginName( const QString &projectFullName ) const
{
  for ( const LocalProject &info : mProjects )
  {
    if ( info.id() == projectFullName )
      return info;
  }
  return LocalProject();
}

LocalProject LocalProjectsManager::projectFromMerginName( const QString &projectNamespace, const QString &projectName ) const
{
  return projectFromMerginName( MerginApi::getFullProjectName( projectNamespace, projectName ) );
}

void LocalProjectsManager::addLocalProject( const QString &projectDir, const QString &projectName )
{
  addProject( projectDir, QString(), projectName );
}

void LocalProjectsManager::addMerginProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName )
{
  addProject( projectDir, projectNamespace, projectName );
}

void LocalProjectsManager::removeLocalProject( const QString &projectId )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].id() == projectId )
    {
      emit aboutToRemoveLocalProject( mProjects[i] );

      QSettings settings;
      settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
      settings.remove( sLocalProjectNameSettingsGroup + "/" + mProjects[i].projectDir );
      settings.endGroup();

      CoreUtils::removeDir( mProjects[i].projectDir );
      mProjects.removeAt( i );

      return;
    }
  }
}

bool LocalProjectsManager::projectIsValid( const QString &path ) const
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].qgisProjectFilePath == path )
    {
      return mProjects[i].projectError.isEmpty();
    }
  }
  return false;
}

QString LocalProjectsManager::projectId( const QString &path ) const
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].qgisProjectFilePath == path )
    {
      return mProjects[i].id();
    }
  }
  return QString();
}

QString LocalProjectsManager::projectName( const QString &projectId ) const
{
  LocalProject project = projectFromProjectId( projectId );

  if ( project.isValid() )
  {
    return MerginApi::getFullProjectName( project.projectNamespace, project.projectName );
  }

  return QString();
}

QString LocalProjectsManager::projectChanges( const QString &projectId )
{
  LocalProject project = projectFromProjectId( projectId );

  if ( project.isValid() )
  {
    return MerginApi::localProjectChanges( project.projectDir ).dump();
  }

  return QString();
}

void LocalProjectsManager::updateLocalVersion( const QString &projectDir, int version )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      mProjects[i].localVersion = version;

      emit localProjectDataChanged( mProjects[i] );
      return;
    }
  }
  Q_ASSERT( false );  // should not happen
}

void LocalProjectsManager::updateNamespace( const QString &projectDir, const QString &projectNamespace )
{
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].projectDir == projectDir )
    {
      mProjects[i].projectNamespace = projectNamespace;

      emit localProjectDataChanged( mProjects[i] );
      return;
    }
  }
}

QString LocalProjectsManager::findQgisProjectFile( const QString &projectDir, QString &err )
{
  if ( QFile::exists( CoreUtils::downloadInProgressFilePath( projectDir ) ) )
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

void LocalProjectsManager::addProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName )
{
  LocalProject project;
  project.projectDir = projectDir;
  project.qgisProjectFilePath = findQgisProjectFile( projectDir, project.projectError );
  project.projectName = projectName;
  project.projectNamespace = projectNamespace;

  mProjects << project;
  emit localProjectAdded( project );
}

QString LocalProjectsManager::renameLocalProject( const QString &projectId, const QString &newName )
{
  if ( newName.trimmed().isEmpty() )
  {
    return tr( "The project name cannot be empty" );
  }

  if ( !CoreUtils::isValidName( newName ) )
  {
    return tr( "The project name contains invalid characters" );
  }

  int projectIndex = -1;
  for ( int i = 0; i < mProjects.count(); ++i )
  {
    if ( mProjects[i].id() == projectId )
    {
      projectIndex = i;
    }

    if ( i != projectIndex && mProjects[i].projectName == newName )
    {
      return tr( "A project name is already taken" );
    }
  }

  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  settings.setValue( sLocalProjectNameSettingsGroup + "/" + mProjects[projectIndex].projectDir, newName );
  settings.endGroup();

  mProjects[projectIndex].projectName = newName;
  emit localProjectDataChanged( mProjects[projectIndex] );
  return QString();
}
