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

void TestModels::testValueRelationOrdering()
{
  /* Tests the four ordering permutations against a layer whose insertion order
   * is neither key-sorted nor label-sorted:
   *
   *   key->label:  1->Alpha  2->Delta  3->Gamma  4->Beta
   *
   *   1. OrderByKey  asc  -> 1(Alpha), 2(Delta), 3(Gamma), 4(Beta)
   *   2. OrderByKey  desc -> 4(Beta),  3(Gamma), 2(Delta), 1(Alpha)
   *   3. OrderByValue asc -> 1(Alpha), 4(Beta),  2(Delta), 3(Gamma)
   *   4. OrderByField "label" asc -> same as 3, but different code path
   *   5. OrderByField desc -> 3(Gamma), 2(Delta), 4(Beta), 1(Alpha)
   */
  QgsProject::instance()->removeAllMapLayers();

  QgsVectorLayer *layer = TestUtils::createVROrderingLayer();
  QVERIFY( layer && layer->isValid() );
  QCOMPARE( static_cast<int>( layer->featureCount() ), 4 );
  QgsProject::instance()->addMapLayer( layer );

  const QVariantMap baseConfig =
  {
    { QStringLiteral( "Layer" ), layer->id() },
    { QStringLiteral( "Key" ),   QStringLiteral( "key" ) },
    { QStringLiteral( "Value" ), QStringLiteral( "label" ) }
  };

  ValueRelationFeaturesModel model;
  model.setConfig( baseConfig );
  QCOMPARE( model.rowCount(), 0 ); // setConfig should not populate the model
  QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );

  auto keyAt = [&]( int row )
  {
    return model.data( model.index( row, 0 ), ValueRelationFeaturesModel::KeyColumn ).toString();
  };
  auto valAt = [&]( int row )
  {
    return model.data( model.index( row, 0 ), ValueRelationFeaturesModel::ValueColumn ).toString();
  };

  auto reload = [&]( const QVariantMap & config )
  {
    spy.clear();
    model.setConfig( config );
    model.reloadFeatures();
    while ( spy.count() < 2 )
      QVERIFY( spy.wait( 5000 ) );
  };

  // 1. OrderByKey ascending
  QVariantMap config = baseConfig;
  config[ QStringLiteral( "OrderByKey" ) ]        = true;
  config[ QStringLiteral( "OrderByDescending" ) ] = false;
  reload( config );
  QCOMPARE( model.rowCount(), 4 );
  QCOMPARE( keyAt( 0 ), QStringLiteral( "1" ) );
  QCOMPARE( keyAt( 1 ), QStringLiteral( "2" ) );
  QCOMPARE( keyAt( 2 ), QStringLiteral( "3" ) );
  QCOMPARE( keyAt( 3 ), QStringLiteral( "4" ) );

  // 2. OrderByKey descending
  config[ QStringLiteral( "OrderByDescending" ) ] = true;
  reload( config );
  QCOMPARE( model.rowCount(), 4 );
  QCOMPARE( keyAt( 0 ), QStringLiteral( "4" ) );
  QCOMPARE( keyAt( 1 ), QStringLiteral( "3" ) );
  QCOMPARE( keyAt( 2 ), QStringLiteral( "2" ) );
  QCOMPARE( keyAt( 3 ), QStringLiteral( "1" ) );

  // 3. OrderByValue ascending
  config = baseConfig;
  config[ QStringLiteral( "OrderByValue" ) ]      = true;
  config[ QStringLiteral( "OrderByDescending" ) ] = false;
  reload( config );
  QCOMPARE( model.rowCount(), 4 );
  QCOMPARE( valAt( 0 ), QStringLiteral( "Alpha" ) );
  QCOMPARE( valAt( 1 ), QStringLiteral( "Beta" ) );
  QCOMPARE( valAt( 2 ), QStringLiteral( "Delta" ) );
  QCOMPARE( valAt( 3 ), QStringLiteral( "Gamma" ) );

  // 4. OrderByField with an explicit field name — same result as 3, different code path
  config = baseConfig;
  config[ QStringLiteral( "OrderByField" ) ]      = true;
  config[ QStringLiteral( "OrderByFieldName" ) ]  = QStringLiteral( "label" );
  config[ QStringLiteral( "OrderByDescending" ) ] = false;
  reload( config );
  QCOMPARE( model.rowCount(), 4 );
  QCOMPARE( valAt( 0 ), QStringLiteral( "Alpha" ) );
  QCOMPARE( valAt( 1 ), QStringLiteral( "Beta" ) );
  QCOMPARE( valAt( 2 ), QStringLiteral( "Delta" ) );
  QCOMPARE( valAt( 3 ), QStringLiteral( "Gamma" ) );

  // 5. OrderByField descending
  config[ QStringLiteral( "OrderByDescending" ) ] = true;
  reload( config );
  QCOMPARE( model.rowCount(), 4 );
  QCOMPARE( valAt( 0 ), QStringLiteral( "Gamma" ) );
  QCOMPARE( valAt( 1 ), QStringLiteral( "Delta" ) );
  QCOMPARE( valAt( 2 ), QStringLiteral( "Beta" ) );
  QCOMPARE( valAt( 3 ), QStringLiteral( "Alpha" ) );

  QgsProject::instance()->removeAllMapLayers();
}

