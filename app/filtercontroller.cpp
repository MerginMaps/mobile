/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "filtercontroller.h"

#include "qgsvectorlayer.h"
#include "qgsproject.h"
#include "qgsexpression.h"
#include "qgsfield.h"
#include "qgseditorwidgetsetup.h"
#include "qgsvaluerelationfieldformatter.h"
#include "qgsfeaturerequest.h"

#include <QDateTime>
#include <QDebug>


FilterController::FilterController( QObject *parent )
  : QObject( parent )
{
}

bool FilterController::hasActiveFilters() const
{
  for ( auto it = mAppliedFilters.constBegin(); it != mAppliedFilters.constEnd(); ++it )
  {
    if ( !it.value().isEmpty() )
    {
      return true;
    }
  }
  return false;
}

QStringList FilterController::filteredLayerIds() const
{
  QStringList ids;
  for ( auto it = mAppliedFilters.constBegin(); it != mAppliedFilters.constEnd(); ++it )
  {
    if ( !it.value().isEmpty() )
    {
      ids << it.key();
    }
  }
  return ids;
}

void FilterController::setFieldFilter( const QString &layerId, const QString &fieldName,
                                       const QString &filterType, const QVariant &value,
                                       const QVariant &valueTo )
{
  FieldFilter filter;
  filter.fieldName = fieldName;
  filter.filterType = filterType;
  filter.value = value;
  filter.valueTo = valueTo;

  if ( !filter.isValid() )
  {
    removeFieldFilter( layerId, fieldName );
    return;
  }

  mFilters[layerId][fieldName] = filter;
}

void FilterController::removeFieldFilter( const QString &layerId, const QString &fieldName )
{
  if ( !mFilters.contains( layerId ) )
    return;

  mFilters[layerId].remove( fieldName );

  if ( mFilters[layerId].isEmpty() )
  {
    mFilters.remove( layerId );
  }
}

void FilterController::setTextFilter( const QString &layerId, const QString &fieldName, const QString &text )
{
  QString trimmedText = text.trimmed();

  if ( trimmedText.isEmpty() )
  {
    removeFieldFilter( layerId, fieldName );
    return;
  }

  setFieldFilter( layerId, fieldName, QStringLiteral( "text" ), trimmedText );
}

void FilterController::setNumberFilter( const QString &layerId, const QString &fieldName,
                                        const QString &fromText, const QString &toText )
{
  QString trimmedFrom = fromText.trimmed();
  QString trimmedTo = toText.trimmed();

  // If both are empty, remove the filter
  if ( trimmedFrom.isEmpty() && trimmedTo.isEmpty() )
  {
    removeFieldFilter( layerId, fieldName );
    return;
  }

  QVariant fromValue;
  QVariant toValue;

  if ( !trimmedFrom.isEmpty() )
  {
    bool ok = false;
    double val = trimmedFrom.toDouble( &ok );
    if ( ok )
    {
      fromValue = val;
    }
  }

  if ( !trimmedTo.isEmpty() )
  {
    bool ok = false;
    double val = trimmedTo.toDouble( &ok );
    if ( ok )
    {
      toValue = val;
    }
  }

  // Only set filter if at least one value was successfully parsed
  if ( fromValue.isValid() || toValue.isValid() )
  {
    setFieldFilter( layerId, fieldName, QStringLiteral( "number" ), fromValue, toValue );
  }
  else
  {
    // Invalid input - remove filter
    removeFieldFilter( layerId, fieldName );
  }
}

