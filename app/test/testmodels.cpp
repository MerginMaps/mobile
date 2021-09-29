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

  QString projectDir = TestUtils::testDataDir() + "/project_value_relations";
  QgsVectorLayer *layer = new QgsVectorLayer( projectDir + "/db.gpkg|layername=main", "base", "ogr" );

  QVERIFY( layer && layer->isValid() );

  fModel.setLayer( layer );
  fModel.reloadFeatures();

  QCOMPARE( fModel.rowCount(), layer->dataProvider()->featureCount() );

  fModel.setSearchExpression( QStringLiteral( "Seco" ) );

  QCOMPARE( fModel.rowCount(), 1 );

  fModel.setSearchExpression( QLatin1String() );

  QCOMPARE( fModel.rowCount(), layer->dataProvider()->featureCount() );

  QVariant title = fModel.data( fModel.index( 0 ), FeaturesModel::FeatureTitle );
  QCOMPARE( title, QStringLiteral( "First" ) );
}
