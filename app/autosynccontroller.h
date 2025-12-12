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

#include <QDateTime>
#include <QTimer>

#include "synchronizationoptions.h"

class QgsProject;

class AutosyncController : public QObject
{
    Q_OBJECT

  public:

    explicit AutosyncController( QgsProject *openedQgsProject, QObject *parent = nullptr );
    ~AutosyncController() override = default;

    // Set mLastUpdateTime to "now", triggered by manual sync
    void updateLastUpdateTime();

  signals:

    void projectChangeDetected( SyncOptions::RequestOrigin origin = SyncOptions::RequestOrigin::AutomaticRequest );
    void projectSyncRequired( SyncOptions::RequestOrigin origin = SyncOptions::RequestOrigin::AutomaticRequest );

  public slots:
    void checkSyncRequiredAfterAppStateChange( Qt::ApplicationState state );

  private:

    QgsProject *mQgsProject = nullptr; // not owned
    QDateTime mLastUpdateTime;
    std::unique_ptr<QTimer> mTimer = nullptr;
};

#endif // AUTOSYNCCONTROLLER_H