void FilterController::setDateFilter( const QString &layerId, const QString &fieldName,
                                      const QVariant &fromDate, const QVariant &toDate,
                                      bool hasTime )
{
  bool hasFrom = fromDate.isValid() && !fromDate.isNull();
  bool hasTo = toDate.isValid() && !toDate.isNull();

  // If both are invalid/null, remove the filter
  if ( !hasFrom && !hasTo )
  {
    removeFieldFilter( layerId, fieldName );
    return;
  }

  QVariant fromValue;
  QVariant toValue;

  if ( hasFrom )
  {
    QDateTime fromDt = fromDate.toDateTime();
    if ( fromDt.isValid() )
    {
      if ( hasTime )
      {
        // Zero out seconds/ms — the time picker doesn't show them,
        // so "from 15:35" should mean "from 15:35:00.000"
        QTime t = fromDt.time();
        fromDt.setTime( QTime( t.hour(), t.minute(), 0, 0 ) );
      }
      else
      {
        // Date-only field: set to start of day
        fromDt.setTime( QTime( 0, 0, 0, 0 ) );
      }
      fromValue = fromDt;
    }
  }

  if ( hasTo )
  {
    QDateTime toDt = toDate.toDateTime();
    if ( toDt.isValid() )
    {
      if ( hasTime )
      {
        // Max out seconds/ms — "to 15:36" should mean "to 15:36:59.999"
        QTime t = toDt.time();
        toDt.setTime( QTime( t.hour(), t.minute(), 59, 999 ) );
      }
      else
      {
        // Date-only field: set to end of day
        toDt.setTime( QTime( 23, 59, 59, 999 ) );
      }
      toValue = toDt;
    }
  }

  // Validate: if from > to, remove the filter (invalid range)
  if ( fromValue.isValid() && toValue.isValid() )
  {
    if ( fromValue.toDateTime() > toValue.toDateTime() )
    {
      removeFieldFilter( layerId, fieldName );
      return;
    }
  }

  if ( fromValue.isValid() || toValue.isValid() )
  {
    setFieldFilter( layerId, fieldName, QStringLiteral( "date" ), fromValue, toValue );
  }
  else
  {
    removeFieldFilter( layerId, fieldName );
  }
}

void FilterController::clearLayerFilters( const QString &layerId )
{
  mFilters.remove( layerId );
}

void FilterController::clearAllFilters()
{
  mFilters.clear();
}

QVariantList FilterController::getLayerFilters( const QString &layerId ) const
{
  QVariantList result;

  if ( !mAppliedFilters.contains( layerId ) )
    return result;

  // Use .value() to get a copy in const context
  QMap<QString, FieldFilter> layerFilters = mAppliedFilters.value( layerId );
  for ( auto it = layerFilters.constBegin(); it != layerFilters.constEnd(); ++it )
  {
    QVariantMap filterMap;
    filterMap[QStringLiteral( "fieldName" )] = it.value().fieldName;
    filterMap[QStringLiteral( "filterType" )] = it.value().filterType;
    filterMap[QStringLiteral( "value" )] = it.value().value;
    filterMap[QStringLiteral( "valueTo" )] = it.value().valueTo;
    result << filterMap;
  }

  return result;
}

QVariant FilterController::getFieldFilterValue( const QString &layerId, const QString &fieldName ) const
{
  if ( !mFilters.contains( layerId ) || !mFilters.value( layerId ).contains( fieldName ) )
    return QVariant();

  return mFilters.value( layerId ).value( fieldName ).value;
}

QVariant FilterController::getFieldFilterValueTo( const QString &layerId, const QString &fieldName ) const
{
  if ( !mFilters.contains( layerId ) || !mFilters.value( layerId ).contains( fieldName ) )
    return QVariant();

  return mFilters.value( layerId ).value( fieldName ).valueTo;
}

