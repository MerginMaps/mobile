#ifndef LOCALPROJECTSMANAGER_H
#define LOCALPROJECTSMANAGER_H

#include <QObject>


enum ProjectStatus
{
  NoVersion,  //!< the project is not available locally
  UpToDate,   //!< both server and local copy are in sync with no extra modifications
  OutOfDate,  //!< server has newer version than what is available locally (but the project is not modified locally)
  Modified    //!< there are some local modifications in the project that need to be pushed (note: also server may have newer version)
};
Q_ENUMS( ProjectStatus )


//! Summary information about a local project
struct LocalProjectInfo
{
  bool isValid() const { return !projectDir.isEmpty(); }

  QString projectDir;  //!< full path to the project directory

  QString qgisProjectFilePath;  //!< path to the .qgs/.qgz file (or empty if not have exactly one such file)

  //
  // mergin-specific project info (may be empty)
  //

  QString projectName;
  QString projectNamespace;

  int localVersion = -1;  //!< the project version that is currently available locally
  int serverVersion = -1;  //!< the project version most recently seen on server (may be -1 if no info from server is available)

  bool syncPending = false;  //!< whether sync is currently in progress
  qreal syncProgress = -1;  //!< progress in case of pending download/upload (values [0..1])
  ProjectStatus status = NoVersion;

};


class LocalProjectsManager : public QObject
{
    Q_OBJECT
  public:
    explicit LocalProjectsManager( const QString &dataDir );

    QString dataDir() const { return mDataDir; }

    QList<LocalProjectInfo> projects() const { return mProjects; }

    LocalProjectInfo projectFromDirectory( const QString &projectDir ) const;

    LocalProjectInfo projectFromMerginName( const QString &projectFullName ) const;
    LocalProjectInfo projectFromMerginName( const QString &projectNamespace, const QString &projectName ) const;

    bool hasMerginProject( const QString &projectFullName ) const;
    bool hasMerginProject( const QString &projectNamespace, const QString &projectName ) const;

    void updateProjectStatus( const QString &projectDir );

    //! Should add an entry about newly created project
    void addMerginProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName );

    //! Should forget about that project (it has been removed already)
    void removeProject( const QString &projectDir );

    //! Recursively removes project's directory (only when it exists in the list)
    void deleteProjectDirectory( const QString &projectDir );

    //
    // updates of mergin info
    //

    //! after successful update/upload - both server and local version are the same
    void updateMerginLocalVersion( const QString &projectDir, int version );

    //! after receiving project info with server version (local version stays the same
    void updateMerginServerVersion( const QString &projectDir, int version );

    //! sets whether we're doing update/upload right now
    void updateMerginSyncPending( const QString &projectDir, bool pending );

    //! during mergin sync - this may be called many times to update the progress
    void updateMerginSyncProgress( const QString &projectDir, qreal progress );


    static ProjectStatus currentProjectStatus( const LocalProjectInfo &project );

  signals:
    void projectSyncPendingChanged( const QString &projectDir, bool pending );
    void projectSyncProgressChanged( const QString &projectDir, qreal progress );
    void projectStatusChanged( const QString &projectDir );
    void localProjectAdded( const QString &projectDir );
    void localProjectRemoved( const QString &projectDir );

  private:
    void updateProjectStatus( LocalProjectInfo &project );

  private:
    QString mDataDir;   //!< directory with all local projects
    QList<LocalProjectInfo> mProjects;
};


#endif // LOCALPROJECTSMANAGER_H
