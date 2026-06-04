/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layerfeaturesmodel.h"

#include "inpututils.h"
#include "qgsproject.h"
#include "qgsvectorlayerfeatureiterator.h"
#include "qgsfeedback.h"

#include <QLocale>
#include <QTimer>
#include <QtConcurrentRun>


LayerFeaturesModel::LayerFeaturesModel( QObject *parent )
  : FeaturesModel( parent ),
    mLayer( nullptr )
{
}

LayerFeaturesModel::~LayerFeaturesModel()
{
  // cancel any long running request
  cancelPendingRequests();

  for ( auto w : std::as_const( mSearchResultWatchers ) )
    w->waitForFinished();
}

QVariant LayerFeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFeatures.count() )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  const FeatureLayerPair pair = mFeatures.at( index.row() );

  if ( role == SearchResult )
    return searchResultPair( pair );

  if ( role == Feature )
  {
    return pair.layer()->getFeature( pair.feature().id() );
  }

  if ( role == FeaturePair )
  {
    const QgsFeature f = pair.layer()->getFeature( pair.feature().id() );
    return QVariant::fromValue<FeatureLayerPair>( FeatureLayerPair( f, pair.layer() ) );
  }

  return FeaturesModel::data( index, role );
}

QHash<int, QByteArray> LayerFeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roles = FeaturesModel::roleNames();
  roles[SearchResult] = QStringLiteral( "SearchResult" ).toLatin1();

  return roles;
}

void LayerFeaturesModel::populate()
{
  if ( mLayer )
  {
    cancelPendingRequests();

    mFetchingResults = true;
    emit fetchingResultsChanged( mFetchingResults );
    beginResetModel();
    mFeatures.clear();
    endResetModel();

    QgsFeatureRequest req;
    setupFeatureRequest( req );

    const int searchId = mNextSearchId.fetchAndAddOrdered( 1 );
    QgsFeedback *feedback = new QgsFeedback( this );
    mFeedbacks[ searchId ] = feedback;
    req.setFeedback( feedback );

    QFutureWatcher<SearchResultData> *watcher = new QFutureWatcher<SearchResultData>( this );
    mSearchResultWatchers[ searchId ] = watcher;
    connect( watcher, &QFutureWatcher<SearchResultData>::finished, this, &LayerFeaturesModel::onFutureFinished );

    QgsVectorLayerFeatureSource *source = new QgsVectorLayerFeatureSource( mLayer );
    watcher->setFuture( QtConcurrent::run( &LayerFeaturesModel::fetchFeatures, source, req, searchId ) );
  }
}

LayerFeaturesModel::SearchResultData LayerFeaturesModel::fetchFeatures( QgsVectorLayerFeatureSource *source, const QgsFeatureRequest &req, int searchId )
{
  std::unique_ptr<QgsVectorLayerFeatureSource> fs( source );
  QgsFeatureList fl;

  // a search might have been queued if no threads were available in the pool, so we also
  // check if canceled before we start as the first iteration can potentially be slow
  if ( req.feedback()->isCanceled() )
  {
    qDebug() << QStringLiteral( "Search (%1) was cancelled before it started!" ).arg( searchId );
    return { searchId, fl };
  }

  QElapsedTimer t;
  t.start();
  QgsFeatureIterator it = fs->getFeatures( req );
  QgsFeature f;

  while ( it.nextFeature( f ) )
  {
    if ( FID_IS_NEW( f.id() ) || FID_IS_NULL( f.id() ) )
    {
      continue; // ignore uncommited features
    }

    fl.append( f );
  }

  const bool canceled = req.feedback()->isCanceled();

  qDebug() << QStringLiteral( "Search (%1) %2 after %3ms, results: %4" ).arg( searchId ).arg( canceled ? QStringLiteral( "was canceled" ) : QStringLiteral( "completed" ) ).arg( t.elapsed() ).arg( fl.count() );
  return { searchId, fl };
}