QString FilterController::buildFieldExpression( const FieldFilter &filter ) const
{
  const QString quotedField = QgsExpression::quotedColumnRef( filter.fieldName );

  if ( filter.filterType == QLatin1String( "bool" ) )
  {
    bool boolValue = filter.value.toBool();
    return QStringLiteral( "%1 = %2" ).arg( quotedField, boolValue ? QStringLiteral( "TRUE" ) : QStringLiteral( "FALSE" ) );
  }
  else if ( filter.filterType == QLatin1String( "text" ) )
  {
    QString textValue = filter.value.toString().toLower();
    // Use LOWER() for case-insensitive search (works with SQLite/GeoPackage)
    return QStringLiteral( "LOWER(%1) LIKE '%%2%'" ).arg( quotedField, textValue.replace( "'", "''" ) );
  }
  else if ( filter.filterType == QLatin1String( "multichoice" ) )
  {
    QStringList values = filter.value.toStringList();
    if ( values.isEmpty() )
      return QString();

    QStringList quotedValues;
    for ( const QString &v : values )
    {
      quotedValues << QgsExpression::quotedValue( v );
    }
    return QStringLiteral( "%1 IN (%2)" ).arg( quotedField, quotedValues.join( QStringLiteral( ", " ) ) );
  }
  else if ( filter.filterType == QLatin1String( "number" ) )
  {
    bool hasFrom = filter.value.isValid() && !filter.value.isNull();
    bool hasTo = filter.valueTo.isValid() && !filter.valueTo.isNull();

    // Skip invalid range where from > to
    if ( hasFrom && hasTo && filter.value.toDouble() > filter.valueTo.toDouble() )
    {
      return QString();
    }

    QStringList conditions;

    if ( hasFrom )
    {
      double fromValue = filter.value.toDouble();
      conditions << QStringLiteral( "%1 >= %2" ).arg( quotedField ).arg( fromValue );
    }

    if ( hasTo )
    {
      double toValue = filter.valueTo.toDouble();
      conditions << QStringLiteral( "%1 <= %2" ).arg( quotedField ).arg( toValue );
    }

    return conditions.join( QStringLiteral( " AND " ) );
  }
  else if ( filter.filterType == QLatin1String( "dropdown" ) )
  {
    QStringList values = filter.value.toStringList();
    if ( values.isEmpty() )
      return QString();

    if ( values.size() == 1 )
    {
      return QStringLiteral( "%1 = %2" ).arg( quotedField, QgsExpression::quotedValue( values.first() ) );
    }

    QStringList quotedValues;
    for ( const QString &v : values )
    {
      quotedValues << QgsExpression::quotedValue( v );
    }
    return QStringLiteral( "%1 IN (%2)" ).arg( quotedField, quotedValues.join( QStringLiteral( ", " ) ) );
  }
  else if ( filter.filterType == QLatin1String( "dropdown-multi" ) )
  {
    // Multi-value fields store values as {k1,k2,k3}
    // Use LIKE patterns to match any position within the braced list
    QStringList values = filter.value.toStringList();
    if ( values.isEmpty() )
      return QString();

    QStringList keyConditions;
    for ( const QString &key : values )
    {
      QString escapedKey = key;
      escapedKey.replace( "'", "''" );

      // Match all positions: only value {k}, first {k,...}, last ...,k}, middle ...,k,...
      keyConditions << QStringLiteral( "(%1 LIKE '{%2}' OR %1 LIKE '{%2,%%' OR %1 LIKE '%%,%2}' OR %1 LIKE '%%,%2,%%')" )
                         .arg( quotedField, escapedKey );
    }
    return keyConditions.join( QStringLiteral( " OR " ) );
  }
  else if ( filter.filterType == QLatin1String( "date" ) )
  {
    // GeoPackage stores datetimes as timezone-naive strings (effectively UTC),
    // so we must convert local datetimes to UTC before comparing.
    // Use a custom format to avoid the 'Z' suffix that Qt::ISODate adds for UTC.
    const QString isoFormat = QStringLiteral( "yyyy-MM-ddTHH:mm:ss" );
    QStringList conditions;

    if ( filter.value.isValid() && !filter.value.isNull() )
    {
      QDateTime fromDate = filter.value.toDateTime();
      if ( fromDate.isValid() )
      {
        conditions << QStringLiteral( "%1 >= '%2'" ).arg( quotedField, fromDate.toUTC().toString( isoFormat ) );
      }
    }

    if ( filter.valueTo.isValid() && !filter.valueTo.isNull() )
    {
      QDateTime toDate = filter.valueTo.toDateTime();
      if ( toDate.isValid() )
      {
        conditions << QStringLiteral( "%1 <= '%2'" ).arg( quotedField, toDate.toUTC().toString( isoFormat ) );
      }
    }

    return conditions.join( QStringLiteral( " AND " ) );
  }

  return QString();
}

