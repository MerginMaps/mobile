/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECTSMODEL_FUTURE_H
#define PROJECTSMODEL_FUTURE_H

#include <QAbstractListModel>
#include <memory>

#include "project_future.h"
#include "merginapi.h"

class LocalProjectsManager;

/**
 * \brief The ProjectModelTypes enum
 */
enum ProjectModelTypes
{
  LocalProjectsModel = 0,
  MyProjectsModel,
  SharedProjectsModel,
  ExploreProjectsModel,
  RecentProjectsModel
};

/**
 * \brief The ProjectsModel_future class
 */
class ProjectsModel_future : public QAbstractListModel
{
    Q_OBJECT

  public:

    enum Roles
    {
      ProjectName = Qt::UserRole + 1,
      ProjectNamespace,
      ProjectFullName, // or ProjectId, filled with folderName if project is not
      ProjectDescription,
      ProjectPending,
      ProjectIsMergin,
      ProjectIsLocal,
      ProjectStatus,
      ProjectProgress
    };
    Q_ENUMS( Roles )

    ProjectsModel_future( MerginApi *merginApi, ProjectModelTypes modelType, LocalProjectsManager &localProjectsManager, QObject *parent = nullptr );
    ~ProjectsModel_future() override {};

    // Needed methods from QAbstractListModel
    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row, int column = 0, const QModelIndex &parent = QModelIndex() ) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    //! Called to list projects, either fetch more or get first
    Q_INVOKABLE void listProjects();

    //! Called to list projects, either fetch more or get first
    Q_INVOKABLE void listProjectsByName();

    //! Syncs specified project - upload or update
    Q_INVOKABLE void syncProject( QString projectNamespace, QString projectName );

    //! Stops running project upload or update
    Q_INVOKABLE void stopProjectSync( QString projectNamespace, QString projectName );

    //! Method merging local and remote projects based on the model type
    void mergeProjects( const MerginProjectsList &merginProjects, Transactions pendingProjects );

  public slots:
    void onListProjectsFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, int projectsCount, int page, QString requestId );
    void onListProjectsByNameFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId );
    void onProjectSyncFinished( const QString &projectDir, const QString &projectFullName, bool successfully = true );
    void onProjectSyncProgressChanged( const QString &projectFullName, qreal progress );

    void onProjectAdded( const QString &projectDir );
    void onProjectRemoved( const QString &projectFullName );
    void onProjectDataChanged( const QString &projectDir );

    void onProjectDetachedFromMergin( const QString &projectFullName );
    void onProjectAttachedToMergin( const QString &projectFullName );

  private:

    QString modelTypeToFlag() const;
    void printProjects() const;
    QStringList projectNames() const;

    bool containsProject( QString projectId ) const;
    std::shared_ptr<Project_future> projectFromId( QString projectId ) const;

    MerginApi *mBackend;
    LocalProjectsManager &mLocalProjectsManager;
    QList<std::shared_ptr<Project_future>> mProjects;

    ProjectModelTypes mModelType;

    //! For pagination
    int mPopulatedPage = -1; // -> on the fly in QML:: QML should pass this to model

    //! For processing only my requests
    QString mLastRequestId;
};

#endif // PROJECTSMODEL_FUTURE_H
