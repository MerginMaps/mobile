/***************************************************************************
     testattributecontroller.cpp
     --------------------------------------
  Date                 : May 2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QObject>
#include <QApplication>
#include <QDesktopWidget>
#include <memory>

#include "qgsapplication.h"
#include "qgstest.h"
#include "qgis.h"
#include "qgsvectorlayer.h"
#include "qgsproject.h"

#include "attributecontroller.h"
#include "attributetabproxymodel.h"
#include "attributetabmodel.h"
#include "attributeformproxymodel.h"
#include "attributeformmodel.h"

class TestAttributeController: public QObject
{
    Q_OBJECT
  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void noFields();
    void twoFieldsAutoLayout();
    void twoTabsDragAndDropLayout();
};

void TestAttributeController::init()
{
}

void TestAttributeController::cleanup()
{
}


void TestAttributeController::noFields()
{
  std::unique_ptr<QgsVectorLayer> layer(
    new QgsVectorLayer( QStringLiteral( "Point" ),
                        QStringLiteral( "layer" ),
                        QStringLiteral( "memory" )
                      )
  );

  QVERIFY( layer && layer->isValid() );
  QgsFeature f1( layer->dataProvider()->fields(), 1 );
  layer->dataProvider()->addFeatures( QgsFeatureList() << f1 );


  AttributeController controller;
  FeatureLayerPair pair( f1, layer.get() );
  controller.setFeatureLayerPair( pair );

  // we do not have tabs
  QVERIFY( !controller.hasTabs() );
  const TabItem *item2 = controller.tabItem( 1 );
  QVERIFY( !item2 );

  // we always have one tabs
  const TabItem *tabItem = controller.tabItem( 0 );
  Q_ASSERT( tabItem );
  QCOMPARE( tabItem->tabIndex(), 0 );
  QCOMPARE( tabItem->name(), "AutoLayoutRoot" );
  QCOMPARE( tabItem->isVisible(), false );

  // but there are no fields!
  const QVector<QUuid> formItems = tabItem->formItems();
  QCOMPARE( formItems.size(), 0 );
}

void TestAttributeController::twoFieldsAutoLayout()
{
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


  AttributeController controller;
  FeatureLayerPair pair( f1, layer.get() );
  controller.setFeatureLayerPair( pair );

  // we do not have tabs
  QVERIFY( !controller.hasTabs() );

  const TabItem *tabItem = controller.tabItem( 0 );
  Q_ASSERT( tabItem );
  QCOMPARE( tabItem->tabIndex(), 0 );
  QCOMPARE( tabItem->name(), "AutoLayoutRoot" );
  QCOMPARE( tabItem->isVisible(), true );
  Q_ASSERT( controller.attributeTabProxyModel() );

  const QVector<QUuid> formItems = tabItem->formItems();
  QCOMPARE( formItems.size(), 2 );
  Q_ASSERT( controller.attributeFormProxyModelForTab( 0 ) );

  const FormItem *item1 = controller.formItem( formItems.at( 0 ) );
  QCOMPARE( item1->name(), "fldtxt" );
  QCOMPARE( controller.formValue( item1->fieldIndex() ), "one" );
  QCOMPARE( item1->isVisible(), true );

  const FormItem *item2 = controller.formItem( formItems.at( 1 ) );
  QCOMPARE( item2->name(), "fldint" );
  QCOMPARE( controller.formValue( item2->fieldIndex() ), 1 );
  QCOMPARE( item2->isVisible(), true );
}

void TestAttributeController::twoTabsDragAndDropLayout()
{
  QString dataDir( TEST_DATA_DIR );  // defined in CMakeLists.txt
  QString planesVectorFile = dataDir + "/points.shp";
  QString qmlStyle = dataDir + "/SimpleTabsForPlanesLayer.qml";
  std::unique_ptr<QgsVectorLayer> layer(
    new QgsVectorLayer( planesVectorFile )
  );
  QVERIFY( layer && layer->isValid() );
  QCOMPARE( layer->featureCount(), 17 );

  bool res;
  layer->loadNamedStyle( qmlStyle, res,  false );
  QVERIFY( res );

  QgsFeature f1 = layer->getFeature( 0 );
  QVERIFY( f1.isValid() );

  AttributeController controller;
  FeatureLayerPair pair( f1, layer.get() );
  controller.setFeatureLayerPair( pair );

  // we do have tabs
  QVERIFY( controller.hasTabs() );

  // first tab
  {
    const TabItem *tabItem = controller.tabItem( 0 );
    Q_ASSERT( tabItem );
    QCOMPARE( tabItem->tabIndex(), 0 );
    QCOMPARE( tabItem->name(), "General" );
    QCOMPARE( tabItem->isVisible(), true );
    Q_ASSERT( controller.attributeTabProxyModel() );

    const QVector<QUuid> formItems = tabItem->formItems();
    QCOMPARE( formItems.size(), 3 );
    Q_ASSERT( controller.attributeFormProxyModelForTab( 0 ) );

    const FormItem *item1 = controller.formItem( formItems.at( 0 ) );
    QCOMPARE( item1->name(), "Class" );
    QCOMPARE( controller.formValue( item1->fieldIndex() ), "Jet" );
    QCOMPARE( item1->isVisible(), true );

    const FormItem *item2 = controller.formItem( formItems.at( 1 ) );
    QCOMPARE( item2->name(), "Heading" );
    QCOMPARE( controller.formValue( item2->fieldIndex() ), 90 );
    QCOMPARE( item2->isVisible(), true );

    const FormItem *item3 = controller.formItem( formItems.at( 2 ) );
    QCOMPARE( item3->name(), "Importance" );
    QCOMPARE( controller.formValue( item3->fieldIndex() ), 3 );
    QCOMPARE( item3->isVisible(), true );
  }

  // second  tab
  {
    const TabItem *tabItem = controller.tabItem( 1 );
    Q_ASSERT( tabItem );
    QCOMPARE( tabItem->tabIndex(), 1 );
    QCOMPARE( tabItem->name(), "People" );
    QCOMPARE( tabItem->isVisible(), true );
    Q_ASSERT( controller.attributeTabProxyModel() );

    const QVector<QUuid> formItems = tabItem->formItems();
    QCOMPARE( formItems.size(), 3 );
    Q_ASSERT( controller.attributeFormProxyModelForTab( 1 ) );

    const FormItem *item1 = controller.formItem( formItems.at( 0 ) );
    QCOMPARE( item1->name(), "Pilots" );
    QCOMPARE( controller.formValue( item1->fieldIndex() ), 2 );
    QCOMPARE( item1->isVisible(), true );

    const FormItem *item2 = controller.formItem( formItems.at( 1 ) );
    QCOMPARE( item2->name(), "Cabin Crew" );
    QCOMPARE( controller.formValue( item2->fieldIndex() ), 0 );
    QCOMPARE( item2->isVisible(), true );

    const FormItem *item3 = controller.formItem( formItems.at( 2 ) );
    QCOMPARE( item3->name(), "Staff" );
    QCOMPARE( controller.formValue( item3->fieldIndex() ), 2 );
    QCOMPARE( item3->isVisible(), true );
  }

  // now change to different feature
  QgsFeature f2 = layer->getFeature( 10 );
  QVERIFY( f2.isValid() );
  FeatureLayerPair pair2( f2, layer.get() );
  controller.setFeatureLayerPair( pair2 );

  // check if the data changed
  {
    const TabItem *tabItem = controller.tabItem( 1 );
    Q_ASSERT( tabItem );
    QCOMPARE( tabItem->tabIndex(), 1 );
    QCOMPARE( tabItem->name(), "People" );
    QCOMPARE( tabItem->isVisible(), true );
    Q_ASSERT( controller.attributeTabProxyModel() );

    const QVector<QUuid> formItems = tabItem->formItems();
    QCOMPARE( formItems.size(), 3 );
    Q_ASSERT( controller.attributeFormProxyModelForTab( 1 ) );

    const FormItem *item1 = controller.formItem( formItems.at( 0 ) );
    QCOMPARE( item1->name(), "Pilots" );
    QCOMPARE( controller.formValue( item1->fieldIndex() ), 1 );
    QCOMPARE( item1->isVisible(), true );

    const FormItem *item2 = controller.formItem( formItems.at( 1 ) );
    QCOMPARE( item2->name(), "Cabin Crew" );
    QCOMPARE( controller.formValue( item2->fieldIndex() ), 1 );
    QCOMPARE( item2->isVisible(), true );

    const FormItem *item3 = controller.formItem( formItems.at( 2 ) );
    QCOMPARE( item3->name(), "Staff" );
    QCOMPARE( controller.formValue( item3->fieldIndex() ), 2 );
    QCOMPARE( item3->isVisible(), true );
  }
}


QGSTEST_MAIN( TestAttributeController )
#include "testattributecontroller.moc"
