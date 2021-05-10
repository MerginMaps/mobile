/***************************************************************************
     testqgsquickattributecontroller.cpp
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

#include "qgsquickattributecontroller.h"
#include "qgsquickattributetabproxymodel.h"
#include "qgsquickattributetabmodel.h"
#include "qgsquickattributeformproxymodel.h"
#include "qgsquickattributeformmodel.h"

class TestQgsQuickAttributeController: public QObject
{
    Q_OBJECT
  private slots:
    void init(); // will be called before each testfunction is executed.
    void cleanup(); // will be called after every testfunction.

    void noFields();
    void twoFields();
};

void TestQgsQuickAttributeController::init()
{
}

void TestQgsQuickAttributeController::cleanup()
{
}


void TestQgsQuickAttributeController::noFields()
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


  QgsQuickAttributeController controller;
  QgsQuickFeatureLayerPair pair( f1, layer.get() );
  controller.setFeatureLayerPair( pair );

  // we do not have more then 2 tabs
  QVERIFY( !controller.hasTabs() );
  const QgsQuickTabItem *item2 = controller.tabItem( 1 );
  QVERIFY( !item2 );

  // we always have one tabs
  const QgsQuickTabItem *tabItem = controller.tabItem( 0 );
  Q_ASSERT( tabItem );
  QCOMPARE( tabItem->tabIndex(), 0 );
  QCOMPARE( tabItem->name(), "AutoLayoutRoot" );
  QCOMPARE( tabItem->isVisible(), true );

  // but there are no fields!
  const QVector<QUuid> formItems = tabItem->formItems();
  QCOMPARE( formItems.size(), 0 );
}

void TestQgsQuickAttributeController::twoFields()
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


  QgsQuickAttributeController controller;
  QgsQuickFeatureLayerPair pair( f1, layer.get() );
  controller.setFeatureLayerPair( pair );

  // we do not have more then 2 tabs
  QVERIFY( !controller.hasTabs() );

  const QgsQuickTabItem *tabItem = controller.tabItem( 0 );
  Q_ASSERT( tabItem );
  QCOMPARE( tabItem->tabIndex(), 0 );
  QCOMPARE( tabItem->name(), "AutoLayoutRoot" );
  QCOMPARE( tabItem->isVisible(), true );
  Q_ASSERT( controller.attributeTabProxyModel() );

  const QVector<QUuid> formItems = tabItem->formItems();
  QCOMPARE( formItems.size(), 2 );
  Q_ASSERT( controller.attributeFormProxyModelForTab( 0 ) );
  Q_ASSERT( !controller.attributeFormProxyModelForTab( 1 ) );

  const QgsQuickFormItem *item1 = controller.formItem( formItems.at( 0 ) );
  QCOMPARE( item1->name(), "fldtxt" );
  QCOMPARE( controller.formValue( item1->fieldIndex() ), "one" );
  QCOMPARE( item1->isVisible(), true );

  const QgsQuickFormItem *item2 = controller.formItem( formItems.at( 1 ) );
  QCOMPARE( item2->name(), "fldint" );
  QCOMPARE( controller.formValue( item2->fieldIndex() ), 1 );
  QCOMPARE( item2->isVisible(), true );
}

QGSTEST_MAIN( TestQgsQuickAttributeController )
#include "testqgsquickattributecontroller.moc"
