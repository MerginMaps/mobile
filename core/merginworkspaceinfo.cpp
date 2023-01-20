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

#include "merginworkspaceinfo.h"

MerginWorkspaceInfo::MerginWorkspaceInfo( QObject *parent )
  : QObject( parent )
{

}

void MerginWorkspaceInfo::clear()
{
  mDiskUsage = 0;
  mStorageLimit = 0;
  mRole = "";

  emit workspaceInfoChanged();
}

void MerginWorkspaceInfo::setFromJson( QJsonObject docObj )
{
  // parse storage data
  mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toDouble();
  mStorageLimit = docObj.value( QStringLiteral( "storage" ) ).toDouble();

  // role is not present if this is the old server
  if ( docObj.contains( QStringLiteral( "role" ) ) )
  {
    mRole = docObj.value( QStringLiteral( "role" ) ).toString();
  }

  emit workspaceInfoChanged();
}

double MerginWorkspaceInfo::diskUsage() const
{
  return mDiskUsage;
}

double MerginWorkspaceInfo::storageLimit() const
{
  return mStorageLimit;
}

QString MerginWorkspaceInfo::role() const
{
  return mRole;
}
