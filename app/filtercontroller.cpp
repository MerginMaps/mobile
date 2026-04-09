/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "filtercontroller.h"

#include <qgsvectorlayer.h>
#include <qgsproject.h>
#include <qgsexpression.h>
#include <qgseditorwidgetsetup.h>
#include <qgsvaluerelationfieldformatter.h>
#include <qgsfeaturerequest.h>

#include <QDateTime>
#include <QDebug>
#include <QJsonArray>

#include "coreutils.h"


FilterController::FilterController( QObject *parent )
  : QObject( parent )
{
}

void FilterController::clearLayerFilters( const QString &layerId )
{
  QgsMapLayer *layer = QgsProject::instance()->mapLayers().value( layerId );
  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
  vectorLayer->setSubsetString( QStringLiteral( "" ) );

  for ( FieldFilter filter : mFieldFilters )
  {
    if ( filter.layerId == layerId )
    {
      filter.value.clear();
    }
  }
}

void FilterController::clearAllFilters()
{
  mFieldFilters.clear();

  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vectorLayer )
    {
      vectorLayer->setSubsetString(QStringLiteral(""));
    }
  }
}

void FilterController::loadFilterConfig( const QgsProject *project )
{
  mFieldFilters.clear();

  bool valueRead = false;
  const bool filteringEnabled = project->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "Filtering/Enabled" ), false, &valueRead );

  //return early if filtering is not setup
  if ( !valueRead )
  {
    return;
  }
  mFilteringEnabled = filteringEnabled;

  const QString filtersDef = project->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "Filtering/Filters" ) );
  QJsonParseError jsonError;
  const QJsonDocument filtersRaw = QJsonDocument::fromJson( filtersDef.toUtf8(), &jsonError );
  if ( jsonError.error != QJsonParseError::NoError )
  {
    CoreUtils::log( QStringLiteral( "Feature Filtering" ), QStringLiteral( "Could not parse filters from json document." ) );
    return;
  }

  if ( !filtersRaw.isEmpty() && filtersRaw.isArray() )
  {
    const QJsonArray filtersArray = filtersRaw.array();
    for ( auto filter = filtersArray.constBegin(); filter != filtersArray.constEnd(); ++filter )
    {
      FieldFilter newFieldFilter;
      QJsonObject filterObject = filter->toObject();

      newFieldFilter.filterId = QUuid::createUuid().toString( QUuid::WithoutBraces );

      newFieldFilter.filterName = filterObject.value( QStringLiteral( "filter_name" ) ).toString();

      QString filterTypeRaw = filterObject.value( QStringLiteral( "filter_type" ) ).toString();
      if ( filterTypeRaw == QStringLiteral( "Text" ) )
      {
        newFieldFilter.filterType = FieldFilter::TextFilter;
      }
      else if ( filterTypeRaw == QStringLiteral( "Number" ) )
      {
        newFieldFilter.filterType = FieldFilter::NumberFilter;
      }
      else if ( filterTypeRaw == QStringLiteral( "Date" ) )
      {
        newFieldFilter.filterType = FieldFilter::DateFilter;
      }
      else if ( filterTypeRaw == QStringLiteral( "Checkbox" ) )
      {
        newFieldFilter.filterType = FieldFilter::CheckboxFilter;
      }
      else if ( filterTypeRaw == QStringLiteral( "Single select" ) )
      {
        newFieldFilter.filterType = FieldFilter::SingleSelectFilter;
      }
      else if ( filterTypeRaw == QStringLiteral( "Multi select" ) )
      {
        newFieldFilter.filterType = FieldFilter::MultiSelectFilter;
      }

      newFieldFilter.fieldName = filterObject.value( QStringLiteral( "field_name" ) ).toString();
      newFieldFilter.provider = filterObject.value( QStringLiteral( "provider" ) ).toString();
      newFieldFilter.sqlExpression = filterObject.value( QStringLiteral( "sql_expression" ) ).toString();
      newFieldFilter.layerId = filterObject.value( QStringLiteral( "layer_id" ) ).toString();

      mFieldFilters.append( newFieldFilter );
    }
  }
}

