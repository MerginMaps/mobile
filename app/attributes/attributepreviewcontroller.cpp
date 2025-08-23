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
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QLocale>

#include "attributepreviewcontroller.h"
#include "featurelayerpair.h"
#include "qgsfield.h"
#include "qgsvectorlayer.h"
#include "qgsexpressioncontextutils.h"
#include "qgseditorwidgetsetup.h"


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

  QString mapTip = mFeatureLayerPair.layer()->mapTipTemplate();
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
        const int idx = fields.indexFromName( field.name() );
        const QVariant raw = mFeatureLayerPair.feature().attribute( idx );

        // Use the editor widget setup to retrieve the same display format the form uses.
        // DO NOT use .toString() directly for date-time values- that can show raw UTC/ISO.
        // This keeps the preview and the editor in perfect sync for locale and timezone.
        const QgsEditorWidgetSetup ew = mFeatureLayerPair.layer()->editorWidgetSetup( idx );
        const QString pretty = formatDateForPreview( fields[idx], raw, ew.config() );

        const QPair<QString, QString> item = qMakePair( field.displayName(), pretty );

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
      int index = fields.indexFromName( lines[i] );
      if ( index >= 0 )
      {
        // Type-aware formatting (dates in local time, honor display_format)
        const QVariant raw = mFeatureLayerPair.feature().attribute( index );
        const QgsEditorWidgetSetup ew = mFeatureLayerPair.layer()->editorWidgetSetup( index );
        const QString pretty = formatDateForPreview( fields[index], raw, ew.config() );

        const QPair<QString, QString> item = qMakePair( fields[index].displayName(), pretty );

        lst.append( item );
      }
      if ( lst.count() == mLimit )
        break;
    }
  }
  return lst;
}

QString AttributePreviewController::formatDateForPreview( const QgsField &field,
    const QVariant &value,
    const QVariantMap &fieldCfg ) const
{
  const QString displayFmt = fieldCfg.value( QStringLiteral( "display_format" ) ).toString();

  //fallback value as raw QString
  const QString fallback = value.toString();

  //QDate
  if ( field.type() == QMetaType::QDate )
  {
    QDate date;
    if ( value.canConvert<QDate>() )
    {
      date = value.toDate();
    }

    else if ( value.userType() == QMetaType::QString )
    {
      date = QDate::fromString( value.toString(), Qt::ISODate );
    }

    if ( !date.isValid() )
    {
      return fallback;
    }

    if ( displayFmt.isEmpty() )
    {
      return QLocale().toString( date, QLocale::ShortFormat );
    }

    return date.toString( displayFmt );
  }

  //QTime
  if ( field.type() == QMetaType::QTime )
  {
    QTime time;
    if ( value.canConvert<QTime>() )
    {
      time = value.toTime();
    }

    else if ( value.userType() == QMetaType::QString )
    {
      time = QTime::fromString( value.toString(), Qt::ISODate );
    }

    if ( !time.isValid() )
    {
      return fallback;
    }

    const QString fmt = displayFmt.isEmpty() ? QStringLiteral( "HH:mm:ss" ) : displayFmt;
    return time.toString( fmt );
  }

  //QDateTime
  if ( field.type() == QMetaType::QDateTime )
  {
    QDateTime dateTime;
    if ( value.canConvert<QDateTime>() )
    {
      dateTime = value.toDateTime();
    }
    else if ( value.userType() == QMetaType::QString )
    {
      dateTime = QDateTime::fromString( value.toString(), Qt::ISODateWithMs );

      if ( !dateTime.isValid() )
      {
        dateTime = QDateTime::fromString( value.toString(), Qt::ISODate );
      }
    }

    if ( !dateTime.isValid() )
    {
      return fallback;
    }

    // IMPORTANT If the source was UTC (ex., "...Z"), convert to local so the preview
    if ( dateTime.timeSpec() != Qt::LocalTime )
    {
      dateTime = dateTime.toLocalTime();
    }

    //force LocalTime to prevent Qt from re-attaching an offset during format
    // on some platforms the spec remains "OffsetFromUTC" or "UTC".
    dateTime.setTimeSpec( Qt::LocalTime );

    // We use the editor widget's display format so the preview obeys the same way
    // formatting rules as the form editor "keeps UX consistent".
    const QString fmt = displayFmt.isEmpty() ? QStringLiteral( "yyyy-MM-dd HH:mm:ss" ) : displayFmt;
    return dateTime.toString( fmt );
  }

  return fallback;
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
