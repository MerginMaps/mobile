/***************************************************************************
  qgsquicklayertreemodel.cpp
  --------------------------------------
  Date                 : Nov 2017
  Copyright            : (C) 2017 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "projectsmodel.h"

#include <QString>
#include <QDirIterator>
#include <QDebug>
#include <QDateTime>

ProjectModel::ProjectModel( const QString &dataDir, QObject *parent )
  : QAbstractListModel( parent )
  , mDataDir( dataDir )
{
  findProjectFiles();
}

ProjectModel::~ProjectModel() {}

void ProjectModel::findProjectFiles()
{
  QStringList entryList = QDir( mDataDir ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );
  for ( QString namespaceFolder : entryList )
  {
    QStringList entryList2 = QDir( mDataDir + "/" + namespaceFolder ).entryList( QDir::NoDotAndDotDot | QDir::Dirs );
    for ( QString folderName : entryList2 )
    {
      addProjectFromPath( mDataDir + "/" + namespaceFolder + "/" + folderName );
    }
  }
  std::sort( mProjectFiles.begin(), mProjectFiles.end() );
}

void ProjectModel::addProjectFromPath( QString path )
{
  if ( path.isEmpty() ) return;

  QDirIterator it( path, QStringList() << QStringLiteral( "*.qgs" ), QDir::Files, QDirIterator::Subdirectories );

  int i = 0;
  int projectExistsAt = -1;
  for ( ProjectFile projectFile : mProjectFiles )
  {
    if ( mDataDir + "/" + projectFile.projectNamespace + "/" + projectFile.folderName == path )
    {
      projectExistsAt = i;
    }
    i++;
  }

  QList<ProjectFile> foundProjects;
  while ( it.hasNext() )
  {
    it.next();
    ProjectFile projectFile;
    projectFile.name = it.fileName().remove( ".qgs" );
    QStringList res = path.split( "/" );
    projectFile.projectNamespace = res.takeAt( res.length() - 2 );
    projectFile.path = it.filePath();
    QDir projectDir( path );
    projectFile.folderName = projectDir.dirName();
    QFileInfo fileInfo( it.filePath() );
    QDateTime created = fileInfo.created().toUTC();
    projectFile.info = QString( created.toString() );
    projectFile.isValid = true;

    foundProjects.append( projectFile );
    qDebug() << "Found QGIS project: " << it.filePath();
  }

  ProjectFile project;
  if ( !foundProjects.isEmpty() )
  {
    project = foundProjects.at( 0 );
    if ( foundProjects.length() > 1 )
    {
      project.info = "invalid project";
      project.isValid = false;
    }
  }
  else
  {
    project.name = "";
    QDir projectDir( path );
    QStringList res = path.split( "/" );
    project.projectNamespace = res.takeAt( res.length() - 2 );
    project.folderName = projectDir.dirName();
    project.path = path;
    project.info = "invalid project";
    project.isValid = false;
  }

  if ( projectExistsAt >= 0 )
    mProjectFiles.removeAt( projectExistsAt );

  mProjectFiles.append( project );
}

QVariant ProjectModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mProjectFiles.count() )
    return QVariant( "" );

  const ProjectFile &projectFile = mProjectFiles.at( row );

  switch ( role )
  {
    case Name: return QVariant( projectFile.name );
    case ProjectNamespace: return QVariant( projectFile.projectNamespace );
    case FolderName: return QVariant( projectFile.folderName );
    case ShortName: return QVariant( projectFile.name.left( mMaxShortNameChars - 3 ) + "..." );
    case Path: return QVariant( projectFile.path );
    case ProjectInfo: return QVariant( projectFile.info );
    case IsValid: return QVariant( projectFile.isValid );
    case PassesFilter: return mSearchExpression.isEmpty() || projectFile.folderName.contains( mSearchExpression, Qt::CaseInsensitive );
  }

  return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  roleNames[ProjectNamespace] = "projectNamespace";
  roleNames[FolderName] = "folderName";
  roleNames[ShortName] = "shortName";
  roleNames[Path] = "path";
  roleNames[ProjectInfo] = "projectInfo";
  roleNames[IsValid] = "isValid";
  roleNames[PassesFilter] = "passesFilter";
  return roleNames;
}

QModelIndex ProjectModel::index( int row ) const
{
  return createIndex( row, 0, nullptr );
}

int ProjectModel::rowAccordingPath( QString path ) const
{
  int i = 0;
  for ( ProjectFile prj : mProjectFiles )
  {
    if ( prj.path == path )
    {
      return i;
    }
    i++;
  }
  return -1;
}

void ProjectModel::deleteProject( int row )
{
  ProjectFile project = mProjectFiles.at( row );
  QDir dir( mDataDir + "/" + project.projectNamespace + "/" + project.folderName );
  dir.removeRecursively();
  beginResetModel();
  mProjectFiles.removeAt( row );
  endResetModel();
  emit projectDeleted( project.projectNamespace + "/" + project.folderName );
}

int ProjectModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent );
  return mProjectFiles.count();
}

QString ProjectModel::dataDir() const
{
  return mDataDir;
}

QString ProjectModel::searchExpression() const
{
  return mSearchExpression;
}

void ProjectModel::setSearchExpression( const QString &searchExpression )
{
  if ( searchExpression != mSearchExpression )
  {
    mSearchExpression = searchExpression;
    // Hack to model changed signal
    endResetModel();
  }
}

void ProjectModel::addProject( QString projectFolder, QString projectName, bool successful )
{
  if ( !successful ) return;

  Q_UNUSED( projectName );
  beginResetModel();
  addProjectFromPath( projectFolder );
  endResetModel();
}
