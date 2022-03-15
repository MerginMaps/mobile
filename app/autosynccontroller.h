/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTOSYNCCONTROLLER_H
#define AUTOSYNCCONTROLLER_H

#include <QObject>
#include <QTimer>

#include "project.h"
#include "merginapi.h"

class QgsProject;

class AutosyncController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( SyncStatus syncStatus READ syncStatus NOTIFY syncStatusChanged )

  public:

    enum SyncStatus
    {
      Synced = 0,
      SyncInProgress,
      PendingChanges, // awaits sync
      SyncKeepsFailing,
      NotAMerginProject
    };
    Q_ENUM( SyncStatus )

    explicit AutosyncController( LocalProject openedProject, QgsProject *openedQgsProject, MerginApi *backend, QObject *parent = nullptr );

    virtual ~AutosyncController();

    SyncStatus syncStatus();

  signals:

    void syncProject( Project *project );

    void syncStatusChanged( SyncStatus status );

  public slots:

    void synchronizationProgressed( const QString &projectFullName, qreal progress );

    void synchronizationFinished( const QString &projectDir, const QString &projectFullName, bool successfully, int version );

    void receivedServerInfo( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId );

    void handleLocalChange();

  private:
    void setSyncStatus( SyncStatus status );

    SyncStatus mSyncStatus = Synced;

    std::unique_ptr<Project> mActiveProject; // owned
    QgsProject *mActiveQgsProject = nullptr; // not owned
    MerginApi *mBackend = nullptr; // not owned

    QString mLastRequestId;
    QTimer mTimer;
};

#endif // AUTOSYNCCONTROLLER_H
