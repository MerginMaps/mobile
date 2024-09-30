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
#include "featuresmodel.h"
#include "featuresproxymodel.h"
#include "valuerelationfeaturesmodel.h"
#include "projectsmodel.h"
#include "projectsproxymodel.h"

#include <QtTest/QtTest>


void TestModels::init()
{

}

void TestModels::cleanup()
{

}

void TestModels::testFeaturesModel()
{
  FeaturesModel fModel;
  QSignalSpy spy( &fModel, &FeaturesModel::fetchingResultsChanged );

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

void TestModels::testFeaturesProxyModel()
{
  FeaturesModel model;
  FeaturesProxyModel proxy;

  QSignalSpy spy( &model, &FeaturesModel::fetchingResultsChanged );

  QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
  QgsVectorLayer *layer = new QgsVectorLayer( projectDir + "/db.gpkg|layername=subsub", "subsub", "ogr" );

  QVERIFY( layer && layer->isValid() );

  // enable sorting
  QgsAttributeTableConfig conf = layer->attributeTableConfig();
  conf.setSortExpression( QStringLiteral( "Name" ) );
  layer->setAttributeTableConfig( conf );

  proxy.setFeaturesSourceModel( &model );
  model.setLayer( layer );
  model.reloadFeatures();


  spy.wait();

  QCOMPARE( proxy.rowCount(), layer->dataProvider()->featureCount() );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::SortValue ), QLatin1String( "A1" ) );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::SortValue ), QLatin1String( "A2" ) );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), FeaturesModel::SortValue ), QLatin1String( "B1" ) );
  QCOMPARE( proxy.data( proxy.index( 3, 0 ), FeaturesModel::SortValue ), QLatin1String( "B2" ) );
  QCOMPARE( proxy.data( proxy.index( 4, 0 ), FeaturesModel::SortValue ), QLatin1String( "C1" ) );
  QCOMPARE( proxy.data( proxy.index( 5, 0 ), FeaturesModel::SortValue ), QLatin1String( "C2" ) );
  QCOMPARE( proxy.data( proxy.index( 6, 0 ), FeaturesModel::SortValue ), QLatin1String( "D1" ) );
  QCOMPARE( proxy.data( proxy.index( 7, 0 ), FeaturesModel::SortValue ), QLatin1String( "D2" ) );
  QCOMPARE( proxy.data( proxy.index( 8, 0 ), FeaturesModel::SortValue ), QLatin1String( "VERYBIG" ) );

  // filter the fModel (this is not proxy model filtering)
  // and reverse sort order
  conf.setSortOrder( Qt::DescendingOrder );
  layer->setAttributeTableConfig( conf );
  model.setupSorting();

  model.setSearchExpression( QStringLiteral( "D" ) );


  spy.wait();

  QCOMPARE( proxy.rowCount(), 2 );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::SortValue ), QLatin1String( "D2" ) );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::SortValue ), QLatin1String( "D1" ) );

  // disable sorting and filtering
  // should get all items with default ordering
  conf.setSortExpression( QString() );
  layer->setAttributeTableConfig( conf );
  model.setupSorting();

  model.setSearchExpression( QString() );

  spy.wait();

  QCOMPARE( proxy.rowCount(), layer->dataProvider()->featureCount() );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::FeatureId ), 1 );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::FeatureId ), 2 );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), FeaturesModel::FeatureId ), 3 );
  QCOMPARE( proxy.data( proxy.index( 3, 0 ), FeaturesModel::FeatureId ), 4 );
  QCOMPARE( proxy.data( proxy.index( 4, 0 ), FeaturesModel::FeatureId ), 5 );
  QCOMPARE( proxy.data( proxy.index( 5, 0 ), FeaturesModel::FeatureId ), 6 );
  QCOMPARE( proxy.data( proxy.index( 6, 0 ), FeaturesModel::FeatureId ), 7 );
  QCOMPARE( proxy.data( proxy.index( 7, 0 ), FeaturesModel::FeatureId ), 8 );
  QCOMPARE( proxy.data( proxy.index( 8, 0 ), FeaturesModel::FeatureId ), 100000000 );
}

