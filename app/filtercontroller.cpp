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
  QgsMapLayer *layer = QgsProject::instance()->mapLayer( layerId );
  QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( layer );
  if ( vectorLayer )
  {
    if ( mPredefinedSubsetStrings.contains( vectorLayer->id() ) )
    {
      vectorLayer->setSubsetString( mPredefinedSubsetStrings[ vectorLayer->id() ] );
    }
    else
    {
      vectorLayer->setSubsetString( QStringLiteral( "" ) );
    }
  }

  for ( FieldFilter &filter : mFieldFilters )
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
  mFilteringActivated = false;
  emit hasFiltersActivatedChanged();

  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vectorLayer )
    {
      if ( mPredefinedSubsetStrings.contains( vectorLayer->id() ) )
      {
        vectorLayer->setSubsetString( mPredefinedSubsetStrings[ vectorLayer->id() ] );
      }
      else
      {
        vectorLayer->setSubsetString( QStringLiteral( "" ) );
      }
    }
  }
}

void FilterController::loadFilterConfig( const QgsProject *project )
{
  mFieldFilters.clear();

  setFiltersActivated( false );

  bool valueRead = false;
  const bool filteringAvailable = project->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "Filtering/Enabled" ), false, &valueRead );

  mFilteringAvailable = filteringAvailable;
  emit hasFiltersAvailableChanged();

  //return early if filtering is not setup
  if ( !filteringAvailable )
  {
    return;
  }

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
      else
      {
        CoreUtils::log( QStringLiteral( "Feature filtering" ), QStringLiteral( "Unknown type of filter specified for filter %1" ).arg( filterObject.value( QStringLiteral( "field_name" ) ).toString() ) );
        continue;
      }

      newFieldFilter.fieldName = filterObject.value( QStringLiteral( "field_name" ) ).toString();
      newFieldFilter.provider = filterObject.value( QStringLiteral( "provider" ) ).toString();
      newFieldFilter.sqlExpression = filterObject.value( QStringLiteral( "sql_expression" ) ).toString();
      newFieldFilter.layerId = filterObject.value( QStringLiteral( "layer_id" ) ).toString();

      // check for missing filter fields
      QStringList missingFilterFields;
      if ( newFieldFilter.layerId.isEmpty() ) missingFilterFields << QStringLiteral( "'layer_id'" );
      if ( newFieldFilter.fieldName.isEmpty() ) missingFilterFields << QStringLiteral( "'field_name'" );
      if ( newFieldFilter.sqlExpression.isEmpty() ) missingFilterFields << QStringLiteral( "'sql_expression'" );

      if ( !missingFilterFields.isEmpty() )
      {
        CoreUtils::log( QStringLiteral( "Feature Filtering" ),
                        QStringLiteral( "Filter '%1' is missing required filter field(s): %2. Skipping." )
                        .arg( newFieldFilter.filterName, missingFilterFields.join( QStringLiteral( ", " ) ) ) );
        continue;
      }

      // check if target layer exists
      const QgsVectorLayer *filterLayer = qobject_cast<QgsVectorLayer *>( project->mapLayer( newFieldFilter.layerId ) );
      if ( !filterLayer )
      {
        CoreUtils::log( QStringLiteral( "Feature Filtering" ),
                        QStringLiteral( "Filter '%1' has no layer with ID '%2' found in project. Skipping." )
                        .arg( newFieldFilter.filterName, newFieldFilter.layerId ) );
        continue;
      }

      // check if target field of target layer exists
      if ( filterLayer->fields().lookupField( newFieldFilter.fieldName ) < 0 )
      {
        CoreUtils::log( QStringLiteral( "Feature Filtering" ),
                        QStringLiteral( "Filter '%1' has no target field '%2' found on layer '%3' (%4). Skipping." )
                        .arg( newFieldFilter.filterName, newFieldFilter.fieldName,
                              filterLayer->name(), newFieldFilter.layerId ) );
        continue;
      }

      mFieldFilters.append( newFieldFilter );
    }
  }

  const QMap<QString, QgsMapLayer *> layers = project->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    const QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vectorLayer  && !vectorLayer->subsetString().isEmpty() )
    {
      mPredefinedSubsetStrings.insert( vectorLayer->id(), vectorLayer->subsetString() );
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
    {
      expressionCopy.replace( QStringLiteral( "@@value@@" ), QgsExpression::quotedValue( filter.value.toList().at( 0 ) ) );
      break;
    }
    case FieldFilter::SingleSelectFilter:
    {
      // check if the value is NULL and if it is search for both NULL and empty string
      if ( QgsVariantUtils::isNull( filter.value.toList().at( 0 ) ) )
      {
        QStringList expressions;
        QString expressionTemplate( expressionCopy );
        expressionTemplate.replace( QStringLiteral( "@@value@@" ), QStringLiteral( "NULL" ) );
        expressions << QStringLiteral( "(%1)" ).arg( expressionTemplate );
        expressionTemplate = QString( expressionCopy );
        expressionTemplate.replace( QStringLiteral( "@@value@@" ), QStringLiteral( "''" ) );
        expressions << QStringLiteral( "(%1)" ).arg( expressionTemplate );

        expressionCopy =  expressions.join( QStringLiteral( " OR " ) );
      }
      else
      {
        expressionCopy.replace( QStringLiteral( "@@value@@" ), QgsExpression::quotedValue( filter.value.toList().at( 0 ) ) );
      }
      break;
    }
    case FieldFilter::NumberFilter:
    {
      const QVariantList values = filter.value.toList();
      if ( values.size() < 2 )
        return {};
      const QVariant variantFrom = values.at( 0 );
      const QString valueFrom = variantFrom.isValid() ? variantFrom.toString() : QString::number( std::numeric_limits<int>::min() );
      const QVariant variantTo = values.at( 1 );
      const QString valueTo = variantTo.isValid() ? variantTo.toString() : QString::number( std::numeric_limits<int>::max() );

      expressionCopy.replace( QStringLiteral( "@@value_from@@" ), valueFrom );
      expressionCopy.replace( QStringLiteral( "@@value_to@@" ), valueTo );
      break;
    }
    case FieldFilter::DateFilter:
    {
      const QVariantList values = filter.value.toList();
      if ( values.size() < 2 )
        return {};
      const QVariant &variantFrom = values.at( 0 );
      const QVariant &variantTo = values.at( 1 );

      QString dateFrom;
      QString dateTo;

      if ( isDateFilterDateTime( filter.filterId ) )
      {
        // GeoPackage stores datetimes as timezone-naive strings (effectively UTC),
        // so we must convert local datetimes to UTC before comparing.
        // Use a custom format to avoid the 'Z' suffix that Qt::ISODate adds for UTC.
        const QString isoFormat = QStringLiteral( "yyyy-MM-ddTHH:mm:ss.zzz" );
        const QString maximumDateTime = QStringLiteral( "9999-12-31T23:59:59.999" );
        const QString minimumDateTime = QStringLiteral( "0001-01-01T00:00:00.000" );

        if ( variantFrom.isValid() )
        {
          QDateTime dateTimeFrom = variantFrom.toDateTime().toUTC();
          QTime timeFrom = dateTimeFrom.time();
          timeFrom.setHMS( timeFrom.hour(), timeFrom.minute(), 0 );
          dateTimeFrom.setTime( timeFrom );
          dateFrom = dateTimeFrom.toString( isoFormat );
        }
        else
        {
          dateFrom = minimumDateTime;
        }

        if ( variantTo.isValid() )
        {
          QDateTime dateTimeTo = variantTo.toDateTime().toUTC();
          QTime timeTo = dateTimeTo.time();
          timeTo.setHMS( timeTo.hour(), timeTo.minute(), 59, 999 );
          dateTimeTo.setTime( timeTo );
          dateTo = dateTimeTo.toString( isoFormat );
        }
        else
        {
          dateTo = maximumDateTime;
        }
      }
      else
      {
        // date-only fields store values as YYYY-MM-DD strings — no time component
        const QString dateFormat = QStringLiteral( "yyyy-MM-dd" );

        dateFrom = variantFrom.isValid() ? variantFrom.toDateTime().toString( dateFormat ) : QStringLiteral( "0001-01-01" );
        dateTo = variantTo.isValid() ? variantTo.toDateTime().toString( dateFormat ) : QStringLiteral( "9999-12-31" );
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
        QString expressionTemplate( expressionCopy );
        if ( QgsVariantUtils::isNull( v ) )
        {
          expressionTemplate.replace( QStringLiteral( "@@value@@" ), QStringLiteral( "NULL" ) );
          expressions << QStringLiteral( "(%1)" ).arg( expressionTemplate );
          expressionTemplate = QString( expressionCopy );
          expressionTemplate.replace( QStringLiteral( "@@value@@" ), QStringLiteral( "''" ) );
          expressions << QStringLiteral( "(%1)" ).arg( expressionTemplate );
        }
        else
        {
          expressionTemplate.replace( QStringLiteral( "@@value@@" ), QgsExpression::quotedValue( v ) );
          expressions << QStringLiteral( "(%1)" ).arg( expressionTemplate );
        }
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

  // prepend any subset string defined in QGIS
  if ( mPredefinedSubsetStrings.contains( layerId ) )
  {
    expressions << mPredefinedSubsetStrings[ layerId ];
  }

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

  qDebug() << "Applied filter to layer" << layer->name() << ":" << filterExpr << "success:" << success;

  // Trigger a layer refresh to ensure the filter takes effect
  if ( success )
  {
    layer->triggerRepaint();
  }
}

void FilterController::applyFiltersToAllLayers()
{
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

bool FilterController::hasFiltersActivated() const
{
  return mFilteringActivated;
}

void FilterController::setFiltersActivated( const bool filtersEnabled )
{
  if ( mFilteringActivated != filtersEnabled )
  {
    mFilteringActivated = filtersEnabled;
    emit hasFiltersActivatedChanged();
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

  bool anyActivated = false;
  for ( const QVariant &value : std::as_const( newFilters ) )
  {
    if ( value.isValid() && !value.isNull() )
    {
      anyActivated = true;
      break;
    }
  }
  setFiltersActivated( anyActivated );
}

bool FilterController::hasActiveFilterOnLayer( const QString &layerId )
{
  const QgsProject *project = QgsProject::instance();
  if ( !project )
    return false;

  const QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( project->mapLayers().value( layerId ) );
  return !layer->subsetString().isEmpty();
}

bool FilterController::isDateFilterDateTime( const QString &filterId ) const
{
  for ( const FieldFilter &filter : mFieldFilters )
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
