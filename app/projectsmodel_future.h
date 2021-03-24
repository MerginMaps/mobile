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
      ProjectFullName,
      ProjectId, // Filled with ProjectFullName for time being
      ProjectDirectory,
      ProjectDescription,
      ProjectPending,
      ProjectIsMergin,
      ProjectIsLocal,
      ProjectFilePath,
      ProjectIsValid,
      ProjectSyncStatus,
      ProjectSyncProgress,
      ProjectRemoteError
    };
    Q_ENUM( Roles )

    /**
     * \brief The ProjectModelTypes enum
     */
    enum ProjectModelTypes
    {
      EmptyProjectsModel = 0, // default, holding no projects ~ invalid model
      LocalProjectsModel,
      MyProjectsModel,
      SharedProjectsModel,
      ExploreProjectsModel,
      RecentProjectsModel
    };
    Q_ENUM( ProjectModelTypes )

    ProjectsModel_future( QObject *parent = nullptr );
    ~ProjectsModel_future() override {};

    Q_PROPERTY( int serverProjectsCount READ serverProjectsCount WRITE setServerProjectsCount NOTIFY serverProjectsCountChanged ) // TODO: replace with builtin canFetchMore

    // From Qt 5.15 we can use REQUIRED keyword here that will ensure object will be always instantiated from QML with these mandatory properties
    Q_PROPERTY( MerginApi *merginApi READ merginApi WRITE setMerginApi )
    Q_PROPERTY( LocalProjectsManager *localProjectsManager READ localProjectsManager WRITE setLocalProjectsManager )
    Q_PROPERTY( ProjectModelTypes modelType READ modelType WRITE setModelType )

    // Needed methods from QAbstractListModel
    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row, int column = 0, const QModelIndex &parent = QModelIndex() ) const override;
    Q_INVOKABLE bool canFetchMore( const QModelIndex &parent ) const override;
    Q_INVOKABLE void fetchMore( const QModelIndex &parent ) override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    //! Called to list projects, either fetch more or get first
    Q_INVOKABLE void listProjects( int page = 1, const QString searchExpression = QString() );

    //! Called to list projects, either fetch more or get first
    Q_INVOKABLE void listProjectsByName();

    //! Syncs specified project - upload or update
    Q_INVOKABLE void syncProject( const QString &projectNamespace, const QString &projectName );

    //! Stops running project upload or update
    Q_INVOKABLE void stopProjectSync( const QString &projectNamespace, const QString &projectName );

    //! Forwards call to LocalProjectsManager to remove local project
    Q_INVOKABLE void removeLocalProject( const QString &projectDir );

    //! Method merging local and remote projects based on the model type
    void mergeProjects( const MerginProjectsList &merginProjects, Transactions pendingProjects, bool keepPrevious = false );

    int serverProjectsCount() const;

    ProjectsModel_future::ProjectModelTypes modelType() const;

    MerginApi *merginApi() const { return mBackend; }

    LocalProjectsManager *localProjectsManager() const { return mLocalProjectsManager; }

public slots:
    // MerginAPI - backend signals
    void onListProjectsFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, int projectsCount, int page, QString requestId );
    void onListProjectsByNameFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId );
    void onProjectSyncFinished( const QString &projectDir, const QString &projectFullName, bool successfully = true );
    void onProjectSyncProgressChanged( const QString &projectFullName, qreal progress );
    void onProjectDetachedFromMergin( const QString &projectFullName );
    void onProjectAttachedToMergin( const QString &projectFullName );

    // LocalProjectsManager signals
    void onProjectAdded( const LocalProject_future &project );
    void onAboutToRemoveProject( const LocalProject_future project );
    void onProjectDataChanged( const LocalProject_future &project );

    void setServerProjectsCount( int serverProjectsCount );
    void setMerginApi( MerginApi *merginApi );
    void setLocalProjectsManager( LocalProjectsManager *localProjectsManager );
    void setModelType( ProjectModelTypes modelType );

signals:
    void serverProjectsCountChanged( int serverProjectsCount );
    void modelInitialized();

private:

    QString modelTypeToFlag() const;
    void printProjects() const;
    QStringList projectNames() const;
    void loadLocalProjects();
    void initializeProjectsModel();

    bool containsProject( QString projectId ) const;
    std::shared_ptr<Project_future> projectFromId( QString projectId ) const;

    MerginApi *mBackend = nullptr;
    LocalProjectsManager *mLocalProjectsManager = nullptr;
    QList<std::shared_ptr<Project_future>> mProjects;

    ProjectModelTypes mModelType = EmptyProjectsModel;

    //! For pagination
    int mServerProjectsCount = -1;

    //! For processing only my requests
    QString mLastRequestId;
};

#endif // PROJECTSMODEL_FUTURE_H
