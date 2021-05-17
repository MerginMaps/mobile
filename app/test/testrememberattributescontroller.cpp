/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "testrememberattributescontroller.h"

#include <QObject>
#include <QApplication>
#include <QDesktopWidget>
#include <memory>

#include "qgsapplication.h"
#include "qgsvectorlayer.h"

#include "featurelayerpair.h"
#include "rememberattributescontroller.h"

void TestRememberAttributesController::init()
{
}

void TestRememberAttributesController::cleanup()
{
}


void TestRememberAttributesController::noFeatureTest()
{
  RememberAttributesController controller;


  QCOMPARE( controller.rememberValuesAllowed(), false );
  controller.setRememberValuesAllowed( true );
  QCOMPARE( controller.rememberValuesAllowed(), true );

  QCOMPARE( controller.shouldRememberValue( nullptr, 1 ), false );
  QVariant val;
  QCOMPARE( controller.rememberedValue( nullptr, 1, val ), false );

}

void TestRememberAttributesController::storedFeatureTest()
{
  RememberAttributesController controller;
  controller.setRememberValuesAllowed( true );

  std::unique_ptr<QgsVectorLayer> layer(
    new QgsVectorLayer( QStringLiteral( "Point?field=fldtxt:string&field=fldint:integer" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      )
  );

  QVERIFY( layer && layer->isValid() );
  QgsFeature f1( layer->dataProvider()->fields(), 1 );
  f1.setAttribute( QStringLiteral( "fldtxt" ), "one" );
  f1.setAttribute( QStringLiteral( "fldint" ), 1 );
  layer->dataProvider()->addFeatures( QgsFeatureList() << f1 );

  std::unique_ptr<QgsVectorLayer> layer2(
    new QgsVectorLayer( QStringLiteral( "Point?field=fldtxt:string&field=fldint:integer" ),
                        QStringLiteral( "layer2" ),
                        QStringLiteral( "memory" )
                      )
  );

  QVERIFY( layer2 && layer2->isValid() );
  QgsFeature f2( layer2->dataProvider()->fields(), 1 );
  f2.setAttribute( QStringLiteral( "fldtxt" ), "two" );
  f2.setAttribute( QStringLiteral( "fldint" ), 2 );

  QgsFeature f3( layer2->dataProvider()->fields(), 1 );
  f3.setAttribute( QStringLiteral( "fldtxt" ), "three" );
  f3.setAttribute( QStringLiteral( "fldint" ), 3 );

  layer2->dataProvider()->addFeatures( QgsFeatureList() << f2 << f3 );

  FeatureLayerPair pair( f1, layer.get() );
  FeatureLayerPair pair2( f2, layer2.get() );
  FeatureLayerPair pair22( f3, layer2.get() );

  // one feature is stored, but user does not want to use any stored values
  controller.storeFeature( pair );
  QVariant val;
  QCOMPARE( controller.shouldRememberValue( layer.get(), 0 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 0, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 1, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer2.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer2.get(), 1, val ), false );

  // one feature is stored, and user does not want to use first field for new feature
  controller.setShouldRememberValue( layer.get(), 0, true );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 0 ), true );
  QCOMPARE( controller.rememberedValue( layer.get(), 0, val ), true );
  QCOMPARE( val, "one" );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 1, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer2.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer2.get(), 1, val ), false );

  // user switched to different layer, but not want to use any field for a new feature
  controller.storeFeature( pair2 );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 0 ), true );
  QCOMPARE( controller.rememberedValue( layer.get(), 0, val ), true );
  QCOMPARE( val, "one" );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 1, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer2.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer2.get(), 1, val ), false );

  // user now wants to use field 1 from the second layer
  controller.setShouldRememberValue( layer2.get(), 1, true );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 0 ), true );
  QCOMPARE( controller.rememberedValue( layer.get(), 0, val ), true );
  QCOMPARE( val, "one" );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 1, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer2.get(), 1 ), true );
  QCOMPARE( controller.rememberedValue( layer2.get(), 1, val ), true );
  QCOMPARE( val, 2 );

  // user digitizing another feature, so the pair2 from layer2 is overriden
  // and the selection of values to use persists
  controller.storeFeature( pair22 );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 0 ), true );
  QCOMPARE( controller.rememberedValue( layer.get(), 0, val ), true );
  QCOMPARE( val, "one" );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 1, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer2.get(), 1 ), true );
  QCOMPARE( controller.rememberedValue( layer2.get(), 1, val ), true );
  QCOMPARE( val, 3 );

  // ok now user switched to completely different project
  controller.reset();
  QCOMPARE( controller.shouldRememberValue( layer.get(), 0 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 0, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer.get(), 1, val ), false );
  QCOMPARE( controller.shouldRememberValue( layer2.get(), 1 ), false );
  QCOMPARE( controller.rememberedValue( layer2.get(), 1, val ), false );
}
