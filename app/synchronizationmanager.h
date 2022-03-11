/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNCHRONIZATIONMANAGER_H
#define SYNCHRONIZATIONMANAGER_H

#include <QObject>
#include <qglobal.h>

#include "project.h"
#include "merginapi.h"

class SynchronizationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool autosyncAllowed READ autosyncAllowed WRITE setAutosyncAllowed NOTIFY autosyncAllowedChanged )

  public:
    explicit SynchronizationManager( MerginApi *backend, QObject *parent = nullptr );

    /**
     * \brief syncProject Starts synchronization of a project if there are local/server changes to be applied
     * \param project Project struct instance
     * \param isAuthOptional Signalizes that auth should be used only when user is logged in, of not, send request anyways, but without auth data.
     *        This is helpful for getting status of projects in home ~ some are public, some are not.
     *        If auth is not optional, it is mandatory.
     */
    Q_INVOKABLE void syncProject( const Project &project, bool isAuthOptional = false );

    //! Stops a running sync process if there is one for project specified by projectFullname
    Q_INVOKABLE void stopProjectSync( const QString &projectFullname );

    bool autosyncAllowed();
    void setAutosyncAllowed( bool );

  signals:
    void autosyncAllowedChanged( bool autosyncAllowed );

  public slots:
    void activeProjectChanged( LocalProject activeProject );
    void receivedServerInfo( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId );

  private:
    bool mAutosyncAllowed = false;
    Project mActiveProject;

    MerginApi *mBackend = nullptr;
};

#endif // SYNCHRONIZATIONMANAGER_H