QString FilterController::generateFilterExpression( const QString &layerId ) const
{
  if ( !mAppliedFilters.contains( layerId ) )
    return QString();

  // Use .value() to get a copy in const context
  QMap<QString, FieldFilter> layerFilters = mAppliedFilters.value( layerId );
  QStringList expressions;

  for ( auto it = layerFilters.constBegin(); it != layerFilters.constEnd(); ++it )
  {
    QString expr = buildFieldExpression( it.value() );
    if ( !expr.isEmpty() )
    {
      expressions << QStringLiteral( "(%1)" ).arg( expr );
    }
  }

  if ( expressions.isEmpty() )
    return QString();

  return expressions.join( QStringLiteral( " AND " ) );
}

void FilterController::applyFiltersToLayer( QgsVectorLayer *layer )
{
  if ( !layer )
    return;

  QString filterExpr = generateFilterExpression( layer->id() );
  bool success = layer->setSubsetString( filterExpr );

  qDebug() << "Applied filter to layer" << layer->name() << ":" << filterExpr << "success:" << success;

  // Trigger a layer refresh to ensure the filter takes effect
  if ( success )
  {
    layer->triggerRepaint();
  }
}

void FilterController::applyFiltersToAllLayers()
{
  bool hadFilters = hasActiveFilters();

  // Commit pending filters to applied state
  mAppliedFilters = mFilters;

  QgsProject *project = QgsProject::instance();
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

  emit filtersChanged();

  if ( hadFilters != hasActiveFilters() )
  {
    emit hasActiveFiltersChanged();
  }
}

void FilterController::discardPendingChanges()
{
  mFilters = mAppliedFilters;
}

QVariantList FilterController::getFilterableFields( QgsVectorLayer *layer ) const
{
  QVariantList result;

  if ( !layer )
    return result;

  QString layerId = layer->id();
  const QgsFields fields = layer->fields();
  for ( int i = 0; i < fields.count(); ++i )
  {
    const QgsField &field = fields.at( i );
    QVariantMap fieldInfo;
    fieldInfo[QStringLiteral( "name" )] = field.name();
    fieldInfo[QStringLiteral( "displayName" )] = field.displayName();

    // Check editor widget type first — ValueMap/ValueRelation override the data type
    QString filterType;
    bool multiSelect = false;
    QgsEditorWidgetSetup widgetSetup = layer->editorWidgetSetup( i );
    QString widgetType = widgetSetup.type();

    if ( widgetType == QLatin1String( "ValueMap" ) )
    {
      filterType = QStringLiteral( "dropdown" );
      multiSelect = false;
    }
    else if ( widgetType == QLatin1String( "ValueRelation" ) )
    {
      filterType = QStringLiteral( "dropdown" );
      multiSelect = widgetSetup.config().value( QStringLiteral( "AllowMulti" ) ).toBool();
    }
    else
    {
      // Determine filter type based on field data type
      QMetaType::Type fieldType = static_cast<QMetaType::Type>( field.type() );

      switch ( fieldType )
      {
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
        case QMetaType::Double:
        case QMetaType::Float:
          filterType = QStringLiteral( "number" );
          break;

        case QMetaType::QDate:
          filterType = QStringLiteral( "date" );
          fieldInfo[QStringLiteral( "hasTime" )] = false;
          break;

        case QMetaType::QDateTime:
          filterType = QStringLiteral( "date" );
          fieldInfo[QStringLiteral( "hasTime" )] = true;
          break;

        default:
          filterType = QStringLiteral( "text" );
          break;
      }
    }

    fieldInfo[QStringLiteral( "filterType" )] = filterType;

    if ( filterType == QLatin1String( "dropdown" ) )
    {
      fieldInfo[QStringLiteral( "multiSelect" )] = multiSelect;
    }

    // Get current filter value from pending state (for drawer UI)
    if ( mFilters.contains( layerId ) && mFilters.value( layerId ).contains( field.name() ) )
    {
      FieldFilter filter = mFilters.value( layerId ).value( field.name() );
      fieldInfo[QStringLiteral( "currentValue" )] = filter.value;
      fieldInfo[QStringLiteral( "currentValueTo" )] = filter.valueTo;

      // For dropdown fields, also look up display texts for currently selected keys
      if ( filterType == QLatin1String( "dropdown" ) )
      {
        QStringList selectedKeys = filter.value.toStringList();
        if ( !selectedKeys.isEmpty() )
        {
          QStringList displayTexts;
          QVariantMap config = widgetSetup.config();

          if ( widgetType == QLatin1String( "ValueMap" ) )
          {
            displayTexts = lookupValueMapTexts( config, selectedKeys );
          }
          else if ( widgetType == QLatin1String( "ValueRelation" ) )
          {
            displayTexts = lookupValueRelationTexts( config, selectedKeys );
          }

          fieldInfo[QStringLiteral( "currentValueTexts" )] = displayTexts;
        }
      }
    }

    result << fieldInfo;
  }

  return result;
}

