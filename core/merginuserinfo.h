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

struct MerginInvitation
{
  QString uuid;
  QString workspace;
  QString role;
  QDateTime expiration;

  static MerginInvitation fromJsonObject( const QJsonObject &invitationInfo );
};


class MerginUserInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString name READ name NOTIFY userInfoChanged )
    Q_PROPERTY( QString email READ email NOTIFY userInfoChanged )
    Q_PROPERTY( QString activeWorkspaceName READ activeWorkspaceName NOTIFY activeWorkspaceChanged )
    Q_PROPERTY( int activeWorkspaceId READ activeWorkspaceId NOTIFY activeWorkspaceChanged )
    Q_PROPERTY( bool hasInvitations READ hasInvitations NOTIFY userInfoChanged )
    Q_PROPERTY( bool hasWorkspaces READ hasWorkspaces NOTIFY hasWorkspacesChanged )
    Q_PROPERTY( bool hasMoreThanOneWorkspace READ hasMoreThanOneWorkspace NOTIFY hasMoreThanOneWorkspaceChanged )

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

    void clear();
    void setFromJson( QJsonObject docObj );

    QString name() const;
    QString email() const;
    QString activeWorkspaceName() const;
    int activeWorkspaceId() const;
    QMap<int, QString> workspaces() const;
    QList<MerginInvitation> invitations() const;
    bool hasInvitations() const;
    bool hasWorkspaces() const;

    void saveWorkspacesData();
    void loadWorkspacesData();
    void clearCachedWorkspacesInfo();

    int findActiveWorkspace( int preferredWorkspace = -1 );
    Q_INVOKABLE void setActiveWorkspace( int newWorkspace );
    void setWorkspaces( QMap<int, QString> workspaces );

    bool hasMoreThanOneWorkspace() const;

  signals:
    void userInfoChanged();
    void activeWorkspaceChanged();
    void hasWorkspacesChanged();
    void hasMoreThanOneWorkspaceChanged();

  private:
    QString mName;
    QString mEmail;
    QMap<int, QString> mWorkspaces;
    QList<MerginInvitation> mInvitations;
    int mActiveWorkspace = -1;
    bool mHasMoreThanOneWorkspace;
};

#endif // MERGINUSERINFO_H
