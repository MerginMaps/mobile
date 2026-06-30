/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layerfeaturesmodel.h"

#include "featurelayerpair.h"

#include "qgsattributetableconfig.h"
#include "qgsfeaturerequest.h"
#include "qgsfeedback.h"
#include "qgsvectorlayer.h"
#include "qgsvectorlayerfeatureiterator.h"

#include <QLocale>
#include <QElapsedTimer>
#include <QtConcurrentRun>


LayerFeaturesModel::LayerFeaturesModel( QObject *parent )
  : FeaturesModel( parent ),
    mLayer( nullptr )
{
}

LayerFeaturesModel::~LayerFeaturesModel()
{
  for ( auto [id, feedbackWatcherPair] : mResultWatchers.asKeyValueRange() )
  {
    const int i = id;
    QgsFeedback *feedback = feedbackWatcherPair.first;
    QFutureWatcher<SearchResultData> *watcher = feedbackWatcherPair.second;

    // watcher should not call ofFutureFinished after this is deleted
    watcher->disconnect( this );
    feedback->cancel();

    // Self-cleanup: delete both once the background task finishes
    connect( watcher, &QFutureWatcher<SearchResultData>::finished, watcher, [watcher, feedback, i]()
    {
      watcher->deleteLater();
      feedback->deleteLater();
      qDebug() << QStringLiteral( "Search (%1) cleaned up in the destructor" ).arg( i );
    } );
  }
  mResultWatchers.clear();
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
  cancelPendingRequests();

  if ( mLayer && mLayer->dataProvider() )
  {
    mFetchingResults = true;
    emit fetchingResultsChanged( mFetchingResults );
    beginResetModel();
    mFeatures.clear();
    endResetModel();

    QgsFeatureRequest req;
    setupFeatureRequest( req );

    const int searchId = mNextSearchId.fetchAndAddOrdered( 1 );
    QgsFeedback *feedback = new QgsFeedback();
    req.setFeedback( feedback );

    QFutureWatcher<SearchResultData> *watcher = new QFutureWatcher<SearchResultData>();

    mResultWatchers[ searchId ] = qMakePair( feedback, watcher );
    connect( watcher, &QFutureWatcher<SearchResultData>::finished, this, [this, searchId] { this->handleFinishedSearch( searchId ); } );

    qDebug() << QStringLiteral( "Search (%1) starting on layer %2" ).arg( searchId ).arg( mLayer->id() );

    watcher->setFuture( QtConcurrent::run( LayerFeaturesModel::fetchFeatures, new QgsVectorLayerFeatureSource( mLayer ), req, searchId ) );
  }
}

LayerFeaturesModel::SearchResultData LayerFeaturesModel::fetchFeatures( QgsAbstractFeatureSource *source, const QgsFeatureRequest &req, int searchId )
{
#ifdef QT_DEBUG
  QElapsedTimer t;
  t.start();
#endif

  std::unique_ptr<QgsAbstractFeatureSource> ownedSource( source );
  QgsFeatureList fl;

  QgsFeatureIterator it = ownedSource->getFeatures( req );
  it.setInterruptionChecker( req.feedback() );
  QgsFeature f;

  while ( it.nextFeature( f ) )
  {
    if ( FID_IS_NEW( f.id() ) || FID_IS_NULL( f.id() ) )
    {
      continue; // ignore uncommited features
    }

    fl.append( f );
  }

#ifdef QT_DEBUG
  const bool canceled = req.feedback()->isCanceled();
  qDebug() << QStringLiteral( "Search (%1) %2 after %3ms, results: %4" ).arg( searchId ).arg( canceled ? QStringLiteral( "was canceled" ) : QStringLiteral( "completed" ) ).arg( t.elapsed() ).arg( fl.count() );
#endif
  return { searchId, fl };
}

void LayerFeaturesModel::handleFinishedSearch( int searchId )
{
  if ( !mResultWatchers.contains( searchId ) )
  {
    // should not happen, this method is called only once for existing searchIds only
    Q_ASSERT( false );
    return;
  }

  auto [feedback, watcher] = mResultWatchers.take( searchId );
  const SearchResultData data = watcher->future().result();

  watcher->deleteLater();
  feedback->deleteLater();
  qDebug() << QStringLiteral( "Search (%1) cleaned up" ).arg( searchId );

  // We ignore the results of cancelled requests
  if ( !feedback->isCanceled() )
  {
    const QgsFeatureList features = data.features;
    beginResetModel();
    mFeatures.clear();
    for ( const QgsFeature &feat : features )
    {
      mFeatures.emplaceBack( feat, mLayer );
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
  for ( auto [id, feedbackWatcherPair] : mResultWatchers.asKeyValueRange() )
  {
    feedbackWatcherPair.first->cancel();
  }
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
