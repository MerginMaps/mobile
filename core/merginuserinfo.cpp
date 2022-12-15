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
  clear();

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
  findActiveWorkspace();
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

QString MerginUserInfo::activeWorkspace() const
{
  return mActiveWorkspaceName;
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
  mPreferredWorkspace = settings.value( "preferredWorkspace", -1 ).toInt();
  mActiveWorkspace = settings.value( "lastUsedWorkspace", -1 ).toInt();

  settings.beginGroup( "workspaces" );
  QStringList keys = settings.childKeys();
  for ( const QString &key : keys )
  {
    mWorkspaces[ key.toInt() ] = settings.value( key ).toString();
  }
  settings.endGroup();

  settings.endGroup();

  findActiveWorkspace();
}

void MerginUserInfo::findActiveWorkspace()
{
  if ( mWorkspaces.isEmpty() )
  {
    mActiveWorkspace = -1;
    mActiveWorkspaceName = "";
  }
  else if ( mWorkspaces.count() == 1 )
  {
    mActiveWorkspace = mWorkspaces.firstKey();
    mActiveWorkspaceName = mWorkspaces.value( mActiveWorkspace );
  }
  else if ( mWorkspaces.count() > 1 )
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
