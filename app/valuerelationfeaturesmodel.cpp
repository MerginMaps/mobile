/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "valuerelationfeaturesmodel.h"
#include "coreutils.h"

#include "qgsvaluerelationfieldformatter.h"
#include "qgsexpressioncontextutils.h"
#include "qgsvectorlayer.h"

ValueRelationFeaturesModel::ValueRelationFeaturesModel( QObject *parent )
  : LayerFeaturesModel( parent )
{
}

ValueRelationFeaturesModel::~ValueRelationFeaturesModel() = default;

QVariant ValueRelationFeaturesModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
  return QVariant();

  const int row = index.row();
  if ( row < 0 || row >= mFeatures.count() )
    return QVariant();

    if ( role == KeyColumn )
      return mFeatures.at( row ).feature().attribute( mKeyFieldIndex ).toString();

      if ( role == ValueColumn )
        return mFeatures.at( row ).feature().attribute( mValueFieldIndex ).toString();

        return LayerFeaturesModel::data( index, role );
      }

QHash<int, QByteArray> ValueRelationFeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roles = LayerFeaturesModel::roleNames();
  roles[KeyColumn]   = QByteArrayLiteral( "KeyColumn" );
  roles[ValueColumn] = QByteArrayLiteral( "ValueColumn" );
  return roles;
}

void ValueRelationFeaturesModel::setupFeatureRequest( QgsFeatureRequest &request )
{
  LayerFeaturesModel::setupFeatureRequest( request );

  // minimal subset of attributes

  request.setSubsetOfAttributes( QgsAttributeList() << mKeyFieldIndex << mValueFieldIndex );
  request.setFlags( Qgis::FeatureRequestFlag::NoGeometry );

  // filter expression

  if ( !mFilterExpression.isEmpty() && mPair.isValid() )
  {
    request.combineFilterExpression( mFilterExpression );

    if ( QgsValueRelationFieldFormatter::expressionIsUsable( mFilterExpression, mPair.feature() ) )
    {
      QgsExpressionContext ctx( QgsExpressionContextUtils::globalProjectLayerScopes( layer() ) );

      if ( mPair.feature().isValid() && QgsValueRelationFieldFormatter::expressionRequiresFormScope( mFilterExpression ) )
      {
        ctx.appendScope( QgsExpressionContextUtils::formScope( mPair.feature() ) );
      }

      request.setExpressionContext( ctx );
    }
  }

  // order

  request.setOrderBy( QgsFeatureRequest::OrderBy( { QgsFeatureRequest::OrderByClause( mOrderByField, mOrderByAsc, false ) } ) );

  // limit

  request.setLimit( VR_FEATURES_LIMIT );
}

QString ValueRelationFeaturesModel::buildSearchExpression()
{
  // Let's search only in the value column, this is a minimal approach compared to the base class implementation
  const QString searchExpr = searchExpression().trimmed();

  if ( searchExpr.isEmpty() )
  {
    return {};
  }

  return QString( "(%1 ILIKE '%%2%')" ).arg( QgsExpression::quotedColumnRef( mValueField ), searchExpr );
}

void ValueRelationFeaturesModel::setup()
{
  mIsInitialized = false;

  if ( mConfig.isEmpty() )
    return;

  QgsVectorLayer *vLayer = QgsValueRelationFieldFormatter::resolveLayer( mConfig, QgsProject::instance() );

  if ( !vLayer || !vLayer->isValid() || vLayer->fields().isEmpty() )
  {
    CoreUtils::log( QStringLiteral( "Value Relation" ), QStringLiteral( "Missing or invalid referenced layer" ) );
    return;
  }

  const QString keyFieldName = mConfig.value( QStringLiteral( "Key" ) ).toString();
  const QString valueFieldName = mConfig.value( QStringLiteral( "Value" ) ).toString();

  if ( vLayer->fields().indexOf( keyFieldName ) < 0 || vLayer->fields().indexOf( valueFieldName ) < 0 )
  {
    CoreUtils::log( QStringLiteral( "ValueRelationFeaturesModel" ), QStringLiteral( "Missing referenced fields for value relations." ) );
    return;
  }

  mKeyField = keyFieldName;
  mValueField = valueFieldName;
  mKeyFieldIndex = vLayer->fields().indexOf( keyFieldName );
  mValueFieldIndex = vLayer->fields().indexOf( valueFieldName );

  mFilterExpression = mConfig.value( QStringLiteral( "FilterExpression" ) ).toString();

  // setLayer() internally resets mAttributeList to all fields, so we must
  // override it afterwards with only the two columns we actually need.
  LayerFeaturesModel::setLayer( vLayer );

  mAttributeList = { mKeyFieldIndex, mValueFieldIndex };

  mOrderByAsc = !mConfig.value( QStringLiteral( "OrderByDescending" ) ).toBool();

  if ( mConfig.value( QStringLiteral( "OrderByKey" ) ).toBool() )
  {
    mOrderByField = mKeyField;
  }
  else if ( mConfig.value( QStringLiteral( "OrderByField" ) ).toBool() )
  {
    QString fieldToOrderBy = mConfig.value( QStringLiteral( "OrderByFieldName" ) ).toString();
    if ( fieldToOrderBy.isEmpty() )
    {
      CoreUtils::log( QStringLiteral( "Value Relation" ), QStringLiteral( "Requested to order results by field, but the field name is empty" ) );
    }

    mOrderByField = fieldToOrderBy;
  }
  else
  {
    // let's use "OrderByValue" by default
    mOrderByField = mValueField;
  }

  mIsInitialized = true;

  // Note: populate() is intentionally NOT called here.
  // The QML drawer calls it explicitly in Component.onCompleted so that features
  // are only fetched when the user actually opens the drawer.
}

void ValueRelationFeaturesModel::reset()
{
  mKeyField.clear();
  mValueField.clear();
  mKeyFieldIndex   = -1;
  mValueFieldIndex = -1;
  mFilterExpression.clear();
  mConfig = QVariantMap();
  mPair = FeatureLayerPair();
  mIsInitialized = false;
  LayerFeaturesModel::reset();
}

QVariantMap ValueRelationFeaturesModel::config() const
{
  return mConfig;
}

void ValueRelationFeaturesModel::setConfig( const QVariantMap &newConfig )
{
  if ( mConfig == newConfig )
    return;

  mConfig = newConfig;
  emit configChanged( mConfig );

  setup();
}

FeatureLayerPair ValueRelationFeaturesModel::pair() const
{
  return mPair;
}

void ValueRelationFeaturesModel::setPair( const FeatureLayerPair &newPair )
{
  if ( mPair == newPair )
    return;

  mPair = newPair;
  emit pairChanged( mPair );
  // No automatic repopulation — the pair is set once at drawer-open time.
  // reloadFeatures() is called separately by Component.onCompleted.
}
