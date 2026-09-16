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
  // sync progress should be a number in range <0, 1> or -1 if invalid
  qreal progress = 0;
  // whether sync is in progress
  bool pending = false;
  // whether it's necessary to reload project after sync is finished (schema changed)
  bool reloadProject = false;
  // not currently being synced, but awaits to be synced
  bool awaitsRetry = false;
  // number of retries that have been done for retryable errors
  int retriesCount = 0;
  // TODO: remove?
  SyncOptions::Strategy strategy = SyncOptions::Singleshot;
  // specifies who requested the sync
  SyncOptions::RequestOrigin requestOrigin = SyncOptions::RequestOrigin::ManualRequest;
};

constexpr int DEFAULT_RETRY_INTERVAL_MS = 100000; // 1 minute

/**
 * Synchronisation manager is a controller class used as interface for managing creation, synchronisation and deletion
 * of projects to server. Internally it calls functions of MerginApi to manage the process.
 *
 * \note This class should be used instead of direct calls to MerginApi.
 */
class SynchronizationManager : public QObject
{
    Q_OBJECT

  public:

    explicit SynchronizationManager( MerginApi *merginApi, QObject *parent = nullptr );

    ~SynchronizationManager() override = default;

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
    void projectDataChanged( const QString &projectFullName );

  public slots:
    void onTransactionFinished( const QString &finishedProjectFullName, bool successful, int version, TransactionStatus::TransactionType finishedTransactionType );

    // Handling of synchronization changes from MerginApi
    void onProjectSyncCanceled( const QString &projectFullName, bool hasError );
    void onProjectSyncProgressChanged( const QString &projectFullName, qreal progress );
    void onProjectSyncFinished( const QString &projectFullName, bool successfully, int version );
    void onTransactionFailure( const QString &message, const QString &topic, int httpCode, const QString &projectFullName );
    void onProjectAttachedToMergin( const QString &projectFullName, const QString &previousName );
    void onProjectReloadNeededAfterSync( const QString &projectFullName );
    void onProjectCreated( const QString &projectName, bool result );

  private:

    // Hashmap of currently running synchronizations, key: project full name
    QHash<QString, SyncProcess> mSyncProcesses;

    MerginApi *mMerginApi = nullptr; // not owned

    int mSyncRetryIntervalSeconds = DEFAULT_RETRY_INTERVAL_MS; // 1 minute between sync retries
};

#endif // SYNCHRONIZATIONMANAGER_H
