/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "featuredraftstorage.h"
#include "coreutils.h"

#include <QSettings>
#include <QJsonDocument>

const QString FeatureDraftStorage::QSETTINGS_DRAFTS_GROUP_NAME = QStringLiteral( "featureDrafts" );

void FeatureDraftStorage::saveDraft( const QString &projectId, const QJsonObject &draft )
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  settings.setValue( settingsKey( projectId ), QJsonDocument( draft ).toJson( QJsonDocument::Compact ) );
  settings.endGroup();

  // explicit flush - a draft must survive a crash, not just a normal exit
  settings.sync();
}

QJsonObject FeatureDraftStorage::loadDraft( const QString &projectId )
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  const QByteArray raw = settings.value( settingsKey( projectId ) ).toByteArray();
  settings.endGroup();

  if ( raw.isEmpty() )
  {
    return QJsonObject();
  }

  return QJsonDocument::fromJson( raw ).object();
}

void FeatureDraftStorage::clearDraft( const QString &projectId )
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  settings.remove( settingsKey( projectId ) );
  settings.endGroup();
  settings.sync();
}

QString FeatureDraftStorage::settingsKey( const QString &projectId )
{
  return QSETTINGS_DRAFTS_GROUP_NAME + "/" + projectId;
}
