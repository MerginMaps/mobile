/***************************************************************************
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
#include <memory>

#include "project.h"
#include "merginapi.h"

class LocalProjectsManager;

/**
 * \brief The ProjectsModel class
 */
class ProjectsModel : public QAbstractListModel
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
      CreatedProjectsModel,
      SharedProjectsModel,
      PublicProjectsModel,
      RecentProjectsModel
    };
    Q_ENUM( ProjectModelTypes )

    ProjectsModel( QObject *parent = nullptr );
    ~ProjectsModel() override {};

    // From Qt 5.15 we can use REQUIRED keyword here that will ensure object will be always instantiated from QML with these mandatory properties
    Q_PROPERTY( MerginApi *merginApi READ merginApi WRITE setMerginApi )
    Q_PROPERTY( LocalProjectsManager *localProjectsManager READ localProjectsManager WRITE setLocalProjectsManager )
    Q_PROPERTY( ProjectModelTypes modelType READ modelType WRITE setModelType )

    Q_PROPERTY( bool hasMoreProjects READ hasMoreProjects NOTIFY hasMoreProjectsChanged )

    // Needed methods from QAbstractListModel
    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row, int column = 0, const QModelIndex &parent = QModelIndex() ) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    //! Called to list projects, either fetch more or get first
    Q_INVOKABLE void listProjects( const QString &searchExpression = QString(), int page = 1 );

    //! Called to list projects, either fetch more or get first
    Q_INVOKABLE void listProjectsByName();

    //! Syncs specified project - upload or update
    Q_INVOKABLE void syncProject( const QString &projectId );

    //! Stops running project upload or update
    Q_INVOKABLE void stopProjectSync( const QString &projectId );

    //! Forwards call to LocalProjectsManager to remove local project
    Q_INVOKABLE void removeLocalProject( const QString &projectId );

    //! Migrates local project to mergin
    Q_INVOKABLE void migrateProject( const QString &projectId );

    Q_INVOKABLE void fetchAnotherPage( const QString &searchExpression );

    Q_INVOKABLE QVariant dataFrom( int fromRole, QVariant fromValue, int desiredRole ) const;

    //! Method merging local and remote projects based on the model type
    void mergeProjects( const MerginProjectsList &merginProjects, Transactions pendingProjects, bool keepPrevious = false );

    ProjectsModel::ProjectModelTypes modelType() const;

    MerginApi *merginApi() const { return mBackend; }

    LocalProjectsManager *localProjectsManager() const { return mLocalProjectsManager; }

    bool hasMoreProjects() const;

    bool containsProject( QString projectId ) const;

    std::shared_ptr<Project> projectFromId( QString projectId ) const;

  public slots:
    // MerginAPI - backend signals
    void onListProjectsFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, int projectsCount, int page, QString requestId );
    void onListProjectsByNameFinished( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId );
    void onProjectSyncFinished( const QString &projectDir, const QString &projectFullName, bool successfully, int newVersion );
    void onProjectSyncProgressChanged( const QString &projectFullName, qreal progress );
    void onProjectDetachedFromMergin( const QString &projectFullName );
    void onProjectAttachedToMergin( const QString &projectFullName );
    void onAuthChanged(); // when user logs out

    // LocalProjectsManager signals
    void onProjectAdded( const LocalProject &project );
    void onAboutToRemoveProject( const LocalProject project );
    void onProjectDataChanged( const LocalProject &project );

    void setMerginApi( MerginApi *merginApi );
    void setLocalProjectsManager( LocalProjectsManager *localProjectsManager );
    void setModelType( ProjectModelTypes modelType );

  signals:
    void modelInitialized();
    void hasMoreProjectsChanged();

  private:
    QString modelTypeToFlag() const;
    void printProjects() const;
    QStringList projectNames() const;
    void loadLocalProjects();
    void initializeProjectsModel();

    MerginApi *mBackend = nullptr;
    LocalProjectsManager *mLocalProjectsManager = nullptr;
    QList<std::shared_ptr<Project>> mProjects;

    ProjectModelTypes mModelType = EmptyProjectsModel;

    //! For pagination
    int mServerProjectsCount = -1;
    int mPaginatedPage = 1;

    //! For processing only my requests
    QString mLastRequestId;
};

#endif // PROJECTSMODEL_H
