/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINPROJECTSTATUSMODEL_H
#define MERGINPROJECTSTATUSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "merginapi.h"

class MerginProjectStatusModel : public QAbstractListModel
{
    Q_OBJECT

  public:

    enum ProjectChangelogStatus
    {
      Added = Qt::UserRole + 1,
      Deleted,
      Updated,
      Changelog, // Diffable file with number of changes per each category (above)
      Message // Error message
    };
    Q_ENUMS( ProjectChangelogStatus )

    struct ProjectStatusItem
    {
      ProjectChangelogStatus status;
      QString text;
      QString filename;
      int inserts = 0;
      int updates = 0;
      int deletes = 0;
      QString section;
    };

    enum Roles
    {
      Status = Qt::UserRole + 1,
      Text,
      Filename,
      Inserts,
      Deletes,
      Updates,
      Section
    };
    Q_ENUMS( Roles )

    explicit MerginProjectStatusModel( LocalProjectsManager &localProjects, QObject *parent = nullptr );

    int rowCount( const QModelIndex &parent ) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;

    Q_INVOKABLE bool loadProjectInfo( const QString &projectFullName );

  private:
    void insertIntoItems( const QSet<QString> &files, const ProjectChangelogStatus &status, const QString &projectDir );
    void infoProjectUpdated( const ProjectDiff &projectDiff, const QString &projectDir );

    ProjectDiff mProjectDiff;
    QList<ProjectStatusItem> mItems;

    LocalProjectsManager &mLocalProjects;

};

#endif // MERGINPROJECTSTATUSMODEL_H
