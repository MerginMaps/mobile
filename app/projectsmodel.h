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
#include "synchronizationmanager.h"

class LocalProjectsManager;

/**
 * \brief The ProjectsModel class holds projects (both local and mergin). Model loads local projects from LocalProjectsManager that hold them
   during runtime. Remote (Mergin) projects are fetched from MerginAPI calling listProjects or listProjectsByName (based on the type of the model).
 *
 * The main job of the model is to merge projects coming from MerginAPI and LocalProjectsManager. By merging it means Each time new response is received from MerginAPI, model erases
 * old remembered projects and fetches new. Merge logic depends on the model type (described below).
 *
 * Model can have different types that affect handling of the projects.
 *  - LocalProjectsModel always keeps all local projects and seek their mergin part when listProjectsByNameFinished
 *  - Workspace-, and PublicProjectsModel does the opposite, keeps all mergin projects and seeks their local part in projects from LocalProjectsManager
 *  - EmptyProjectsModel is default state
 *
 *  To avoid overriding of requests, model remembers last sent request ID and upon receiving signal from MerginAPI about listProjectsFinished, it firsts compares
 *  the remembered ID with returned ID. If they do not match, response is ignored.
 *
 *  Model also support pagination. To fetch another page call fetchAnotherPage.
 *
 *  This is a QML type with 3 required properties (pointer to merginApi, pointer to localProjectsManager and modelType). Without these properties model does nothing.
 *  After setting all of these properties, model is initialized, starts listening to various signals and offers data.
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
      ProjectId,
      ProjectDirectory,
      ProjectDescription,
      ProjectIsMergin,
      ProjectIsLocal,
      ProjectFilePath,
      ProjectIsValid,
      ProjectStatus,
      ProjectSyncPending,
      ProjectSyncProgress,
      ProjectRemoteError,
      ProjectIsActiveProject
    };
    Q_ENUM( Roles )

    /**
     * \brief The ProjectModelTypes enum:
     * - LocalProjectsModel always keeps all local projects and seek their mergin part when listProjectsByNameFinished
     * - Workspace-, and PublicProjectsModel does the opposite, keeps all mergin projects and seeks their local part in projects from LocalProjectsManager
     * - EmptyProjectsModel is default state
     */
    enum ProjectModelTypes
    {
      EmptyProjectsModel = 0, // default, holding no projects ~ invalid model
      LocalProjectsModel,
      PublicProjectsModel,
      WorkspaceProjectsModel,
      RecentProjectsModel
    };
    Q_ENUM( ProjectModelTypes )

    enum MergeStrategy
    {
      KeepPrevious = 0,
      DiscardPrevious
    };

    ProjectsModel( QObject *parent = nullptr );
    ~ProjectsModel() override {};

    // From Qt 5.15 we can use REQUIRED keyword here, that will ensure object will be always instantiated from QML with these mandatory properties
    Q_PROPERTY( MerginApi *merginApi READ merginApi WRITE setMerginApi NOTIFY merginApiChanged )
    Q_PROPERTY( ProjectModelTypes modelType READ modelType WRITE setModelType NOTIFY modelTypeChanged )
    Q_PROPERTY( SynchronizationManager *syncManager READ syncManager WRITE setSyncManager NOTIFY syncManagerChanged )
    Q_PROPERTY( LocalProjectsManager *localProjectsManager READ localProjectsManager WRITE setLocalProjectsManager NOTIFY localProjectsManagerChanged )

    //! Indicates that model has more projects to fetch, so view can call fetchAnotherPage
    Q_PROPERTY( bool hasMoreProjects READ hasMoreProjects NOTIFY hasMoreProjectsChanged )

    //! Indicates that model is currently processing projects, filling its storage.
    //! Models loading starts when listProjectsAPI is sent and finishes after endResetModel signal is emitted when projects are merged.
    Q_PROPERTY( bool isLoading READ isLoading NOTIFY isLoadingChanged )

    //! Use to store the active project id in the model, so that Roles::ProjectIsActiveProject can be used
    Q_PROPERTY( QString activeProjectId READ activeProjectId WRITE setActiveProjectId NOTIFY activeProjectIdChanged )

    // Needed methods from QAbstractListModel
    Q_INVOKABLE QVariant data( const QModelIndex &index, int role ) const override;
    Q_INVOKABLE QModelIndex index( int row, int column = 0, const QModelIndex &parent = QModelIndex() ) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;

    //! lists projects, either fetch more or get first, search expression
    Q_INVOKABLE void listProjects( const QString &searchExpression = QString(), int page = 1 );

    //! lists projects via listProjectsByName API, used in LocalProjectsModel
    Q_INVOKABLE void listProjectsByName();

    //! Syncs specified project - upload or update
    Q_INVOKABLE void syncProject( const QString &projectId );

    //! Stops running project upload or update
    Q_INVOKABLE void stopProjectSync( const QString &projectId );

    //! Forwards call to LocalProjectsManager to remove local project
    Q_INVOKABLE void removeLocalProject( const QString &projectId );

    //! Migrates local project to mergin
    Q_INVOKABLE void migrateProject( const QString &projectId );

    //! Calls listProjects with incremented page
    Q_INVOKABLE void fetchAnotherPage( const QString &searchExpression );

    //! Merges local and remote projects based on the model type
    void mergeProjects( const MerginProjectsList &merginProjects, MergeStrategy mergeStrategy = DiscardPrevious );

    //! Returns Project deep copy from projectId
    Project projectFromId( const QString &projectId ) const;

    //! Returns model index from projectId
    Q_INVOKABLE QModelIndex projectModelIndexFromId( const QString &projectId ) const;

    MerginApi *merginApi() const;
    SynchronizationManager *syncManager() const;
    LocalProjectsManager *localProjectsManager() const;
    ProjectsModel::ProjectModelTypes modelType() const;

    QString activeProjectId() const;

    bool isLoading() const;
    bool hasMoreProjects() const;

  public slots:
    // MerginAPI - project list signals
    void onListProjectsFinished( const MerginProjectsList &merginProjects, int projectsCount, int page, QString requestId );
    void onListProjectsByNameFinished( const MerginProjectsList &merginProjects, QString requestId );

    // Synchonization signals
    void onProjectSyncStarted( const QString &projectFullName );
    void onProjectSyncCancelled( const QString &projectFullName );
    void onProjectSyncProgressChanged( const QString &projectFullName, qreal progress );
    void onProjectSyncFinished( const QString &projectFullName, bool successfully, int newVersion );

    void onProjectDetachedFromMergin( const QString &projectFullName );
    void onProjectAttachedToMergin( const QString &projectFullName );

    // LocalProjectsManager signals
    void onProjectAdded( const LocalProject &project );
    void onAboutToRemoveProject( const LocalProject &project );
    void onProjectDataChanged( const LocalProject &project );

    void onAuthChanged();

    void setMerginApi( MerginApi *merginApi );
    void setModelType( ProjectModelTypes modelType );
    void setSyncManager( SynchronizationManager *newSyncManager );
    void setLocalProjectsManager( LocalProjectsManager *localProjectsManager );

    void setActiveProjectId( const QString &projectId );

  signals:
    void modelInitialized();
    void hasMoreProjectsChanged();

    void isLoadingChanged( bool isLoading );

    void merginApiChanged( MerginApi *api );
    void modelTypeChanged( ProjectModelTypes type );
    void syncManagerChanged( SynchronizationManager *syncManager );
    void localProjectsManagerChanged( LocalProjectsManager *projectsManager );

    void activeProjectIdChanged( QString projectId );

  private:

    int projectIndexFromId( const QString &projectId ) const;

    void setModelIsLoading( bool state );

    QString modelTypeToFlag() const;
    QStringList projectNames() const;
    void clearProjects();
    void loadLocalProjects();
    void initializeProjectsModel();

    QList<Project> mProjects;

    ProjectModelTypes mModelType = EmptyProjectsModel;

    //! For pagination
    int mServerProjectsCount = -1;
    int mPaginatedPage = 1;

    //! For processing requests sent via this model
    QString mLastRequestId;

    bool mModelIsLoading;

    MerginApi *mBackend = nullptr; // not owned
    LocalProjectsManager *mLocalProjectsManager = nullptr; // not owned
    SynchronizationManager *mSyncManager = nullptr; // not owned

    QString mActiveProjectId;

    friend class TestModels;
};

#endif // PROJECTSMODEL_H
