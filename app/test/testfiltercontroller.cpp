/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testfiltercontroller.h"
#include "testutils.h"
#include "filtercontroller.h"

#include <QtTest/QtTest>
#include <QDateTime>

#include <qgsproject.h>
#include <qgsvectorlayer.h>

// Field name and SQL templates reused across all tests
static const QString FIELD_NAME = QStringLiteral( "ts_field" );

// Template for single/multi select: @@value@@ is replaced by a quoted value
static const QString SELECT_SQL = QStringLiteral( "\"ts_field\" = @@value@@" );

// Template for date range: @@value_from@@ and @@value_to@@ are replaced by quoted date strings
static const QString RANGE_SQL = QStringLiteral( "\"ts_field\" >= '@@value_from@@' AND \"ts_field\" <= '@@value_to@@'" );

void TestFilterController::init()
{
  QgsProject::instance()->clear();
  mController = std::make_unique<FilterController>();
}

void TestFilterController::cleanup()
{
  mController.reset();
}

// Date range
void TestFilterController::testDateRangeDateTime()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 14 ), QTime( 0, 0, 0 ), Qt::UTC ) ) );   // before range
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 12, 0, 0 ), Qt::UTC ) ) );  // inside range
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 17 ), QTime( 0, 0, 0 ), Qt::UTC ) ) );   // after range

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), FIELD_NAME, RANGE_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime from = QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2024, 3, 16 ), QTime( 18, 30, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  // 'to' seconds are capped to 59 and ms to 999, hour and minute are preserved
  const QString expected = QStringLiteral(
                             "(\"ts_field\" >= '2024-03-15T10:00:00.000Z' AND \"ts_field\" <= '2024-03-16T18:30:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testDateRangeDate()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "date" ) );
  QVERIFY( layer );

  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2023, 12, 31 ) ) );  // before range
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2024, 6, 15 ) ) );   // inside range
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2025, 1, 1 ) ) );    // after range

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), FIELD_NAME, RANGE_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime from = QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2024, 12, 31 ), QTime( 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  // Date-only fields produce yyyy-MM-dd format
  const QString expected = QStringLiteral(
                             "(\"ts_field\" >= '2024-01-01' AND \"ts_field\" <= '2024-12-31')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testDateRangeDateTimeNull()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // Sentinel range covers everything, so both features must appear after filtering
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2000, 1, 1 ), QTime( 0, 0, 0 ), Qt::UTC ) ) );
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 6, 15 ), QTime( 12, 0, 0 ), Qt::UTC ) ) );

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), FIELD_NAME, RANGE_SQL );
  QVERIFY( !filterId.isEmpty() );

  // Both bounds invalid — should fall back to the sentinel strings
  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant(), QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"ts_field\" >= '0001-01-01T00:00:00.000Z' AND \"ts_field\" <= '9999-12-31T23:59:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

void TestFilterController::testDateRangeDateNull()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "date" ) );
  QVERIFY( layer );

  // Sentinel range covers everything, so both features must appear after filtering
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2000, 1, 1 ) ) );
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2024, 6, 15 ) ) );

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), FIELD_NAME, RANGE_SQL );
  QVERIFY( !filterId.isEmpty() );

  // Both bounds invalid — should fall back to the sentinel strings
  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant(), QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"ts_field\" >= '0001-01-01' AND \"ts_field\" <= '9999-12-31')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

void TestFilterController::testDateRangeDateTimeFeatureAtLowerBound()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // Feature exactly at the lower bound — >= is inclusive, so it must be counted.
  // Feature just before the bound must be excluded.
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 0, 0, 0 ), Qt::UTC ) ) );   // at lower bound
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 9, 59, 59, 999 ), Qt::UTC ) ) ); // just before

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), FIELD_NAME, RANGE_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime from = QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2024, 3, 15 ), QTime( 18, 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"ts_field\" >= '2024-03-15T10:00:00.000Z' AND \"ts_field\" <= '2024-03-15T18:00:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testDateRangeDateTimeMidnightLowerBound()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // When the user picks a date without a time the picker passes midnight (00:00:00.000).
  // A feature stored at midnight on that date must be included — it is at the
  // start of the day, not before it.
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 0, 0, 0, 0 ), Qt::UTC ) ) );    // at midnight lower bound
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 14 ), QTime( 23, 59, 59, 999 ), Qt::UTC ) ) ); // previous day, excluded

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), FIELD_NAME, RANGE_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime from = QDateTime( QDate( 2024, 3, 15 ), QTime( 0, 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2024, 3, 15 ), QTime( 23, 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"ts_field\" >= '2024-03-15T00:00:00.000Z' AND \"ts_field\" <= '2024-03-15T23:00:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testDateRangeDateTimeZeroMsInsideRange()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // A feature with ms=0 is ambiguous for single/multi select (requires double OR'd expression),
  // but range filters use >= / <=. QGIS does datetime-aware comparison, so the feature is
  // counted regardless of whether it was stored as '.000Z' or without the ms suffix.
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 12, 0, 0, 0 ), Qt::UTC ) ) );  // inside range, 0 ms
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 9, 0, 0, 0 ), Qt::UTC ) ) );   // before range, excluded

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), FIELD_NAME, RANGE_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime from = QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2024, 3, 15 ), QTime( 18, 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  // Single expression — no double-expr trick needed for range filters
  const QString expected = QStringLiteral(
                             "(\"ts_field\" >= '2024-03-15T10:00:00.000Z' AND \"ts_field\" <= '2024-03-15T18:00:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

// Single select
void TestFilterController::testSingleSelectDateTimeNonZeroMs()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 30, 45, 123 ), Qt::UTC ) ) );  // matches
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC ) ) );        // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime dt = QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 30, 45, 123 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  // Non-zero ms → single expression with full timestamp format
  const QString expected = QStringLiteral( "(\"ts_field\" = '2024-03-15T10:30:45.123Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testSingleSelectDateTimeZeroMs()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // Exactly 0 ms: ambiguous between "stored as .000Z" and "stored without ms" in the data.
  // The filter must match both representations.
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 30, 45, 0 ), Qt::UTC ) ) );  // matches (0ms)
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC ) ) );      // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime dt = QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 30, 45, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  // Zero ms → two OR'd expressions: one with .000Z and one without ms suffix
  const QString expected = QStringLiteral(
                             "((\"ts_field\" = '2024-03-15T10:30:45.000Z') OR (\"ts_field\" = '2024-03-15T10:30:45Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testSingleSelectDateTimeNull()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // Null value → two OR'd expressions: one with NULL, one with empty string
  // Note: "= NULL" is invalid SQL and never matches; proper IS NULL support is a separate feature
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QVariant() ) );                                                          // not matched (= NULL is invalid SQL)
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0 ), Qt::UTC ) ) );        // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"ts_field\" = NULL) OR (\"ts_field\" = ''))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 0 );
}

