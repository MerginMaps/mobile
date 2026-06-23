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

void TestFilterController::initTestCase()
{
  const QString projectPath = TestUtils::testDataDir() + "/filtering/test_feature_filtering.qgz";
  QVERIFY2( QgsProject::instance()->read( projectPath ),
            qPrintable( QStringLiteral( "Failed to load filtering test project: %1" ).arg( projectPath ) ) );

  // Save original subset strings so we can restore them between tests
  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vl && !vl->subsetString().isEmpty() )
      mOriginalSubsets.insert( vl->id(), vl->subsetString() );
  }
}

void TestFilterController::cleanupTestCase()
{
  QgsProject::instance()->clear();
}

void TestFilterController::init()
{
  // Restore original subset strings -- clears filter-applied subsets, preserves predefined ones
  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vl )
    {
      if ( mOriginalSubsets.contains( vl->id() ) )
        vl->setSubsetString( mOriginalSubsets[vl->id()] );
      else
        vl->setSubsetString( QString() );
    }
  }
  mController = std::make_unique<FilterController>();
}

void TestFilterController::cleanup()
{
  mController.reset();
}

// Date range
void TestFilterController::testDateRangeDateTime()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" >= '@@value_from@@' AND \"date-time\" <= '@@value_to@@'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // April 2026
  const QDateTime from = QDateTime( QDate( 2026, 4, 1 ), QTime( 0, 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2026, 4, 30 ), QTime( 23, 59, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"date-time\" >= '2026-04-01T00:00:00.000Z' AND \"date-time\" <= '2026-04-30T23:59:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 5 );
}

void TestFilterController::testDateRangeDate()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date" );
  const QString sql = QStringLiteral( "\"date\" >= '@@value_from@@' AND \"date\" <= '@@value_to@@'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // May 15–21, 2026
  const QDateTime from = QDateTime( QDate( 2026, 5, 15 ), QTime( 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2026, 5, 21 ), QTime( 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"date\" >= '2026-05-15' AND \"date\" <= '2026-05-21')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 9 );
}

void TestFilterController::testDateRangeDateTimeNull()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" >= '@@value_from@@' AND \"date-time\" <= '@@value_to@@'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // Both bounds invalid — should fall back to sentinel strings, all features visible
  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant(), QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"date-time\" >= '0001-01-01T00:00:00.000Z' AND \"date-time\" <= '9999-12-31T23:59:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 43 );
}

void TestFilterController::testDateRangeDateNull()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date" );
  const QString sql = QStringLiteral( "\"date\" >= '@@value_from@@' AND \"date\" <= '@@value_to@@'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant(), QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"date\" >= '0001-01-01' AND \"date\" <= '9999-12-31')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 43 );
}

void TestFilterController::testDateRangeDateTimeLowerBoundInclusive()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" >= '@@value_from@@' AND \"date-time\" <= '@@value_to@@'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // from exactly at fid 66's timestamp -- >= is inclusive, so fid 66 must be counted
  const QDateTime from = QDateTime( QDate( 2026, 4, 17 ), QTime( 15, 20, 39, 268 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2026, 4, 17 ), QTime( 15, 22, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  // to is capped to 15:22:59.999; range covers fids 66, 52, 193
  const QString expected = QStringLiteral(
                             "(\"date-time\" >= '2026-04-17T15:20:39.268Z' AND \"date-time\" <= '2026-04-17T15:22:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 3 );
}

void TestFilterController::testDateRangeDateTimeMidnightLowerBound()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" >= '@@value_from@@' AND \"date-time\" <= '@@value_to@@'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // When the user picks a date without a time the picker passes midnight (00:00:00.000).
  // QTime(0,0,0,0) is valid — it must be treated as "start of day", not "no time".
  const QDateTime from = QDateTime( QDate( 2026, 4, 17 ), QTime( 0, 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2026, 4, 17 ), QTime( 16, 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  // Covers fids 66 (15:20:39.268), 52 (15:21:06.780), 193 (15:22:01.883)
  const QString expected = QStringLiteral(
                             "(\"date-time\" >= '2026-04-17T00:00:00.000Z' AND \"date-time\" <= '2026-04-17T16:00:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 3 );
}

void TestFilterController::testDateRangeDateTimeZeroMsInsideRange()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" >= '@@value_from@@' AND \"date-time\" <= '@@value_to@@'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::DateFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // fid 219 stored as '2026-05-18T22:00:00Z' (0 ms). Range uses >= / <=, so QGIS
  // datetime comparison matches it without the double-expression trick.
  const QDateTime from = QDateTime( QDate( 2026, 5, 18 ), QTime( 21, 0, 0, 0 ), Qt::UTC );
  const QDateTime to   = QDateTime( QDate( 2026, 5, 18 ), QTime( 23, 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ from, to };
  mController->processFilters( filterValues );

  // Covers fids 212 (21:38:57.818) and 219 (22:00:00Z)
  const QString expected = QStringLiteral(
                             "(\"date-time\" >= '2026-05-18T21:00:00.000Z' AND \"date-time\" <= '2026-05-18T23:00:59.999Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

// Single select
void TestFilterController::testSingleSelectText()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "type" );
  const QString sql = QStringLiteral( "\"type\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QStringLiteral( "Pub" ) };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral( "(\"type\" = 'Pub')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 11 );
}

void TestFilterController::testSingleSelectDateTimeNonZeroMs()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // fid 18: 2026-05-27T12:27:37.674Z — non-zero ms --> single expression
  const QDateTime dt = QDateTime( QDate( 2026, 5, 27 ), QTime( 12, 27, 37, 674 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral( "(\"date-time\" = '2026-05-27T12:27:37.674Z')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testSingleSelectDateTimeZeroMs()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // fid 219: stored as '2026-05-18T22:00:00Z' (no ms suffix in GeoPackage)
  // 0 ms --> two OR'd expressions to cover both storage formats
  const QDateTime dt = QDateTime( QDate( 2026, 5, 18 ), QTime( 22, 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"date-time\" = '2026-05-18T22:00:00.000Z') OR (\"date-time\" = '2026-05-18T22:00:00Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testSingleSelectDate()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date" );
  const QString sql = QStringLiteral( "\"date\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // QML always passes QDateTime even for date-only fields; the field type drives formatting
  const QDateTime dt = QDateTime( QDate( 2026, 4, 17 ), QTime( 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral( "(\"date\" = '2026-04-17')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 3 );
}

void TestFilterController::testSingleSelectNull()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "type" );
  const QString sql = QStringLiteral( "\"type\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // Null -- "= NULL" is invalid SQL (matches nothing); "= ''" matches empty strings.
  // TODO: update expected expression and count if null handling switches to IS NULL.
  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"type\" = NULL) OR (\"type\" = ''))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 0 );
}

// Multi select
void TestFilterController::testMultiSelectText()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "type" );
  const QString sql = QStringLiteral( "\"type\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QStringLiteral( "Pub" ), QStringLiteral( "Shop" ) };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"type\" = 'Pub') OR (\"type\" = 'Shop'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 19 );
}

void TestFilterController::testMultiSelectDateTimeNonZeroMs()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // Both values have non-zero ms --> one expression per value, joined by OR
  const QDateTime dt1 = QDateTime( QDate( 2026, 5, 27 ), QTime( 12, 27, 37, 674 ), Qt::UTC );  // fid 18
  const QDateTime dt2 = QDateTime( QDate( 2026, 4, 17 ), QTime( 15, 21, 6, 780 ), Qt::UTC );   // fid 52

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt1, dt2 };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"date-time\" = '2026-05-27T12:27:37.674Z') OR (\"date-time\" = '2026-04-17T15:21:06.780Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

void TestFilterController::testMultiSelectDateTimeZeroMs()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // Single value with 0 ms --> produces two OR'd expressions for ambiguity coverage
  const QDateTime dt = QDateTime( QDate( 2026, 5, 18 ), QTime( 22, 0, 0, 0 ), Qt::UTC );  // fid 219

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"date-time\" = '2026-05-18T22:00:00.000Z') OR (\"date-time\" = '2026-05-18T22:00:00Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 1 );
}

void TestFilterController::testMultiSelectDateTimeMixed()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date-time" );
  const QString sql = QStringLiteral( "\"date-time\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // First value has non-zero ms (fid 18: 674ms) --> one expression.
  // Second value has 0 ms (fid 219: stored without ms) --> expands to two expressions.
  const QDateTime dt1 = QDateTime( QDate( 2026, 5, 27 ), QTime( 12, 27, 37, 674 ), Qt::UTC );
  const QDateTime dt2 = QDateTime( QDate( 2026, 5, 18 ), QTime( 22, 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt1, dt2 };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"date-time\" = '2026-05-27T12:27:37.674Z')"
                             " OR (\"date-time\" = '2026-05-18T22:00:00.000Z')"
                             " OR (\"date-time\" = '2026-05-18T22:00:00Z'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 2 );
}

void TestFilterController::testMultiSelectDate()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "date" );
  const QString sql = QStringLiteral( "\"date\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  const QDateTime dt1 = QDateTime( QDate( 2026, 4, 17 ), QTime( 0, 0, 0 ), Qt::UTC );
  const QDateTime dt2 = QDateTime( QDate( 2026, 5, 18 ), QTime( 0, 0, 0 ), Qt::UTC );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ dt1, dt2 };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"date\" = '2026-04-17') OR (\"date\" = '2026-05-18'))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 5 );
}

void TestFilterController::testMultiSelectNull()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "type" );
  const QString sql = QStringLiteral( "\"type\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // Null -- "= NULL" is invalid SQL (matches nothing); "= ''" matches empty strings.
  // TODO: update expected expression and count if null handling switches to IS NULL.
  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QVariant() };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "((\"type\" = NULL) OR (\"type\" = ''))" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 0 );
}

void TestFilterController::testMultiSelectEmpty()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "POI" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "type" );
  const QString sql = QStringLiteral( "\"type\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::MultiSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  // Empty list --> no subset string --> all features visible
  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{};
  mController->processFilters( filterValues );

  QVERIFY( layer->subsetString().isEmpty() );
  QCOMPARE( layer->featureCount(), ( long long ) 43 );
}

// Number range
void TestFilterController::testNumberRange()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "roads" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "Lenthg" );
  const QString sql = QStringLiteral( "\"Lenthg\" >= @@value_from@@ AND \"Lenthg\" <= @@value_to@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::NumberFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ 1000, 3000 };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral(
                             "(\"Lenthg\" >= 1000 AND \"Lenthg\" <= 3000)" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 6 );
}

// Text filter
void TestFilterController::testTextFilter()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "roads" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "Condition" );
  const QString sql = QStringLiteral( "\"Condition\" LIKE '%@@value@@%'" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::TextFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QStringLiteral( "great" ) };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral( "(\"Condition\" LIKE '%great%')" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 14 );
}

// Checkbox filter
void TestFilterController::testCheckboxFilter()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "roads" ) ).at( 0 ) );
  QVERIFY( layer );

  const QString fieldName = QStringLiteral( "Paved?" );
  const QString sql = QStringLiteral( "\"Paved?\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::CheckboxFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ 1 };
  mController->processFilters( filterValues );

  const QString expected = QStringLiteral( "(\"Paved?\" = 1)" );
  QCOMPARE( layer->subsetString(), expected );
  QCOMPARE( layer->featureCount(), ( long long ) 19 );
}

