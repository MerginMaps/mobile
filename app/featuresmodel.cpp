/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "featuresmodel.h"

FeaturesModel::FeaturesModel( Loader &loader, QObject *parent )
  : QAbstractListModel( parent ),
    mLoader( loader ),
    mFeaturesCount( 0 )
{
}

QgsQuickFeatureLayerPair FeaturesModel::featureLayerPair( const int &featureId )
{
  for ( QgsQuickFeatureLayerPair i : mFeatures )
  {
    if ( i.feature().id() == featureId )
      return i;
  }
  return QgsQuickFeatureLayerPair();
}

int FeaturesModel::rowCount( const QModelIndex &parent ) const
{
  // For list models only the root node (an invalid parent) should return the list's size. For all
  // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
  if ( parent.isValid() )
    return 0;

  return mFeatures.count();
}

QVariant FeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFeatures.count() )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  const QgsQuickFeatureLayerPair feat = mFeatures.at( index.row() );

  switch ( role )
  {
    case FeatureTitle:
    {
      const QString title = mLoader.featureTitle( feat );
      if ( title.isEmpty() )
        return QVariant( feat.feature().id() );
      return QVariant( title );
    }
    case FeatureId: return QVariant( feat.feature().id() );
    case Description: return QVariant( QString( "Feature ID %1" ).arg( feat.feature().id() ) );
    case GeometryType: return QVariant( feat.feature().geometry().type() );
    case IconSource:
      switch ( feat.feature().geometry().type() )
      {
        case QgsWkbTypes::GeometryType::PointGeometry: return QVariant( "mIconPointLayer.svg" );
        case QgsWkbTypes::GeometryType::LineGeometry: return QVariant( "mIconLineLayer.svg" );
        case QgsWkbTypes::GeometryType::PolygonGeometry: return QVariant( "mIconPolygonLayer.svg" );

        case QgsWkbTypes::GeometryType::NullGeometry: // fall through
        case QgsWkbTypes::GeometryType::UnknownGeometry: return QVariant( "mIconTableLayer.svg" );
      }
    case FoundPair:
    {
      if ( mFilterExpression.isEmpty() )
        return QString();
      return QString("Something will be here");
    }
    default: return QVariant();
  }
}

QString FeaturesModel::buildFilterExpression()
{
  if ( mFilterExpression.isEmpty() || !mCurrentLayer )
    return QString();

  const QgsFields fields = mCurrentLayer->fields();
  QStringList expressionParts;

  bool filterExpressionIsNumeric;
  mFilterExpression.toInt( &filterExpressionIsNumeric );

  for ( const QgsField &field : fields )
  {
    if ( field.isNumeric() && filterExpressionIsNumeric )
      expressionParts << QStringLiteral( "%1 ~ '%2.*'" ).arg( QgsExpression::quotedColumnRef( field.name() ), QString::number( mFilterExpression.toInt() ) );
    else if ( field.type() == QVariant::String )
      expressionParts << QStringLiteral( "%1 ILIKE '%%2%'" ).arg( QgsExpression::quotedColumnRef( field.name() ), mFilterExpression );
    // TODO: Maybe add check for Date?
  }

  QString expression = QStringLiteral( "(%1)" ).arg( expressionParts.join( QStringLiteral( " ) OR ( " ) ) );

  return expression;
}

void FeaturesModel::reloadDataFromLayer( QgsVectorLayer *layer )
{
  emptyData();

  beginResetModel();

  if ( layer )
  {
    mCurrentLayer = layer;

    QgsFeatureRequest req;
    req.setFlags( QgsFeatureRequest::Flag::NoGeometry );

    if ( !mFilterExpression.isEmpty() )
    {
      QgsExpressionContextUtils::layerScope( layer );
      req.setFilterExpression( buildFilterExpression() );
    }

    req.setLimit( FEATURES_LIMIT );

    QgsFeatureIterator it = layer->getFeatures( req );
    QgsFeature f;

    while ( it.nextFeature( f ) )
    {
      mFeatures << QgsQuickFeatureLayerPair( f, layer );
    }

    setFeaturesCount( layer->featureCount() );
  }

  endResetModel();
}

void FeaturesModel::activeProjectChanged()
{
  emptyData();
}

void FeaturesModel::activeMapThemeChanged( const QString &mapTheme )
{
  Q_UNUSED( mapTheme )
  emptyData();
}

void FeaturesModel::emptyData()
{
  beginResetModel();

  mFeatures.clear();
  mCurrentLayer = nullptr;

  setFeaturesCount( 0 );

  endResetModel();
}

QHash<int, QByteArray> FeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[FeatureTitle] = QStringLiteral( "FeatureTitle" ).toLatin1();
  roleNames[FeatureId] = QStringLiteral( "FeatureId" ).toLatin1();
  roleNames[Description] = QStringLiteral( "Description" ).toLatin1();
  roleNames[IconSource] = QStringLiteral( "IconSource" ).toLatin1();
  roleNames[FoundPair] = QStringLiteral("FoundPair").toLatin1();
  return roleNames;
}

bool FeaturesModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  // Mocked method - for future when attributes will be editable (it changes data)
  Q_UNUSED( index );
  Q_UNUSED( value );
  Q_UNUSED( role );
  return false;
}

Qt::ItemFlags FeaturesModel::flags( const QModelIndex &index ) const
{
  // Mocked method - for future when attributes will be editable (it checks if data is editable)
  if ( !index.isValid() )
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable;
}

int FeaturesModel::featuresCount() const
{
  return mFeaturesCount;
}

void FeaturesModel::setFeaturesCount( int count )
{
  mFeaturesCount = count;

  if ( mFeaturesCount > FEATURES_LIMIT )
    emit tooManyFeaturesInLayer( FEATURES_LIMIT );

  emit featuresCountChanged( mFeaturesCount );
}

QString FeaturesModel::filterExpression() const
{
  return mFilterExpression;
}

void FeaturesModel::setFilterExpression( const QString &filterExpression )
{
  mFilterExpression = filterExpression;
  emit filterExpressionChanged( mFilterExpression );
  reloadDataFromLayer( mCurrentLayer );
}
