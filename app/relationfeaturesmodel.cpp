/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "relationfeaturesmodel.h"
#include "inpututils.h"

RelationFeaturesModel::RelationFeaturesModel( QObject *parent )
  : FeaturesListModel( parent )
{
}

QVariant RelationFeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFeatures.count() )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  if ( role == PhotoPath )
  {
    const FeatureLayerPair pair = mFeatures.at( index.row() );
    return relationPhotoPath( pair );
  }
  else
    return FeaturesListModel::data( index, role );
}

QHash<int, QByteArray> RelationFeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roles = FeaturesListModel::roleNames();
  roles[PhotoPath] = QStringLiteral( "PhotoPath" ).toLatin1();

  return roles;
}

void RelationFeaturesModel::setup()
{
  emptyData();
}

void RelationFeaturesModel::populate()
{
  if ( !mRelation.isValid() || !mParentFeatureLayerPair.isValid() )
    return;

  beginResetModel();
  mFeatures.clear();

  QgsFeatureIterator it = mRelation.getRelatedFeatures( mParentFeatureLayerPair.feature() );
  QgsFeature feat;

  while ( it.nextFeature( feat ) )
  {
    mFeatures << FeatureLayerPair( feat, mRelation.referencingLayer() );
  }

  endResetModel();

  setIsTextType( photoFieldIndex( mRelation.referencingLayer() ) == -1 );
}

void RelationFeaturesModel::setParentFeatureLayerPair( FeatureLayerPair pair )
{
  if ( mParentFeatureLayerPair != pair )
  {
    mParentFeatureLayerPair = pair;
    emit parentFeatureLayerPairChanged( mParentFeatureLayerPair );

    populate();
  }
}

void RelationFeaturesModel::setRelation( QgsRelation relation )
{
  if ( mRelation.id() != relation.id() )
  {
    mRelation = relation;
    emit relationChanged( mRelation );

    populate();
  }
}

FeatureLayerPair RelationFeaturesModel::parentFeatureLayerPair() const
{
  return mParentFeatureLayerPair;
}

QgsRelation RelationFeaturesModel::relation() const
{
  return mRelation;
}

QVariant RelationFeaturesModel::relationPhotoPath( const FeatureLayerPair &featurePair ) const
{
  // Feature title used to get path of the referenced image.
  QString path = featureTitle( featurePair ).toString();

  int fieldIndex = photoFieldIndex( featurePair.layer() );
  QgsEditorWidgetSetup setup = featurePair.layer()->editorWidgetSetup( fieldIndex );
  QVariantMap config = setup.config();

  QString finalPath = InputUtils::resolvePath( path, homePath(), config, featurePair, QgsProject::instance() );

  return QVariant( finalPath );
}

int RelationFeaturesModel::photoFieldIndex( QgsVectorLayer *layer ) const
{

  QgsFields fields = layer->fields();
  for ( int i = 0; i < fields.size(); i++ )
  {
    // Lets try by widget type
    QgsEditorWidgetSetup setup = layer->editorWidgetSetup( i );
    QVariantMap config = setup.config();
    if ( setup.type() == QStringLiteral( "ExternalResource" ) )
    {
      return i;
    }
  }
  return -1;
}

bool RelationFeaturesModel::isTextType() const
{
  return mIsTextType;
}

void RelationFeaturesModel::setIsTextType( bool isTextType )
{
  if ( isTextType != mIsTextType )
  {
    mIsTextType = isTextType;
    emit isTextTypeChanged();
  }
}

QString RelationFeaturesModel::homePath() const
{
  return mHomePath;
}

void RelationFeaturesModel::setHomePath( const QString &homePath )
{
  if ( homePath != mHomePath )
  {
    mHomePath = homePath;
    emit homePathChanged();
  }
}
