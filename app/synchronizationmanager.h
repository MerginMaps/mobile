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

struct SyncTransation
{
  bool pending = false;
  qreal progress = -1;
  QString serverInfoRequestId;
};

class SynchronizationManager : public QObject
{
    Q_OBJECT

  public:

    explicit SynchronizationManager( MerginApi *merginApi, QObject *parent = nullptr );

    virtual ~SynchronizationManager();

    //! Stops a running sync process if there is one for project specified by projectFullname
    Q_INVOKABLE void stopProjectSync( const QString &projectFullname );

  signals:

    void syncProjectProgressChanged( const QString &projectFullName, qreal progress );

    void syncProjectFinished( const QString &projectDir, const QString &projectFullName, bool successfully, int version );

  public slots:

    /**
     * \brief syncProject Starts synchronization of a project if there are local/server changes to be applied
     *
     * \param project Project struct instance
     * \param withAut Bears an information whether authorization should be included in sync requests.
     *                Authorization can be omitted for pull of public projects
     */
    void syncProject( const Project &project, bool withAuth = true );

    Q_INVOKABLE void syncProject( const LocalProject &project, bool withAuth = true );

    void receivedRemoteProjectInfo( const MerginProjectsList &merginProjects, Transactions pendingProjects, QString requestId );

  private:

    QHash<QString, SyncTransation> mOngoingSyncs;

    MerginApi *mMerginApi = nullptr; // not owned
};

#endif // SYNCHRONIZATIONMANAGER_H
