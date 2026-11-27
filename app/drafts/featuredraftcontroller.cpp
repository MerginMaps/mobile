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
#include "coreutils.h"

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
  return mCachedDraft.isExistingFeature();
}

QString FeatureDraftController::draftFeatureTitle() const
{
  return mDraftFeatureTitle;
}

void FeatureDraftController::checkForDraft( const QString &projectId, QgsProject *project )
{
  mProjectId = projectId;
  mProject = project;

  if ( mProjectId.isEmpty() || !mProject )
  {
    mCachedDraft = FeatureDraft();
    setDraft( false );
    return;
  }

  const FeatureDraft draft = FeatureDraftStorage::loadDraft( mProjectId );

  if ( draft.isEmpty() )
  {
    mCachedDraft = FeatureDraft();
    setDraft( false );
    return;
  }

  QgsVectorLayer *layer = resolveDraftLayer( draft, mProject );

  if ( !layer || !isDraftValid( draft, layer ) )
  {
    clearDraft(); // also resets mCachedDraft and the pending state
    return;
  }

  mCachedDraft = draft;

  QString featureTitle;

  if ( draft.isExistingFeature() )
  {
    const QgsFeature feature = layer->getFeature( draft.featureId );
    featureTitle = InputUtils::featureTitle( FeatureLayerPair( feature, layer ), mProject );
  }

  setDraft( true, layer, draft.stage, featureTitle );
}

FeatureLayerPair FeatureDraftController::applyDraft()
{
  if ( !mHasDraft )
    return {};

  // re-resolved and re-validated here: a sync-triggered reload can replace the project's layers
  // or change/remove the drafted feature without checkForDraft() running again to catch it
  QgsVectorLayer *layer = resolveDraftLayer( mCachedDraft, mProject );

  if ( !layer || !isDraftValid( mCachedDraft, layer ) )
  {
    clearDraft(); // also resets mCachedDraft and the pending state
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

      // apply to the layer too, so the map shows the drafted geometry;
      // startEditing() returns false if the layer is already editable
      if ( layer->isEditable() || layer->startEditing() )
      {
        layer->changeGeometry( draft.featureId, geometry );
        layer->triggerRepaint();
      }
      else
      {
        CoreUtils::log( QStringLiteral( "Feature draft" ), QStringLiteral( "Could not start editing layer %1 to apply drafted geometry" ).arg( layer->name() ) );
      }
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

  // only the prompt is dismissed here; AttributeController/RecordingMapTool call clearDraft()
  // once the edit actually concludes (save, cancel or delete)
  setDraft( false );

  return pair;
}

void FeatureDraftController::discardDraft()
{
  if ( !mHasDraft )
    return;

  clearDraft();
}

void FeatureDraftController::saveDraft( const FeatureDraft &draft ) const
{
  if ( mProjectId.isEmpty() )
    return;

  FeatureDraftStorage::saveDraft( mProjectId, draft );
}

void FeatureDraftController::clearDraft()
{
  if ( mProjectId.isEmpty() )
    return;

  FeatureDraftStorage::clearDraft( mProjectId );
  mCachedDraft = FeatureDraft();
  setDraft( false );
}

QgsVectorLayer *FeatureDraftController::resolveDraftLayer( const FeatureDraft &draft, QgsProject *project )
{
  if ( !project )
    return nullptr;

  return qobject_cast<QgsVectorLayer *>( project->mapLayer( draft.layerId ) );
}

bool FeatureDraftController::isDraftValid( const FeatureDraft &draft, QgsVectorLayer *layer )
{
  if ( !draft.timestamp.isValid() || draft.timestamp.secsTo( QDateTime::currentDateTimeUtc() ) > MAX_DRAFT_AGE_SECS )
  {
    return false;
  }

  if ( draft.isExistingFeature() && !layer->getFeature( draft.featureId ).isValid() )
  {
    return false; // the feature this draft was editing no longer exists
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

  return true;
}

void FeatureDraftController::setDraft( bool hasDraft, QgsVectorLayer *layer, DraftStage stage, const QString &featureTitle )
{
  if ( mHasDraft != hasDraft || mDraftLayer != layer || mDraftStage != stage || mDraftFeatureTitle != featureTitle )
  {
    mHasDraft = hasDraft;
    mDraftLayer = layer;
    mDraftStage = stage;
    mDraftFeatureTitle = featureTitle;
    emit hasDraftChanged();
  }
}
