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
#include "fieldvalidator.h"
#include "appresources.h"

#include <QtTest/QtTest>
#include <memory>

#include "qgsvectorlayer.h"
#include "qgsrelationmanager.h"

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

  for ( auto uuid : formItems )
  {
    const FormItem *xitem = controller.formItem( uuid );
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

  for ( auto id : { headingFieldId, importanceFieldId, pilotsFieldId, cabinCrewFieldId, staffFieldId } )
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
    QString expectedValidationMessage;
    FieldValidator::ValidationMessageLevel expectedValidationMessageLevel;
  };

  namespace V = Resources::Texts::Validation;

  QList<combination> combinations =
  {
    // field "Heading", Int, range <100; 1000>, range editable
    { "", headingFieldId, "", FieldValidator::Info },
    { "1", headingFieldId, V::numberLowerBoundReached.arg( 100 ), FieldValidator::Error },
    { "-1", headingFieldId, V::numberLowerBoundReached.arg( 100 ), FieldValidator::Error },
    { "10", headingFieldId, V::numberLowerBoundReached.arg( 100 ), FieldValidator::Error },
    { "-10", headingFieldId, V::numberLowerBoundReached.arg( 100 ), FieldValidator::Error },
    { "-100", headingFieldId, V::numberLowerBoundReached.arg( 100 ), FieldValidator::Error },
    { "100", headingFieldId, "", FieldValidator::Info },
    { "100h", headingFieldId, V::numberValidationFailed, FieldValidator::Error },
    { "100", headingFieldId, "", FieldValidator::Info },
    { "1000", headingFieldId, "", FieldValidator::Info },
    { "1001", headingFieldId, V::numberUpperBoundReached.arg( 1000 ), FieldValidator::Error },

    // field "Importance", Real, range <-100.00; 100.00>, step:0.01, precision: 2, range editable
    { "", importanceFieldId, "", FieldValidator::Info },
    { "0", importanceFieldId, "", FieldValidator::Info },
    { "-1.00", importanceFieldId, "", FieldValidator::Info },
    { "100.00", importanceFieldId, "", FieldValidator::Info },
    { "100.002", importanceFieldId, V::numberUpperBoundReached.arg( 100.00 ), FieldValidator::Error },
    { "100.002fdsa", importanceFieldId, V::numberValidationFailed, FieldValidator::Error },
    { "100.,.,.,", importanceFieldId, V::numberValidationFailed, FieldValidator::Error },
    { "1 000", importanceFieldId, V::numberValidationFailed, FieldValidator::Error },
    // { "15,2", importanceFieldId, FieldValidator::ValueOutOfRange, true}, // <-- TODO: Check this case

    // field "Pilots", Int, range <-1000; -100>, range editable
    { "", pilotsFieldId, "", FieldValidator::Info },
    { "-100", pilotsFieldId, "", FieldValidator::Info },
    { "-1000", pilotsFieldId, "", FieldValidator::Info },
    { "0", pilotsFieldId, V::numberUpperBoundReached.arg( -100 ), FieldValidator::Error },
    { "150", pilotsFieldId, V::numberUpperBoundReached.arg( -100 ), FieldValidator::Error },
    { "-1001", pilotsFieldId, V::numberLowerBoundReached.arg( -1000 ), FieldValidator::Error },
    // { "-51216354321435", pilotsFieldId, FieldValidator::InvalidValue, false}, // <-- TODO: Check this case, we should rather show that the number is too big / small
    { "--100", pilotsFieldId, V::numberValidationFailed, FieldValidator::Error },
    { "--100fsda", pilotsFieldId, V::numberValidationFailed, FieldValidator::Error },
    { "-100", pilotsFieldId, "", FieldValidator::Info },

    // field "Cabin Crew", Int, no limit, range editable
    { "", cabinCrewFieldId, "", FieldValidator::Info },
    { "-100", cabinCrewFieldId, "", FieldValidator::Info },
    { "-1000", cabinCrewFieldId, "", FieldValidator::Info },
    { "-2147483647", cabinCrewFieldId, "", FieldValidator::Info }, // int limit from QGIS
    { "2147483647", cabinCrewFieldId, "", FieldValidator::Info }, // int limit from QGIS
    // { "214748364799", cabinCrewFieldId, FieldValidator::InvalidValue, false}, // <-- TODO: Check this case, we should rather show that the number is too big / small
    // { "-214748364799", cabinCrewFieldId, FieldValidator::InvalidValue, false}, // <-- TODO: Check this case, we should rather show that the number is too big / small
    { "-214748-", cabinCrewFieldId, V::numberValidationFailed, FieldValidator::Error },

    // field "Staff", Int, no limit, range slider
    { "", staffFieldId, "", FieldValidator::Info },
    { "10", staffFieldId, "", FieldValidator::Info },
    { "-10", staffFieldId, "", FieldValidator::Info },
    // QML Slider does not allow to enter values higher or lower than specified range
  };

  // compare results
  for ( const auto &c : combinations )
  {
    controller.setFormValue( c.fieldUuid, c.value );
    const FormItem *item = controller.formItem( c.fieldUuid );

    QCOMPARE( item->validationMessage(), c.expectedValidationMessage );
    QCOMPARE( item->validationMessageLevel(), c.expectedValidationMessageLevel );
  }

  // field "Cabin Crew" stayed with invalid input, check controller flag of values validity
  QCOMPARE( controller.hasValidationErrors(), false );
}

