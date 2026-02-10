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
  for ( const QgsField &field : fields )
  {
    QVariantMap fieldInfo;
    fieldInfo[QStringLiteral( "name" )] = field.name();
    fieldInfo[QStringLiteral( "displayName" )] = field.displayName();

    // Determine filter type based on field type
    QString filterType;
    QMetaType::Type fieldType = static_cast<QMetaType::Type>( field.type() );

    // Simplified: only number and text filters for prototype
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

    fieldInfo[QStringLiteral( "filterType" )] = filterType;

    // Get current filter value from pending state (for drawer UI)
    if ( mFilters.contains( layerId ) && mFilters.value( layerId ).contains( field.name() ) )
    {
      // Use .value() to safely get a copy, not operator[] which returns temporary in const context
      FieldFilter filter = mFilters.value( layerId ).value( field.name() );
      fieldInfo[QStringLiteral( "currentValue" )] = filter.value;
      fieldInfo[QStringLiteral( "currentValueTo" )] = filter.valueTo;
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
