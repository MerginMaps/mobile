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

#include <QDateTime>

#include "qgsproject.h"
#include "qgsvectorlayer.h"

constexpr qint64 MAX_DRAFT_AGE_SECS = 10 * 24 * 60 * 60; // 10 days

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

FeatureDraftController::DraftStage FeatureDraftController::draftStage() const
{
  return mDraftStage;
}

bool FeatureDraftController::draftIsExistingFeature() const
{
  return mDraftIsExistingFeature;
}

QString FeatureDraftController::draftFeatureTitle() const
{
  return mDraftFeatureTitle;
}

void FeatureDraftController::checkForDraft()
{
  const QString projectId = QgsProject::instance()->homePath();
  const FeatureDraft draft = FeatureDraftStorage::loadDraft( projectId );

  if ( draft.isEmpty() )
  {
    mCachedDraft = FeatureDraft();
    setDraft( false );
    return;
  }

  QgsVectorLayer *layer = resolveDraftLayer( draft );

  if ( !layer || !isDraftValid( draft, layer ) )
  {
    FeatureDraftStorage::clearDraft( projectId );
    mCachedDraft = FeatureDraft();
    setDraft( false );
    return;
  }

  mCachedDraft = draft;

  QString featureTitle;

  if ( draft.isExistingFeature() )
  {
    const QgsFeature feature = layer->getFeature( draft.featureId );
    featureTitle = InputUtils::featureTitle( FeatureLayerPair( feature, layer ), QgsProject::instance() );
  }

  setDraft( true, layer, toQmlStage( draft.stage ), draft.isExistingFeature(), featureTitle );
}

FeatureLayerPair FeatureDraftController::resumeDraft()
{
  if ( !mHasDraft )
    return {};

  QgsVectorLayer *layer = mDraftLayer;

  // re-validated against the cached draft - state may have changed since checkForDraft()
  if ( !layer || !isDraftValid( mCachedDraft, layer ) )
  {
    FeatureDraftStorage::clearDraft( QgsProject::instance()->homePath() );
    mCachedDraft = FeatureDraft();
    setDraft( false );
    return {};
  }

  const FeatureDraft &draft = mCachedDraft;

  FeatureLayerPair pair;

  if ( draft.isExistingFeature() )
  {
    // existing feature: start from the live one, then overlay the draft on top
    pair = FeatureLayerPair( layer->getFeature( draft.featureId ), layer );

    if ( !draft.geometry.isNull() )
    {
      QgsGeometry geometry = draft.geometry;
      pair.featureRef().setGeometry( geometry );

      // push into the layer too, so the map shows the resumed shape right away
      // instead of the stale committed one until the next vertex edit
      layer->startEditing();
      layer->changeGeometry( draft.featureId, geometry );
      layer->triggerRepaint();
    }
  }
  else
  {
    const QgsGeometry geometry = draft.geometry.isNull() ? InputUtils::emptyGeometry() : draft.geometry;
    pair = InputUtils::createFeatureLayerPair( layer, geometry, nullptr );
  }

  const QgsFields fields = layer->fields();

  for ( const FeatureDraftAttribute &attribute : draft.attributes )
  {
    const int fieldIndex = fields.indexOf( attribute.name );

    if ( fieldIndex >= 0 )
    {
      pair.featureRef().setAttribute( fieldIndex, attribute.value );
    }
  }

  // draft stays in storage - only the pending state (the prompt) is cleared here
  mCachedDraft = FeatureDraft();
  setDraft( false );

  return pair;
}

void FeatureDraftController::discardDraft()
{
  if ( !mHasDraft )
    return;

  FeatureDraftStorage::clearDraft( QgsProject::instance()->homePath() );
  mCachedDraft = FeatureDraft();
  setDraft( false );
}

QgsVectorLayer *FeatureDraftController::resolveDraftLayer( const FeatureDraft &draft )
{
  return qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayer( draft.layerId ) );
}

bool FeatureDraftController::isDraftValid( const FeatureDraft &draft, QgsVectorLayer *layer )
{
  if ( !draft.timestamp.isValid() || draft.timestamp.secsTo( QDateTime::currentDateTimeUtc() ) > MAX_DRAFT_AGE_SECS )
  {
    return false;
  }

  const QgsFields fields = layer->fields();

  for ( const FeatureDraftAttribute &attribute : draft.attributes )
  {
    const int fieldIndex = fields.indexOf( attribute.name );

    if ( fieldIndex < 0 )
    {
      return false; // field removed or renamed since the draft was written
    }

    if ( fields.at( fieldIndex ).typeName() != attribute.typeName )
    {
      return false; // field type changed since the draft was written
    }
  }

  if ( draft.isExistingFeature() && !layer->getFeature( draft.featureId ).isValid() )
  {
    return false; // the feature this draft was editing no longer exists
  }

  return true;
}

FeatureDraftController::DraftStage FeatureDraftController::toQmlStage( FeatureDraft::Stage stage )
{
  return stage == FeatureDraft::GeometryCapture ? GeometryCapture : AttributeForm;
}

void FeatureDraftController::setDraft( bool hasDraft, QgsVectorLayer *layer, DraftStage stage, bool isExistingFeature, const QString &featureTitle )
{
  const QString layerName = layer ? layer->name() : QString();

  if ( mHasDraft != hasDraft || mDraftLayer != layer || mDraftStage != stage || mDraftIsExistingFeature != isExistingFeature || mDraftFeatureTitle != featureTitle )
  {
    mHasDraft = hasDraft;
    mDraftLayer = layer;
    mDraftLayerName = layerName;
    mDraftStage = stage;
    mDraftIsExistingFeature = isExistingFeature;
    mDraftFeatureTitle = featureTitle;
    emit hasDraftChanged();
  }
}