void TestFormEditors::testRelationsWidgetPresence()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";

  QSignalSpy spy( QgsProject::instance()->relationManager(), &QgsRelationManager::relationsLoaded );
  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );
  QCOMPARE( spy.count(), 1 );


  QgsMapLayer *airportsLayer = QgsProject::instance()->mapLayersByName( QStringLiteral( "airports" ) ).at( 0 );
  QgsVectorLayer *airportsVLayer = static_cast<QgsVectorLayer *>( airportsLayer );

  QgsMapLayer *airportTowersLayer = QgsProject::instance()->mapLayersByName( QStringLiteral( "airport-towers" ) ).at( 0 );
  QgsVectorLayer *airportTowersVLayer = static_cast<QgsVectorLayer *>( airportTowersLayer );

  QVERIFY( airportsVLayer && airportsVLayer->isValid() );
  QVERIFY( airportTowersVLayer && airportTowersVLayer->isValid() );

  // check if we added exactly one relation widget when project has autogenerated layout

  QgsFeature f = airportsVLayer->getFeature( 1 );
  FeatureLayerPair pair( f, airportsVLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  const TabItem *tabItem = controller.tabItem( 0 );
  QVector<QUuid> formItems = tabItem->formItems();

  int relationsCount = 0;
  int relationReferencesCount = 0;

  for ( const auto &itemId : formItems )
  {
    const FormItem *item = controller.formItem( itemId );
    if ( item->editorWidgetType() == QStringLiteral( "relation" ) )
      relationsCount++;
    else if ( item->editorWidgetType() == QStringLiteral( "RelationReference" ) )
      relationReferencesCount++;
  }

  QVERIFY( relationsCount == 1 );
  QVERIFY( relationReferencesCount == 0 );

  // check if we added exactly one relation reference widget when project has autogenerated layout

  QgsFeature ft = airportTowersVLayer->getFeature( 1 );
  FeatureLayerPair pairTower( ft, airportTowersVLayer );

  controller.setFeatureLayerPair( pairTower );

  const TabItem *tabItemTower = controller.tabItem( 0 );
  formItems = tabItemTower->formItems();

  relationsCount = 0;
  relationReferencesCount = 0;

  for ( const auto &itemId : formItems )
  {
    const FormItem *item = controller.formItem( itemId );
    if ( item->editorWidgetType() == QStringLiteral( "relation" ) )
      relationsCount++;
    else if ( item->editorWidgetType() == QStringLiteral( "RelationReference" ) )
      relationReferencesCount++;
  }

  QVERIFY( relationsCount == 0 );
  QVERIFY( relationReferencesCount == 1 );
}
