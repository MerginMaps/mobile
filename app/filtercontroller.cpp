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
  for ( FieldFilter &filter : mFieldFilters )
  {
    filter.value.clear();
  }
  mFilteringEnabled = false;
  emit hasFiltersEnabledChanged();

  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vectorLayer )
    {
      vectorLayer->setSubsetString( QStringLiteral( "" ) );
    }
  }
}

void FilterController::loadFilterConfig( const QgsProject *project )
{
  mFieldFilters.clear();
  setFiltersEnabled( false );

  bool valueRead = false;
  const bool filteringAvailable = project->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "Filtering/Enabled" ), false, &valueRead );

  //return early if filtering is not setup
  if ( !valueRead )
  {
    return;
  }
  mFilteringAvailable = filteringAvailable;
  emit hasFiltersAvailableChanged();

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
      QString textValue = QgsExpression::quotedString( filter.value.toList().at( 0 ).toString() );
      // remove single quotes from the beginning and end of returned string
      textValue = textValue.slice( 1, textValue.size() - 2 );
      expressionCopy.replace( QStringLiteral( "@@value@@" ), textValue );
      break;
    }
    case FieldFilter::CheckboxFilter:
    case FieldFilter::SingleSelectFilter:
    {
      expressionCopy.replace( QStringLiteral( "@@value@@" ), QgsExpression::quotedValue( filter.value.toList().at( 0 ) ) );
      break;
    }
    case FieldFilter::NumberFilter:
    {
      const QVariant &variantFrom = filter.value.toList().at( 0 );
      const QString valueFrom = variantFrom.isValid() ? variantFrom.toString() : QString::number( std::numeric_limits<int>::min() );
      const QVariant &variantTo = filter.value.toList().at( 1 );
      const QString valueTo = variantTo.isValid() ? variantTo.toString() : QString::number( std::numeric_limits<int>::max() );

      expressionCopy.replace( QStringLiteral( "@@value_from@@" ), valueFrom );
      expressionCopy.replace( QStringLiteral( "@@value_to@@" ), valueTo );
      break;
    }
    case FieldFilter::DateFilter:
    {
      // GeoPackage stores datetimes as timezone-naive strings (effectively UTC),
      // so we must convert local datetimes to UTC before comparing.
      // Use a custom format to avoid the 'Z' suffix that Qt::ISODate adds for UTC.
      const QString isoFormat = QStringLiteral( "yyyy-MM-ddTHH:mm:ss.zzz" );
      const QString minimumDateTime = QStringLiteral( "0001-01-01T00:00:00.000" );
      const QString maximumDateTime = QStringLiteral( "9999-12-31T23:59:59.999" );

      QString dateFrom;
      const QVariant &variantFrom = filter.value.toList().at( 0 );
      if ( variantFrom.isValid() )
      {
        QDateTime dateTimeFrom = variantFrom.toDateTime( );
        QTime timeFrom = dateTimeFrom.time();
        timeFrom.setHMS( timeFrom.hour(), timeFrom.minute(), 0 );
        dateTimeFrom.setTime( timeFrom );
        dateFrom = dateTimeFrom.toString( isoFormat );
      }
      else
      {
        dateFrom = minimumDateTime;
      }

      const QVariant &variantTo = filter.value.toList().at( 1 );
      QString dateTo;
      if ( variantTo.isValid() )
      {
        if ( variantTo.toDateTime().time().hour() > 0 || variantTo.toDateTime().time().minute() > 0 )
        {
          QDateTime dateTimeTo = variantFrom.toDateTime( );
          QTime timeFrom = dateTimeTo.time();
          timeFrom.setHMS( timeFrom.hour(), timeFrom.minute(), 59, 999 );
          dateTimeTo.setTime( timeFrom );
          dateTo = dateTimeTo.toString( isoFormat );
        }
        else
        {
          dateTo = variantFrom.toDateTime().toString( isoFormat );
        }
      }
      else
      {
        dateTo = maximumDateTime;
      }

      expressionCopy.replace( QStringLiteral( "@@value_from@@" ), dateFrom );
      expressionCopy.replace( QStringLiteral( "@@value_to@@" ), dateTo );
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

      QStringList expressions;
      for ( const QVariant &v : values )
      {
        QString expressionTemplate = expressionCopy;
        expressionTemplate.replace( QStringLiteral( "@@value@@" ), QgsExpression::quotedValue( v ) );
        expressions << QStringLiteral( "(%1)" ).arg( expressionTemplate );
      }
      expressionCopy =  expressions.join( QStringLiteral( " OR " ) );
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
    if ( filter.layerId != layerId || !filter.value.isValid() ) continue;

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

  if ( !filterExpr.isEmpty() && success && !mFilteringEnabled )
  {
    mFilteringEnabled = true;
    emit hasFiltersEnabledChanged();
  }

  qDebug() << "Applied filter to layer" << layer->name() << ":" << filterExpr << "success:" << success;

  // Trigger a layer refresh to ensure the filter takes effect
  if ( success )
  {
    layer->triggerRepaint();
  }
}

