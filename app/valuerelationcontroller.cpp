/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "valuerelationcontroller.h"
#include "coreutils.h"

#include "qgsproject.h"
#include "qgsfeedback.h"
#include "qgsvaluerelationfieldformatter.h"
#include "qgsexpressioncontextutils.h"
#include "qgsvectorlayerfeatureiterator.h"

#include <QtConcurrentRun>

ValueRelationController::ValueRelationController( QObject *parent )
  : QObject( parent )
{
  connect( &mLookupWatcher, &QFutureWatcher<QgsFeatureList>::finished,
           this, &ValueRelationController::onLookupFinished );
}

ValueRelationController::~ValueRelationController()
{
  // Cancel any in-flight fetch before the watcher and feedback are destroyed.
  if ( auto fb = mLastLookupFeedback.lock() )
  {
    fb->cancel();
  }
  mLookupWatcher.waitForFinished();
}

QStringList ValueRelationController::qgisFormatToArray( const QVariant &qgsValue ) const
{
  if ( !mIsInitialized )
{
  CoreUtils::log( QStringLiteral( "Value Relation" ), QStringLiteral( "Attempted to convert QGIS format to array, but the class is not initialized!" ) );
    return {};
  }

  if ( qgsValue.isNull() || qgsValue.toString().isEmpty() )
    return {};

    if ( mIsMultiSelection )
  {
    const QString str = qgsValue.toString().trimmed();

      if ( str.startsWith( '{' ) || str.startsWith( '[' ) )
      {
        return QgsValueRelationFieldFormatter::valueToStringList( qgsValue );
      }
    }

  return { qgsValue.toString() };
}

QString ValueRelationController::arrayToQgisFormat( const QStringList &keys ) const
{
  if ( !mIsInitialized )
{
  CoreUtils::log( QStringLiteral( "Value Relation" ), QStringLiteral( "Attempted to convert array to QGIS format, but the class is not initialized!" ) );
    return {};
  }

  // empty -> empty
  if ( keys.isEmpty() )
    return {};

    if ( mIsMultiSelection )
  {
    return QString( "{%1}" ).arg( keys.join( ',' ) );
    }
    else
    {
      return keys.at( 0 );
    }
}

void ValueRelationController::lookupDisplayTextOnValueChanged( const QString &currentValue )
{
  lookupDisplayTextAsync( currentValue );
}

void ValueRelationController::lookupDisplayTextOnHotreload( const QString &currentValue, const QgsFeature &feature )
{
  if ( mFilterExpression.isEmpty() )
  {
    return; // no hotreload for fields without filter expression
  }

  lookupDisplayTextAsync( currentValue, true, feature );
}

void ValueRelationController::lookupDisplayTextAsync( const QString &currentValue, bool useFilterExpression, const QgsFeature &feature )
{
  if ( !mIsInitialized || !mTargetLayer )
  {
    CoreUtils::log( QStringLiteral( "Value Relation" ), QStringLiteral( "Called lookupDisplayTextAsync, but the class is not initialized or layer is invalid!" ) );
    return;
  }

  const QStringList keys = qgisFormatToArray( currentValue );

  if ( keys.isEmpty() )
  {
    setDisplayText( {} );
    return;
  }

  // Cancel any in-flight lookup before creating a new one
  if ( auto fb = mLastLookupFeedback.lock() )
  {
    fb->cancel();
    mLookupWatcher.waitForFinished();
  }

  auto feedback = std::make_shared<QgsFeedback>();
  mLastLookupFeedback = feedback; // weak_ptr

  //
  // Build filter expression: key IN (k1, k2, ...)
  //

  const QgsField keyFieldDef = mTargetLayer->fields().field( mTargetLayerKeyFieldIndex );

  // Keys come from QML as strings, we might need to convert them to numbers
  QStringList quotedKeys;
  quotedKeys.reserve( keys.size() );
  for ( const QString &k : keys )
  {
    const QMetaType::Type keyType = keyFieldDef.isNumeric() ? QMetaType::Int : QMetaType::QString;
    quotedKeys << QgsExpression::quotedValue( k, keyType );
  }

  const QString keyExpr = QString( "%1 IN (%2)" ).arg( QgsExpression::quotedColumnRef( mTargetLayerKeyField ), quotedKeys.join( ',' ) );

  QgsFeatureRequest request;
  request.setFilterExpression( keyExpr );

  if ( useFilterExpression && !mFilterExpression.isEmpty() )
  {
    request.combineFilterExpression( mFilterExpression );

    QgsExpressionContext ctx( QgsExpressionContextUtils::globalProjectLayerScopes( mTargetLayer ) );

    if ( feature.isValid() && QgsValueRelationFieldFormatter::expressionRequiresFormScope( mFilterExpression ) )
    {
      ctx.appendScope( QgsExpressionContextUtils::formScope( feature ) );
    }

    request.setExpressionContext( ctx );

    mLastLookupReason = LookupReason::HotReload;
  }
  else
  {
    mLastLookupReason = LookupReason::ValueChanged;
  }

  request.setFlags( Qgis::FeatureRequestFlag::NoGeometry );
  request.setSubsetOfAttributes( QgsAttributeList() << mTargetLayerKeyFieldIndex << mTargetLayerValueFieldIndex );
  request.setLimit( keys.size() );

  request.setFeedback( feedback.get() );

  // QgsVectorLayerFeatureSource is a thread-safe snapshot; ownership passed to the background thread
  QgsVectorLayerFeatureSource *s = new QgsVectorLayerFeatureSource( mTargetLayer );
  mLookupWatcher.setFuture( QtConcurrent::run( &ValueRelationController::_performLookup, s, request, std::move( feedback ) ) );
}

