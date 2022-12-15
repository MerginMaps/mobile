/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MERGINUSERINFO_H
#define MERGINUSERINFO_H

#include <QObject>
#include <QString>
#include <QJsonObject>

#include "merginsubscriptionstatus.h"
#include "merginsubscriptiontype.h"

class MerginUserInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString email READ email NOTIFY userInfoChanged )
    Q_PROPERTY( double storageLimit READ storageLimit NOTIFY userInfoChanged )
    Q_PROPERTY( double diskUsage READ diskUsage NOTIFY userInfoChanged ) // in Bytes
    Q_PROPERTY( QString activeWorkspace READ activeWorkspace NOTIFY userInfoChanged )

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

    void clear();
    void setFromJson( QJsonObject docObj );

    QString email() const;
    double diskUsage() const;
    double storageLimit() const;
    QString activeWorkspace() const;

    void saveWorkspacesData();
    void loadWorkspacesData();

    void findActiveWorkspace();
    void saveLastActiveWorkspace();

  signals:
    void userInfoChanged();

  public slots:
    void onStorageChanged( double storage );

  private:
    QString mEmail;
    double mDiskUsage = 0.0; // in Bytes
    double mStorageLimit = 0.0; // in Bytes
    int mPreferredWorkspace = -1;
    QMap<int, QString> mWorkspaces;
    int mActiveWorkspace = -1;
    QString mActiveWorkspaceName = "";
};

#endif // MERGINUSERINFO_H