void FilterController::applyFiltersToAllLayers()
{
  // Change filters enabled to false before enabling filters to find out if any are active
  mFilteringEnabled = false;
  emit hasFiltersEnabledChanged();

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
    if ( newFilters.contains( filter.filterId ) )
    {
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

bool FilterController::isDateFilterDateTime( const QString &filterId )
{
  for ( FieldFilter &filter : mFieldFilters )
  {
    if ( filter.filterId == filterId )
    {
      const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayer( filter.layerId ) );
      const QMetaType::Type fieldType = layer->fields().field( filter.fieldName ).type();
      return fieldType == QMetaType::QDateTime;
    }
  }
  return false;
}

QVariantMap FilterController::getCheckboxConfiguration( const QString &filterId )
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

  if ( fieldFilter.filterType != FieldFilter::CheckboxFilter ) return {};

  const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayer( fieldFilter.layerId ) );
  if ( !layer ) return {};

  const int fieldIndex = layer->fields().lookupField( fieldFilter.fieldName );
  if ( fieldIndex < 0 ) return {};

  const QgsEditorWidgetSetup widgetSetup = layer->editorWidgetSetup( fieldIndex );
  if ( QString::compare( widgetSetup.type(), QStringLiteral( "CheckBox" ), Qt::CaseInsensitive ) != 0 ) return {};

  const QVariantMap config = widgetSetup.config();
  const QString checkedState = config.value( QStringLiteral( "CheckedState" ) ).toString();
  const QString uncheckedState = config.value( QStringLiteral( "UncheckedState" ) ).toString();

  if ( checkedState.isEmpty() && uncheckedState.isEmpty() ) return {};

  const QMetaType::Type fieldType = static_cast<QMetaType::Type>( layer->fields().field( fieldIndex ).type() );
  const bool isIntField = ( fieldType == QMetaType::Int || fieldType == QMetaType::UInt ||
                            fieldType == QMetaType::LongLong || fieldType == QMetaType::ULongLong );

  QVariantMap result;

  if ( !checkedState.isEmpty() )
  {
    result[QStringLiteral( "customLabelForTrue" )] = checkedState;
    bool ok = false;
    const int intVal = checkedState.toInt( &ok );
    result[QStringLiteral( "customValueForTrue" )] = ( isIntField && ok ) ? QVariant( intVal ) : QVariant( checkedState );
  }

  if ( !uncheckedState.isEmpty() )
  {
    result[QStringLiteral( "customLabelForFalse" )] = uncheckedState;
    bool ok = false;
    const int intVal = uncheckedState.toInt( &ok );
    result[QStringLiteral( "customValueForFalse" )] = ( isIntField && ok ) ? QVariant( intVal ) : QVariant( uncheckedState );
  }

  return result;
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
  const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( project->mapLayers().value( fieldFilter.layerId ) );

  if ( !layer ) return {};

  const int fieldIndex = layer->fields().lookupField( fieldFilter.fieldName );
  const QgsEditorWidgetSetup fieldConfig = layer->editorWidgetSetup( fieldIndex );

  if ( QString::compare( fieldConfig.type(), QStringLiteral( "ValueMap" ), Qt::CaseInsensitive ) == 0 )
  {
    map["type"] = QStringLiteral( "value_map" );
    map["config"] = fieldConfig.config();
  }
  else if ( QString::compare( fieldConfig.type(), QStringLiteral( "ValueRelation" ), Qt::CaseInsensitive ) == 0 )
  {
    map["type"] = QStringLiteral( "value_relation" );
    map["config"] = fieldConfig.config();
  }
  else
  {
    map["type"] = QStringLiteral( "unique_values" );
    map["layer_id"] = fieldFilter.layerId;
    map["field_name"] = fieldFilter.fieldName;
  }

  return map;
}
