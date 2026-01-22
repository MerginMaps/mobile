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

    Q_INVOKABLE void setIsSyncPaused( const bool isSyncPaused )
    {
      mIsSyncPaused = isSyncPaused;
    }


  signals:
    void projectSyncRequested( SyncOptions::RequestOrigin origin );

  public slots:
    void checkSyncRequiredAfterAppStateChange( Qt::ApplicationState state );
    // This triggers sync after a change has been saved to layer
    Q_INVOKABLE void syncLayerChange();

  private:

    QgsProject *mQgsProject = nullptr; // not owned
    QDateTime mLastUpdateTime;
    std::unique_ptr<QTimer> mTimer = nullptr;
    bool mIsSyncPaused = false;
};

#endif // AUTOSYNCCONTROLLER_H