QStringList FilterController::getFieldUniqueValues( QgsVectorLayer *layer, const QString &fieldName ) const
{
  QStringList result;

  if ( !layer )
    return result;

  int fieldIndex = layer->fields().lookupField( fieldName );
  if ( fieldIndex < 0 )
    return result;

  QSet<QVariant> uniqueValues = layer->uniqueValues( fieldIndex, 100 );
  for ( const QVariant &v : uniqueValues )
  {
    if ( !v.isNull() && !v.toString().isEmpty() )
    {
      result << v.toString();
    }
  }

  result.sort();
  return result;
}

QVariantList FilterController::getVectorLayers() const
{
  QVariantList result;

  QgsProject *project = QgsProject::instance();
  if ( !project )
    return result;

  const QMap<QString, QgsMapLayer *> layers = project->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vectorLayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vectorLayer )
    {
      QVariantMap layerInfo;
      layerInfo[QStringLiteral( "layerId" )] = it.key();
      layerInfo[QStringLiteral( "layerName" )] = vectorLayer->name();
      layerInfo[QStringLiteral( "layer" )] = QVariant::fromValue( vectorLayer );
      result << layerInfo;
    }
  }

  return result;
}

void FilterController::setDropdownFilter( const QString &layerId, const QString &fieldName, const QVariant &selectedKeys, bool multiValue )
{
  QStringList keys = selectedKeys.toStringList();

  if ( keys.isEmpty() )
  {
    removeFieldFilter( layerId, fieldName );
    return;
  }

  QString filterType = multiValue ? QStringLiteral( "dropdown-multi" ) : QStringLiteral( "dropdown" );
  setFieldFilter( layerId, fieldName, filterType, QVariant( keys ) );
}

QVariantList FilterController::getDropdownOptions( QgsVectorLayer *layer, const QString &fieldName, const QString &searchText, int limit )
{
  if ( !layer )
    return QVariantList();

  int fieldIndex = layer->fields().lookupField( fieldName );
  if ( fieldIndex < 0 )
    return QVariantList();

  QgsEditorWidgetSetup widgetSetup = layer->editorWidgetSetup( fieldIndex );
  QString widgetType = widgetSetup.type();
  QVariantMap config = widgetSetup.config();

  if ( widgetType == QLatin1String( "ValueMap" ) )
  {
    return extractValueMapOptions( config, searchText );
  }
  else if ( widgetType == QLatin1String( "ValueRelation" ) )
  {
    // Get currently selected keys so they always appear in the list
    QStringList currentlySelectedKeys;
    QString layerId = layer->id();
    if ( mFilters.contains( layerId ) && mFilters.value( layerId ).contains( fieldName ) )
    {
      currentlySelectedKeys = mFilters.value( layerId ).value( fieldName ).value.toStringList();
    }

    return extractValueRelationOptions( config, searchText, limit, currentlySelectedKeys );
  }

  return QVariantList();
}

