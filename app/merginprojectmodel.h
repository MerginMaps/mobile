/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MERGINPROJECTMODEL_H
#define MERGINPROJECTMODEL_H

#include <QObject>
#include <QString>
#include <QAbstractListModel>
#include <memory>
#include "merginapi.h"


/**
 * Basic information about a remote mergin project from the received list of projects.
 *
 * We add some local information for project is also available locally:
 * - general local info: project dir, downloaded version number, project status
 * - sync status: whether sync is active, progress indicator
 */
struct MerginProject
{
  QString projectName;
  QString projectNamespace;
  QString projectDir;  // full path to the project directory
  QDateTime clientUpdated; // client's version of project files
  QDateTime serverUpdated; // available latest version of project files on server
  bool pending = false; // if there is a pending request for downlaod/update a project
  ProjectStatus status = NoVersion;
  qreal progress = 0;  // progress in case of pending download/upload (values [0..1])
};

typedef QList<std::shared_ptr<MerginProject>> ProjectList;


class MerginProjectModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression )
    Q_PROPERTY( int lastPage READ lastPage NOTIFY lastPageChanged )

  public:
    enum Roles
    {
      ProjectName = Qt::UserRole + 1,
      ProjectNamespace,
      Size,
      ProjectInfo,
      Status,
      Pending,
      PassesFilter,
      SyncProgress
    };
    Q_ENUMS( Roles )

    explicit MerginProjectModel( LocalProjectsManager &localProjects, QObject *parent = nullptr );

    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;

    ProjectList projects();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    /**
    * Updates list of projects with synchronization progress if a project is pending.
    * \param merginProjects List of mergin projects
    * \param pendingProjects Projects in pending state
    * \param expectedProjectCount Total number of projects
    * \param page Int representing page.
    */
    void updateModel( const MerginProjectList &merginProjects, QHash<QString, TransactionStatus> pendingProjects, int expectedProjectCount, int page );

    int filterCreator() const; // TODO: remove, no use
    void setFilterCreator( int filterCreator );

    int filterWriter() const; // TODO: remove, no use
    void setFilterWriter( int filterWriter );

    QString searchExpression() const; // TODO: remove, search will be in proxy model
    void setSearchExpression( const QString &searchExpression );

    int lastPage() const;
    void setLastPage( int lastPage );

  signals:
    void lastPageChanged();

  public slots:
    void syncProjectStatusChanged( const QString &projectFullName, qreal progress );

  private slots:

    void projectMetadataChanged( const QString &projectDir );
    void onLocalProjectAdded( const QString &projectDir );
    void onLocalProjectRemoved( const QString &projectDir );

  private:

    int findProjectIndex( const QString &projectFullName );

    ProjectList mMerginProjects;
    LocalProjectsManager &mLocalProjects;
    QString mSearchExpression;
    int mLastPage;
    //! Special item as a placeholder for custom component with extended funtionality
    std::shared_ptr<MerginProject> mAdditionalItem = std::make_shared<MerginProject>();

};
#endif // MERGINPROJECTMODEL_H
