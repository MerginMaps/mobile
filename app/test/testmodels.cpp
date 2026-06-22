/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testmodels.h"
#include "testutils.h"
#include "layerfeaturesmodel.h"
#include "staticfeaturesmodel.h"
#include "inputmapsettings.h"
#include "valuerelationfeaturesmodel.h"
#include "valuerelationcontroller.h"
#include "projectsmodel.h"
#include "projectsproxymodel.h"

#include <QtTest/QtTest>


void TestModels::init()
{

}

void TestModels::cleanup()
{

}

void TestModels::testStaticFeaturesModel()
{
  StaticFeaturesModel model;

  QVERIFY( model.rowCount() == 0 );

  QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
  QgsVectorLayer *layer = new QgsVectorLayer( projectDir + "/db.gpkg|layername=main", "base", "ogr" );

  QVERIFY( layer && layer->isValid() );

  FeatureLayerPairs pairs;
  QgsFeature f1 = layer->getFeature( 1 );
  QVERIFY( f1.isValid() );

  QgsFeature f2 = layer->getFeature( 2 );
  QVERIFY( f2.isValid() );

  pairs << FeatureLayerPair( f1, layer )
        << FeatureLayerPair( f2, layer );

  model.populate( pairs );
  QCOMPARE( model.rowCount(), 2 );

  InputMapSettings settings;
  QgsCoordinateReferenceSystem targetCrs( QStringLiteral( "EPSG:3857" ) );
  settings.setDestinationCrs( targetCrs );

  QgsGeometry geom = model.collectGeometries( &settings );
  QCOMPARE( geom.asWkt( 1 ), QLatin1String( "MultiPoint ((-132502.9 79191),(-132502.9 79191))" ) );

  FeatureLayerPairs returnedPairs = model.features();
  QVERIFY( pairs == returnedPairs );

  model.remove( FeatureLayerPair( f1, layer ) );
  QCOMPARE( model.rowCount(), 1 );

  // cannot remove what's not there
  model.remove( FeatureLayerPair( f1, layer ) );
  QCOMPARE( model.rowCount(), 1 );

  geom = model.collectGeometries( &settings );
  QCOMPARE( geom.asWkt( 1 ), QLatin1String( "MultiPoint ((-132502.9 79191))" ) );

  model.append( FeatureLayerPair( f1, layer ) );
  QCOMPARE( model.rowCount(), 2 );

  // cannot add the same feature pair twice
  model.append( FeatureLayerPair( f1, layer ) );
  QCOMPARE( model.rowCount(), 2 );
}

void TestModels::testLayerFeaturesModel()
{
  LayerFeaturesModel fModel;
  QSignalSpy spy( &fModel, &LayerFeaturesModel::fetchingResultsChanged );

  QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
  QgsVectorLayer *layer = new QgsVectorLayer( projectDir + "/db.gpkg|layername=main", "base", "ogr" );

  QVERIFY( layer && layer->isValid() );

  fModel.setLayer( layer );
  fModel.reloadFeatures();
  spy.wait();

  QCOMPARE( fModel.rowCount(), layer->dataProvider()->featureCount() );

  fModel.setSearchExpression( QStringLiteral( "Seco" ) );

  spy.wait();
  QCOMPARE( fModel.rowCount(), 1 );

  fModel.setSearchExpression( QLatin1String() );

  spy.wait();
  QCOMPARE( fModel.rowCount(), layer->dataProvider()->featureCount() );

  QVariant title = fModel.data( fModel.index( 0 ), FeaturesModel::FeatureTitle );
  QCOMPARE( title, QStringLiteral( "First" ) );
}

