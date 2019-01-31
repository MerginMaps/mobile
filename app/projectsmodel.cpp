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

ProjectModel::ProjectModel(const QString &dataDir, QObject* parent)
  : QAbstractListModel( parent )
  , mDataDir(dataDir + "/")
{
    findProjectFiles();
}

ProjectModel::~ProjectModel() {}

void ProjectModel::findProjectFiles() {
    QDirIterator it(mDataDir, QDir::Dirs);
    while (it.hasNext()) {
       it.next();
       addProjectFromPath(it.fileInfo().absolutePath());
    }
    std::sort(mProjectFiles.begin(), mProjectFiles.end());
}

void ProjectModel::addProjectFromPath(QString path)
{
    if (path.isEmpty()) return;

    QDirIterator it(path, QStringList() << QStringLiteral("*.qgs"), QDir::Files, QDirIterator::Subdirectories);
    QSet<QString> projectFilePaths;

    for (ProjectFile projectFile: mProjectFiles) {
        projectFilePaths << projectFile.path;
    }


    QList<ProjectFile> foundProjects;
    while (it.hasNext())
    {
        it.next();
        ProjectFile projectFile;
        projectFile.name = it.fileName().remove(".qgs");
        QDir projectDir(it.fileInfo().absoluteDir());
        projectFile.folderName = projectDir.dirName();
        projectFile.path = it.filePath();
        QFileInfo fileInfo(it.filePath());
        QDateTime created = fileInfo.created();
        projectFile.info = QString(created.toString());

        if (!projectFilePaths.contains(projectFile.path))
            foundProjects.append(projectFile);

        qDebug() << "Found QGIS project: " << it.filePath();
    }

    if(!foundProjects.isEmpty()) {
        ProjectFile project = foundProjects.at(0);
        project.name = ""; // invalid project
        project.path = "";
        project.info = "invalid project";
        mProjectFiles.append(project);
    } else {
        ProjectFile projectFile;
        projectFile.name = "";
        QDir projectDir(it.fileInfo().absoluteDir());
        projectFile.folderName = path;
        projectFile.path = "";
        projectFile.info = "invalid project";
    }
    // std::sort(mProjectFiles.begin(), mProjectFiles.end());
}


QVariant ProjectModel::data( const QModelIndex& index, int role ) const
{
  int row = index.row();
  if (row < 0 || row >= mProjectFiles.count())
      return QVariant("");

  const ProjectFile& projectFile = mProjectFiles.at(row);

  switch ( role )
  {
    case Name: return QVariant(projectFile.folderName);
    case FolderName: return QVariant(projectFile.folderName);
    case ShortName: return QVariant(projectFile.name.left(mMaxShortNameChars - 3) + "...");
    case Path: return QVariant(projectFile.path);
    case ProjectInfo: return QVariant(projectFile.info);
  }

  return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  roleNames[FolderName] = "folderName";
  roleNames[ShortName] = "shortName";
  roleNames[Path] = "path";
  roleNames[ProjectInfo] = "projectInfo";
  return roleNames;
}

QModelIndex ProjectModel::index( int row ) const {
    return createIndex(row, 0, nullptr);
}

int ProjectModel::rowAccordingPath(QString path) const{
    int i = 0;
    for (ProjectFile prj: mProjectFiles) {
        if (prj.path == path) {
             return i;
        }
        i++;
    }
    return -1;
}

int ProjectModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return mProjectFiles.count();
}

QString ProjectModel::dataDir() const {
    return mDataDir;
}

void ProjectModel::addProject(QString projectFolder, QString projectName)
{
    Q_UNUSED(projectName);
    beginResetModel();
    addProjectFromPath(projectFolder);
    endResetModel();
    emit projectsChanged();
}
