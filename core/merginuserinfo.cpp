/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QJsonArray>
#include <QSettings>

#include "merginuserinfo.h"
#include "coreutils.h"

MerginInvitation MerginInvitation::fromJsonObject( const QJsonObject &invitationInfo )
{
  MerginInvitation merginInvitation;
  merginInvitation.uuid = invitationInfo.value( QStringLiteral( "uuid" ) ).toString();
  merginInvitation.workspace = invitationInfo.value( QStringLiteral( "workspace" ) ).toString();
  merginInvitation.workspaceId = invitationInfo.value( QStringLiteral( "workspace_id" ) ).toInt();
  merginInvitation.role = invitationInfo.value( QStringLiteral( "role" ) ).toString();
  merginInvitation.expiration = invitationInfo.value( QStringLiteral( "expire" ) ).toVariant().toDateTime();

  return merginInvitation;
}

MerginUserInfo::MerginUserInfo( QObject *parent )
  : QObject( parent )
{
}

QString MerginUserInfo::nameAbbr() const
{
  return mNameAbbr;
}

QString MerginUserInfo::name() const
{
  return mName;
}

QString MerginUserInfo::email() const
{
  return mEmail;
}

QString MerginUserInfo::username() const
{
  return mUsername;
}

int MerginUserInfo::activeWorkspaceId() const
{
  return mActiveWorkspace;
}

QString MerginUserInfo::activeWorkspaceName() const
{
  return mWorkspaces.value( mActiveWorkspace );
}

void MerginUserInfo::setActiveWorkspace( int newWorkspace )
{
  if ( mActiveWorkspace == newWorkspace )
  {
    return;
  }

  mActiveWorkspace = newWorkspace;

  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.setValue( "lastUsedWorkspace", mActiveWorkspace );
  settings.endGroup();

  const QString logMessage = QStringLiteral( "Switched to workspace '%1' with ID %2" )
                             .arg( activeWorkspaceName() )
                             .arg( newWorkspace );

  CoreUtils::log( QStringLiteral( "Workspace Switch" ), logMessage );

  emit activeWorkspaceChanged();
}

bool MerginUserInfo::hasWorkspaces() const
{
  return mWorkspaces.count() > 0;
}

QMap<int, QString> MerginUserInfo::workspaces() const
{
  return mWorkspaces;
}

void MerginUserInfo::updateWorkspacesList( QMap<int, QString> workspaces )
{
  if ( mWorkspaces == workspaces )
  {
    return;
  }

  mWorkspaces = workspaces;

  saveData();

  emit hasWorkspacesChanged();
  emit userInfoChanged();
}

bool MerginUserInfo::hasInvitations() const
{
  return mInvitations.count() > 0;
}

int MerginUserInfo::invitationsCount() const
{
  return mInvitations.count();
}

QList<MerginInvitation> MerginUserInfo::invitations() const
{
  return mInvitations;
}

void MerginUserInfo::clear()
{
  mName = "";
  mNameAbbr = "";
  mEmail = "";
  mUsername = "";
  mActiveWorkspace = -1;
  mWorkspaces.clear();
  mInvitations.clear();

  QSettings settings;
  settings.beginGroup( "Input/" );

  settings.remove( "name" );
  settings.remove( "email" );
  settings.remove( "username" );
  settings.remove( "workspaces" );
  settings.setValue( "lastUsedWorkspace", -1 );

  settings.endGroup();

  emit activeWorkspaceChanged();
  emit hasWorkspacesChanged();
  emit userInfoChanged();
}

void MerginUserInfo::setFromJson( QJsonObject docObj )
{
  // parse profile data
  mEmail = docObj.value( QStringLiteral( "email" ) ).toString();
  mName = docObj.value( QStringLiteral( "name" ) ).toString();
  mUsername = docObj.value( QStringLiteral( "username" ) ).toString();
  mNameAbbr = CoreUtils::nameAbbr( mName, mEmail );

  int preferredWorkspace = -1;
  if ( docObj.contains( QStringLiteral( "preferred_workspace" ) ) )
  {
    preferredWorkspace = docObj.value( QStringLiteral( "preferred_workspace" ) ).toInt();
  }

  mWorkspaces.clear();
  if ( docObj.contains( QStringLiteral( "workspaces" ) ) )
  {
    QJsonArray workspaces = docObj.value( "workspaces" ).toArray();
    for ( auto it = workspaces.constBegin(); it != workspaces.constEnd(); ++it )
    {
      QJsonObject ws = it->toObject();
      mWorkspaces.insert( ws.value( QStringLiteral( "id" ) ).toInt(), ws.value( QStringLiteral( "name" ) ).toString() );
    }
  }

  mInvitations.clear();
  if ( docObj.contains( QStringLiteral( "invitations" ) ) )
  {
    QJsonArray invitations = docObj.value( "invitations" ).toArray();
    for ( auto it = invitations.constBegin(); it != invitations.constEnd(); ++it )
    {
      mInvitations.append( MerginInvitation::fromJsonObject( it->toObject() ) );
    }
  }

  int workspace = findActiveWorkspace( preferredWorkspace );
  setActiveWorkspace( workspace );

  saveData();

  emit userInfoChanged();
  emit hasWorkspacesChanged();
}

void MerginUserInfo::saveData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );

  settings.setValue( "name", mName );
  settings.setValue( "email", mEmail );
  settings.setValue( "username", mUsername );

  settings.beginGroup( "workspaces" );
  QMap<int, QString>::const_iterator it = mWorkspaces.constBegin();
  while ( it != mWorkspaces.constEnd() )
  {
    settings.setValue( QString::number( it.key() ), it.value() );
    ++it;
  }
  settings.endGroup();

  settings.setValue( "lastUsedWorkspace", mActiveWorkspace );

  settings.endGroup();
}

void MerginUserInfo::loadData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );

  mName = settings.value( "name", QString() ).toString();
  mEmail = settings.value( "email", QString() ).toString();
  mUsername = settings.value( "username", QString() ).toString();

  settings.beginGroup( "workspaces" );

  QStringList keys = settings.childKeys();
  for ( const QString &key : std::as_const( keys ) )
  {
    mWorkspaces[ key.toInt() ] = settings.value( key ).toString();
  }

  settings.endGroup();

  mActiveWorkspace = findActiveWorkspace();

  settings.endGroup();

  emit hasWorkspacesChanged();
  emit activeWorkspaceChanged();
  emit userInfoChanged();
}

int MerginUserInfo::findActiveWorkspace( int preferredWorkspace )
{
  int workspace = -1;

  if ( mWorkspaces.isEmpty() )
  {
    workspace = -1;
  }
  else if ( mWorkspaces.count() == 1 )
  {
    workspace = mWorkspaces.firstKey();
  }
  else if ( mWorkspaces.count() > 1 )
  {
    QSettings settings;
    settings.beginGroup( "Input/" );
    int lastUsedWorkspace = settings.value( "lastUsedWorkspace", -1 ).toInt();
    settings.endGroup();

    if ( mWorkspaces.contains( lastUsedWorkspace ) )
    {
      workspace = lastUsedWorkspace;
    }
    else
    {
      if ( preferredWorkspace >= 0 )
      {
        workspace = preferredWorkspace;
      }
      else
      {
        workspace = mWorkspaces.firstKey();
      }
    }
  }

  return workspace;
}