// Predefined subset string
void TestFilterController::testPredefinedSubsetString()
{
  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayersByName( QStringLiteral( "addresses" ) ).at( 0 ) );
  QVERIFY( layer );

  // The addresses layer has a predefined subset: "PSC" IN (90201, 84101, 84102)
  QVERIFY( !layer->subsetString().isEmpty() );

  const QString fieldName = QStringLiteral( "OBEC" );
  const QString sql = QStringLiteral( "\"OBEC\" = @@value@@" );
  const QString filterId = TestUtils::setupControllerWithFilter(
                             mController.get(), FieldFilter::SingleSelectFilter, layer->id(), fieldName, sql );
  QVERIFY( !filterId.isEmpty() );

  QVariantMap filterValues;
  filterValues[filterId] = QVariantList{ QStringLiteral( "Pezinok" ) };
  mController->processFilters( filterValues );

  // The predefined subset and the new filter should be AND'd together.
  // Normalize whitespace so the test survives project re-saves in QGIS.
  const QString actual = layer->subsetString().simplified();
  QVERIFY2( actual.startsWith( QStringLiteral( "\"PSC\" IN (90201, 84101, 84102)" ) ),
            qPrintable( QStringLiteral( "Predefined subset missing or changed: %1" ).arg( actual ) ) );
  QVERIFY2( actual.endsWith( QStringLiteral( "AND (\"OBEC\" = 'Pezinok')" ) ),
            qPrintable( QStringLiteral( "Filter expression not appended: %1" ).arg( actual ) ) );
  QCOMPARE( layer->featureCount(), ( long long ) 39 );
}
