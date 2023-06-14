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

ValueRelationFeaturesModel::ValueRelationFeaturesModel( QObject *parent )
  : FeaturesModel( parent )
{
}

ValueRelationFeaturesModel::~ValueRelationFeaturesModel() = default;

void ValueRelationFeaturesModel::setupFeatureRequest( QgsFeatureRequest &request )
{
  FeaturesModel::setupFeatureRequest( request );

  if ( !mFilterExpression.isEmpty() )
  {
    request.combineFilterExpression( mFilterExpression );

    // create context for filter expression
    if ( QgsValueRelationFieldFormatter::expressionIsUsable( mFilterExpression, mPair.feature() ) )
    {
      QgsExpression exp( mFilterExpression );
      QgsExpressionContext filterContext = QgsExpressionContext( QgsExpressionContextUtils::globalProjectLayerScopes( FeaturesModel::layer() ) );

      if ( mPair.feature().isValid() && QgsValueRelationFieldFormatter::expressionRequiresFormScope( mFilterExpression ) )
        filterContext.appendScope( QgsExpressionContextUtils::formScope( mPair.feature() ) );

      request.setExpressionContext( filterContext );
    }
  }
}

void ValueRelationFeaturesModel::setup()
{
  if ( mConfig.isEmpty() )
    return;

  QgsVectorLayer *layer = QgsValueRelationFieldFormatter::resolveLayer( mConfig, QgsProject::instance() );

  if ( layer && layer->fields().size() != 0 )
  {
    QgsFields fields = layer->fields();

    QString keyFieldName = mConfig.value( QStringLiteral( "Key" ) ).toString();
    QString valueFieldName = mConfig.value( QStringLiteral( "Value" ) ).toString();

    if ( fields.indexOf( keyFieldName ) >= 0 && fields.indexOf( valueFieldName ) >= 0 )
    {
      mKeyField = keyFieldName;
      mTitleField = valueFieldName;

      mFilterExpression = mConfig.value( QStringLiteral( "FilterExpression" ) ).toString();
      FeaturesModel::setLayer( layer );

      mAllowMulti = mConfig.value( QStringLiteral( "AllowMulti" ) ).toBool();
      mIsInitialized = true;
    }
    else
      CoreUtils::log( QStringLiteral( "ValueRelations" ), QStringLiteral( "Missing referenced fields for value relations." ) );
  }
  else
    CoreUtils::log( QStringLiteral( "ValueRelations" ), QStringLiteral( "Missing referenced layer for value relations." ) );
}

void ValueRelationFeaturesModel::reset()
{
  mKeyField.clear();
  mTitleField.clear();
  mPair = FeatureLayerPair();
  mConfig = QVariantMap();
  mIsInitialized = false;
  FeaturesModel::reset();
}

QVariant ValueRelationFeaturesModel::featureTitle( const FeatureLayerPair &pair ) const
{
  if ( !mTitleField.isEmpty() )
  {
    return pair.feature().attribute( mTitleField );
  }

  return FeaturesModel::featureTitle( pair );
}

QVariant ValueRelationFeaturesModel::convertToKey( const QVariant &id )
{
  QgsFeature f = FeaturesModel::convertRoleValue( FeaturesModel::FeatureId, id, Feature ).value<QgsFeature>();
  return f.attribute( mKeyField );
}

QVariant ValueRelationFeaturesModel::convertToQgisType( const QVariantList &featureIds )
{
  if ( !mIsInitialized )
  {
    return QVariant();
  }

  QVariant qgsFormat;

  QStringList keys;
  for ( const QVariant &id : featureIds )
  {
    keys << convertToKey( id ).toString();
  }

  qgsFormat = QStringLiteral( "{%1}" ).arg( keys.join( ',' ) );

  return qgsFormat;
}

QVariant ValueRelationFeaturesModel::convertFromQgisType( QVariant qgsValue, ModelRoles toRole )
{
  if ( !mIsInitialized )
  {
    return QVariant();
  }

  QStringList keyList;

  if ( mAllowMulti )
  {
    keyList = QgsValueRelationFieldFormatter::valueToStringList( qgsValue );
  }
  else
  {
    keyList << qgsValue.toString();
  }

  QList<QVariant> roleList;

  // optimize it a little bit
  QMap<QVariant, QVariant> keyMap;
  for ( const QString &key : keyList )
  {
    keyMap.insert( key, QLatin1String() );
  }

  for ( int ix = 0; ix < FeaturesModel::rowCount(); ++ix )
  {
    QgsFeature f = FeaturesModel::data( index( ix, 0 ), Feature ).value<QgsFeature>();

    if ( keyMap.contains( f.attribute( mKeyField ).toString() ) )
    {
      if ( toRole == FeatureId )
        roleList.append( f.id() );
      else
      {
        QVariant attr = FeaturesModel::convertRoleValue( FeatureId, f.id(), toRole );
        if ( !attr.isNull() )
          roleList.append( attr );
      }
    }
  }

  if ( roleList.isEmpty() && !qgsValue.isNull() )
  {
    // could not convert qgs value
    emit invalidate();
  }

  return roleList;
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

  if ( mIsInitialized )
  {
    populate();
  }
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
