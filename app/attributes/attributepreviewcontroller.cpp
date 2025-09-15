/***************************************************************************
  attributepreviewcontroller.cpp
  --------------------------------------
  Date                 : 5.5.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "attributepreviewcontroller.h"
#include "featurelayerpair.h"
#include "qgsfield.h"
#include "qgsvectorlayer.h"
#include "qgsexpressioncontextutils.h"

AttributePreviewModel::AttributePreviewModel( const QVector<QPair<QString, QString>> &items )
  : QAbstractListModel( nullptr )
  , mItems( items )
{
}

QHash<int, QByteArray> AttributePreviewModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
  roles[AttributePreviewModel::Name]  = QByteArray( "Name" );
  roles[AttributePreviewModel::Value] = QByteArray( "Value" );
  return roles;
}

AttributePreviewModel::~AttributePreviewModel() = default;

int AttributePreviewModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mItems.size();
}

QVariant AttributePreviewModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  const int row = index.row();
  if ( row < 0 || row >= mItems.size() )
    return QVariant();

  switch ( role )
  {
    case AttributePreviewModel::Name:
      return mItems.at( row ).first;
    case AttributePreviewModel::Value:
      return mItems.at( row ).second;
    default:
      return QVariant();
  }
}

QVector<QPair<QString, QString>> AttributePreviewController::mapTipFields( )
{
  if ( !mFeatureLayerPair.layer() || !mFeatureLayerPair.feature().isValid() )
    return QVector<QPair<QString, QString>> ();

  const QString mapTip = mFeatureLayerPair.layer()->mapTipTemplate().replace( QStringLiteral( "\r" ), QString() );
  QVector<QPair<QString, QString>> lst;
  const QgsFields fields = mFeatureLayerPair.layer()->fields();

  if ( mapTip.isEmpty() )
  {
    // user has not provided any map tip - let's use first two fields to show
    // at least something.
    QString featureTitleExpression = mFeatureLayerPair.layer()->displayExpression();
    for ( const QgsField &field : fields )
    {
      if ( featureTitleExpression != field.name() )
      {
        const QPair<QString, QString> item = qMakePair(
                                               field.displayName(),
                                               mFeatureLayerPair.feature().attribute( field.name() ).toString()
                                             );

        lst.append( item );
      }

      if ( lst.count() == mLimit )
        break;
    }
  }
  else
  {
    // user has specified "# fields" on the first line and then each next line is a field name
    QStringList lines = mapTip.split( '\n' );
    for ( int i = 1; i < lines.count(); ++i ) // starting from index to avoid first line with "# fields"
    {
      const int index = fields.indexFromName( lines[i] );
      if ( index >= 0 )
      {
        const QString val = mFeatureLayerPair.feature().attribute( index ).toString();
        const QPair<QString, QString> item = qMakePair(
                                               fields[index].displayName(),
                                               val
                                             );

        lst.append( item );
      }
      if ( lst.count() == mLimit )
        break;
    }
  }
  return lst;
}

QString AttributePreviewController::mapTipImage()
{
  QgsExpressionContext context( globalProjectLayerScopes( mFeatureLayerPair.layer() ) );
  context.setFeature( mFeatureLayerPair.feature() );
  QString mapTip = mFeatureLayerPair.layer()->mapTipTemplate().remove( "# image\n" ); // first line is "# image"
  return QgsExpression::replaceExpressionText( mapTip, &context );
}

QString AttributePreviewController::mapTipHtml( )
{
  QgsExpressionContext context( globalProjectLayerScopes( mFeatureLayerPair.layer() ) );
  context.setFeature( mFeatureLayerPair.feature() );
  return QgsExpression::replaceExpressionText( mFeatureLayerPair.layer()->mapTipTemplate(), &context );
}

QString AttributePreviewController::featureTitle( )
{
  QgsExpressionContext context( globalProjectLayerScopes( mFeatureLayerPair.layer() ) );
  context.setFeature( mFeatureLayerPair.feature() );
  QgsExpression expr( mFeatureLayerPair.layer()->displayExpression() );

  QString title = expr.evaluate( &context ).toString();

  if ( title.isEmpty() )
  {
    // let's use a default format with layer name as user did not set any specific display expression
    if ( mFeatureLayerPair.layer() && mFeatureLayerPair.layer()->isValid() && mFeatureLayerPair.feature().isValid() )
    {
      QString layerName = mFeatureLayerPair.layer()->name();
      if ( layerName.isEmpty() )
      {
        layerName = tr( "Unnamed Layer" );
      }

      title = QStringLiteral( "%1 (%2)" ).arg( layerName ).arg( mFeatureLayerPair.feature().id() );
    }
  }

  return title;
}

QList<QgsExpressionContextScope *> AttributePreviewController::globalProjectLayerScopes( QgsMapLayer *layer )
{
  // can't use QgsExpressionContextUtils::globalProjectLayerScopes() because it uses QgsProject::instance()
  QList<QgsExpressionContextScope *> scopes;
  scopes << QgsExpressionContextUtils::globalScope();
  scopes << QgsExpressionContextUtils::projectScope( mProject );
  scopes << QgsExpressionContextUtils::layerScope( layer );
  return scopes;
}

AttributePreviewModel *AttributePreviewController::fieldModel() const
{
  return mFieldModel.get();
}

AttributePreviewController::AttributePreviewController( QObject *parent )
  : QObject( parent )
  , mFieldModel( new AttributePreviewModel() )
{
}

void AttributePreviewController::reset()
{
  setFeatureLayerPair( FeatureLayerPair() );
}

AttributePreviewController::~AttributePreviewController() = default;

void AttributePreviewController::setFeatureLayerPair( const FeatureLayerPair &pair )
{
  if ( mFeatureLayerPair != pair )
  {
    mFeatureLayerPair = pair;
    recalculate();
    emit featureLayerPairChanged();
  }
}

FeatureLayerPair AttributePreviewController::featureLayerPair() const
{
  return mFeatureLayerPair;
}

void AttributePreviewController::setProject( QgsProject *project )
{
  if ( mProject != project )
  {
    mProject = project;
    setFeatureLayerPair( FeatureLayerPair() );
    emit projectChanged();
  }
}

QgsProject *AttributePreviewController::project() const
{
  return mProject;
}

void AttributePreviewController::recalculate()
{
  mHtml.clear();
  mPhoto.clear();
  mTitle.clear();
  mType = AttributePreviewController::Empty;
  mFieldModel.reset( new AttributePreviewModel() );

  if ( !mFeatureLayerPair.layer() || !mFeatureLayerPair.feature().isValid() )
    return;

  mTitle = featureTitle();

  // Stripping extra CR char to unify Windows lines with Unix.
  QString mapTip = mFeatureLayerPair.layer()->mapTipTemplate().replace( QStringLiteral( "\r" ), QStringLiteral( "" ) );
  if ( mapTip.startsWith( "# image\n" ) )
  {
    mType = AttributePreviewController::Photo;
    mPhoto = mapTipImage();
  }
  else if ( mapTip.startsWith( "# fields\n" ) || mapTip.isEmpty() )
  {
    const QVector<QPair<QString, QString>> items = mapTipFields();
    if ( !items.empty() )
    {
      mType = AttributePreviewController::Fields;
      mFieldModel.reset( new AttributePreviewModel( items ) );
    }
  }
  else
  {
    mType = AttributePreviewController::HTML;
    mHtml = mapTipHtml();
  }
}

AttributePreviewController::PreviewType AttributePreviewController::type() const
{
  return mType;
}

QString AttributePreviewController::title() const
{
  return mTitle;
}

QString AttributePreviewController::photo() const
{
  return mPhoto;
}

QString AttributePreviewController::html() const
{
  return mHtml;
}