void TestModels::testFeaturesProxyModelWithValueRelation()
{
  QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
  QString projectName = "proj.qgz";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *mainL = QgsProject::instance()->mapLayersByName( QStringLiteral( "main" ) ).at( 0 );
  QgsVectorLayer *mainLayer = static_cast<QgsVectorLayer *>( mainL );

  QVERIFY( mainLayer && mainLayer->isValid() );

  QgsMapLayer *subsubL = QgsProject::instance()->mapLayersByName( QStringLiteral( "subsub" ) ).at( 0 );
  QgsVectorLayer *subsubLayer = static_cast<QgsVectorLayer *>( subsubL );

  QVERIFY( subsubLayer && subsubLayer->isValid() );

  QgsFeature f = mainLayer->getFeature( 1 );
  FeatureLayerPair pair( f, mainLayer );

  ValueRelationFeaturesModel model;
  FeaturesProxyModel proxy;
  proxy.setFeaturesSourceModel( &model );

  QSignalSpy spy( &model, &FeaturesModel::fetchingResultsChanged );

  // setup value relation, initially unsorted
  QVariantMap config =
  {
    { QStringLiteral( "Layer" ), QStringLiteral( "subsub_df9d0ba0_2ec8_4a2c_9f96_84576e37c126" ) },
    { QStringLiteral( "Key" ), QStringLiteral( "fid" ) },
    { QStringLiteral( "Value" ), QStringLiteral( "Name" ) },
  };
  model.setConfig( config );
  model.setPair( pair );

  spy.wait();

  QCOMPARE( model.rowCount(), 9 );
  QCOMPARE( model.layer()->id(), subsubLayer->id() );

  QCOMPARE( proxy.rowCount(), 9 );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::FeatureId ), 1 );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::FeatureId ), 2 );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), FeaturesModel::FeatureId ), 3 );
  QCOMPARE( proxy.data( proxy.index( 3, 0 ), FeaturesModel::FeatureId ), 4 );
  QCOMPARE( proxy.data( proxy.index( 4, 0 ), FeaturesModel::FeatureId ), 5 );
  QCOMPARE( proxy.data( proxy.index( 5, 0 ), FeaturesModel::FeatureId ), 6 );
  QCOMPARE( proxy.data( proxy.index( 6, 0 ), FeaturesModel::FeatureId ), 7 );
  QCOMPARE( proxy.data( proxy.index( 7, 0 ), FeaturesModel::FeatureId ), 8 );
  QCOMPARE( proxy.data( proxy.index( 8, 0 ), FeaturesModel::FeatureId ), 100000000 );

  // enable order by value for the value relation
  model.reset();
  config[ QStringLiteral( "OrderByValue" ) ] = true;
  model.setConfig( config );
  model.setupSorting();
  model.setPair( pair );

  spy.wait();

  QCOMPARE( proxy.rowCount(), 9 );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::SortValue ), QLatin1String( "A1" ) );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::SortValue ), QLatin1String( "A2" ) );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), FeaturesModel::SortValue ), QLatin1String( "B1" ) );
  QCOMPARE( proxy.data( proxy.index( 3, 0 ), FeaturesModel::SortValue ), QLatin1String( "B2" ) );
  QCOMPARE( proxy.data( proxy.index( 4, 0 ), FeaturesModel::SortValue ), QLatin1String( "C1" ) );
  QCOMPARE( proxy.data( proxy.index( 5, 0 ), FeaturesModel::SortValue ), QLatin1String( "C2" ) );
  QCOMPARE( proxy.data( proxy.index( 6, 0 ), FeaturesModel::SortValue ), QLatin1String( "D1" ) );
  QCOMPARE( proxy.data( proxy.index( 7, 0 ), FeaturesModel::SortValue ), QLatin1String( "D2" ) );
  QCOMPARE( proxy.data( proxy.index( 8, 0 ), FeaturesModel::SortValue ), QLatin1String( "VERYBIG" ) );

  // add a search expression to base model
  model.setSearchExpression( QStringLiteral( "D" ) );

  spy.wait();
  QCOMPARE( model.rowCount(), 2 );
  QCOMPARE( proxy.rowCount(), 2 );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::SortValue ), QLatin1String( "D1" ) );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::SortValue ), QLatin1String( "D2" ) );

  // add a filter expression to the base model
  config[ QStringLiteral( "FilterExpression" ) ] = "subFk = 1";
  model.setConfig( config );
  model.setupSorting();
  model.setSearchExpression( QString() );

  spy.wait();

  QCOMPARE( model.rowCount(), 2 );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::SortValue ), QLatin1String( "A1" ) );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::SortValue ), QLatin1String( "A2" ) );

  // remove filters and sorting
  model.reset();
  config.remove( QStringLiteral( "OrderByValue" ) );
  config.remove( QStringLiteral( "FilterExpression" ) );
  model.setConfig( config );
  model.setupSorting();
  model.setPair( pair );

  spy.wait();

  QCOMPARE( proxy.rowCount(), 9 );
  QCOMPARE( proxy.data( proxy.index( 0, 0 ), FeaturesModel::FeatureId ), 1 );
  QCOMPARE( proxy.data( proxy.index( 1, 0 ), FeaturesModel::FeatureId ), 2 );
  QCOMPARE( proxy.data( proxy.index( 2, 0 ), FeaturesModel::FeatureId ), 3 );
  QCOMPARE( proxy.data( proxy.index( 3, 0 ), FeaturesModel::FeatureId ), 4 );
  QCOMPARE( proxy.data( proxy.index( 4, 0 ), FeaturesModel::FeatureId ), 5 );
  QCOMPARE( proxy.data( proxy.index( 5, 0 ), FeaturesModel::FeatureId ), 6 );
  QCOMPARE( proxy.data( proxy.index( 6, 0 ), FeaturesModel::FeatureId ), 7 );
  QCOMPARE( proxy.data( proxy.index( 7, 0 ), FeaturesModel::FeatureId ), 8 );
  QCOMPARE( proxy.data( proxy.index( 8, 0 ), FeaturesModel::FeatureId ), 100000000 );
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
