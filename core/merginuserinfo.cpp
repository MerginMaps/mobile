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

MerginUserInfo::MerginUserInfo( QObject *parent )
  : QObject( parent )
{
  clear();
}

void MerginUserInfo::clear()
{
  mEmail = "";
  mActiveWorkspace = -1;
  mWorkspaces.clear();

  saveWorkspacesData();

  emit userInfoChanged();
}

void MerginUserInfo::setFromJson( QJsonObject docObj )
{
  blockSignals( true );
  clear();
  blockSignals( false );

  // parse profile data
  mEmail = docObj.value( QStringLiteral( "email" ) ).toString();

  int preferredWorkspace = -1;
  if ( docObj.contains( QStringLiteral( "preferred_workspace" ) ) )
  {
    preferredWorkspace = docObj.value( QStringLiteral( "preferred_workspace" ) ).toInt();
  }

  if ( docObj.contains( QStringLiteral( "workspaces" ) ) )
  {
    QJsonArray workspaces = docObj.value( "workspaces" ).toArray();
    for ( auto it = workspaces.constBegin(); it != workspaces.constEnd(); ++it )
    {
      QJsonObject ws = it->toObject();
      mWorkspaces.insert( ws.value( QStringLiteral( "id" ) ).toInt(), ws.value( QStringLiteral( "name" ) ).toString() );
    }
  }

  saveWorkspacesData();
  findActiveWorkspace( preferredWorkspace );
  emit userInfoChanged();
}

QString MerginUserInfo::email() const
{
  return mEmail;
}

QString MerginUserInfo::activeWorkspaceName() const
{
  return mWorkspaces.value( mActiveWorkspace );
}

int MerginUserInfo::activeWorkspaceId() const
{
  return mActiveWorkspace;
}

QMap<int, QString> MerginUserInfo::workspaces() const
{
  return mWorkspaces;
}

void MerginUserInfo::saveWorkspacesData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );

  settings.beginGroup( "workspaces" );
  QMap<int, QString>::const_iterator it = mWorkspaces.constBegin();
  while ( it != mWorkspaces.constEnd() )
  {
    settings.setValue( QString::number( it.key() ), it.value() );
    ++it;
  }
  settings.endGroup();

  settings.endGroup();
}

void MerginUserInfo::loadWorkspacesData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );
  mActiveWorkspace = settings.value( "lastUsedWorkspace", -1 ).toInt();

  settings.beginGroup( "workspaces" );
  QStringList keys = settings.childKeys();
  for ( const QString &key : keys )
  {
    mWorkspaces[ key.toInt() ] = settings.value( key ).toString();
  }
  settings.endGroup();

  settings.endGroup();

  int workspace = findActiveWorkspace();
  setActiveWorkspace( workspace );
}

int MerginUserInfo::findActiveWorkspace( int preferredWorkspace )
{
  int workspace;
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

  setActiveWorkspace( workspace );
  return workspace;
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

  emit activeWorkspaceChanged();
  emit userInfoChanged();
}
