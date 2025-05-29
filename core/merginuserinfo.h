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

struct MerginInvitation
{
    Q_GADGET

  public:
    Q_PROPERTY( QString workspace MEMBER workspace )
    Q_PROPERTY( int workspaceId MEMBER workspaceId )
    Q_PROPERTY( QString uuid MEMBER uuid )
    Q_PROPERTY( QString role MEMBER role )
    Q_PROPERTY( QDateTime expiration MEMBER expiration )

    QString uuid;
    QString workspace;
    int workspaceId;
    QString role;
    QDateTime expiration;

    static MerginInvitation fromJsonObject( const QJsonObject &invitationInfo );
};

class MerginUserInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString nameAbbr READ nameAbbr NOTIFY userInfoChanged )
    Q_PROPERTY( QString name READ name NOTIFY userInfoChanged )
    Q_PROPERTY( QString email READ email NOTIFY userInfoChanged )
    Q_PROPERTY( QString username READ username NOTIFY userInfoChanged )
    Q_PROPERTY( QString activeWorkspaceName READ activeWorkspaceName NOTIFY activeWorkspaceChanged )
    Q_PROPERTY( int activeWorkspaceId READ activeWorkspaceId NOTIFY activeWorkspaceChanged )
    Q_PROPERTY( bool hasInvitations READ hasInvitations NOTIFY userInfoChanged )
    Q_PROPERTY( bool invitationsCount READ invitationsCount NOTIFY userInfoChanged )
    Q_PROPERTY( bool hasWorkspaces READ hasWorkspaces NOTIFY hasWorkspacesChanged )

  public:
    explicit MerginUserInfo( QObject *parent = nullptr );
    ~MerginUserInfo() = default;

    QString nameAbbr() const;
    QString name() const; // fullname, empty if not set on server
    QString email() const;
    QString username() const;

    int activeWorkspaceId() const;
    QString activeWorkspaceName() const;
    Q_INVOKABLE void setActiveWorkspace( int newWorkspaceId );

    bool hasWorkspaces() const;
    QMap<int, QString> workspaces() const;
    //! Updates workspaces cache with /v1/workspaces endpoint reponse
    void updateWorkspacesList( QMap<int, QString> workspaces );

    bool hasInvitations() const;
    int invitationsCount() const;
    Q_INVOKABLE QList<MerginInvitation> invitations() const;

    void clear(); // on logout
    void setFromJson( QJsonObject docObj );

    void saveData();
    void loadData();

  signals:
    void userInfoChanged();
    void activeWorkspaceChanged();
    void hasWorkspacesChanged();

  private:
    int findActiveWorkspace( int preferredWorkspace = -1 );

    QString mName;
    QString mNameAbbr;
    QString mEmail;
    QString mUsername;
    QMap<int, QString> mWorkspaces;
    QList<MerginInvitation> mInvitations;
    int mActiveWorkspace = -1;
};

#endif // MERGINUSERINFO_H
