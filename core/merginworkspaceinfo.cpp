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
  clearService();

  emit workspaceInfoChanged();
}

void MerginWorkspaceInfo::clearService()
{
  mServiceState = ServiceState::Unknown;

  emit workspaceInfoChanged();
}

void MerginWorkspaceInfo::setFromJson( QJsonObject docObj )
{
  // parse storage data
  mDiskUsage = docObj.value( QStringLiteral( "disk_usage" ) ).toDouble(); // bytes
  mStorageLimit = docObj.value( QStringLiteral( "storage" ) ).toDouble();

  // role is not present if this is the old server
  if ( docObj.contains( QStringLiteral( "role" ) ) )
  {
    mRole = docObj.value( QStringLiteral( "role" ) ).toString();
  }

  emit workspaceInfoChanged();
}

void MerginWorkspaceInfo::setServiceFromJson( QJsonObject docObj )
{
  if ( !docObj.contains( QStringLiteral( "service" ) ) )
    return;  

  const QJsonObject serviceObj = docObj.value( QStringLiteral( "service" ) ).toObject();

  if ( !serviceObj.contains( QStringLiteral( "state" ) ) )
    return;

  const QString newStateRaw = serviceObj.value( QStringLiteral( "state" ) ).toString();
  ServiceState newState = mServiceState;

  if ( newStateRaw == QStringLiteral( "active" ) )
  {
    newState = ServiceState::Active;
  }
  else if ( newStateRaw == QStringLiteral( "expired" ) )
  {
    newState = ServiceState::Expired;
  }
  else if ( newStateRaw == QStringLiteral( "payment_profile_issues" ) )
  {
    newState = ServiceState::PaymentProfileIssues;
  }
  else if ( newStateRaw == QStringLiteral( "device_share_policy_violation" ) )
  {
    newState = ServiceState::DeviceSharePolicyViolation;
  }
  else
  {
    newState = ServiceState::Unknown;
  }

  if ( newState != mServiceState )
  {
    mServiceState = newState;
    emit workspaceInfoChanged();
  }
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

MerginWorkspaceInfo::ServiceState MerginWorkspaceInfo::serviceState() const
{
  return mServiceState;
}
