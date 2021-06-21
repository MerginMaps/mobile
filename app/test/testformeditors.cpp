/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testformeditors.h"
#include "testutils.h"
#include "attributecontroller.h"
#include "featurelayerpair.h"
#include "attributedata.h"

#include <QtTest/QtTest>
#include <memory>

#include "qgsvectorlayer.h"

void TestFormEditors::init()
{
}

void TestFormEditors::cleanup()
{
}

void TestFormEditors::testNumericFields()
{
  QString dataDir = TestUtils::testDataDir();
  QString planesVectorFile = dataDir + "/planes/points.shp";
  QString qmlStyle = dataDir + "/planes/PlanesNumericFields.qml";
  std::unique_ptr<QgsVectorLayer> layer(
    new QgsVectorLayer( planesVectorFile )
  );
  QVERIFY( layer && layer->isValid() );

  bool res;
  layer->loadNamedStyle( qmlStyle, res,  false );
  QVERIFY( res );

  AttributeController controller;
  QgsFeature f1( layer->dataProvider()->fields(), 1 );

  FeatureLayerPair pair( f1, layer.get() );
  controller.setFeatureLayerPair( pair );

  const TabItem *tabItem = controller.tabItem( 0 );
  const QVector<QUuid> formItems = tabItem->formItems();

  // find field uuids
  QUuid headingFieldId, importanceFieldId, pilotsFieldId, cabinCrewFieldId, staffFieldId;

  for ( auto uuid: formItems )
  {
    const FormItem* xitem = controller.formItem( uuid );
    if ( xitem->name() == "Heading" )
      headingFieldId = xitem->id();
    else if ( xitem->name() == "Importance" )
      importanceFieldId = xitem->id();
    else if ( xitem->name() == "Pilots" )
      pilotsFieldId = xitem->id();
    else if ( xitem->name() == "Cabin Crew" )
      cabinCrewFieldId = xitem->id();
    else if ( xitem->name() == "Staff" )
      staffFieldId = xitem->id();
  }

  for ( auto id: { headingFieldId, importanceFieldId, pilotsFieldId, cabinCrewFieldId, staffFieldId } )
  {
    const FormItem *item = controller.formItem( id );
    QVERIFY( item );
  }

  // let's test with en_US locale,
  // spec: https://lh.2xlibre.net/locale/en_US/
  QLocale enLocale = QLocale( "en_US" );
  QLocale::setDefault( enLocale );

  struct combination
  {
    QString value;
    QUuid fieldUuid;
    FormItem::ValueState expectedValueState;
    bool expectedSuccess;
  };

  QList<combination> combinations =
  {
    // field "Heading", Int, range <100; 1000>, range editable
    {"", headingFieldId, FormItem::ValidValue, false}, // because field can be null, but we do not yet handle null values
    {"1", headingFieldId, FormItem::ValueOutOfRange, true},
    {"-1", headingFieldId, FormItem::ValueOutOfRange, true},
    {"10", headingFieldId, FormItem::ValueOutOfRange, true},
    {"-10", headingFieldId, FormItem::ValueOutOfRange, true},
    {"-100", headingFieldId, FormItem::ValueOutOfRange, true},
    {"100", headingFieldId, FormItem::ValidValue, true},
    {"100h", headingFieldId, FormItem::InvalidValue, false},
    {"100", headingFieldId, FormItem::ValidValue, true},
    {"1000", headingFieldId, FormItem::ValidValue, true},
//    {"1000,5", headingFieldId, FormItem::InvalidValue, false}, // currently decimals are accepted and are being rounded up
    {"1001", headingFieldId, FormItem::ValueOutOfRange, true},

    // field "Importance", Real, range <-100.00; 100.00>, step:0.01, precision: 2, range editable
    {"", importanceFieldId, FormItem::ValidValue, false},
    {"0", importanceFieldId, FormItem::ValidValue, true},
    {"-1.00", importanceFieldId, FormItem::ValidValue, true},
    {"100.00", importanceFieldId, FormItem::ValidValue, true},
    {"100.002", importanceFieldId, FormItem::ValueOutOfRange, true},
    {"100.002fdsa", importanceFieldId, FormItem::InvalidValue, false},
    {"100.,.,.,", importanceFieldId, FormItem::InvalidValue, false},
    {"1 000", importanceFieldId, FormItem::InvalidValue, false},
    {"15,2", importanceFieldId, FormItem::ValueOutOfRange, true},

    // field "Pilots", Int, range <-1000; -100>, range editable
    {"", pilotsFieldId, FormItem::ValidValue, false}, // because field can be null
    {"-100", pilotsFieldId, FormItem::ValidValue, true},
    {"-1000", pilotsFieldId, FormItem::ValidValue, true},
    {"0", pilotsFieldId, FormItem::ValueOutOfRange, true},
    {"150", pilotsFieldId, FormItem::ValueOutOfRange, true},
    {"-1001", pilotsFieldId, FormItem::ValueOutOfRange, true},
    {"-51216354321435", pilotsFieldId, FormItem::InvalidValue, false},
    {"--100", pilotsFieldId, FormItem::InvalidValue, false},
    {"--100fsda", pilotsFieldId, FormItem::InvalidValue, false},
    {"-100", pilotsFieldId, FormItem::ValidValue, true},

    // field "Cabin Crew", Int, no limit, range editable
    {"", cabinCrewFieldId, FormItem::ValidValue, false}, // because field can be null
    {"-100", cabinCrewFieldId, FormItem::ValidValue, true},
    {"-1000", cabinCrewFieldId, FormItem::ValidValue, true},
    {"-2147483647", cabinCrewFieldId, FormItem::ValidValue, true}, // int limit from QGIS
    {"2147483647", cabinCrewFieldId, FormItem::ValidValue, true}, // int limit from QGIS
    {"214748364799", cabinCrewFieldId, FormItem::InvalidValue, false},
    {"-214748364799", cabinCrewFieldId, FormItem::InvalidValue, false},
    {"-214748-", cabinCrewFieldId, FormItem::InvalidValue, false},

    // field "Staff", Int, no limit, range slider
    {"", staffFieldId, FormItem::ValidValue, false}, // because field can be null
    {"10", staffFieldId, FormItem::ValidValue, true},
    {"-10", staffFieldId, FormItem::ValidValue, true},
    // QML Slider does not allow to enter values higher or lower than specified range
  };

  // compare results
  for ( const auto &c: combinations )
  {
    bool res = controller.setFormValue( c.fieldUuid, c.value );
    QCOMPARE( res, c.expectedSuccess );

    const FormItem *item = controller.formItem( c.fieldUuid );
    QCOMPARE( item->valueState(), c.expectedValueState );
    // In future when we will store invalid value, we can also check if c.value is the same as value in featureLayerPair
  }

  // field "Cabin Crew" stayed with invalid input, check controller flag of values validity
  QCOMPARE( controller.fieldValuesValid(), false );
}
