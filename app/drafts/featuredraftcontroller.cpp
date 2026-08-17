/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "featuredraftcontroller.h"
#include "featuredraftstorage.h"
#include "inpututils.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>

#include "qgsproject.h"
#include "qgsvectorlayer.h"

namespace
{
  constexpr qint64 MAX_DRAFT_AGE_SECS = 10 * 24 * 60 * 60; // 10 days
}

FeatureDraftController::FeatureDraftController( QObject *parent )
  : QObject( parent )
{
}

bool FeatureDraftController::hasDraft() const
{
  return mHasDraft;
}

QString FeatureDraftController::draftLayerName() const
{
  return mDraftLayerName;
}

QgsVectorLayer *FeatureDraftController::draftLayer() const
{
  return mDraftLayer;
}

QString FeatureDraftController::draftStage() const
{
  return mDraftStage;
}

bool FeatureDraftController::draftIsEdit() const
{
  return mDraftIsEdit;
}

QString FeatureDraftController::draftFeatureTitle() const
{
  return mDraftFeatureTitle;
}

void FeatureDraftController::checkForDraft()
{
  const QString projectId = QgsProject::instance()->homePath();
  const QJsonObject draft = FeatureDraftStorage::loadDraft( projectId );

  if ( draft.isEmpty() )
  {
    setDraft( false );
    return;
  }

  QgsVectorLayer *layer = resolveDraftLayer( draft );

  if ( !layer || !isDraftValid( draft, layer ) )
  {
    FeatureDraftStorage::clearDraft( projectId );
    setDraft( false );
    return;
  }

  const bool isEdit = draft.contains( QStringLiteral( "featureId" ) );
  QString featureTitle;

  if ( isEdit )
  {
    const QgsFeatureId featureId = draft.value( QStringLiteral( "featureId" ) ).toVariant().toLongLong();
    const QgsFeature feature = layer->getFeature( featureId );
    featureTitle = InputUtils::featureTitle( FeatureLayerPair( feature, layer ), QgsProject::instance() );
  }

  setDraft( true, layer, draft.value( QStringLiteral( "stage" ) ).toString(), isEdit, featureTitle );
}

FeatureLayerPair FeatureDraftController::resumeDraft()
{
  if ( !mHasDraft )
    return FeatureLayerPair();

  const QString projectId = QgsProject::instance()->homePath();
  const QJsonObject draft = FeatureDraftStorage::loadDraft( projectId );
  QgsVectorLayer *layer = resolveDraftLayer( draft );

  if ( !layer || !isDraftValid( draft, layer ) )
  {
    // re-validated here too - time passed since the draft was detected
    FeatureDraftStorage::clearDraft( projectId );
    setDraft( false );
    return FeatureLayerPair();
  }

  FeatureLayerPair pair;

  if ( draft.contains( QStringLiteral( "featureId" ) ) )
  {
    // existing feature: start from the live one, then overlay the draft on top
    const QgsFeatureId featureId = draft.value( QStringLiteral( "featureId" ) ).toVariant().toLongLong();
    pair = FeatureLayerPair( layer->getFeature( featureId ), layer );

    const QString wkt = draft.value( QStringLiteral( "geometry" ) ).toString();
    if ( !wkt.isEmpty() )
    {
      QgsGeometry geometry = QgsGeometry::fromWkt( wkt );
      pair.featureRef().setGeometry( geometry );

      // push into the layer too, so the map shows the resumed shape right away
      // instead of the stale committed one until the next vertex edit
      layer->startEditing();
      layer->changeGeometry( featureId, geometry );
      layer->triggerRepaint();
    }
  }
  else
  {
    pair = InputUtils::createFeatureLayerPair( layer, InputUtils::emptyGeometry(), nullptr );

    const QString wkt = draft.value( QStringLiteral( "geometry" ) ).toString();
    if ( !wkt.isEmpty() )
    {
      pair.featureRef().setGeometry( QgsGeometry::fromWkt( wkt ) );
    }
  }

  const QgsFields fields = layer->fields();
  const QJsonArray attributes = draft.value( QStringLiteral( "attributes" ) ).toArray();

  for ( const QJsonValue &attributeValue : attributes )
  {
    const QJsonObject attribute = attributeValue.toObject();
    const int fieldIndex = fields.indexOf( attribute.value( QStringLiteral( "name" ) ).toString() );

    if ( fieldIndex >= 0 )
    {
      pair.featureRef().setAttribute( fieldIndex, attribute.value( QStringLiteral( "value" ) ).toVariant() );
    }
  }

  // draft stays in storage - only the pending state (the prompt) is cleared here
  setDraft( false );

  return pair;
}

