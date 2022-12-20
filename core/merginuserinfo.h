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
    Q_PROPERTY( QString activeWorkspace READ activeWorkspace NOTIFY userInfoChanged )
    Q_PROPERTY( QStringList workspaces READ workspaces NOTIFY userInfoChanged )

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

    void clear();
    void setFromJson( QJsonObject docObj );

    QString email() const;
    QString activeWorkspace() const;
    QStringList workspaces() const;

    void saveWorkspacesData();
    void loadWorkspacesData();

    int findActiveWorkspace( int preferredWorkspace = -1 );
    void setActiveWorkspace( int newWorkspace );

  signals:
    void userInfoChanged();
    void activeWorkspaceChanged();

  private:
    QString mEmail;
    QMap<int, QString> mWorkspaces;
    int mActiveWorkspace = -1;
};

#endif // MERGINUSERINFO_H