QVariantList FilterController::extractValueMapOptions( const QVariantMap &config, const QString &searchText ) const
{
  QVariantList result;

  QVariantList mapList = config.value( QStringLiteral( "map" ) ).toList();
  for ( const QVariant &entry : mapList )
  {
    QVariantMap entryMap = entry.toMap();
    if ( entryMap.isEmpty() )
      continue;

    // Each entry is a single-key map: {"Display Text": "stored_value"}
    QString displayText = entryMap.constBegin().key();
    QString storedValue = entryMap.constBegin().value().toString();

    // Filter by search text
    if ( !searchText.isEmpty() && !displayText.contains( searchText, Qt::CaseInsensitive ) )
      continue;

    QVariantMap option;
    option[QStringLiteral( "text" )] = displayText;
    option[QStringLiteral( "value" )] = storedValue;
    result << option;
  }

  return result;
}

QVariantList FilterController::extractValueRelationOptions( const QVariantMap &config, const QString &searchText, int limit, const QStringList &alwaysIncludeKeys ) const
{
  QVariantList result;

  QgsVectorLayer *referencedLayer = QgsValueRelationFieldFormatter::resolveLayer( config, QgsProject::instance() );
  if ( !referencedLayer )
    return result;

  QString keyFieldName = config.value( QStringLiteral( "Key" ) ).toString();
  QString valueFieldName = config.value( QStringLiteral( "Value" ) ).toString();

  if ( referencedLayer->fields().indexOf( keyFieldName ) < 0 || referencedLayer->fields().indexOf( valueFieldName ) < 0 )
    return result;

  // Build feature request
  QgsFeatureRequest request;
  request.setFlags( Qgis::FeatureRequestFlag::NoGeometry );
  request.setSubsetOfAttributes( QStringList( { keyFieldName, valueFieldName } ), referencedLayer->fields() );

  // Apply search filter
  if ( !searchText.isEmpty() )
  {
    QString escapedSearch = searchText;
    escapedSearch.replace( "'", "''" );
    QString filterExpr = QStringLiteral( "LOWER(%1) LIKE '%%2%'" )
                           .arg( QgsExpression::quotedColumnRef( valueFieldName ), escapedSearch.toLower() );
    request.setFilterExpression( filterExpr );
  }

  // Apply configured filter expression (only if it doesn't require form scope)
  QString configFilterExpr = config.value( QStringLiteral( "FilterExpression" ) ).toString();
  if ( !configFilterExpr.isEmpty() && !QgsValueRelationFieldFormatter::expressionRequiresFormScope( configFilterExpr ) )
  {
    request.combineFilterExpression( configFilterExpr );
  }

  // Apply ordering
  if ( config.value( QStringLiteral( "OrderByValue" ) ).toBool() )
  {
    request.setOrderBy( QgsFeatureRequest::OrderBy( { QgsFeatureRequest::OrderByClause( valueFieldName ) } ) );
  }

  request.setLimit( limit );

  // Fetch features
  QSet<QString> seenKeys;
  QgsFeatureIterator it = referencedLayer->getFeatures( request );
  QgsFeature feature;
  while ( it.nextFeature( feature ) )
  {
    QString key = feature.attribute( keyFieldName ).toString();
    QString value = feature.attribute( valueFieldName ).toString();
    seenKeys.insert( key );

    QVariantMap option;
    option[QStringLiteral( "text" )] = value;
    option[QStringLiteral( "value" )] = key;
    result << option;
  }

  // Ensure currently selected keys are always visible in the list
  if ( !alwaysIncludeKeys.isEmpty() )
  {
    QStringList missingKeys;
    for ( const QString &key : alwaysIncludeKeys )
    {
      if ( !seenKeys.contains( key ) )
      {
        missingKeys << key;
      }
    }

    if ( !missingKeys.isEmpty() )
    {
      QStringList quotedKeys;
      for ( const QString &k : missingKeys )
      {
        quotedKeys << QgsExpression::quotedValue( k );
      }

      QgsFeatureRequest selectedRequest;
      selectedRequest.setFlags( Qgis::FeatureRequestFlag::NoGeometry );
      selectedRequest.setSubsetOfAttributes( QStringList( { keyFieldName, valueFieldName } ), referencedLayer->fields() );
      selectedRequest.setFilterExpression(
        QStringLiteral( "%1 IN (%2)" ).arg( QgsExpression::quotedColumnRef( keyFieldName ), quotedKeys.join( QStringLiteral( ", " ) ) )
      );

      QVariantList selectedItems;
      QgsFeatureIterator selIt = referencedLayer->getFeatures( selectedRequest );
      QgsFeature selFeature;
      while ( selIt.nextFeature( selFeature ) )
      {
        QVariantMap option;
        option[QStringLiteral( "text" )] = selFeature.attribute( valueFieldName ).toString();
        option[QStringLiteral( "value" )] = selFeature.attribute( keyFieldName ).toString();
        selectedItems << option;
      }

      // Prepend selected items so they appear first
      selectedItems.append( result );
      result = selectedItems;
    }
  }

  return result;
}