QgsGeometry FeatureDraftController::resumeGeometryDraft()
{
  if ( !mHasDraft )
    return QgsGeometry();

  const QString projectId = QgsProject::instance()->homePath();
  const QJsonObject draft = FeatureDraftStorage::loadDraft( projectId );
  QgsVectorLayer *layer = resolveDraftLayer( draft );

  if ( !layer || !isDraftValid( draft, layer ) )
  {
    FeatureDraftStorage::clearDraft( projectId );
    setDraft( false );
    return QgsGeometry();
  }

  const QString wkt = draft.value( QStringLiteral( "geometry" ) ).toString();

  // draft stays in storage - only the pending state (the prompt) is cleared here
  setDraft( false );

  if ( wkt.isEmpty() )
    return QgsGeometry();

  return QgsGeometry::fromWkt( wkt );
}

void FeatureDraftController::discardDraft()
{
  if ( !mHasDraft )
    return;

  FeatureDraftStorage::clearDraft( QgsProject::instance()->homePath() );
  setDraft( false );
}

QgsVectorLayer *FeatureDraftController::resolveDraftLayer( const QJsonObject &draft ) const
{
  const QString layerId = draft.value( QStringLiteral( "layerId" ) ).toString();
  return qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayer( layerId ) );
}

bool FeatureDraftController::isDraftValid( const QJsonObject &draft, QgsVectorLayer *layer ) const
{
  const QDateTime timestamp = QDateTime::fromString( draft.value( QStringLiteral( "timestamp" ) ).toString(), Qt::ISODate );

  if ( !timestamp.isValid() || timestamp.secsTo( QDateTime::currentDateTimeUtc() ) > MAX_DRAFT_AGE_SECS )
  {
    return false;
  }

  const QgsFields fields = layer->fields();
  const QJsonArray attributes = draft.value( QStringLiteral( "attributes" ) ).toArray();

  for ( const QJsonValue &attributeValue : attributes )
  {
    const QJsonObject attribute = attributeValue.toObject();
    const int fieldIndex = fields.indexOf( attribute.value( QStringLiteral( "name" ) ).toString() );

    if ( fieldIndex < 0 )
    {
      return false; // field removed or renamed since the draft was written
    }

    if ( fields.at( fieldIndex ).typeName() != attribute.value( QStringLiteral( "type" ) ).toString() )
    {
      return false; // field type changed since the draft was written
    }
  }

  if ( draft.contains( QStringLiteral( "featureId" ) ) )
  {
    const QgsFeatureId featureId = draft.value( QStringLiteral( "featureId" ) ).toVariant().toLongLong();
    if ( !layer->getFeature( featureId ).isValid() )
    {
      return false; // the feature this draft was editing no longer exists
    }
  }

  return true;
}

void FeatureDraftController::setDraft( bool hasDraft, QgsVectorLayer *layer, const QString &stage, bool isEdit, const QString &featureTitle )
{
  const QString layerName = layer ? layer->name() : QString();

  if ( mHasDraft != hasDraft || mDraftLayer != layer || mDraftStage != stage || mDraftIsEdit != isEdit || mDraftFeatureTitle != featureTitle )
  {
    mHasDraft = hasDraft;
    mDraftLayer = layer;
    mDraftLayerName = layerName;
    mDraftStage = stage;
    mDraftIsEdit = isEdit;
    mDraftFeatureTitle = featureTitle;
    emit hasDraftChanged();
  }
}
