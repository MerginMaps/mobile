﻿/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "featuresmodel.h"
#include "coreutils.h"

#include "inpututils.h"
#include "qgsproject.h"
#include "qgsexpressioncontextutils.h"
#include "qgsvectorlayerfeatureiterator.h"

#include <QLocale>
#include <QTimer>
#include <QtConcurrent>


FeaturesModel::FeaturesModel( QObject *parent )
  : QAbstractListModel( parent ),
    mLayer( nullptr )
{
  connect( &mSearchResultWatcher, &QFutureWatcher<QgsFeatureList>::finished, this, &FeaturesModel::onFutureFinished );
}

FeaturesModel::~FeaturesModel() = default;

void FeaturesModel::populateStaticModel( FeatureLayerPairs pairs )
{
  beginResetModel();
  mFeatures.clear();
  mFeatures.append( pairs );
  endResetModel();
  emit countChanged( rowCount() );
}

void FeaturesModel::populate()
{
  if ( mLayer )
  {
    mFetchingResults = true;
    emit fetchingResultsChanged( mFetchingResults );
    beginResetModel();
    mFeatures.clear();
    endResetModel();

    QgsFeatureRequest req;
    setupFeatureRequest( req );

    int searchId = mNextSearchId.fetchAndAddOrdered( 1 );
    QgsVectorLayerFeatureSource *source = new QgsVectorLayerFeatureSource( mLayer );
    mSearchResultWatcher.setFuture( QtConcurrent::run( &FeaturesModel::fetchFeatures, this, source, req, searchId ) );
  }
}

QgsFeatureList FeaturesModel::fetchFeatures( QgsVectorLayerFeatureSource *source, QgsFeatureRequest req, int searchId )
{
  std::unique_ptr<QgsVectorLayerFeatureSource> fs( source );
  QgsFeatureList fl;

  // a search might have been queued if no threads were available in the pool, so we also
  // check if canceled before we start as the first iteration can potentially be slow
  bool canceled = searchId + 1 != mNextSearchId.loadAcquire();
  if ( canceled )
  {
    qDebug() << QString( "Search (%1) was cancelled before it started!" ).arg( searchId );
    return fl;
  }

  QElapsedTimer t;
  t.start();
  QgsFeatureIterator it = fs->getFeatures( req );
  QgsFeature f;

  while ( it.nextFeature( f ) )
  {
    if ( searchId + 1 != mNextSearchId.loadAcquire() )
    {
      canceled = true;
      break;
    }

    if ( FID_IS_NEW( f.id() ) || FID_IS_NULL( f.id() ) )
    {
      continue; // ignore uncommited features
    }

    fl.append( f );
  }

  qDebug() << QString( "Search (%1) %2 after %3ms, results: %4" ).arg( searchId ).arg( canceled ? "was canceled" : "completed" ).arg( t.elapsed() ).arg( fl.count() );
  return fl;
}

void FeaturesModel::onFutureFinished()
{
  QFutureWatcher<QgsFeatureList> *watcher = static_cast< QFutureWatcher<QgsFeatureList> *>( sender() );
  const QgsFeatureList features = watcher->future().result();
  beginResetModel();
  mFeatures.clear();
  for ( const auto &f : features )
  {
    mFeatures << FeatureLayerPair( f, mLayer );
  }
  emit layerFeaturesCountChanged( layerFeaturesCount() );
  emit countChanged( rowCount() );
  endResetModel();
  mFetchingResults = false;
  emit fetchingResultsChanged( mFetchingResults );
}


void FeaturesModel::setup()
{
  // define in submodels
}

QVariant FeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFeatures.count() )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  const FeatureLayerPair pair = mFeatures.at( index.row() );

  switch ( role )
  {
    case FeatureTitle: return featureTitle( pair );
    case FeatureId: return QVariant( pair.feature().id() );
    case Feature: return QVariant::fromValue<QgsFeature>( pair.feature() );
    case FeaturePair: return QVariant::fromValue<FeatureLayerPair>( pair );
    case Description: return QVariant( QString( "Feature ID %1" ).arg( pair.feature().id() ) );
    case SearchResult: return searchResultPair( pair );
    case LayerName: return pair.layer() ? pair.layer()->name() : QString();
    case LayerIcon: return pair.layer() ? InputUtils::loadIconFromLayer( pair.layer() ) : QString();
    case Qt::DisplayRole: return featureTitle( pair );
  }

  return QVariant();
}