void TestModels::testValueRelationSearch()
{
  /* Tests that search applies only to the value (label) column, not the key.
   *
   * Layer:
   *   key=1    label="Alpha"  — key contains "1", label does not
   *   key=2    label="Val1"   — label contains "1", key does not
   *   key=100  label="Gamma"  — key contains "1", label does not
   *
   * Search "1" must return only "Val1" — proving that key=1 and key=100 are
   * ignored because buildSearchExpression() filters on the value field only.
   */
  QgsProject::instance()->removeAllMapLayers();

  auto *layer = new QgsVectorLayer(
    QStringLiteral( "None?field=key:integer&field=label:string" ),
    QStringLiteral( "vr_search" ),
    QStringLiteral( "memory" )
  );
  QVERIFY( layer && layer->isValid() );

  struct Row { int key; QString label; };
  const QList<Row> rows = { {1, QStringLiteral( "Alpha" )}, {2, QStringLiteral( "Val1" )}, {100, QStringLiteral( "Gamma" )} };

  QgsFeatureList features;
  for ( const auto &row : rows )
  {
    QgsFeature f( layer->fields() );
    f.setAttribute( QStringLiteral( "key" ),   row.key );
    f.setAttribute( QStringLiteral( "label" ), row.label );
    features << f;
  }
  layer->dataProvider()->addFeatures( features );
  QCOMPARE( layer->featureCount(), ( long long ) 3 );
  QgsProject::instance()->addMapLayer( layer );

  const QVariantMap config =
  {
    { QStringLiteral( "Layer" ),        layer->id() },
    { QStringLiteral( "Key" ),          QStringLiteral( "key" ) },
    { QStringLiteral( "Value" ),        QStringLiteral( "label" ) },
    { QStringLiteral( "OrderByValue" ), true }
  };

  ValueRelationFeaturesModel model;
  QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );
  auto waitForReload = [&]()
  {
    while ( spy.count() < 2 )
      QVERIFY( spy.wait( 5000 ) );
  };

  model.setConfig( config );

  // Initial load: all 3 features
  model.reloadFeatures();
  waitForReload();
  QCOMPARE( model.rowCount(), 3 );

  // Search "1": only "Val1" matches (value column); key=1 and key=100 are ignored
  spy.clear();
  model.setSearchExpression( QStringLiteral( "1" ) );
  waitForReload();
  QCOMPARE( model.rowCount(), 1 );
  QCOMPARE( model.data( model.index( 0, 0 ), ValueRelationFeaturesModel::ValueColumn ).toString(),
            QStringLiteral( "Val1" ) );

  // Clear search: all 3 features again
  spy.clear();
  model.setSearchExpression( QString() );
  waitForReload();
  QCOMPARE( model.rowCount(), 3 );

  // Search with no match
  spy.clear();
  model.setSearchExpression( QStringLiteral( "xyz" ) );
  waitForReload();
  QCOMPARE( model.rowCount(), 0 );

  QgsProject::instance()->removeAllMapLayers();
}

void TestModels::testValueRelationHotreload()
{
  /* Tests that calling reloadFeatures() after modifying the underlying layer
   * triggers hot reload -> reloading after the drawer is already open
   * Also tests auto-reload: LayerFeaturesModel connects featureAdded to populate(),
   * so adding a feature during an editing session triggers an async reload.
   */
  QgsProject::instance()->removeAllMapLayers();

  QgsVectorLayer *layer = TestUtils::createVRLookupLayer( 3 );
  QVERIFY( layer && layer->isValid() );
  QgsProject::instance()->addMapLayer( layer );

  const QVariantMap config =
  {
    { QStringLiteral( "Layer" ),      layer->id() },
    { QStringLiteral( "Key" ),        QStringLiteral( "key" ) },
    { QStringLiteral( "Value" ),      QStringLiteral( "label" ) },
    { QStringLiteral( "OrderByKey" ), true }
  };

  ValueRelationFeaturesModel model;
  QSignalSpy spy( &model, &LayerFeaturesModel::fetchingResultsChanged );
  auto waitForReload = [&]()
  {
    while ( spy.count() < 2 )
      QVERIFY( spy.wait( 5000 ) );
  };

  model.setConfig( config );

  // Initial load: 3 features
  model.reloadFeatures();
  waitForReload();
  QCOMPARE( model.rowCount(), 3 );

  // Add a feature directly to the provider
  QgsFeature newFeature( layer->fields() );
  newFeature.setAttribute( QStringLiteral( "key" ),   4 );
  newFeature.setAttribute( QStringLiteral( "label" ), QStringLiteral( "Label 4" ) );
  QVERIFY( layer->dataProvider()->addFeatures( QgsFeatureList() << newFeature ) );

  // Manual hot reload: reloadFeatures() must pick up the added feature
  spy.clear();
  model.reloadFeatures();
  waitForReload();
  QCOMPARE( model.rowCount(), 4 );
  QCOMPARE( model.data( model.index( 3, 0 ), ValueRelationFeaturesModel::KeyColumn ).toString(),
            QStringLiteral( "4" ) );

  // Auto-reload via featureAdded signal:
  // startEditing + addFeature fires featureAdded, which LayerFeaturesModel connects to populate().
  // addFeature and commitChanges can each trigger a populate cycle (2 emissions each);
  // wait until fetching has stopped (last emission is false) to avoid reading rowCount mid-flight.
  spy.clear();
  layer->startEditing();
  QgsFeature editFeature( layer->fields() );
  editFeature.setAttribute( QStringLiteral( "key" ),   5 );
  editFeature.setAttribute( QStringLiteral( "label" ), QStringLiteral( "Label 5" ) );
  QVERIFY( layer->addFeature( editFeature ) );
  QVERIFY( layer->commitChanges() );
  while ( spy.isEmpty() || spy.last().at( 0 ).toBool() )
    QVERIFY( spy.wait( 5000 ) );
  QCOMPARE( model.rowCount(), 5 );

  QgsProject::instance()->removeAllMapLayers();
}

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
