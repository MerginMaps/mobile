/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILTERCONTROLLER_H
#define FILTERCONTROLLER_H

#include <QObject>
#include <QVariantMap>
#include <QVariantList>

class QgsVectorLayer;
class QgsMapLayer;

/**
 * @brief Single field filter definition
 */
struct FieldFilter
{
    Q_GADGET
    Q_PROPERTY( QString fieldName MEMBER fieldName )
    Q_PROPERTY( QString filterType MEMBER filterType )
    Q_PROPERTY( QVariant value MEMBER value )
    Q_PROPERTY( QVariant valueTo MEMBER valueTo )

  public:
    QString fieldName;
    QString filterType;  // "text", "number", "date", "bool", "multichoice", "dropdown", "dropdown-multi"
    QVariant value;      // single value or list for multichoice, "from" value for ranges
    QVariant valueTo;    // "to" value for range filters (number, date)

    bool isValid() const
    {
      // For range filters (number, date), either value or valueTo being valid is enough
      bool hasValue = value.isValid() && !value.isNull();
      bool hasValueTo = valueTo.isValid() && !valueTo.isNull();
      return !fieldName.isEmpty() && ( hasValue || hasValueTo );
    }
};
Q_DECLARE_METATYPE( FieldFilter )


/**
 * @brief FilterController manages feature filtering across all layers in the project.
 *
 * It stores filter definitions per layer and generates QGIS filter expressions.
 * The filtering affects both map rendering and feature lists.
 */
class FilterController : public QObject
{
    Q_OBJECT

    /**
     * Whether any filters are currently active across any layer
     */
    Q_PROPERTY( bool hasActiveFilters READ hasActiveFilters NOTIFY hasActiveFiltersChanged )

    /**
     * List of layer IDs that have active filters
     */
    Q_PROPERTY( QStringList filteredLayerIds READ filteredLayerIds NOTIFY filtersChanged )

  public:
    explicit FilterController( QObject *parent = nullptr );
    ~FilterController() override = default;

    bool hasActiveFilters() const;
    QStringList filteredLayerIds() const;

    /**
     * @brief Sets a filter for a specific field on a layer
     * @param layerId The layer ID
     * @param fieldName The field name to filter
     * @param filterType The type of filter: "text", "number", "date", "bool", "multichoice"
     * @param value The filter value (or "from" value for ranges, or list for multichoice)
     * @param valueTo Optional "to" value for range filters
     */
    Q_INVOKABLE void setFieldFilter( const QString &layerId, const QString &fieldName,
                                     const QString &filterType, const QVariant &value,
                                     const QVariant &valueTo = QVariant() );

    /**
     * @brief Sets a dropdown filter from selected key values
     * @param layerId The layer ID
     * @param fieldName The field name to filter
     * @param selectedKeys List of selected key values (stored values, not display texts)
     */
    Q_INVOKABLE void setDropdownFilter( const QString &layerId, const QString &fieldName, const QVariant &selectedKeys, bool multiValue = false );

    /**
     * @brief Gets dropdown options for a ValueMap or ValueRelation field (lazy-loaded)
     * @param layer The vector layer containing the field
     * @param fieldName The field name to get options for
     * @param searchText Filter options by display text (case-insensitive)
     * @param limit Maximum number of options to return (for ValueRelation)
     * @return List of maps with "text" (display) and "value" (key) entries
     */
    Q_INVOKABLE QVariantList getDropdownOptions( QgsVectorLayer *layer, const QString &fieldName, const QString &searchText = QString(), int limit = 100 );

    /**
     * @brief Sets a text filter from raw input string
     * Handles validation - empty strings remove the filter
     * @param layerId The layer ID
     * @param fieldName The field name to filter
     * @param text The raw text input from UI
     */
    Q_INVOKABLE void setTextFilter( const QString &layerId, const QString &fieldName, const QString &text );

    /**
     * @brief Sets a number range filter from raw input strings
     * Handles parsing and validation - empty strings are treated as no limit
     * @param layerId The layer ID
     * @param fieldName The field name to filter
     * @param fromText The raw "from" input (min value)
     * @param toText The raw "to" input (max value)
     */
    Q_INVOKABLE void setNumberFilter( const QString &layerId, const QString &fieldName,
                                      const QString &fromText, const QString &toText );

