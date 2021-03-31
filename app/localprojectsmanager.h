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

class LocalProjectsManager : public QObject
{
    Q_OBJECT
  public:
    explicit LocalProjectsManager( const QString &dataDir );

    //! Loads all projects from mDataDir, removes all old projects
    void reloadDataDir();

    QString dataDir() const { return mDataDir; }

    LocalProjectsList projects() const { return mProjects; }

    LocalProject_future projectFromDirectory( const QString &projectDir ) const;
    LocalProject_future projectFromProjectFilePath( const QString &projectFilePath ) const;

    LocalProject_future projectFromMerginName( const QString &projectFullName ) const;
    LocalProject_future projectFromMerginName( const QString &projectNamespace, const QString &projectName ) const;

    //! Adds entry about newly created project
    void addLocalProject( const QString &projectDir, const QString &projectName );

    //! Adds entry for downloaded project
    void addMerginProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName );

    //! Should forget about that project (it has been removed already)
    Q_INVOKABLE void removeLocalProject( const QString &projectId );

    Q_INVOKABLE bool projectIsValid( const QString &path ) const;

    Q_INVOKABLE QString projectId( const QString &path ) const;

    //! after successful update/upload - both server and local version are the same
    void updateLocalVersion( const QString &projectDir, int version );

    //! Updates proejct's namespace
    void updateNamespace( const QString &projectDir, const QString &projectNamespace );

    //! Finds all QGIS project files and set the err variable if any occured.
    QString findQgisProjectFile( const QString &projectDir, QString &err );

  signals:
    void projectMetadataChanged( const QString &projectDir );
    void localMerginProjectAdded( const QString &projectDir );
    void localProjectAdded( const LocalProject_future &project );
    void aboutToRemoveLocalProject( const LocalProject_future project );
    void localProjectDataChanged( const LocalProject_future &project );
    void dataDirReloaded();

  private:
    void addProject( const QString &projectDir, const QString &projectNamespace, const QString &projectName );

    QString mDataDir;   //!< directory with all local projects
    LocalProjectsList mProjects;
};


#endif // LOCALPROJECTSMANAGER_H
