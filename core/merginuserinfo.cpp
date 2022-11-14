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
  mDiskUsage = 0;
  mStorageLimit = 0;

  mPreferredWorkspace = -1;
  mActiveWorkspace = -1;
  mWorkspaces.clear();

  emit userInfoChanged();
}

void MerginUserInfo::setFromJson( QJsonObject docObj )
{
  // parse profile data
  mEmail = docObj.value( QStringLiteral( "email" ) ).toString();
  mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toDouble();
  mStorageLimit = docObj.value( QStringLiteral( "storage" ) ).toDouble();

  if ( docObj.contains( QStringLiteral( "preferred_workspace" ) ) )
  {
    mPreferredWorkspace = docObj.value( QStringLiteral( "preferred_workspace" ) ).toInt();
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
  emit userInfoChanged();
}

QString MerginUserInfo::email() const
{
  return mEmail;
}

double MerginUserInfo::diskUsage() const
{
  return mDiskUsage;
}

double MerginUserInfo::storageLimit() const
{
  return mStorageLimit;
}

void MerginUserInfo::onStorageChanged( double storage )
{
  mStorageLimit = storage;
  emit userInfoChanged();
}

void MerginUserInfo::saveWorkspacesData()
{
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.setValue( "preferredWorkspace", mPreferredWorkspace );
  //settings.setValue( "workspaces", mWorkspaces );
  settings.endGroup();
}

void MerginUserInfo::findActiveWorkspace()
{
  if ( mWorkspaces.isEmpty() )
  {
    mActiveWorkspace = -1;
    mActiveWorkspaceName = "";
  }

  if ( mWorkspaces.count() == 1 )
  {
    mActiveWorkspace = mWorkspaces.firstKey();
    mActiveWorkspaceName = mWorkspaces.value( mActiveWorkspace );
  }
  else
  {
    QSettings settings;
    settings.beginGroup( "Input/" );
    int lastUsedWorkspace = settings.value( "lastUsedWorkspace", -1 ).toInt();
    settings.endGroup();

    if ( mWorkspaces.contains( lastUsedWorkspace ) )
    {
      mActiveWorkspace = lastUsedWorkspace;
      mActiveWorkspaceName = mWorkspaces.value( mActiveWorkspace );
    }
    else
    {
      if ( mPreferredWorkspace >= 0 )
      {
        mActiveWorkspace = mPreferredWorkspace;
        mActiveWorkspaceName = mWorkspaces.value( mActiveWorkspace );
      }
      else
      {
        mActiveWorkspace = mWorkspaces.firstKey();
        mActiveWorkspaceName = mWorkspaces.value( mActiveWorkspace );
      }
    }
  }

  saveLastActiveWorkspace();
}

void MerginUserInfo::saveLastActiveWorkspace()
{
  QSettings settings;
  settings.beginGroup( "Input/" );
  settings.setValue( "lastUsedWorkspace", mActiveWorkspace );
  settings.endGroup();
}