    /**
     * @brief Sets a date range filter from QVariant date values
     * Handles validation - invalid dates remove the filter.
     * For date-only fields (hasTime=false): adjusts to start/end of day.
     * For datetime fields (hasTime=true): zeros/maxes seconds and milliseconds.
     * @param layerId The layer ID
     * @param fieldName The field name to filter
     * @param fromDate The start date (QVariant from JS Date)
     * @param toDate The end date (QVariant from JS Date)
     * @param hasTime Whether the field has a time component
     */
    Q_INVOKABLE void setDateFilter( const QString &layerId, const QString &fieldName,
                                    const QVariant &fromDate, const QVariant &toDate,
                                    bool hasTime = true );

    /**
     * @brief Removes a specific field filter from a layer
     */
    Q_INVOKABLE void removeFieldFilter( const QString &layerId, const QString &fieldName );

    /**
     * @brief Clears all filters for a specific layer
     */
    Q_INVOKABLE void clearLayerFilters( const QString &layerId );

    /**
     * @brief Clears all filters for all layers
     */
    Q_INVOKABLE void clearAllFilters();

    /**
     * @brief Gets the current filters for a layer as a list of FieldFilter objects
     * @return QVariantList of FieldFilter objects
     */
    Q_INVOKABLE QVariantList getLayerFilters( const QString &layerId ) const;

    /**
     * @brief Gets a specific field filter value
     */
    Q_INVOKABLE QVariant getFieldFilterValue( const QString &layerId, const QString &fieldName ) const;

    /**
     * @brief Gets a specific field filter "to" value (for range filters)
     */
    Q_INVOKABLE QVariant getFieldFilterValueTo( const QString &layerId, const QString &fieldName ) const;

    /**
     * @brief Generates a QGIS filter expression string for a layer
     * @param layerId The layer ID
     * @return Filter expression string, empty if no filters active
     */
    Q_INVOKABLE QString generateFilterExpression( const QString &layerId ) const;

    /**
     * @brief Applies the current filters to a vector layer's subset string
     * @param layer The layer to apply filters to
     */
    Q_INVOKABLE void applyFiltersToLayer( QgsVectorLayer *layer );

    /**
     * @brief Applies filters to all vector layers in the current project
     */
    Q_INVOKABLE void applyFiltersToAllLayers();

    /**
     * @brief Gets filterable fields for a layer (for testing: all fields)
     * @return List of field info maps with name, type, and available values
     */
    Q_INVOKABLE QVariantList getFilterableFields( QgsVectorLayer *layer ) const;

    /**
     * @brief Gets unique values for a field (for multichoice filters)
     */
    Q_INVOKABLE QStringList getFieldUniqueValues( QgsVectorLayer *layer, const QString &fieldName ) const;

    /**
     * @brief Gets list of all vector layers in the current project
     * @return QVariantList of maps with keys: layerId, layerName, layer (QgsVectorLayer*)
     */
    Q_INVOKABLE QVariantList getVectorLayers() const;

    /**
     * @brief Discards pending filter changes, reverting to the last applied state.
     * Call this when the user closes the filter drawer without pressing "Show results".
     */
    Q_INVOKABLE void discardPendingChanges();

  signals:
    void filtersChanged();
    void hasActiveFiltersChanged();
    void layerFilterChanged( const QString &layerId );

  private:
    QString buildFieldExpression( const FieldFilter &filter ) const;
    QVariantList extractValueMapOptions( const QVariantMap &config, const QString &searchText ) const;
    QVariantList extractValueRelationOptions( const QVariantMap &config, const QString &searchText, int limit, const QStringList &alwaysIncludeKeys ) const;
    QStringList lookupValueMapTexts( const QVariantMap &config, const QStringList &keys ) const;
    QStringList lookupValueRelationTexts( const QVariantMap &config, const QStringList &keys ) const;

    // layerId -> fieldName -> FieldFilter (pending/working state, updated as user types)
    QMap<QString, QMap<QString, FieldFilter>> mFilters;

    // Applied state, updated only when the user confirms via "Show results"
    QMap<QString, QMap<QString, FieldFilter>> mAppliedFilters;
};

#endif // FILTERCONTROLLER_H
