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


struct MerginInvitation
{
  QString uuid;
  QString workspace;
  QString role;

  static MerginInvitation fromJsonObject( const QJsonObject &invitationInfo );
};


class MerginUserInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString email READ email NOTIFY userInfoChanged )
    Q_PROPERTY( QString activeWorkspace READ activeWorkspaceName NOTIFY userInfoChanged )
    Q_PROPERTY( bool hasInvitations READ hasInvitations NOTIFY userInfoChanged )

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

    void clear();
    void setFromJson( QJsonObject docObj );

    QString email() const;
    QString activeWorkspaceName() const;
    int activeWorkspaceId() const;
    QMap<int, QString> workspaces() const;
    QList<MerginInvitation> invitations() const;
    bool hasInvitations() const;

    void saveWorkspacesData();
    void loadWorkspacesData();

    int findActiveWorkspace( int preferredWorkspace = -1 );
    Q_INVOKABLE void setActiveWorkspace( int newWorkspace );

  signals:
    void userInfoChanged();
    void activeWorkspaceChanged();

  private:
    QString mEmail;
    QMap<int, QString> mWorkspaces;
    QList<MerginInvitation> mInvitations;
    int mActiveWorkspace = -1;
};

#endif // MERGINUSERINFO_H
