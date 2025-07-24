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

#include "project.h"
#include "merginapi.h"
#include "synchronizationoptions.h"

struct SyncProcess
{
  qreal progress;
  bool pending;

  bool reloadProject = false;

  bool awaitsRetry; // not currently being synced, but awaits to be synced
  int retriesCount = 0;
  SyncOptions::Strategy strategy = SyncOptions::Singleshot;
  // In future: current state (push/pull)
};

class SynchronizationManager : public QObject
{
    Q_OBJECT

  public:

    explicit SynchronizationManager( MerginApi *merginApi, QObject *parent = nullptr );

    ~SynchronizationManager() override;

    //! Stops a running sync process if there is one for project specified by projectId
    void stopProjectSync( const QString &projectId ) const;

    Q_INVOKABLE void migrateProjectToMergin( const QString &projectName, const QString &projectId );

    //! Returns sync progress of specified project in range <0, 1>. Returns -1 if this project is not being synchronised.
    qreal syncProgress( const QString &projectId ) const;

    //! Returns true if specified project is being synchronised, false otherwise.
    Q_INVOKABLE bool hasPendingSync( const QString &projectId ) const;

    //! Returns list of UUIDs of pending projects
    QList<QString> pendingProjects() const;

  signals:

    // Synchronization signals
    void syncStarted( const QString &projectId );
    void syncCancelled( const QString &projectId );
    void syncProgressChanged( const QString &projectId, qreal progress );
    void syncFinished( const QString &projectId, bool success, int newVersion, bool reloadNeeded );
    void syncError( const QString &projectId, int errorType, bool willRetry = false, const QString &errorMessage = QLatin1String() );

  public slots:

    /**
     * \brief syncProject Starts synchronization of a project if there are local/server changes to be applied
     *
     * \param project Project struct instance
     * \param auth Bears an information whether authorization should be included in sync requests.
     *                Authorization can be omitted for pull of public projects
     * \param strategy The fetching strategy to use
     */
    void syncProject( const LocalProject &project, SyncOptions::Authorization auth = SyncOptions::Authorized, SyncOptions::Strategy strategy = SyncOptions::Singleshot );

    //! Overloaded method, allows to sync with Project instance. Can be used in case of first download of remote project (it has invalid LocalProject info).
    void syncProject( const Project &project, SyncOptions::Authorization auth = SyncOptions::Authorized, SyncOptions::Strategy strategy = SyncOptions::Singleshot );

    // Handling of synchronization changes from MerginApi
    void onProjectSyncCanceled( const QString &projectId );
    void onProjectSyncProgressChanged( const QString &projectId, qreal progress );
    void onProjectSyncFinished( const QString &projectId, bool successfully, int version );
    void onProjectSyncFailure( const QString &message, int httpCode, const QString &projectId );
    void onProjectReloadNeededAfterSync( const QString &projectId );
    void onProjectCreated( const QString &projectId, bool result );

  private:

    // Hashmap of currently running synchronizations, key: project ID
    QHash<QString, SyncProcess> mSyncProcesses;

    MerginApi *mMerginApi = nullptr; // not owned

    int mSyncRetryIntervalSeconds = 100000; // 1 minute between sync retries
};

#endif // SYNCHRONIZATIONMANAGER_H
