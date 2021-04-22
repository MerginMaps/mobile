/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

  emit userInfoChanged();
}

void MerginUserInfo::setFromJson( QJsonObject docObj )
{
  // parse profile data
  mEmail = docObj.value( QStringLiteral( "email" ) ).toString();
  mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toDouble();
  mStorageLimit = docObj.value( QStringLiteral( "storage" ) ).toDouble();

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