QStringList FilterController::lookupValueMapTexts( const QVariantMap &config, const QStringList &keys ) const
{
  QStringList texts;
  QSet<QString> keySet( keys.begin(), keys.end() );

  QVariantList mapList = config.value( QStringLiteral( "map" ) ).toList();
  for ( const QVariant &entry : mapList )
  {
    QVariantMap entryMap = entry.toMap();
    if ( entryMap.isEmpty() )
      continue;

    QString displayText = entryMap.constBegin().key();
    QString storedValue = entryMap.constBegin().value().toString();

    if ( keySet.contains( storedValue ) )
    {
      texts << displayText;
    }
  }

  return texts;
}

QStringList FilterController::lookupValueRelationTexts( const QVariantMap &config, const QStringList &keys ) const
{
  QStringList texts;

  QgsVectorLayer *referencedLayer = QgsValueRelationFieldFormatter::resolveLayer( config, QgsProject::instance() );
  if ( !referencedLayer )
    return texts;

  QString keyFieldName = config.value( QStringLiteral( "Key" ) ).toString();
  QString valueFieldName = config.value( QStringLiteral( "Value" ) ).toString();

  if ( referencedLayer->fields().indexOf( keyFieldName ) < 0 || referencedLayer->fields().indexOf( valueFieldName ) < 0 )
    return texts;

  QStringList quotedKeys;
  for ( const QString &k : keys )
  {
    quotedKeys << QgsExpression::quotedValue( k );
  }

  QgsFeatureRequest request;
  request.setFlags( Qgis::FeatureRequestFlag::NoGeometry );
  request.setSubsetOfAttributes( QStringList( { keyFieldName, valueFieldName } ), referencedLayer->fields() );
  request.setFilterExpression(
    QStringLiteral( "%1 IN (%2)" ).arg( QgsExpression::quotedColumnRef( keyFieldName ), quotedKeys.join( QStringLiteral( ", " ) ) )
  );

  QgsFeatureIterator it = referencedLayer->getFeatures( request );
  QgsFeature feature;
  while ( it.nextFeature( feature ) )
  {
    texts << feature.attribute( valueFieldName ).toString();
  }

  return texts;
}