void TestModels::testLayerFeaturesModelSorted()
{
  LayerFeaturesModel model;

  QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );

  QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
  QgsVectorLayer *layer = new QgsVectorLayer( projectDir + "/db.gpkg|layername=subsub", "subsub", "ogr" );

  QVERIFY( layer && layer->isValid() );

  // enable sorting
  model.mUseAttributeTableSortOrder = true;
  QgsAttributeTableConfig conf = layer->attributeTableConfig();
  conf.setSortExpression( QStringLiteral( "Name" ) );
  layer->setAttributeTableConfig( conf );

  model.setLayer( layer );
  model.reloadFeatures();


  spy.wait();

  QCOMPARE( model.rowCount(), layer->dataProvider()->featureCount() );
  QCOMPARE( model.data( model.index( 0, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "A1" ) );
  QCOMPARE( model.data( model.index( 1, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "A2" ) );
  QCOMPARE( model.data( model.index( 2, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "B1" ) );
  QCOMPARE( model.data( model.index( 3, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "B2" ) );
  QCOMPARE( model.data( model.index( 4, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "C1" ) );
  QCOMPARE( model.data( model.index( 5, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "C2" ) );
  QCOMPARE( model.data( model.index( 6, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "D1" ) );
  QCOMPARE( model.data( model.index( 7, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "D2" ) );
  QCOMPARE( model.data( model.index( 8, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "VERYBIG" ) );

  // filter the model and reverse sort order
  conf.setSortOrder( Qt::DescendingOrder );
  layer->setAttributeTableConfig( conf );

  model.setSearchExpression( QStringLiteral( "D" ) );


  spy.wait();

  QCOMPARE( model.rowCount(), 2 );
  QCOMPARE( model.data( model.index( 0, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "D2" ) );
  QCOMPARE( model.data( model.index( 1, 0 ), FeaturesModel::ModelRoles::FeatureTitle ), QLatin1String( "D1" ) );

  // disable sorting and filtering
  // should get all items with default ordering
  model.mUseAttributeTableSortOrder = false;

  model.setSearchExpression( QString() );

  spy.wait();

  QCOMPARE( model.rowCount(), layer->dataProvider()->featureCount() );
  QCOMPARE( model.data( model.index( 0, 0 ), FeaturesModel::ModelRoles::FeatureId ), 1 );
  QCOMPARE( model.data( model.index( 1, 0 ), FeaturesModel::ModelRoles::FeatureId ), 2 );
  QCOMPARE( model.data( model.index( 2, 0 ), FeaturesModel::ModelRoles::FeatureId ), 3 );
  QCOMPARE( model.data( model.index( 3, 0 ), FeaturesModel::ModelRoles::FeatureId ), 4 );
  QCOMPARE( model.data( model.index( 4, 0 ), FeaturesModel::ModelRoles::FeatureId ), 5 );
  QCOMPARE( model.data( model.index( 5, 0 ), FeaturesModel::ModelRoles::FeatureId ), 6 );
  QCOMPARE( model.data( model.index( 6, 0 ), FeaturesModel::ModelRoles::FeatureId ), 7 );
  QCOMPARE( model.data( model.index( 7, 0 ), FeaturesModel::ModelRoles::FeatureId ), 8 );
  QCOMPARE( model.data( model.index( 8, 0 ), FeaturesModel::ModelRoles::FeatureId ), 100000000 );
}

void TestModels::testValueRelationFeaturesModel()
{
  // Tests the drawer model: lazy loading, new KeyColumn/ValueColumn roles,
  // filter expressions and search — all without form-pair dependency.

  const QString projectDir  = TestUtils::testDataDir() + "/project_value_relations";
  QVERIFY( QgsProject::instance()->read( projectDir + "/proj.qgz" ) );

  QgsMapLayer *subsubL = QgsProject::instance()->mapLayersByName( QStringLiteral( "subsub" ) ).at( 0 );
  QgsVectorLayer *subsubLayer = static_cast<QgsVectorLayer *>( subsubL );
  QVERIFY( subsubLayer && subsubLayer->isValid() );

  const QVariantMap baseConfig =
  {
    { QStringLiteral( "Layer" ), QStringLiteral( "subsub_df9d0ba0_2ec8_4a2c_9f96_84576e37c126" ) },
    { QStringLiteral( "Key" ),   QStringLiteral( "fid" ) },
    { QStringLiteral( "Value" ), QStringLiteral( "Name" ) },
  };

  // ── 1. No auto-load: setConfig must not trigger populate ──────────────
  {
    ValueRelationFeaturesModel model;
    QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );

    model.setConfig( baseConfig );

    // Give the event loop a moment; no async work should have started.
    QTest::qWait( 100 );
    QVERIFY( spy.isEmpty() );
    QCOMPARE( model.rowCount(), 0 );
    QCOMPARE( model.layer()->id(), subsubLayer->id() );
  }

  // ── 2. Explicit populate loads features; KeyColumn / ValueColumn roles ─
  {
    ValueRelationFeaturesModel model;
    QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );

    model.setConfig( baseConfig );
    model.reloadFeatures();
    spy.wait();

    QCOMPARE( model.rowCount(), 9 );

    // KeyColumn returns the raw key-field attribute
    QCOMPARE( model.data( model.index( 0, 0 ), ValueRelationFeaturesModel::KeyColumn ), QVariant( 1 ) );
    QCOMPARE( model.data( model.index( 8, 0 ), ValueRelationFeaturesModel::KeyColumn ), QVariant( 100000000 ) );

    // ValueColumn returns the display-label attribute
    // (insert order from the fixture: first entry is not "A1" without sorting)
    QVERIFY( !model.data( model.index( 0, 0 ), ValueRelationFeaturesModel::ValueColumn ).toString().isEmpty() );
  }

  // ── 3. OrderByValue sorts by the value field ───────────────────────────
  {
    QVariantMap config = baseConfig;
    config[ QStringLiteral( "OrderByValue" ) ] = true;

    ValueRelationFeaturesModel model;
    QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );

    model.setConfig( config );
    model.reloadFeatures();
    spy.wait();

    QCOMPARE( model.rowCount(), 9 );
    QCOMPARE( model.data( model.index( 0, 0 ), ValueRelationFeaturesModel::ValueColumn ), QLatin1String( "A1" ) );
    QCOMPARE( model.data( model.index( 1, 0 ), ValueRelationFeaturesModel::ValueColumn ), QLatin1String( "A2" ) );
    QCOMPARE( model.data( model.index( 8, 0 ), ValueRelationFeaturesModel::ValueColumn ), QLatin1String( "VERYBIG" ) );
  }

  // ── 4. Search expression filters loaded results ────────────────────────
  {
    QVariantMap config = baseConfig;
    config[ QStringLiteral( "OrderByValue" ) ] = true;

    ValueRelationFeaturesModel model;
    QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );

    model.setConfig( config );
    model.reloadFeatures();
    spy.wait();

    model.setSearchExpression( QStringLiteral( "D" ) );
    spy.wait();

    QCOMPARE( model.rowCount(), 2 );
    QCOMPARE( model.data( model.index( 0, 0 ), ValueRelationFeaturesModel::ValueColumn ), QLatin1String( "D1" ) );
    QCOMPARE( model.data( model.index( 1, 0 ), ValueRelationFeaturesModel::ValueColumn ), QLatin1String( "D2" ) );
  }

  // ── 5. Static FilterExpression restricts results ───────────────────────
  {
    QVariantMap config = baseConfig;
    config[ QStringLiteral( "OrderByValue" ) ]     = true;
    config[ QStringLiteral( "FilterExpression" ) ] = QStringLiteral( "subFk = 1" );

    ValueRelationFeaturesModel model;
    QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );

    model.setConfig( config );
    model.reloadFeatures();
    spy.wait();

    QCOMPARE( model.rowCount(), 2 );
    QCOMPARE( model.data( model.index( 0, 0 ), ValueRelationFeaturesModel::ValueColumn ), QLatin1String( "A1" ) );
    QCOMPARE( model.data( model.index( 1, 0 ), ValueRelationFeaturesModel::ValueColumn ), QLatin1String( "A2" ) );
  }
}

void TestModels::testValueRelationController() {}
// {
//   // The conversion helpers are non-static Q_INVOKABLE methods; an unconfigured
//   // helper is sufficient since they use no instance state.
//   ValueRelationController conv;

//   // ── convertFromQgisFormat ─────────────────────────────────────────────
//   // Multi-value: parse QGIS "{...}" wire format
//   QCOMPARE( conv.convertFromQgisFormat( QStringLiteral( "{1,2,3}" ), true ),
//             QStringList( { QStringLiteral( "1" ), QStringLiteral( "2" ), QStringLiteral( "3" ) } ) );

//   // Single-value: treat as plain value, not list syntax
//   QCOMPARE( conv.convertFromQgisFormat( QVariant( 42 ), false ),
//             QStringList( { QStringLiteral( "42" ) } ) );

//   // Null/empty input produces empty list
//   QVERIFY( conv.convertFromQgisFormat( QVariant(), false ).isEmpty() );
//   QVERIFY( conv.convertFromQgisFormat( QVariant(), true ).isEmpty() );
//   QVERIFY( conv.convertFromQgisFormat( QStringLiteral( "" ), true ).isEmpty() );

//   // Large integer key (no scientific-notation rounding)
//   QCOMPARE( conv.convertFromQgisFormat( QStringLiteral( "{100000000}" ), true ),
//             QStringList( { QStringLiteral( "100000000" ) } ) );

//   // ── convertToQgisFormat ───────────────────────────────────────────────
//   QCOMPARE( conv.convertToQgisFormat( { QStringLiteral( "1" ), QStringLiteral( "2" ), QStringLiteral( "3" ) } ),
//             QStringLiteral( "{1,2,3}" ) );
//   QCOMPARE( conv.convertToQgisFormat( { QStringLiteral( "42" ) } ), QStringLiteral( "{42}" ) );
//   QCOMPARE( conv.convertToQgisFormat( {} ), QStringLiteral( "{}" ) );

//   // Round-trip
//   {
//     const QString original = QStringLiteral( "{7,8,100000000}" );
//     QCOMPARE( conv.convertToQgisFormat( conv.convertFromQgisFormat( original, true ) ), original );
//   }

//   // ── Instance method: lookupDisplayValues ──────────────────────────────
//   const QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
//   QVERIFY( QgsProject::instance()->read( projectDir + "/proj.qgz" ) );

//   // Single-value config (AllowMulti=false, the default)
//   const QVariantMap singleConfig =
//   {
//     { QStringLiteral( "Layer" ), QStringLiteral( "subsub_df9d0ba0_2ec8_4a2c_9f96_84576e37c126" ) },
//     { QStringLiteral( "Key" ),   QStringLiteral( "fid" ) },
//     { QStringLiteral( "Value" ), QStringLiteral( "Name" ) },
//   };

//   // Multi-value config (AllowMulti=true) for {…} wire format
//   const QVariantMap multiConfig =
//   {
//     { QStringLiteral( "Layer" ),     QStringLiteral( "subsub_df9d0ba0_2ec8_4a2c_9f96_84576e37c126" ) },
//     { QStringLiteral( "Key" ),       QStringLiteral( "fid" ) },
//     { QStringLiteral( "Value" ),     QStringLiteral( "Name" ) },
//     { QStringLiteral( "AllowMulti" ), true },
//   };

//   ValueRelationController helper;
//   helper.setConfig( singleConfig );
//   QSignalSpy resultSpy( &helper, &ValueRelationController::displayValuesReady );

//   // Helper to extract the latest result from the spy
//   auto latestResult = [&]() -> QVariantList {
//     return resultSpy.last().at( 0 ).toList();
//   };

//   // Single-value lookup: key 1 → one display label
//   helper.lookupDisplayValues( QVariant( 1 ) );
//   QVERIFY( resultSpy.wait() );
//   QCOMPARE( latestResult().size(), 1 );
//   QVERIFY( !latestResult().at( 0 ).toString().isEmpty() );

//   // Null field value: empty result emitted synchronously (no async work started).
//   // QSignalSpy::wait() expects one *new* emission after the call, but this one
//   // fires before wait() returns, so check the count directly instead.
//   {
//     const int countBefore = resultSpy.count();
//     helper.lookupDisplayValues( QVariant() );
//     QCOMPARE( resultSpy.count(), countBefore + 1 );
//     QVERIFY( latestResult().isEmpty() );
//   }

//   // Large FID key does not lose precision
//   helper.lookupDisplayValues( QVariant( 100000000 ) );
//   QVERIFY( resultSpy.wait() );
//   QCOMPARE( latestResult().size(), 1 );
//   QCOMPARE( latestResult().at( 0 ).toString(), QStringLiteral( "VERYBIG" ) );

//   // Multi-value lookup: "{1,2}" parsed with AllowMulti=true → two results
//   {
//     ValueRelationController multiHelper;
//     QSignalSpy multiSpy( &multiHelper, &ValueRelationController::displayValuesReady );
//     multiHelper.setConfig( multiConfig );
//     multiHelper.lookupDisplayValues( QStringLiteral( "{1,2}" ) );
//     QVERIFY( multiSpy.wait() );
//     QCOMPARE( multiSpy.last().at( 0 ).toList().size(), 2 );
//   }
// }

void TestModels::testValueRelationControllerInvalidation() {}
// {
//   // Invalidation must fire when a FilterExpression is present and the lookup
//   // returns nothing (value became unavailable due to a context change).
//   // Without a FilterExpression, invalidation must NOT fire even for a missing key.

//   const QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
//   QVERIFY( QgsProject::instance()->read( projectDir + "/proj.qgz" ) );

//   QgsMapLayer *mainL = QgsProject::instance()->mapLayersByName( QStringLiteral( "main" ) ).at( 0 );
//   QgsVectorLayer *mainLayer = static_cast<QgsVectorLayer *>( mainL );
//   QVERIFY( mainLayer && mainLayer->isValid() );

//   // ── No FilterExpression: no invalidate, even for a bogus key ──────────
//   {
//     const QVariantMap config =
//     {
//       { QStringLiteral( "Layer" ), QStringLiteral( "subsub_df9d0ba0_2ec8_4a2c_9f96_84576e37c126" ) },
//       { QStringLiteral( "Key" ),   QStringLiteral( "fid" ) },
//       { QStringLiteral( "Value" ), QStringLiteral( "Name" ) },
//     };

//     ValueRelationController helper;
//     QSignalSpy invalidateSpy( &helper, &ValueRelationController::invalidate );
//     QSignalSpy resultSpy( &helper, &ValueRelationController::displayValuesReady );
//     helper.setConfig( config );

//     helper.lookupDisplayValues( QVariant( 9999 ) ); // key that does not exist
//     QVERIFY( resultSpy.wait() ); // wait for async to complete
//     QVERIFY( invalidateSpy.isEmpty() ); // must NOT invalidate
//   }

//   // ── With FilterExpression: invalidate when value is outside filtered set
//   {
//     // subFk=1 restricts subsub to only fid 1 and 2.
//     // If the stored value is fid=5 (which has subFk=3), the lookup returns nothing
//     // → should emit invalidate.
//     const QVariantMap config =
//     {
//       { QStringLiteral( "Layer" ),            QStringLiteral( "subsub_df9d0ba0_2ec8_4a2c_9f96_84576e37c126" ) },
//       { QStringLiteral( "Key" ),              QStringLiteral( "fid" ) },
//       { QStringLiteral( "Value" ),            QStringLiteral( "Name" ) },
//       { QStringLiteral( "FilterExpression" ), QStringLiteral( "subFk = 1" ) },
//     };

//     ValueRelationController helper;
//     QSignalSpy invalidateSpy( &helper, &ValueRelationController::invalidate );
//     QSignalSpy resultSpy( &helper, &ValueRelationController::displayValuesReady );
//     helper.setConfig( config );

//     // fid=5 is not in the subFk=1 subset → should trigger invalidate
//     helper.lookupDisplayValues( QVariant( 5 ) );
//     QVERIFY( resultSpy.wait() );
//     QCOMPARE( invalidateSpy.count(), 1 );
//   }

//   // ── With FilterExpression: no invalidate when value IS in filtered set ─
//   {
//     const QVariantMap config =
//     {
//       { QStringLiteral( "Layer" ),            QStringLiteral( "subsub_df9d0ba0_2ec8_4a2c_9f96_84576e37c126" ) },
//       { QStringLiteral( "Key" ),              QStringLiteral( "fid" ) },
//       { QStringLiteral( "Value" ),            QStringLiteral( "Name" ) },
//       { QStringLiteral( "FilterExpression" ), QStringLiteral( "subFk = 1" ) },
//     };

//     ValueRelationController helper;
//     QSignalSpy spy( &helper, &ValueRelationController::invalidate );
//     helper.setConfig( config );

//     // From the fixture the "subFk=1" subset contains fid 1 and 2 (A1/A2 or similar).
//     // We look up all features with that filter to find a valid key, then use it.
//     ValueRelationFeaturesModel probeModel;
//     QSignalSpy probeSpy( &probeModel, &LayerFeaturesModel::fetchingResultsChanged );
//     probeModel.setConfig( config );
//     probeModel.reloadFeatures();
//     probeSpy.wait();
//     QVERIFY( probeModel.rowCount() > 0 );

//     const QVariant validKey = probeModel.data( probeModel.index( 0, 0 ), ValueRelationFeaturesModel::KeyColumn );

//     QSignalSpy invalidateSpy( &helper, &ValueRelationController::invalidate );
//     QSignalSpy resultSpy( &helper, &ValueRelationController::displayValuesReady );
//     helper.lookupDisplayValues( validKey );
//     QVERIFY( resultSpy.wait() );
//     QVERIFY( !resultSpy.last().at( 0 ).toList().isEmpty() );
//     QVERIFY( invalidateSpy.isEmpty() );
//   }
// }

void TestModels::testProjectsModel()
{
  Project p0;
  p0.local.projectNamespace = QStringLiteral( "namespace" );
  p0.local.projectName = QStringLiteral( "project_B" );
  p0.local.projectDir = QStringLiteral( "project_B_dir" );

  Project p1;
  p1.local.projectNamespace = QStringLiteral( "namespace" );
  p1.local.projectName = QStringLiteral( "project_A" );
  p1.local.projectDir = QStringLiteral( "project_A_dir" );

  Project p2;
  p2.local.projectNamespace = QStringLiteral( "namespace" );
  p2.local.projectName = QStringLiteral( "project_C" );
  p2.local.projectDir = QStringLiteral( "project_C_dir" );

  ProjectsModel model;
  model.setModelType( ProjectsModel::LocalProjectsModel );
  model.mProjects << p0 << p1 << p2;

  QCOMPARE( model.rowCount(), 3 );

  // test ProjectsModel::Roles::ProjectIsActiveProject

  // No active project initially
  QVERIFY( !model.data( model.index( 0 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );
  QVERIFY( !model.data( model.index( 1 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );
  QVERIFY( !model.data( model.index( 2 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );

  // Set an active project
  model.setActiveProjectId( p0.id() );
  QCOMPARE( model.activeProjectId(), p0.id() );
  QVERIFY( model.data( model.index( 0 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );
  QVERIFY( !model.data( model.index( 1 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );
  QVERIFY( !model.data( model.index( 2 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );

  // Change active project
  model.setActiveProjectId( p2.id() );
  QCOMPARE( model.activeProjectId(), p2.id() );
  QVERIFY( !model.data( model.index( 0 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );
  QVERIFY( !model.data( model.index( 1 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );
  QVERIFY( model.data( model.index( 2 ), ProjectsModel::Roles::ProjectIsActiveProject ).toBool() );
}

void TestModels::testProjectsProxyModel()
{
  Project p0;
  p0.local.projectNamespace = QStringLiteral( "namespace" );
  p0.local.projectName = QStringLiteral( "project_B" );
  p0.local.projectDir = QStringLiteral( "project_B_dir" );

  Project p1;
  p1.local.projectNamespace = QStringLiteral( "namespace" );
  p1.local.projectName = QStringLiteral( "project_A" );
  p1.local.projectDir = QStringLiteral( "project_A_dir" );

  Project p2;
  p2.local.projectNamespace = QStringLiteral( "namespace" );
  p2.local.projectName = QStringLiteral( "project_C" );
  p2.local.projectDir = QStringLiteral( "project_C_dir" );

  ProjectsModel model;
  model.setModelType( ProjectsModel::LocalProjectsModel );
  model.mProjects << p0 << p1 << p2;

  QCOMPARE( model.rowCount(), 3 );

  ProjectsProxyModel proxy;
  proxy.setProjectSourceModel( &model );
  proxy.initialize();

  QCOMPARE( proxy.rowCount(), 3 );

  // No active project initially, normally sorted
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p1.id() );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p0.id() );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p2.id() );

  // Active project set, still normally sorted
  model.setActiveProjectId( p2.id() );
  proxy.invalidate(); // re-sort
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p1.id() );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p0.id() );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p2.id() );

  // Active project always first set, active project is first
  proxy.setActiveProjectAlwaysFirst( true );
  QVERIFY( proxy.activeProjectAlwaysFirst() );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p2.id() );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p1.id() );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), ProjectsModel::Roles::ProjectId ).toString(), p0.id() );
}