QgsFeatureList ValueRelationController::_performLookup( QgsVectorLayerFeatureSource *source, QgsFeatureRequest req, std::shared_ptr<QgsFeedback> feedback )
{
  std::unique_ptr<QgsVectorLayerFeatureSource> fs( source );
  QgsFeatureList features;

  QgsFeatureIterator it = fs->getFeatures( req );
  QgsFeature f;
  while ( it.nextFeature( f ) )
  {
    if ( feedback->isCanceled() )
      break;
    features << f;
  }

  return features;
}

void ValueRelationController::onLookupFinished()
{
  const QgsFeatureList features = mLookupWatcher.result();

  QStringList displayValues;
  displayValues.reserve( features.size() );

  for ( const QgsFeature &f : features )
  {
    displayValues << f.attribute( mTargetLayerValueFieldIndex ).toString();
  }

  //
  // This logic is not well-optimized for scenarios when you receive just
  // a subset of values, e.g. lookup of "fid" IN (1,2,3) would return just
  // two results - we do not invalidate the third one.
  //

  if ( !displayValues.isEmpty() )
  {
    setDisplayText( displayValues.join( QStringLiteral( ", " ) ) );
    return;
  }

  if ( mLastLookupReason == LookupReason::HotReload )
  {
    if ( mIsEditable )
    {
      emit invalidateSelection(); // will reset display text to "" on the next lookup
    }
    // Intentionally no else branch here - if this field is not editable, we do not clear out the previous text
  }
  else
  {
    //
    // Value changed, but it could not be found in the target layer,
    // we show the raw value instead, see https://github.com/MerginMaps/mobile/issues/2148
    //
    setDisplayText( {} );
    emit presentRawValue();
  }
}

void ValueRelationController::clearLayer()
{
  if ( mTargetLayer )
  {
    disconnect( mTargetLayer, nullptr, this, nullptr );
    mTargetLayer = nullptr;
  }

  // Cancel any in-flight fetch — its result will be discarded by the session check.
  if ( auto fb = mLastLookupFeedback.lock() )
  {
    fb->cancel();
  }

  mIsInitialized = false;
}

void ValueRelationController::setup()
{
  clearLayer();

  if ( mConfig.isEmpty() )
    return;

  QgsVectorLayer *layer = QgsValueRelationFieldFormatter::resolveLayer( mConfig, QgsProject::instance() );
  if ( !layer || !layer->isValid() || layer->fields().isEmpty() )
  {
    CoreUtils::log( QStringLiteral( "ValueRelationController" ), QStringLiteral( "Missing or invalid referenced layer." ) );
    return;
  }

  const QString keyFieldName = mConfig.value( QStringLiteral( "Key" ) ).toString();
  const QString valueFieldName = mConfig.value( QStringLiteral( "Value" ) ).toString();

  if ( layer->fields().indexOf( keyFieldName ) < 0 || layer->fields().indexOf( valueFieldName ) < 0 )
  {
    CoreUtils::log( QStringLiteral( "ValueRelationController" ), QStringLiteral( "Missing referenced fields for value relations." ) );
    return;
  }

  mTargetLayer = layer;
  mTargetLayerKeyField = keyFieldName;
  mTargetLayerKeyFieldIndex = layer->fields().indexOf( keyFieldName );
  mTargetLayerValueFieldIndex = layer->fields().indexOf( valueFieldName );

  mFilterExpression = mConfig.value( QStringLiteral( "FilterExpression" ) ).toString();

  mIsMultiSelection = mConfig.value( QStringLiteral( "AllowMulti" ) ).toBool();

  mIsInitialized = true;

  emit isMultiSelectionChanged();

  connect( mTargetLayer, &QgsMapLayer::willBeDeleted, this, &ValueRelationController::clearLayer );
}

QVariantMap ValueRelationController::config() const
{
  return mConfig;
}

void ValueRelationController::setConfig( const QVariantMap &newConfig )
{
  if ( mConfig == newConfig )
    return;

  mConfig = newConfig;
  emit configChanged();
  setup();
}

bool ValueRelationController::isEditable() const
{
  return mIsEditable;
}

void ValueRelationController::setIsEditable( bool newIsEditable )
{
  if ( mIsEditable != newIsEditable )
  {
    mIsEditable = newIsEditable;
    emit isEditableChanged();
  }
}

bool ValueRelationController::isMultiSelection() const
{
  return mIsMultiSelection;
}

const QString &ValueRelationController::displayText() const
{
  return mDisplayText;
}

void ValueRelationController::setDisplayText( const QString &newText )
{
  if ( mDisplayText != newText )
  {
    mDisplayText = newText;
    emit displayTextChanged();
  }
}