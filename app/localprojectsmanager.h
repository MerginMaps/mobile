/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LOCALPROJECTSMANAGER_H
#define LOCALPROJECTSMANAGER_H

#include <QObject>
#include <project_future.h>

//enum ProjectStatus
//{
//  NoVersion,  //!< the project is not available locally
//  UpToDate,   //!< both server and local copy are in sync with no extra modifications
//  OutOfDate,  //!< server has newer version than what is available locally (but the project is not modified locally)
//  Modified,    //!< there are some local modifications in the project that need to be pushed (note: also server may have newer version)
//  NonProjectItem      //!< only for mock projects, acts like a hook to enable extra functionality for models working with projects .
//};
//Q_ENUMS( ProjectStatus )


//! Summary information about a local project
//struct LocalProjectInfo
//{
//  bool isValid() const { return !projectDir.isEmpty(); }

//  bool isShowable() const { return qgisProjectError.isEmpty(); }

//  QString projectDir;  //!< full path to the project directory

//  QString qgisProjectFilePath;  //!< path to the .qgs/.qgz file (or empty if not have exactly one such file)

//  QString qgisProjectError; //!< If project is invalid, projectError carry more information why
//  // TODO: reset when project is synchronized

//  //
//  // mergin-specific project info (may be empty)
//  //

//  QString projectName;
//  QString projectNamespace;

//  int localVersion = -1;  //!< the project version that is currently available locally
//  int serverVersion = -1;  //!< the project version most recently seen on server (may be -1 if no info from server is available)

//  ProjectStatus status = NoVersion;

//  bool operator ==( const LocalProjectInfo &other ) { return ( projectName == other.projectName && projectNamespace == other.projectNamespace );}
//  bool operator !=( const LocalProjectInfo &other ) { return !( *this == other ); }

//  // Sync status (e.g. progress) is not kept here because if a project does not exist locally yet
//  // and it is only being downloaded for the first time, it's not in the list of local projects either
//  // and we would need to do some workarounds for that.
//};


class LocalProjectsManager : public QObject
{
    Q_OBJECT
  public:
    explicit LocalProjectsManager( const QString &dataDir );

    QString dataDir() const { return mDataDir; }

    QList<LocalProject_future> projects() const { return mProjects; }

    //! Loads all projects from mDataDir, removes all old projects
    void reloadDataDir();

    LocalProject_future projectFromDirectory( const QString &projectDir ) const;
    LocalProject_future projectFromProjectFilePath( const QString &projectFilePath ) const;

    LocalProject_future projectFromMerginName( const QString &projectFullName ) const;
    LocalProject_future projectFromMerginName( const QString &projectNamespace, const QString &projectName ) const;

    //! Adds entry about newly created project
    void addLocalProject( const QString &projectDir, const QString &projectName );

//    bool hasMerginProject( const QString &projectFullName ) const;
//    bool hasMerginProject( const QString &projectNamespace, const QString &projectName ) const;

//    void updateProjectStatus( const QString &projectDir );

    //! Adds entry for downloaded project
    void addMerginProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName );

    //! Should add an entry about newly created local project
//    void addLocalProject( const QString &projectDir, const QString &projectName );

    //! Should forget about that project (it has been removed already)
    void removeLocalProject( const QString &projectDir );

    //! Resets mergin related info for given project.
//    void removeMerginInfo( const QString &projectFullName );

    //! Recursively removes project's directory (only when it exists in the list)
//    void deleteProjectDirectory( const QString &projectDir );

    //
    // updates of mergin info
    //

    //! after successful update/upload - both server and local version are the same
    void updateLocalVersion( const QString &projectDir, int version );

    //! after receiving project info with server version (local version stays the same
//    void updateMerginServerVersion( const QString &projectDir, int version );

    //! Updates qgisProjectError (after successful project synced)
//    void updateProjectErrors( const QString &projectDir, const QString &errMsg );

    //! Updates proejct's namespace
    void updateNamespace( const QString &projectDir, const QString &projectNamespace );

    //! Finds all QGIS project files and set the err variable if any occured.
    QString findQgisProjectFile( const QString &projectDir, QString &err );


    static ProjectStatus_future currentProjectStatus( const Project_future &project ); // TODO: maybe move somewhere else?

  signals:
    void projectMetadataChanged( const QString &projectDir );
    void localMerginProjectAdded( const QString &projectDir );
    void localProjectAdded( const QString &projectDir );
    void localProjectRemoved( const QString &projectDir );
    void localProjectDataChanged( const QString &projectDir );

//  private:
//    void updateProjectStatus( LocalProject_future &project ); // TODO: local project manager should not have status

  private:
    void addProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName );

    QString mDataDir;   //!< directory with all local projects
    QList<LocalProject_future> mProjects;
};


#endif // LOCALPROJECTSMANAGER_H
