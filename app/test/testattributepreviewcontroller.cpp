/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testattributepreviewcontroller.h"

#include "qgsvectorlayer.h"
#include "qgsfeature.h"
#include "featurelayerpair.h"
#include "qgsproject.h"

#include "attributepreviewcontroller.h"

TestAttributePreviewController::TestAttributePreviewController()
{

}

TestAttributePreviewController::~TestAttributePreviewController() = default;

void TestAttributePreviewController::initTestCase()
{

}

void TestAttributePreviewController::cleanupTestCase()
{

}

void TestAttributePreviewController::testPreviewForms()
{
  // Prepare Layers!
  QgsVectorLayer *layer =
    new QgsVectorLayer( QStringLiteral( "Point?field=fldtxt:string&field=fldint:integer" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      );
  QVERIFY( layer && layer->isValid() );
  QgsFeature f1( layer->dataProvider()->fields(), 1 );
  f1.setAttribute( QStringLiteral( "fldtxt" ), "one" );
  f1.setAttribute( QStringLiteral( "fldint" ), 1 );
  QgsFeature f2( layer->dataProvider()->fields(), 2 );
  f2.setAttribute( QStringLiteral( "fldtxt" ), "two" );
  f2.setAttribute( QStringLiteral( "fldint" ), 2 );
  layer->dataProvider()->addFeatures( QgsFeatureList() << f1 << f2 );
  QgsProject::instance()->addMapLayer( layer );


  QgsVectorLayer *layerFields =
    new QgsVectorLayer( QStringLiteral( "Point?field=fldtxt:string&field=fldtxt2:string&field=fldtxt3:string" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      );
  QVERIFY( layerFields && layerFields->isValid() );
  layerFields->setMapTipTemplate( "# fields\nfldtxt2\nfldtxt3" );
  QgsFeature fields1( layerFields->dataProvider()->fields(), 3 );
  fields1.setAttribute( QStringLiteral( "fldtxt" ), "f_one" );
  fields1.setAttribute( QStringLiteral( "fldtxt2" ), "f_two" );
  fields1.setAttribute( QStringLiteral( "fldtxt3" ), "f_three" );
  layerFields->dataProvider()->addFeatures( QgsFeatureList() << fields1 );
  QgsProject::instance()->addMapLayer( layerFields );

  QgsVectorLayer *layerHtml =
    new QgsVectorLayer( QStringLiteral( "Point?field=fldtxt:string" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      );
  QVERIFY( layerHtml && layerHtml->isValid() );
  layerHtml->setMapTipTemplate( "<div>HELLO</div>" );
  QgsFeature h1( layerHtml->dataProvider()->fields(), 4 );
  h1.setAttribute( QStringLiteral( "fldtxt" ), "h_one" );
  layerHtml->dataProvider()->addFeatures( QgsFeatureList() << h1 );
  QgsProject::instance()->addMapLayer( layerHtml );


  QgsVectorLayer *layerPhoto =
    new QgsVectorLayer( QStringLiteral( "Point?field=fldtxt:string" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      );
  QVERIFY( layerPhoto && layerPhoto->isValid() );
  layerPhoto->setMapTipTemplate( "# image\nphoto_relative_path" );
  QgsFeature p1( layerPhoto->dataProvider()->fields(), 5 );
  p1.setAttribute( QStringLiteral( "fldtxt" ), "myphoto" );
  layerPhoto->dataProvider()->addFeatures( QgsFeatureList() << p1 );
  QgsProject::instance()->addMapLayer( layerPhoto );

  AttributePreviewController controller;
  controller.setProject( QgsProject::instance() );
  QCOMPARE( controller.type(), AttributePreviewController::Empty );

  // Set Layer without any definition
  FeatureLayerPair pair( f1, layer );
  controller.setFeatureLayerPair( pair );
  QCOMPARE( controller.type(), AttributePreviewController::Fields );
  QCOMPARE( controller.title(), "one" );

  Q_ASSERT( controller.fieldModel() );
  QCOMPARE( controller.fieldModel()->rowCount(), 2 );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 0, 0 ), AttributePreviewModel::Name ), "fldtxt" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 0, 0 ), AttributePreviewModel::Value ), "one" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 1, 0 ), AttributePreviewModel::Name ), "fldint" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 1, 0 ), AttributePreviewModel::Value ), 1 );

  // Set other feature from the same layer
  FeatureLayerPair pair2( f2, layer );
  controller.setFeatureLayerPair( pair2 );
  QCOMPARE( controller.title(), "two" );
  QCOMPARE( controller.type(), AttributePreviewController::Fields );
  Q_ASSERT( controller.fieldModel() );
  QCOMPARE( controller.fieldModel()->rowCount(), 2 );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 0, 0 ), AttributePreviewModel::Name ), "fldtxt" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 0, 0 ), AttributePreviewModel::Value ), "two" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 1, 0 ), AttributePreviewModel::Name ), "fldint" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 1, 0 ), AttributePreviewModel::Value ), 2 );

  // Set layer with the fields definition
  FeatureLayerPair pair3( fields1, layerFields );
  controller.setFeatureLayerPair( pair3 );
  QCOMPARE( controller.title(), "f_one" );
  QCOMPARE( controller.type(), AttributePreviewController::Fields );
  Q_ASSERT( controller.fieldModel() );
  QCOMPARE( controller.fieldModel()->rowCount(), 2 );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 0, 0 ), AttributePreviewModel::Name ), "fldtxt2" );
  QString val = controller.fieldModel()->data( controller.fieldModel()->index( 0, 0 ), AttributePreviewModel::Value ).toString();
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 0, 0 ), AttributePreviewModel::Value ), "f_two" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 1, 0 ), AttributePreviewModel::Name ), "fldtxt3" );
  QCOMPARE( controller.fieldModel()->data( controller.fieldModel()->index( 1, 0 ), AttributePreviewModel::Value ), "f_three" );

  // Set layer with the HTML definition
  FeatureLayerPair pair4( h1, layerHtml );
  controller.setFeatureLayerPair( pair4 );
  QCOMPARE( controller.title(), "h_one" );
  QCOMPARE( controller.type(), AttributePreviewController::HTML );
  QCOMPARE( controller.html(), "<div>HELLO</div>" );

  // Set layer with the photo definition
  FeatureLayerPair pair5( p1, layerPhoto );
  controller.setFeatureLayerPair( pair5 );
  QCOMPARE( controller.type(), AttributePreviewController::Photo );
  QCOMPARE( controller.photo(), "photo_relative_path" );


  // Reset
  controller.reset();
  QCOMPARE( controller.type(), AttributePreviewController::Empty );

  // Cleanup
  QgsProject::instance()->removeAllMapLayers();
}
