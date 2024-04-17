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