int FeaturesModel::rowCount( const QModelIndex &parent ) const
{
  // For list models only the root node (an invalid parent) should return the list's size. For all
  // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
  if ( parent.isValid() )
    return 0;

  return mFeatures.count();
}

QVariant FeaturesModel::featureTitle( const FeatureLayerPair &featurePair ) const
{
  if ( !featurePair.layer() || !featurePair.layer()->isValid() )
  {
    CoreUtils::log( QStringLiteral( "Features Model" ), QStringLiteral( "Received invalid feature layer pair!" ) );
    return tr( "Unknown title" );
  }

  QString title;

  QgsExpressionContext context( QgsExpressionContextUtils::globalProjectLayerScopes( featurePair.layer() ) );
  context.setFeature( featurePair.feature() );
  QgsExpression expr( featurePair.layer()->displayExpression() );
  title = expr.evaluate( &context ).toString();

  if ( title.isEmpty() )
    return featurePair.feature().id();

  return title;
}

QString FeaturesModel::searchResultPair( const FeatureLayerPair &pair ) const
{
  if ( mSearchExpression.isEmpty() )
    return QString();

  QgsFields fields = pair.feature().fields();
  const QStringList words = mSearchExpression.split( ' ', Qt::SkipEmptyParts );
  QStringList foundPairs;

  for ( const QString &word : words )
  {
    for ( const QgsField &field : fields )
    {
      if ( field.configurationFlags().testFlag( Qgis::FieldConfigurationFlag::NotSearchable ) )
        continue;

      QString attrValue = pair.feature().attribute( field.name() ).toString();

      if ( attrValue.toLower().indexOf( word.toLower() ) != -1 )
      {
        foundPairs << field.name() + ": " + attrValue;

        // remove found field from list of fields to not select it more than once
        fields.remove( fields.lookupField( field.name() ) );
      }
    }
  }

  return foundPairs.join( ", " );
}

QString FeaturesModel::buildSearchExpression()
{
  if ( mSearchExpression.isEmpty() || !mLayer )
    return QString();

  const QgsFields fields = mLayer->fields();
  QStringList expressionParts;
  QStringList wordExpressions;

  const QLocale locale;
  const QStringList words = mSearchExpression.split( ' ', Qt::SkipEmptyParts );

  for ( const QString &word : words )
  {
    bool searchExpressionIsNumeric;
    // we only need to know if expression is numeric, return value is not used
    locale.toFloat( word, &searchExpressionIsNumeric );


    for ( const QgsField &field : fields )
    {
      if ( field.configurationFlags().testFlag( Qgis::FieldConfigurationFlag::NotSearchable ) ||
           ( field.isNumeric() && !searchExpressionIsNumeric ) )
        continue;
      else if ( field.type() == QVariant::String || field.isNumeric() )
        expressionParts << QStringLiteral( "%1 ILIKE '%%2%'" ).arg( QgsExpression::quotedColumnRef( field.name() ), word );
    }
    wordExpressions << QStringLiteral( "(%1)" ).arg( expressionParts.join( QLatin1String( " ) OR ( " ) ) );
    expressionParts.clear();
  }

  const QString expression = QStringLiteral( "(%1)" ).arg( wordExpressions.join( QLatin1String( " ) AND ( " ) ) );

  return expression;
}

