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

void TestAttributePreviewController::testMultilineMapTips()
{
  // Layer creation
  QgsVectorLayer *layerPhoto =
    new QgsVectorLayer( QStringLiteral( "Point?field=fldtxt:string" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      );
  QVERIFY( layerPhoto && layerPhoto->isValid() );
  layerPhoto->setMapTipTemplate( "# image\nfile:///my/path/to/image/[%\n  CASE WHEN fldtxt = 'myphoto' THEN\n    'hello.jpg'\n  ELSE\n    'world.jpg'\n  END\n%]" );

  // Feature 1 setup
  QgsFeature p1( layerPhoto->dataProvider()->fields() );
  p1.setAttribute( QStringLiteral( "fldtxt" ), "myphoto" );
  layerPhoto->dataProvider()->addFeatures( QgsFeatureList() << p1 );

  // Feature 2 setup
  QgsFeature p2( layerPhoto->dataProvider()->fields() );
  p2.setAttribute( QStringLiteral( "fldtxt" ), "notmyphoto" );
  layerPhoto->dataProvider()->addFeatures( QgsFeatureList() << p2 );
  QgsProject::instance()->addMapLayer( layerPhoto );

  // Controller setup
  AttributePreviewController controller;
  controller.setProject( QgsProject::instance() );
  QCOMPARE( controller.type(), AttributePreviewController::Empty );

  // Assertion for matching feature
  FeatureLayerPair pair1( p1, layerPhoto );
  controller.setFeatureLayerPair( pair1 );
  QCOMPARE( controller.type(), AttributePreviewController::Photo );
  QCOMPARE( controller.photo(), "file:///my/path/to/image/hello.jpg" );

  // Assertion for non-matching feature
  FeatureLayerPair pair2( p2, layerPhoto );
  controller.setFeatureLayerPair( pair2 );
  QCOMPARE( controller.type(), AttributePreviewController::Photo );
  QCOMPARE( controller.photo(), "file:///my/path/to/image/world.jpg" );

  // Reset
  controller.reset();
  QCOMPARE( controller.type(), AttributePreviewController::Empty );

  // Cleanup
  QgsProject::instance()->removeAllMapLayers();
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

void TestAttributePreviewController::testFeatureTitle()
{
  // Layer creation
  QgsVectorLayer *layer =
    new QgsVectorLayer( QStringLiteral( "Point?field=name:string" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      );
  QVERIFY( layer && layer->isValid() );

  layer->setName( QStringLiteral( "Point" ) );

  QgsFeature f1( layer->dataProvider()->fields() );
  f1.setAttribute( QStringLiteral( "name" ), "My feature" );
  layer->dataProvider()->addFeatures( QgsFeatureList() << f1 );

  QgsFeature f2( layer->dataProvider()->fields() );
  f2.setAttribute( QStringLiteral( "name" ), "" );
  layer->dataProvider()->addFeatures( QgsFeatureList() << f2 );

  QgsProject::instance()->addMapLayer( layer );

  AttributePreviewController controller;
  controller.setProject( QgsProject::instance() );

  //
  // Valid scenarios
  //

  FeatureLayerPair pair1( layer->getFeature( 1 ), layer );
  controller.setFeatureLayerPair( pair1 );
  QCOMPARE( controller.title(), QStringLiteral( "My feature" ) );

  FeatureLayerPair pair2( layer->getFeature( 2 ), layer );
  controller.setFeatureLayerPair( pair2 );
  QCOMPARE( controller.title(), QStringLiteral( "Point (2)" ) );

  //
  // FeaturePair is somehow malformed
  //

  FeatureLayerPair pair3( QgsFeature(), nullptr );
  controller.setFeatureLayerPair( pair3 );
  QCOMPARE( controller.title(), QStringLiteral( "" ) );

  FeatureLayerPair pair4( layer->getFeature( 2 ), nullptr );
  controller.setFeatureLayerPair( pair4 );
  QCOMPARE( controller.title(), QStringLiteral( "" ) );

  FeatureLayerPair pair5( QgsFeature(), layer );
  controller.setFeatureLayerPair( pair5 );
  QCOMPARE( controller.title(), QStringLiteral( "" ) );

  //
  // Set layer name as empty and check if default "Unnamed Layer" is applied
  //
  layer->setName( QStringLiteral( "" ) );
  controller.setFeatureLayerPair( pair2 );
  QCOMPARE( controller.title(), QStringLiteral( "Unnamed Layer (2)" ) );
}


void TestAttributePreviewController::testFormatDateForPreview()
{
    //Layer with a single datetime field
    auto *layer = new QgsVectorLayer(
        QStringLiteral("Point?field=dt:datetime"),
        QStringLiteral("layer_dt"), QStringLiteral("memory"));
    QVERIFY(layer && layer->isValid());

    //Apply display_format so we can assert a deterministic string
    const int idx = layer->fields().indexFromName(QStringLiteral("dt"));
    QVariantMap cfg; cfg["display_format"] = QStringLiteral("yyyy-MM-dd HH:mm:ss");
    layer->setEditorWidgetSetup(idx, QgsEditorWidgetSetup(QStringLiteral("DateTime"), cfg));

    //Valid ISO UTC string → should convert to LOCAL time and format
    const QString rawUtc = QStringLiteral("2021-06-07T08:09:10Z");
    QDateTime dt = QDateTime::fromString(rawUtc, Qt::ISODate);
    QVERIFY(dt.isValid());
    const QString expected = dt.toLocalTime().toString(cfg["display_format"].toString());

    QgsFeature f(layer->dataProvider()->fields(), 1);
    f.setAttribute(QStringLiteral("dt"), rawUtc);
    QVERIFY(layer->dataProvider()->addFeatures(QgsFeatureList() << f));

    layer->setMapTipTemplate(QStringLiteral("# fields\ndt")); // show only that field
    QgsProject::instance()->addMapLayer(layer);

    AttributePreviewController c; c.setProject(QgsProject::instance());
    c.setFeatureLayerPair(FeatureLayerPair(f, layer));
    QCOMPARE(c.type(), AttributePreviewController::Fields);
    QCOMPARE(c.fieldModel()->rowCount(), 1);
    QCOMPARE(c.fieldModel()->data(c.fieldModel()->index(0,0), AttributePreviewModel::Value), expected);

    //Invalid string -> should fall back to raw value
    QgsFeature fBad(layer->dataProvider()->fields(), 2);
    const QString bad = QStringLiteral("2021-13-40T77:88:99");
    fBad.setAttribute(QStringLiteral("dt"), bad);

    c.setFeatureLayerPair(FeatureLayerPair(fBad, layer));
    QCOMPARE(c.type(), AttributePreviewController::Fields);
    QCOMPARE(c.fieldModel()->rowCount(), 1);
    QCOMPARE(c.fieldModel()->data(c.fieldModel()->index(0,0), AttributePreviewModel::Value), bad);

    QgsProject::instance()->removeAllMapLayers();
}