QString FilterController::buildFieldExpression( const FieldFilter &filter ) const
{
  QString expressionCopy = filter.sqlExpression;
  switch ( filter.filterType )
  {
    case FieldFilter::TextFilter:
    {
      const QString textValue = filter.value.toList().at( 0 ).toString();
      expressionCopy.replace( QStringLiteral( "%%value%%" ), QgsExpression::quotedString( textValue ) );
      break;
    }
    case FieldFilter::CheckboxFilter:
    case FieldFilter::SingleSelectFilter:
    {
      expressionCopy.replace( QStringLiteral( "%%value%%" ), QgsExpression::quotedValue( filter.value.toList().at( 0 ) ) );
      break;
    }
    case FieldFilter::NumberFilter:
    {
      const QString valueFrom = filter.value.toList().at( 0 ).toString();
      const QString valueTo = filter.value.toList().at( 1 ).toString();

      if ( valueFrom.isEmpty() || valueTo.isEmpty() )
      {
        expressionCopy = {};
        break;
      }

      expressionCopy.replace( QStringLiteral( "%%value_from%%" ), valueFrom );
      expressionCopy.replace( QStringLiteral( "%%value_to%%" ), valueTo );
      break;
    }
    case FieldFilter::DateFilter:
    {
      // GeoPackage stores datetimes as timezone-naive strings (effectively UTC),
      // so we must convert local datetimes to UTC before comparing.
      // Use a custom format to avoid the 'Z' suffix that Qt::ISODate adds for UTC.
      const QString isoFormat = QStringLiteral( "yyyy-MM-ddTHH:mm:ss" );
      const QString dateFrom = filter.value.toList().at( 0 ).toDateTime().toUTC().toString( isoFormat );
      const QString dateTo = filter.value.toList().at( 1 ).toDateTime().toUTC().toString( isoFormat );

      if ( dateFrom.isEmpty() || dateTo.isEmpty() )
      {
        expressionCopy = {};
        break;
      }

      expressionCopy.replace( QStringLiteral( "%%value_from%%" ), QgsExpression::quotedString( dateFrom ) );
      expressionCopy.replace( QStringLiteral( "%%value_to%%" ), QgsExpression::quotedString( dateTo ) );
      break;
    }
    case FieldFilter::MultiSelectFilter:
    {
      const QVariantList values = filter.value.toList();
      if ( values.isEmpty() )
      {
        expressionCopy = {};
        break;
      }

      QStringList quotedValues;
      for ( const QVariant &v : values )
      {
        quotedValues << QgsExpression::quotedValue( v );
      }
      expressionCopy.replace( QStringLiteral( "%%values%%" ), quotedValues.join( QStringLiteral( ", " ) ) );
      break;
    }
  }

  return expressionCopy;
}

QString FilterController::generateFilterExpression( const QString &layerId ) const
{
  QStringList expressions;

  for ( const FieldFilter &filter : mFieldFilters )
  {
    if ( filter.layerId != layerId || !filter.value.isValid()) continue;

    QString expr = buildFieldExpression( filter );
    if ( !expr.isEmpty() )
    {
      expressions << QStringLiteral( "(%1)" ).arg( expr );
    }
  }

  if ( expressions.isEmpty() )
    return {};

  return expressions.join( QStringLiteral( " AND " ) );
}

void FilterController::applyFiltersToLayer( QgsVectorLayer *layer )
{
  if ( !layer )
    return;

  const QString filterExpr = generateFilterExpression( layer->id() );
  const bool success = layer->setSubsetString( filterExpr );

  qDebug() << "Applied filter to layer" << layer->name() << ":" << filterExpr << "success:" << success;

  // Trigger a layer refresh to ensure the filter takes effect
  if ( success )
  {
    layer->triggerRepaint();
  }
}