void FeaturesModel::setupFeatureRequest( QgsFeatureRequest &request )
{
  if ( !mSearchExpression.isEmpty() )
  {
    request.setFilterExpression( buildSearchExpression() );
  }

  if ( mUseAttributeTableSortOrder && mLayer && !mLayer->attributeTableConfig().sortExpression().isEmpty() )
  {
    // get a context with global, project and layer scopes
    // QGIS docs are not very clear, but this context is also used for evaluation of the request's 'order by' expressions too
    QgsExpressionContext context = mLayer->createExpressionContext();
    request.setExpressionContext( context );
    request.setOrderBy( QgsFeatureRequest::OrderBy(
    {
      QgsFeatureRequest::OrderByClause(
      mLayer->attributeTableConfig().sortExpression(),
      mLayer->attributeTableConfig().sortOrder() == Qt::AscendingOrder )
    } ) );
  }

  request.setLimit( FEATURES_LIMIT );
}

void FeaturesModel::reloadFeatures()
{
  populate();
}

int FeaturesModel::layerFeaturesCount() const
{
  if ( mLayer && mLayer->isValid() )
  {
    return mLayer->dataProvider()->featureCount();
  }

  return 0;
}

QHash<int, QByteArray> FeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[FeatureTitle] = QStringLiteral( "FeatureTitle" ).toLatin1();
  roleNames[FeatureId] = QStringLiteral( "FeatureId" ).toLatin1();
  roleNames[Feature] = QStringLiteral( "Feature" ).toLatin1();
  roleNames[FeaturePair] = QStringLiteral( "FeaturePair" ).toLatin1();
  roleNames[Description] = QStringLiteral( "Description" ).toLatin1();
  roleNames[SearchResult] = QStringLiteral( "SearchResult" ).toLatin1();
  roleNames[LayerName] = QStringLiteral( "LayerName" ).toLatin1();
  roleNames[LayerIcon] = QStringLiteral( "LayerIcon" ).toLatin1();
  return roleNames;
}

int FeaturesModel::rowFromRoleValue( const int role, const QVariant &value ) const
{
  for ( int i = 0; i < mFeatures.count(); ++i )
  {
    QVariant d = data( index( i, 0 ), role );
    if ( d == value )
    {
      return i;
    }
  }
  return -1;
}

QVariant FeaturesModel::convertRoleValue( const int role, const QVariant &value, const int requestedRole ) const
{
  for ( int i = 0; i < mFeatures.count(); ++i )
  {
    QVariant d = data( index( i, 0 ), role );
    if ( d.toString().trimmed() == value.toString().trimmed() )
    {
      QVariant key = data( index( i, 0 ), requestedRole );
      return key;
    }
  }
  return QVariant();
}

void FeaturesModel::reset()
{
  mFeatures.clear();
  mLayer = nullptr;
  mSearchExpression.clear();
}

QString FeaturesModel::searchExpression() const
{
  return mSearchExpression;
}

int FeaturesModel::count() const
{
  return rowCount();
}

void FeaturesModel::setSearchExpression( const QString &searchExpression )
{
  if ( mSearchExpression != searchExpression )
  {
    mSearchExpression = searchExpression;
    emit searchExpressionChanged( mSearchExpression );
    populate();
  }
}

int FeaturesModel::featuresLimit() const
{
  return FEATURES_LIMIT;
}

void FeaturesModel::setLayer( QgsVectorLayer *newLayer )
{
  if ( mLayer != newLayer )
  {
    if ( mLayer )
    {
      disconnect( mLayer );
    }

    mLayer = newLayer;
    emit layerChanged( mLayer );

    if ( mLayer )
    {
      // avoid dangling pointers to mLayer when switching projects
      connect( mLayer, &QgsMapLayer::willBeDeleted, this, &FeaturesModel::reset );

      connect( mLayer, &QgsVectorLayer::featureAdded, this, &FeaturesModel::populate );
      connect( mLayer, &QgsVectorLayer::featuresDeleted, this, &FeaturesModel::populate );
      connect( mLayer, &QgsVectorLayer::attributeValueChanged, this, &FeaturesModel::populate );
    }

    emit layerFeaturesCountChanged( layerFeaturesCount() );
  }
}

QgsVectorLayer *FeaturesModel::layer() const
{
  return mLayer;
}
