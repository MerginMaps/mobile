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
#include <QNetworkReply>

#include "project.h"
#include "merginapi.h"
#include "synchronizationerror.h"
#include "synchronizationoptions.h"

struct SyncProcess
{
  qreal progress;
  bool pending;

  bool reloadProject = false;

  bool awaitsRetry; // not currently being synced, but awaits to be synced
  int retriesCount = 0;
  SyncOptions::Strategy strategy = SyncOptions::Singleshot;
  SyncOptions::RequestOrigin requestOrigin;
  // In future: current state (push/pull)
};

class SynchronizationManager : public QObject
{
    Q_OBJECT

  public:

    explicit SynchronizationManager( MerginApi *merginApi, QObject *parent = nullptr );

    virtual ~SynchronizationManager();

    //! Stops a running sync process if there is one for project specified by projectFullname
    void stopProjectSync( const QString &projectFullName );

    Q_INVOKABLE void migrateProjectToMergin( const QString &projectName );

    //! Returns sync progress of specified project in range <0, 1>. Returns -1 if this project is not being synchronised.
    qreal syncProgress( const QString &projectFullName ) const;

    //! Returns true if specified project is being synchronised, false otherwise.
    Q_INVOKABLE bool hasPendingSync( const QString &projectFullName ) const;

    QList<QString> pendingProjects() const;

  signals:

    // Synchronization signals
    void syncStarted( const QString &projectFullName );
    void syncCancelled( const QString &projectFullName );
    void syncProgressChanged( const QString &projectFullName, qreal progress );
    void syncFinished( const QString &projectFullName, bool success, int newVersion, bool reloadNeeded );

    void syncError( const QString &projectFullName, int errorType, bool willRetry = false, const QString &errorMessage = QLatin1String() );
    void projectAlreadyOnLatestVersion( const QString &projectFullName );

  public slots:

    /**
     * \brief syncProject Starts synchronization of a project if there are local/server changes to be applied
     *
     * \param project Project struct instance
     * \param auth Bears an information whether authorization should be included in sync requests.
     *                Authorization can be omitted for pull of public projects
     * \param strategy Describes whether sync will be tried again after temporary error
     * \param requestOrigin Flags if the request is coming from user or autosync controller
     */
    void syncProject( const LocalProject &project, SyncOptions::Authorization auth = SyncOptions::Authorized, SyncOptions::Strategy strategy = SyncOptions::Singleshot, SyncOptions
                      ::RequestOrigin requestOrigin = SyncOptions::RequestOrigin::ManualRequest );

    //! Overloaded method, allows to sync with Project instance. Can be used in case of first download of remote project (it has invalid LocalProject info).
    void syncProject( const Project &project, SyncOptions::Authorization auth = SyncOptions::Authorized, SyncOptions::Strategy strategy = SyncOptions::Singleshot, SyncOptions
                      ::RequestOrigin requestOrigin = SyncOptions::RequestOrigin::ManualRequest );

    // Handling of synchronization changes from MerginApi
    void onProjectSyncCanceled( const QString &projectFullName, bool hasError );
    void onProjectSyncProgressChanged( const QString &projectFullName, qreal progress );
    void onProjectSyncFinished( const QString &projectFullName, bool successfully, int version );
    void onProjectSyncFailure( const QString &message, const QString &topic, int httpCode, const QString &projectFullName );
    void onProjectAttachedToMergin( const QString &projectFullName, const QString &previousName );
    void onProjectReloadNeededAfterSync( const QString &projectFullName );
    void onProjectCreated( const QString &projectName, bool result );

  private:

    // Hashmap of currently running synchronizations, key: project full name
    QHash<QString, SyncProcess> mSyncProcesses;

    MerginApi *mMerginApi = nullptr; // not owned

    int mSyncRetryIntervalSeconds = 100000; // 1 minute between sync retries
};

#endif // SYNCHRONIZATIONMANAGER_H
