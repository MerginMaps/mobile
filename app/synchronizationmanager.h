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

#include "merginapi.h"
#include "autosynccontroller.h"

class ActiveProjectManager;

class SynchronizationManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool autosyncAllowed READ autosyncAllowed WRITE setAutosyncAllowed NOTIFY autosyncAllowedChanged )

    Q_PROPERTY( AutosyncController *autosyncController READ autosyncController NOTIFY autosyncControllerChanged )

  public:

    explicit SynchronizationManager( MerginApi *backend, ActiveProjectManager *, QObject *parent = nullptr );

    virtual ~SynchronizationManager();

    /**
     * \brief syncProject Starts synchronization of a project if there are local/server changes to be applied
     *
     * \param project Project struct instance
     * \param withAut Bears an information whether authorization should be included in sync requests.
     *                Authorization can be omitted for pull of public projects
     */
    Q_INVOKABLE void syncProject( const Project &project, bool withAuth = true );

    //! Stops a running sync process if there is one for project specified by projectFullname
    Q_INVOKABLE void stopProjectSync( const QString &projectFullname );

    bool autosyncAllowed() const;

    void setAutosyncAllowed( bool );

    AutosyncController *autosyncController() const;

  signals:

    void syncProjectStatusChanged( const QString &projectFullName, qreal progress );

    void syncProjectFinished( const QString &projectDir, const QString &projectFullName, bool successfully, int version );

    void autosyncAllowedChanged( bool autosyncAllowed );

    void autosyncControllerChanged( AutosyncController *controller );

  private:

    void setupAutosync();

    void clearAutosync();

    void setupAutosyncController();

    void clearAutosyncController();

    bool mAutosyncAllowed = false;
    std::unique_ptr<AutosyncController> mAutosyncController; // owned

    MerginApi *mBackend = nullptr; // not owned
    ActiveProjectManager *mActiveProjectManager = nullptr; // not owned
    QString mLastRequestId;
};

#endif // SYNCHRONIZATIONMANAGER_H