void FilterController::applyFiltersToAllLayers()
{
  const bool hadFilters = mFilteringEnabled;

  const QgsProject *project = QgsProject::instance();
  if ( !project )
    return;

  const QMap<QString, QgsMapLayer *> layers = project->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vectorLayer )
    {
      applyFiltersToLayer( vectorLayer );
    }
  }

  //TODO: probably can be removed
  emit filtersChanged();

  if ( hadFilters != mFilteringEnabled )
  {
    emit hasFiltersEnabledChanged();
  }
}

bool FilterController::hasFiltersAvailable() const
{
  return mFilteringAvailable;
}

bool FilterController::hasFiltersEnabled() const
{
  return mFilteringEnabled;
}

void FilterController::setFiltersEnabled( const bool filtersEnabled )
{
  if ( mFilteringEnabled != filtersEnabled )
  {
    mFilteringEnabled = filtersEnabled;
    emit hasFiltersEnabledChanged();
  }
}

QVariantList FilterController::getFilters() const
{
  QVariantList uiFilters;
  for ( const FieldFilter &filter : mFieldFilters )
  {
    QVariantMap filterLite;
    filterLite.insert( QStringLiteral( "filterName" ), filter.filterName );
    filterLite.insert( QStringLiteral( "filterType" ), filter.filterType );
    filterLite.insert( QStringLiteral( "filterId" ), filter.filterId );
    filterLite.insert( QStringLiteral( "value" ), filter.value );

    uiFilters.append( filterLite );
  }
  return uiFilters;
}

void FilterController::processFilters( const QVariantMap &newFilters )
{
  // save all newFilter values to mFieldFilters values
  for ( FieldFilter &filter : mFieldFilters )
  {
    if ( newFilters.contains(filter.filterId) )
    {
      //TODO: we need to have both upper and lower bounds for numbers and dates,
      //if user didn't supply use numeric_limits for numbers and year 1 to 9999 for dates
      filter.value = newFilters.value( filter.filterId );
    }
  }

  applyFiltersToAllLayers();
}

bool FilterController::hasActiveFilterOnLayer( const QString &layerId )
{
  const QgsProject *project = QgsProject::instance();
  if ( !project )
    return false;

  const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( project->mapLayers().value( layerId ) );
  return !layer->subsetString().isEmpty();
}

QVariantMap FilterController::getDropdownConfiguration( const QString &filterId )
{
  if ( filterId.isEmpty() ) return {};

  FieldFilter fieldFilter;
  for ( const FieldFilter &filter : std::as_const( mFieldFilters ) )
  {
    if ( filterId == filter.filterId )
    {      
      fieldFilter = filter;
      break;
    }
  }

  if ( !fieldFilter.hasFilterInfo() ) return {};

  if ( fieldFilter.filterType != FieldFilter::SingleSelectFilter && fieldFilter.filterType != FieldFilter::MultiSelectFilter ) return {};

  QVariantMap map;

  const QgsProject *project = QgsProject::instance();

  if ( !project ) return {};

  const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( project->mapLayers().value( fieldFilter.layerId ) );

  if ( !layer ) return {};

  const int fieldIndex = layer->fields().lookupField( fieldFilter.fieldName );
  const QgsEditorWidgetSetup fieldConfig = layer->editorWidgetSetup( fieldIndex );

  if ( QString::compare( fieldConfig.type(), QStringLiteral( "ValueMap" ), Qt::CaseInsensitive ) == 0 )
  {
    map["type"] = QStringLiteral("value_map");
    map["config"] = fieldConfig.config();
  }
  else if ( QString::compare( fieldConfig.type(), QStringLiteral( "ValueRelation" ), Qt::CaseInsensitive ) == 0 )
  {
    map["type"] = QStringLiteral("value_relation");
    map["config"] = fieldConfig.config();
  }
  else
  {
    map["type"] = QStringLiteral("unique_values");
    map["layer_id"] = fieldFilter.layerId;
    map["field_name"] = fieldFilter.fieldName;
  }

  return map;
}
