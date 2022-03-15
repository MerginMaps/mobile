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

#include "qgsproject.h"

#include "project.h"

class AutosyncController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( SyncStatus syncStatus READ syncStatus NOTIFY syncStatusChanged )

  public:

    enum SyncStatus
    {
      Synced = 0,
      SyncInProgress,
      PendingChanges,
      SyncKeepsFailing
    };
    Q_ENUMS( SyncStatus )

    explicit AutosyncController( QObject *parent = nullptr );

    virtual ~AutosyncController();

    SyncStatus syncStatus();

    void setActiveProject( Project project );

    void setActiveQgsProject( QgsProject *qgsProject );

  signals:

    void syncProject( Project *project );

    void syncStatusChanged( SyncStatus status );

  public slots:

    void handleSyncFinished();

  private:

    SyncStatus mSyncStatus = Synced;

    Project *mActiveProject = nullptr;

    QgsProject *mActiveQgsProject = nullptr;

    QTimer mTimer;
};

#endif // AUTOSYNCCONTROLLER_H
