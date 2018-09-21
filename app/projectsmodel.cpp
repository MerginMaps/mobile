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

ProjectModel::ProjectModel(const QString &dataDir, QObject* parent)
  : QAbstractListModel( parent )
  , mDataDir(dataDir)
{
    findProjectFiles();
}

ProjectModel::~ProjectModel() {}

void ProjectModel::findProjectFiles() {
    QDirIterator it(mDataDir, QStringList() << "*.qgs", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        it.next();
        ProjectFile projectFile;
        projectFile.name = it.fileName().remove(".qgs");
        projectFile.path = it.filePath();
        mProjectFiles.append(projectFile);

        qDebug() << "Found QGIS project: " << it.filePath();
    }
    std::sort(mProjectFiles.begin(), mProjectFiles.end());
}


QVariant ProjectModel::data( const QModelIndex& index, int role ) const
{
  int row = index.row();
  if (row < 0 || row > mProjectFiles.count())
      return QVariant("");

  // TODO should be moved to index() ?
  const ProjectFile& projectFile = mProjectFiles.at(row);

  switch ( role )
  {
    case Name: return QVariant(projectFile.name);
    case ShortName: return QVariant(projectFile.name.left(mMaxShortNameChars - 3) + "...");
    case Path: return QVariant(projectFile.path);
  }

  return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[Name] = "name";
  roleNames[ShortName] = "short_name";
  roleNames[Path] = "path";
  return roleNames;
}

QModelIndex ProjectModel::index( int row ) const {
    return createIndex(row, 0, nullptr);
}

int ProjectModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return mProjectFiles.count();
}

QString ProjectModel::dataDir() const {
    return mDataDir;
}