void LayerFeaturesModel::onFutureFinished()
{
  QFutureWatcher<SearchResultData> *watcher = static_cast< QFutureWatcher<SearchResultData> *>( sender() );
  const SearchResultData data = watcher->future().result();

  mSearchResultWatchers.remove( data.searchId );
  watcher->deleteLater();

  QgsFeedback *feedback = mFeedbacks.take( data.searchId );
  feedback->deleteLater();

  // We ignore the results of cancelled requests
  if ( !feedback->isCanceled() )
  {
    const QgsFeatureList features = data.features;
    beginResetModel();
    mFeatures.clear();
    for ( const auto &f : features )
    {
      mFeatures.emplaceBack( f, mLayer );
    }
    emit layerFeaturesCountChanged( layerFeaturesCount() );
    emit countChanged( rowCount() );
    endResetModel();
  }

  // Only fire signal if that was the latest request
  if ( data.searchId + 1 == mNextSearchId.loadAcquire() )
  {
    mFetchingResults = false;
    emit fetchingResultsChanged( mFetchingResults );
  }
}

QString LayerFeaturesModel::searchResultPair( const FeatureLayerPair &pair ) const
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

void LayerFeaturesModel::cancelPendingRequests()
{
  for ( auto fb : std::as_const( mFeedbacks ) )
    fb->cancel();
}

QString LayerFeaturesModel::buildSearchExpression()
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
      else if ( field.type() == QMetaType::QString || field.isNumeric() )
        expressionParts << QStringLiteral( "%1 ILIKE '%%2%'" ).arg( QgsExpression::quotedColumnRef( field.name() ), word );
    }
    wordExpressions << QStringLiteral( "(%1)" ).arg( expressionParts.join( QLatin1String( " ) OR ( " ) ) );
    expressionParts.clear();
  }

  const QString expression = QStringLiteral( "(%1)" ).arg( wordExpressions.join( QLatin1String( " ) AND ( " ) ) );

  return expression;
}

void LayerFeaturesModel::setupFeatureRequest( QgsFeatureRequest &request )
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
      QgsFeatureRequest::OrderByClause( mLayer->attributeTableConfig().sortExpression(), mLayer->attributeTableConfig().sortOrder() == Qt::AscendingOrder )
    } ) );
  }

  request.setSubsetOfAttributes( mAttributeList );

  request.setLimit( FEATURES_LIMIT );
}

void LayerFeaturesModel::reloadFeatures()
{
  populate();
}

int LayerFeaturesModel::layerFeaturesCount() const
{
  if ( mLayer && mLayer->isValid() )
  {
    return mLayer->dataProvider()->featureCount();
  }

  return 0;
}

void LayerFeaturesModel::reset()
{
  cancelPendingRequests();
  mFeatures.clear();
  mLayer = nullptr;
  mSearchExpression.clear();
}

QString LayerFeaturesModel::searchExpression() const
{
  return mSearchExpression;
}

void LayerFeaturesModel::setSearchExpression( const QString &searchExpression )
{
  if ( mSearchExpression != searchExpression )
  {
    mSearchExpression = searchExpression;
    emit searchExpressionChanged( mSearchExpression );
    populate();
  }
}

int LayerFeaturesModel::featuresLimit() const
{
  return FEATURES_LIMIT;
}

void LayerFeaturesModel::setLayer( QgsVectorLayer *newLayer )
{
  if ( mLayer != newLayer )
  {
    cancelPendingRequests();

    if ( mLayer )
    {
      disconnect( mLayer );
    }

    mLayer = newLayer;
    emit layerChanged( mLayer );

    if ( mLayer )
    {
      // avoid dangling pointers to mLayer when switching projects
      connect( mLayer, &QgsMapLayer::willBeDeleted, this, &LayerFeaturesModel::reset );

      connect( mLayer, &QgsVectorLayer::featureAdded, this, &LayerFeaturesModel::populate );
      connect( mLayer, &QgsVectorLayer::featuresDeleted, this, &LayerFeaturesModel::populate );
      connect( mLayer, &QgsVectorLayer::attributeValueChanged, this, &LayerFeaturesModel::populate );

      // by default use all attributes
      mAttributeList = mLayer->attributeList();
    }

    emit layerFeaturesCountChanged( layerFeaturesCount() );
  }
}

QgsVectorLayer *LayerFeaturesModel::layer() const
{
  return mLayer;
}
