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
#include "merginprojectmetadata.h"

#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>

const QString QSETTINGS_DRAFTS_GROUP_NAME = QStringLiteral( "featureDrafts" );

QHash<QString, QString> FeatureDraftStorage::sProjectKeyCache;

void FeatureDraftStorage::saveDraft( const QString &projectDir, const FeatureDraft &draft )
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  settings.setValue( settingsKey( projectDir ), QJsonDocument( toJson( draft ) ).toJson( QJsonDocument::Compact ) );
  settings.endGroup();

  // explicit flush - a draft must survive a crash, not just a normal exit
  settings.sync();
}

FeatureDraft FeatureDraftStorage::loadDraft( const QString &projectDir )
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  const QByteArray raw = settings.value( settingsKey( projectDir ) ).toByteArray();
  settings.endGroup();

  if ( raw.isEmpty() )
  {
    return {};
  }

  return fromJson( QJsonDocument::fromJson( raw ).object() );
}

void FeatureDraftStorage::clearDraft( const QString &projectDir )
{
  QSettings settings;
  settings.beginGroup( CoreUtils::QSETTINGS_APP_GROUP_NAME );
  settings.remove( settingsKey( projectDir ) );
  settings.endGroup();
  settings.sync();
}

void FeatureDraftStorage::clearCache()
{
  sProjectKeyCache.clear();
}

QString FeatureDraftStorage::projectKey( const QString &projectDir )
{
  const auto cached = sProjectKeyCache.constFind( projectDir );
  if ( cached != sProjectKeyCache.constEnd() )
  {
    return cached.value();
  }

  const QString merginId = MerginProjectMetadata::fromCachedJson( CoreUtils::getProjectMetadataPath( projectDir ) ).projectId;
  const QString key = merginId.isEmpty() ? projectDir : merginId;

  sProjectKeyCache.insert( projectDir, key );
  return key;
}

QString FeatureDraftStorage::settingsKey( const QString &projectDir )
{
  return QSETTINGS_DRAFTS_GROUP_NAME + "/" + projectKey( projectDir );
}

QJsonObject FeatureDraftStorage::toJson( const FeatureDraft &draft )
{
  QJsonObject json;
  json[ QStringLiteral( "layerId" ) ] = draft.layerId;
  json[ QStringLiteral( "stage" ) ] = draft.stage == FeatureDraft::GeometryCapture
                                       ? QStringLiteral( "geometryCapture" ) : QStringLiteral( "attributeForm" );
  json[ QStringLiteral( "timestamp" ) ] = draft.timestamp.toString( Qt::ISODate );

  if ( !draft.geometry.isNull() )
  {
    json[ QStringLiteral( "geometry" ) ] = draft.geometry.asWkt();
  }

  if ( draft.isExistingFeature() )
  {
    json[ QStringLiteral( "featureId" ) ] = QJsonValue( draft.featureId );
  }

  QJsonArray attributes;
  for ( const FeatureDraftAttribute &attribute : draft.attributes )
  {
    QJsonObject attributeJson;
    attributeJson[ QStringLiteral( "name" ) ] = attribute.name;
    attributeJson[ QStringLiteral( "type" ) ] = attribute.typeName;
    attributeJson[ QStringLiteral( "value" ) ] = QJsonValue::fromVariant( attribute.value );
    attributes.append( attributeJson );
  }
  json[ QStringLiteral( "attributes" ) ] = attributes;

  return json;
}

FeatureDraft FeatureDraftStorage::fromJson( const QJsonObject &json )
{
  if ( json.isEmpty() )
  {
    return {};
  }

  FeatureDraft draft;
  draft.layerId = json.value( QStringLiteral( "layerId" ) ).toString();
  draft.stage = json.value( QStringLiteral( "stage" ) ).toString() == QLatin1String( "geometryCapture" )
                ? FeatureDraft::GeometryCapture : FeatureDraft::AttributeForm;
  draft.timestamp = QDateTime::fromString( json.value( QStringLiteral( "timestamp" ) ).toString(), Qt::ISODate );

  const QString wkt = json.value( QStringLiteral( "geometry" ) ).toString();
  if ( !wkt.isEmpty() )
  {
    draft.geometry = QgsGeometry::fromWkt( wkt );
  }

  if ( json.contains( QStringLiteral( "featureId" ) ) )
  {
    draft.featureId = json.value( QStringLiteral( "featureId" ) ).toVariant().toLongLong();
  }

  const QJsonArray attributes = json.value( QStringLiteral( "attributes" ) ).toArray();
  for ( const auto &attributeValue : attributes )
  {
    const QJsonObject attributeJson = attributeValue.toObject();
    draft.attributes.append( {
      attributeJson.value( QStringLiteral( "name" ) ).toString(),
      attributeJson.value( QStringLiteral( "type" ) ).toString(),
      attributeJson.value( QStringLiteral( "value" ) ).toVariant()
    } );
  }

  return draft;
}