void TestFilterController::testSingleSelectDate()
{
  // QML always passes QDateTime even for date-only fields; the field type drives formatting
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "date" ) );
  QVERIFY( layer );

  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2024, 6, 10 ) ) );   // matches
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2024, 6, 11 ) ) );   // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime dt = QDateTime( QDate( 2024, 6, 10 ), QTime( 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  // Date field → yyyy-MM-dd format only
  const QString expected = QStringLiteral( "(\"ts_field\" = '2024-06-10')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

// Multi select
void TestFilterController::testMultiSelectDateTimeNonZeroMs()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  const QDateTime dt1 = QDateTime( QDate( 2024, 3, 15 ), QTime( 10, 30, 45, 100 ), Qt::UTC );
  const QDateTime dt2 = QDateTime( QDate( 2024, 3, 16 ), QTime( 11, 0, 0, 500 ), Qt::UTC );
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, dt1 ) );                                                                  // matches dt1
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, dt2 ) );                                                                  // matches dt2
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC ) ) );     // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt1, dt2 };
  mController->processFilters( filterValues );

  // Both values have non-zero ms → one expression per value, joined by OR
  const QString expected = QStringLiteral(
                             "((\"ts_field\" = '2024-03-15T10:30:45.100Z') OR (\"ts_field\" = '2024-03-16T11:00:00.500Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

void TestFilterController::testMultiSelectDateTimeZeroMs()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // Single value with 0 ms → produces two OR'd expressions for ambiguity coverage
  const QDateTime dt = QDateTime( QDate( 2024, 5, 1 ), QTime( 8, 0, 0, 0 ), Qt::UTC );
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, dt ) );                                                                   // matches (0ms)
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC ) ) );     // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"ts_field\" = '2024-05-01T08:00:00.000Z') OR (\"ts_field\" = '2024-05-01T08:00:00Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testMultiSelectDateTimeMixed()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // First value has non-zero ms → one expression.
  // Second value has 0 ms → expands to two expressions (with and without ms).
  const QDateTime dt1 = QDateTime( QDate( 2024, 1, 10 ), QTime( 9, 15, 30, 250 ), Qt::UTC );
  const QDateTime dt2 = QDateTime( QDate( 2024, 2, 20 ), QTime( 14, 0, 0, 0 ), Qt::UTC );
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, dt1 ) );                                                                  // matches dt1
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, dt2 ) );                                                                  // matches dt2 (0ms)
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 6, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC ) ) );     // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt1, dt2 };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"ts_field\" = '2024-01-10T09:15:30.250Z')"
                             " OR (\"ts_field\" = '2024-02-20T14:00:00.000Z')"
                             " OR (\"ts_field\" = '2024-02-20T14:00:00Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

void TestFilterController::testMultiSelectDateTimeNull()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // Null value → two OR'd expressions: one with NULL, one with empty string
  // Note: "= NULL" is invalid SQL and never matches; proper IS NULL support is a separate feature
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QVariant() ) );                                                          // not matched (= NULL is invalid SQL)
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0 ), Qt::UTC ) ) );        // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"ts_field\" = NULL) OR (\"ts_field\" = ''))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 0 );
}

void TestFilterController::testMultiSelectDateTimeEmpty()
{
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "datetime" ) );
  QVERIFY( layer );

  // Empty values list → short-circuits to empty expression → no subset string applied → all features visible
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 1, 1 ), QTime( 0, 0, 0 ), Qt::UTC ) ) );
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDateTime( QDate( 2024, 6, 1 ), QTime( 0, 0, 0 ), Qt::UTC ) ) );

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{};
  mController->processFilters( filterValues );

  QVERIFY( layer->subsetString().isEmpty() );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

void TestFilterController::testMultiSelectDate()
{
  // Date-only field: all QDateTime values are formatted as yyyy-MM-dd
  QgsVectorLayer *layer = TestUtils::createFilterTestLayer( FIELD_NAME, QStringLiteral( "date" ) );
  QVERIFY( layer );

  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2024, 6, 1 ) ) );   // matches dt1
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2024, 6, 15 ) ) );  // matches dt2
  QVERIFY( TestUtils::addFeatureToLayer( layer, FIELD_NAME, QDate( 2024, 7, 1 ) ) );   // no match

  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), FIELD_NAME, SELECT_SQL );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime dt1 = QDateTime( QDate( 2024, 6, 1 ), QTime( 0, 0, 0 ), Qt::UTC );
  const QDateTime dt2 = QDateTime( QDate( 2024, 6, 15 ), QTime( 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt1, dt2 };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"ts_field\" = '2024-06-01') OR (\"ts_field\" = '2024-06-15'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}
