/***************************************************************************
  qgsquicklayertreemodel.h
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


#ifndef PROJECTSMODEL_H
#define PROJECTSMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QModelIndex>

/*
 * Given data directory, find all QGIS projects (*.qgs) in the directory and subdirectories
 * and create list model from them. Available are full path to the file, name of the project
 * and short name of the project (clipped to N chars)
 */
class ProjectModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( QString dataDir READ dataDir) // never changes

  public:
    enum Roles
    {
      Name = Qt::UserRole + 1,
      FolderName,
      Path,
      ShortName, // name shortened to maxShortNameChars
      ProjectInfo,
      Size,
      IsValid
    };
    Q_ENUMS( Roles )

    explicit ProjectModel(const QString& dataDir, QObject* parent = nullptr);
    ~ProjectModel();

    Q_INVOKABLE QVariant data( const QModelIndex& index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row ) const;
    Q_INVOKABLE int rowAccordingPath(QString path) const;
    Q_INVOKABLE void deleteProject( int row );

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QString dataDir() const;
signals:
    void projectDeleted(QString path);

public slots:
    void addProject(QString projectFolder, QString projectName);

  private:
    void findProjectFiles();
    void addProjectFromPath(QString path);

    struct ProjectFile {
        QString name;
        QString folderName;
        QString path;
        QString info;
        bool isValid;

        bool operator < (const ProjectFile& str) const
        {
            return (folderName < str.folderName);
        }
    };
    QList<ProjectFile> mProjectFiles;
    QString mDataDir;
    const int mMaxShortNameChars = 10;

};

#endif // PROJECTSMODEL_H
