/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDateTime>
#include <QDirIterator>
#include <memory>

struct Project;
struct LocalProject;

namespace ProjectStatus
{
  Q_NAMESPACE
  enum Status
  {
    NoVersion,  //!< the project is not downloaded
    UpToDate,   //!< both server and local copy are in sync with no extra modifications
    NeedsSync,  //!< server has newer version than what is available locally and/or the project is modified locally
  };
  Q_ENUM_NS( Status )

  //! Returns project state from ProjectStatus::Status enum for the project
  Status projectStatus( const Project &project );

  bool hasLocalChanges( const LocalProject &project );
}

/**
 * \brief The LocalProject struct is used as a struct for projects that are available on the device.
 * The struct is used in the \see Projects struct and also for communication between LocalProjectsManager and ProjectsModel
 *
 * \note Struct contains member id() which in this time returns projects full name, however, once we
 * start using projects IDs, it can be replaced for that ID.
 */
struct LocalProject
{
    Q_GADGET

  public:

    Q_PROPERTY( QString qgisProjectFilePath MEMBER qgisProjectFilePath )

    LocalProject() {};
    ~LocalProject() {};

    QString projectName;
    QString projectNamespace;

    Q_INVOKABLE QString id() const; //! projectFullName for time being
    QString fullName() const;

    QString projectDir;
    QString projectError; // Error that leads to project not being able to open in app

    QString qgisProjectFilePath;

    int localVersion = -1;

    bool isValid() const { return !projectDir.isEmpty(); }

    //! Returns true if the local version instance has a mergin counterpart based on localVersion.
    //! LocalVersion comes from metadata file stored in .mergin folder.
    //! Note: this is just for scenarios where you only have LocalProject instance and not Project,
    //!       Project->isMergin() is recommended to use over this one
    bool hasMerginMetadata() const { return localVersion > -1; }

    bool operator ==( const LocalProject &other )
    {
      return ( this->id() == other.id() );
    }

    bool operator !=( const LocalProject &other )
    {
      return !( *this == other );
    }
};

/**
 * \brief The MerginProject struct is used for projects that comes from Mergin.
 * This struct is used in the \see Projects struct and also for communication between MerginAPI and ProjectsModel
 *
 * \note Struct contains member id() which in this time returns projects full name, however, once we
 * start using projects IDs, it can be replaced for that ID.
 */
struct MerginProject
{
  MerginProject() {};
  ~MerginProject() {};

  QString projectName;
  QString projectNamespace;

  QString id() const; //!< projectFullName for time being

  QDateTime serverUpdated; // available latest version of project files on server
  int serverVersion = -1;

  ProjectStatus::Status status = ProjectStatus::NoVersion;

  QString remoteError; // Error leading to project not being able to sync (received error code from server)

  bool isValid() const { return !projectName.isEmpty() && !projectNamespace.isEmpty(); }

  bool operator ==( const MerginProject &other )
  {
    return ( this->id() == other.id() );
  }

  bool operator !=( const MerginProject &other )
  {
    return !( *this == other );
  }
};

/**
 * \brief The Project struct serves as a struct for any kind of project (local/mergin).
 * It consists of two main parts - mergin and local.
 * Both parts are pointers to their specific structs and based on the pointer value (nullptr or assigned) this structs
 * decides if the project is local, mergin or both.
 */
struct Project
{
  Project() {};
  ~Project() {};

  MerginProject mergin;
  LocalProject local;

  bool isMergin() const { return mergin.isValid(); }
  bool isLocal() const { return local.isValid(); }

  QString projectName() const
  {
    if ( isMergin() ) return mergin.projectName;
    else if ( isLocal() ) return local.projectName;
    return QString();
  }

  QString projectNamespace() const
  {
    if ( isMergin() ) return mergin.projectNamespace;
    else if ( isLocal() ) return local.projectNamespace;
    return QString();
  }

  QString id() const
  {
    if ( isMergin() ) return mergin.id();
    else if ( isLocal() ) return local.id();
    return QString();
  }

  QString fullName() const
  {
    return id();
  }

  bool operator ==( const Project &other )
  {
    if ( this->isLocal() && other.isLocal() )
    {
      return this->local.id() == other.local.id();
    }
    else if ( this->isMergin() && other.isMergin() )
    {
      return this->mergin.id() == other.mergin.id();
    }
    return false;
  }

  bool operator !=( const Project &other )
  {
    return !( *this == other );
  }
};

typedef QList<MerginProject> MerginProjectsList;
typedef QList<LocalProject> LocalProjectsList;
Q_DECLARE_METATYPE( MerginProjectsList )



#include <QAbstractListModel>
#include <memory>

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
    ProjectRemoteError
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

  /*
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

  MerginApi *merginApi() const;
  SynchronizationManager *syncManager() const;
  LocalProjectsManager *localProjectsManager() const;
  ProjectsModel::ProjectModelTypes modelType() const;

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

  signals:
  void modelInitialized();
  void hasMoreProjectsChanged();

  void isLoadingChanged( bool isLoading );

  void merginApiChanged( MerginApi *api );
  void modelTypeChanged( ProjectModelTypes type );
  void syncManagerChanged( SynchronizationManager *syncManager );
  void localProjectsManagerChanged( LocalProjectsManager *projectsManager );

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
*/
};




#endif // PROJECT_H
