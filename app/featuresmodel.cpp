/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "featuresmodel.h"

#include "inpututils.h"
#include "qgsexpressioncontextutils.h"
#include "coreutils.h"


FeaturesModel::FeaturesModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

FeaturesModel::~FeaturesModel() = default;

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

QHash<int, QByteArray> FeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[FeatureTitle] = QStringLiteral( "FeatureTitle" ).toLatin1();
  roleNames[FeatureId] = QStringLiteral( "FeatureId" ).toLatin1();
  roleNames[Feature] = QStringLiteral( "Feature" ).toLatin1();
  roleNames[FeaturePair] = QStringLiteral( "FeaturePair" ).toLatin1();
  roleNames[Description] = QStringLiteral( "Description" ).toLatin1();
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
}

int FeaturesModel::count() const
{
  return rowCount();
}
