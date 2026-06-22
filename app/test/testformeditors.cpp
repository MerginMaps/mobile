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
#include "relationfeaturesmodel.h"
#include "relationreferencefeaturesmodel.h"
#include "valuerelationfeaturesmodel.h"
#include "valuerelationcontroller.h"

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
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *planesL = QgsProject::instance()->mapLayersByName( QStringLiteral( "Planes" ) ).at( 0 );
  QgsVectorLayer *planesLayer = static_cast<QgsVectorLayer *>( planesL );

  QVERIFY( planesLayer && planesLayer->isValid() );

  // set constraints
  QString qmlStyle = projectDir + "/PlanesNumericFields.qml";
  bool res;
  planesLayer->loadNamedStyle( qmlStyle, res, false );
  QVERIFY( res );

  QgsFeature feat( planesLayer->dataProvider()->fields() );
  feat.setValid( true ); // empty feature is by default invalid
  FeatureLayerPair pair( feat, planesLayer );

  AttributeController controller;
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
    QVariant value;
    QUuid fieldUuid;
    QString expectedValidationMessage;
    FieldValidator::ValidationStatus expectedValidationStatus;
  };

  QList<combination> combinations =
  {
    // field "Heading", Int, range <100; 1000>, range editable
    { QVariant( QString() ), headingFieldId, "", FieldValidator::Valid },
    { "1", headingFieldId, FieldValidator::numberLowerBoundReached().arg( 100 ), FieldValidator::Error },
    { "-1", headingFieldId, FieldValidator::numberLowerBoundReached().arg( 100 ), FieldValidator::Error },
    { "10", headingFieldId, FieldValidator::numberLowerBoundReached().arg( 100 ), FieldValidator::Error },
    { "-10", headingFieldId, FieldValidator::numberLowerBoundReached().arg( 100 ), FieldValidator::Error },
    { "-100", headingFieldId, FieldValidator::numberLowerBoundReached().arg( 100 ), FieldValidator::Error },
    { "100", headingFieldId, "", FieldValidator::Valid },
    { "100h", headingFieldId, FieldValidator::numberInvalid(), FieldValidator::Error },
    { "100", headingFieldId, "", FieldValidator::Valid },
    { "1001", headingFieldId, FieldValidator::numberUpperBoundReached().arg( 1000 ), FieldValidator::Error },
    { "1000", headingFieldId, "", FieldValidator::Valid },

    // field "Importance", Real, range <-100.00; 100.00>, step:0.01, precision: 2, range editable
    { QVariant( QString() ), importanceFieldId, "", FieldValidator::Valid },
    { "0", importanceFieldId, "", FieldValidator::Valid },
    { "-1.00", importanceFieldId, "", FieldValidator::Valid },
    { "100.00", importanceFieldId, "", FieldValidator::Valid },
    { "100.002", importanceFieldId, FieldValidator::numberUpperBoundReached().arg( 100.00 ), FieldValidator::Error },
    { "100.002fdsa", importanceFieldId, FieldValidator::numberInvalid(), FieldValidator::Error },
    { "100.,.,.,", importanceFieldId, FieldValidator::numberInvalid(), FieldValidator::Error },
    { "1 000", importanceFieldId, FieldValidator::numberInvalid(), FieldValidator::Error },
    { "10", importanceFieldId, "", FieldValidator::Valid },

    // field "Pilots", Int, range <-1000; -100>, range editable
    { QVariant( QString() ), pilotsFieldId, "", FieldValidator::Valid },
    { "-100", pilotsFieldId, "", FieldValidator::Valid },
    { "-1000", pilotsFieldId, "", FieldValidator::Valid },
    { "0", pilotsFieldId, FieldValidator::numberUpperBoundReached().arg( -100 ), FieldValidator::Error },
    { "150", pilotsFieldId, FieldValidator::numberUpperBoundReached().arg( -100 ), FieldValidator::Error },
    { "-1001", pilotsFieldId, FieldValidator::numberLowerBoundReached().arg( -1000 ), FieldValidator::Error },
    { "-51216354321435", pilotsFieldId, FieldValidator::numberExceedingVariableLimits(), FieldValidator::Error },
    { "--100", pilotsFieldId, FieldValidator::numberInvalid(), FieldValidator::Error },
    { "--100fsda", pilotsFieldId, FieldValidator::numberInvalid(), FieldValidator::Error },
    { "-100", pilotsFieldId, "", FieldValidator::Valid },

    // field "Cabin Crew", Int, no limit, range editable
    { QVariant( QString() ), cabinCrewFieldId, "", FieldValidator::Valid },
    { "-100", cabinCrewFieldId, "", FieldValidator::Valid },
    { "-1000", cabinCrewFieldId, "", FieldValidator::Valid },
    { "-2147483647", cabinCrewFieldId, "", FieldValidator::Valid }, // int limit from QGIS
    { "2147483647", cabinCrewFieldId, "", FieldValidator::Valid }, // int limit from QGIS
    { "214748364799", cabinCrewFieldId, FieldValidator::numberExceedingVariableLimits(), FieldValidator::Error },
    { "-214748364799", cabinCrewFieldId, FieldValidator::numberExceedingVariableLimits(), FieldValidator::Error },
    { "-214748-", cabinCrewFieldId, FieldValidator::numberInvalid(), FieldValidator::Error },

    // field "Staff", Int, no limit, range slider
    { QVariant( QString() ), staffFieldId, "", FieldValidator::Valid },
    { "10", staffFieldId, "", FieldValidator::Valid },
    { "-10", staffFieldId, "", FieldValidator::Valid },
    // QML Slider does not allow to enter values higher or lower than specified range
  };

  // compare results
  for ( const auto &c : combinations )
  {
    controller.setFormValue( c.fieldUuid, c.value );
    const FormItem *item = controller.formItem( c.fieldUuid );

    QCOMPARE( item->validationMessage(), c.expectedValidationMessage );
    QCOMPARE( item->validationStatus(), c.expectedValidationStatus );
  }

  // field "Cabin Crew" stayed with invalid input, check controller flag of values validity
  QCOMPARE( controller.hasValidationErrors(), true );

  // invalidate some attribute and check if hasValidationErrors responds correctly
  controller.setFormValue( cabinCrewFieldId, "100" );
  QCOMPARE( controller.hasValidationErrors(), false );
}

void TestFormEditors::testRelationsEditor()
{
  /* Test project: project_relations
   * It has 2 relations set up:
   *
   * |Layer Main|    has many      |Layer Sub|     has many      |Layer Subsub|
   * |  point   |                  | no geo  |                   |   no geo   |
   */
  QString projectDir = TestUtils::testDataDir() + "/project_relations";
  QString projectName = "proj.qgz";

  QSignalSpy spy( QgsProject::instance()->relationManager(), &QgsRelationManager::relationsLoaded );
  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );
  QCOMPARE( spy.count(), 1 );

  QgsMapLayer *mainL = QgsProject::instance()->mapLayersByName( QStringLiteral( "main" ) ).at( 0 );
  QgsVectorLayer *mainLayer = static_cast<QgsVectorLayer *>( mainL );

  QVERIFY( mainLayer && mainLayer->isValid() );

  QgsMapLayer *subL = QgsProject::instance()->mapLayersByName( QStringLiteral( "sub" ) ).at( 0 );
  QgsVectorLayer *subLayer = static_cast<QgsVectorLayer *>( subL );

  QVERIFY( subLayer && subLayer->isValid() );

  QgsMapLayer *subsubL = QgsProject::instance()->mapLayersByName( QStringLiteral( "subsub" ) ).at( 0 );
  QgsVectorLayer *subsubLayer = static_cast<QgsVectorLayer *>( subsubL );

  QVERIFY( subsubLayer && subsubLayer->isValid() );

  AttributeController mainController, subController, subsubController;

  QgsFeature mainFeature = mainLayer->getFeature( 1 );
  FeatureLayerPair mainPair( mainFeature, mainLayer );
  mainController.setFeatureLayerPair( mainPair );

  // check if formItem has correct relation set
  const TabItem *mainTab = mainController.tabItem( 0 );
  QVector<QUuid> mainItems = mainTab->formItems();

  QCOMPARE( mainItems.count(), 4 ); // fid, name, uuid, relation

  const FormItem *mainRelation = mainController.formItem( mainItems.at( 3 ) ); // last one should be relation

  QCOMPARE( mainRelation->relation().id(), QStringLiteral( "sub_c0a8dd_mainFK_main_2ceda_pk" ) );
  QCOMPARE( mainRelation->name(), QStringLiteral( "mainsub" ) );

  // test relation features model - should fetch one feature
  RelationFeaturesModel mainRelationModel;
  QSignalSpy modelSpy( &mainRelationModel, &LayerFeaturesModel::fetchingResultsChanged );
  mainRelationModel.setRelation( mainRelation->relation() );
  mainRelationModel.setParentFeatureLayerPair( mainPair );

  modelSpy.wait();
  int featuresCount = mainRelationModel.rowCount();
  QVERIFY( featuresCount > 0 );

  QModelIndex featIdx = mainRelationModel.index( 0 );
  FeatureLayerPair subTempPair = mainRelationModel.data( featIdx, FeaturesModel::FeaturePair ).value<FeatureLayerPair>();

  QCOMPARE( subTempPair.feature().attribute( QStringLiteral( "Name" ) ), QStringLiteral( "SubFirst" ) );

  // let's add new feature to sublayer and see if the mainRelationModel reflects that
  QgsFeature newSubFeature( subLayer->dataProvider()->fields() );
  FeatureLayerPair newSubPair( newSubFeature, subLayer );

  subController.setFeatureLayerPair( newSubPair );
  subController.setParentController( &mainController );
  subController.setLinkedRelation( mainRelation->relation() );

  newSubPair = subController.featureLayerPair();

  // check if relation reference field has been prefilled with parent uuid
  QVariant parentPk = mainFeature.attribute( QStringLiteral( "pk" ) );
  QVariant childFk = newSubPair.feature().attribute( QStringLiteral( "mainFK" ) );

  QCOMPARE( childFk, parentPk );

  subLayer->startEditing();
  QVERIFY( subLayer->addFeature( newSubPair.featureRef() ) );

  // catch feature id
  QSignalSpy catchFeatureIdSpy( subLayer, &QgsVectorLayer::featureAdded );

  QVERIFY( subLayer->commitChanges() );

  QCOMPARE( catchFeatureIdSpy.count(), 1 );
  QgsFeatureId addedFeatureId = catchFeatureIdSpy.takeFirst().at( 0 ).value<QgsFeatureId>();

  modelSpy.wait();
  int newFeaturesCount = mainRelationModel.rowCount();
  QCOMPARE( newFeaturesCount, featuresCount + 1 ); // we have added one feature

  // remove last feature and check count again

  subLayer->startEditing();

  QList<QgsFeatureId> ids = subLayer->allFeatureIds().values();
  QVERIFY( ids.count() > 1 );

  QVERIFY( subLayer->deleteFeature( addedFeatureId ) );
  QVERIFY( subLayer->commitChanges() );

  modelSpy.wait();
  int reducedFeaturesCount = mainRelationModel.rowCount();
  QCOMPARE( reducedFeaturesCount, featuresCount );

  // check relation in sublayer
  QgsFeature subFeature = subLayer->getFeature( 1 );
  FeatureLayerPair subPair( subFeature, subLayer );
  subController.setFeatureLayerPair( subPair );

  // check if formItem has correct relation set
  const TabItem *subTab = subController.tabItem( 0 );
  QVector<QUuid> subItems = subTab->formItems();

  QCOMPARE( subItems.count(), 5 ); // fid, name, uuid(foreign key), uuid(primary key), relation

  const FormItem *subRelation = subController.formItem( subItems.at( 4 ) ); // last one should be relation

  QCOMPARE( subRelation->relation().id(), QStringLiteral( "subsub_64f_subFK_sub_c0a8dd_pk" ) );
  // relation does not have a name, nor field has alias, name of the field should thus be set to referencing layer name
  QCOMPARE( subRelation->name(), QStringLiteral( "subsub" ) );

  RelationFeaturesModel subRelationModel;
  subRelationModel.setRelation( subRelation->relation() );
  subRelationModel.setParentFeatureLayerPair( subPair );

  featuresCount = mainRelationModel.rowCount();
  QVERIFY( featuresCount > 0 );
}

void TestFormEditors::testRelationsReferenceEditor()
{
  /* Test project: project_relations
   * It has 2 relations set up:
   *
   * |Layer Main|    has many      |Layer Sub|     has many      |Layer Subsub|
   * |  point   |                  | no geo  |                   |   no geo   |
   */
  QString projectDir = TestUtils::testDataDir() + "/project_relations";
  QString projectName = "proj.qgz";

  QSignalSpy spy( QgsProject::instance()->relationManager(), &QgsRelationManager::relationsLoaded );
  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );
  QCOMPARE( spy.count(), 1 );

  QgsMapLayer *mainL = QgsProject::instance()->mapLayersByName( QStringLiteral( "main" ) ).at( 0 );
  QgsVectorLayer *mainLayer = static_cast<QgsVectorLayer *>( mainL );

  QVERIFY( mainLayer && mainLayer->isValid() );

  QgsMapLayer *subL = QgsProject::instance()->mapLayersByName( QStringLiteral( "sub" ) ).at( 0 );
  QgsVectorLayer *subLayer = static_cast<QgsVectorLayer *>( subL );

  QVERIFY( subLayer && subLayer->isValid() );

  QgsMapLayer *subsubL = QgsProject::instance()->mapLayersByName( QStringLiteral( "subsub" ) ).at( 0 );
  QgsVectorLayer *subsubLayer = static_cast<QgsVectorLayer *>( subsubL );

  QVERIFY( subsubLayer && subsubLayer->isValid() );

  AttributeController mainController, subController, subsubController;

  QgsFeature subFeature = subLayer->getFeature( 1 );
  FeatureLayerPair subPair( subFeature, subLayer );
  subController.setFeatureLayerPair( subPair );

  const TabItem *subTab = subController.tabItem( 0 );
  QVector<QUuid> subItems = subTab->formItems();

  const FormItem *subRelationRef = subController.formItem( subItems.at( 2 ) ); // relation ref
  QCOMPARE( subRelationRef->editorWidgetType(), QStringLiteral( "RelationReference" ) );

  // We want to test if relation reference models are filled with correct data
  RelationReferenceFeaturesModel subRelationRefModel;
  QSignalSpy subSpy( &subRelationRefModel, &LayerFeaturesModel::fetchingResultsChanged );
  subRelationRefModel.setConfig( subRelationRef->editorWidgetConfig() );
  subRelationRefModel.setProject( QgsProject::instance() );

  subSpy.wait();
  QgsFeature parentFeat = mainLayer->getFeature( 1 ); // this is parent feature
  QVariant fk = subRelationRefModel.foreignKeyFromReferencedFeatureId( parentFeat.id() );

  QCOMPARE( fk, parentFeat.attribute( "pk" ) );

  // Now the same for subsubLayer
  QgsFeature subsubFeature = subsubLayer->getFeature( 1 );
  FeatureLayerPair subsubPair( subsubFeature, subsubLayer );
  subsubController.setFeatureLayerPair( subsubPair );

  const TabItem *subsubTab = subsubController.tabItem( 0 );
  QVector<QUuid> subsubItems = subsubTab->formItems();

  const FormItem *subsubRelationRef = subsubController.formItem( subsubItems.at( 2 ) ); // relation ref
  QCOMPARE( subsubRelationRef->editorWidgetType(), QStringLiteral( "RelationReference" ) );

  RelationReferenceFeaturesModel subsubRelationRefModel;
  subsubRelationRefModel.setConfig( subsubRelationRef->editorWidgetConfig() );
  subsubRelationRefModel.setProject( QgsProject::instance() );

  QgsFeature parentSubFeat = subLayer->getFeature( 1 ); // this is parent feature
  QVariant subsubFk = subRelationRefModel.foreignKeyFromReferencedFeatureId( parentSubFeat.id() );

  QCOMPARE( subsubFk, parentSubFeat.attribute( "pk" ) );
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

void TestFormEditors::testValueRelationConversions()
{
  /* Tests qgisFormatToArray() and arrayToQgisFormat() for null, single,
   * and multi values.
   *
   * AllowMulti=false:
   *   QGIS value -> keys:  "1"       -> ["1"]
   *   keys -> QGIS value:  ["42"]    -> "42"
   *
   * AllowMulti=true:
   *   QGIS value -> keys:  "{1,3,4}" -> ["1","3","4"]
   *   keys -> QGIS value:  ["1","3","4"] -> "{1,3,4}";  ["1"] -> "{1}"
   */
  QgsProject::instance()->removeAllMapLayers();

  QgsVectorLayer *layer = TestUtils::createVRLookupLayer( 5 );
  QVERIFY( layer && layer->isValid() );
  QgsProject::instance()->addMapLayer( layer );

  // Single value test
  ValueRelationController controller;
  controller.setConfig(
  {
    { QStringLiteral( "Layer" ),      layer->id() },
    { QStringLiteral( "Key" ),        QStringLiteral( "key" ) },
    { QStringLiteral( "Value" ),      QStringLiteral( "label" ) },
    { QStringLiteral( "AllowMulti" ), false }
  } );

  // null/empty : empty list
  QCOMPARE( controller.qgisFormatToArray( QVariant() ),             QStringList() );
  QCOMPARE( controller.qgisFormatToArray( QStringLiteral( "" ) ),   QStringList() );

  // single value "1" : ["1"]
  QCOMPARE( controller.qgisFormatToArray( QStringLiteral( "1" ) ),
            QStringList( { QStringLiteral( "1" ) } ) );

  // empty keys : ""
  QCOMPARE( controller.arrayToQgisFormat( {} ), QString() );

  // single key : plain value
  QCOMPARE( controller.arrayToQgisFormat( { QStringLiteral( "42" ) } ),
            QStringLiteral( "42" ) );

  // Multi value test
  controller.setConfig(
  {
    { QStringLiteral( "Layer" ),      layer->id() },
    { QStringLiteral( "Key" ),        QStringLiteral( "key" ) },
    { QStringLiteral( "Value" ),      QStringLiteral( "label" ) },
    { QStringLiteral( "AllowMulti" ), true }
  } );

  // null : empty list
  QCOMPARE( controller.qgisFormatToArray( QVariant() ), QStringList() );

  // "{1,3,4}" : ["1","3","4"]
  QCOMPARE( controller.qgisFormatToArray( QStringLiteral( "{1,3,4}" ) ),
            QStringList( { QStringLiteral( "1" ), QStringLiteral( "3" ), QStringLiteral( "4" ) } ) );

  // "{1}" : ["1"]
  QCOMPARE( controller.qgisFormatToArray( QStringLiteral( "{1}" ) ),
            QStringList( { QStringLiteral( "1" ) } ) );

  // empty keys : ""
  QCOMPARE( controller.arrayToQgisFormat( {} ), QString() );

  // ["1","3","4"] : "{1,3,4}"
  QCOMPARE( controller.arrayToQgisFormat( { QStringLiteral( "1" ), QStringLiteral( "3" ), QStringLiteral( "4" ) } ),
            QStringLiteral( "{1,3,4}" ) );

  // ["1"] : "{1}"
  QCOMPARE( controller.arrayToQgisFormat( { QStringLiteral( "1" ) } ),
            QStringLiteral( "{1}" ) );

  QgsProject::instance()->removeAllMapLayers();
}

void TestFormEditors::testValueRelationControllerLookup()
{
  /* Tests async display-text lookup for ValueRelationController:
   *
   * baseConfig controller (no FilterExpression):
   * 1. Missing key -> presentRawValue fires; invalidateSelection does NOT fire; displayText=""
   * 2. lookupDisplayTextOnHotreload without FilterExpression -> returns early, no signals
   * 3. Basic lookup: lookupDisplayTextOnValueChanged("1") -> "Cat1-A"
   *
   * filterConfig controller (FilterExpression set):
   * 4. lookupDisplayTextOnHotreload, key valid in context -> displayText updated ("Cat1-A")
   * 5. lookupDisplayTextOnHotreload, key no longer in context -> invalidateSelection fires
   *
   * Cases 1-2 run before case 3 so that mDisplayText is "" when case 1 checks
   * displaySpy.isEmpty() -> the setDisplayText guard skips emission for equal values.
   *
   * Lookup layer:
   *   key=1  label="Cat1-A"  category=1
   *   key=2  label="Cat1-B"  category=1
   *   key=3  label="Cat2-A"  category=2
   *   key=4  label="Cat2-B"  category=2
   *
   * Filter expression:  "category" = current_value('cat')
   * Form feature cat=1 -> keys 1,2 reachable; cat=2 -> keys 3,4 reachable.
   */
  QgsProject::instance()->removeAllMapLayers();

  // create the lookup layer
  QgsVectorLayer *lookupLayer = new QgsVectorLayer(
    QStringLiteral( "None?field=key:integer&field=label:string&field=category:integer" ),
    QStringLiteral( "vr_lookup" ),
    QStringLiteral( "memory" )
  );
  QVERIFY( lookupLayer && lookupLayer->isValid() );

  const QStringList labels = { QStringLiteral( "Cat1-A" ), QStringLiteral( "Cat1-B" ),
                               QStringLiteral( "Cat2-A" ), QStringLiteral( "Cat2-B" )
                             };
  QgsFeatureList features;
  for ( int i = 0; i < 4; ++i )
  {
    QgsFeature feature( lookupLayer->fields() );
    feature.setAttribute( QStringLiteral( "key" ),      i + 1 );
    feature.setAttribute( QStringLiteral( "label" ),    labels.at( i ) );
    feature.setAttribute( QStringLiteral( "category" ), i < 2 ? 1 : 2 );
    features << feature;
  }
  lookupLayer->dataProvider()->addFeatures( features );
  QgsProject::instance()->addMapLayer( lookupLayer );

  // create the form layer, that provides fields for building form-scope features
  QgsVectorLayer *formLayer = new QgsVectorLayer(
    QStringLiteral( "None?field=cat:integer" ),
    QStringLiteral( "vr_form" ),
    QStringLiteral( "memory" )
  );
  QVERIFY( formLayer && formLayer->isValid() );
  QgsProject::instance()->addMapLayer( formLayer );

  const QVariantMap baseConfig =
  {
    { QStringLiteral( "Layer" ),      lookupLayer->id() },
    { QStringLiteral( "Key" ),        QStringLiteral( "key" ) },
    { QStringLiteral( "Value" ),      QStringLiteral( "label" ) },
    { QStringLiteral( "AllowMulti" ), false }
  };

  QVariantMap filterConfig = baseConfig;
  filterConfig[ QStringLiteral( "FilterExpression" ) ] =
    QStringLiteral( "\"category\" = current_value('cat')" );

  ValueRelationController baseController;
  baseController.setConfig( baseConfig );

  QSignalSpy rawSpy( &baseController, &ValueRelationController::presentRawValue );
  QSignalSpy invalidateSpy( &baseController, &ValueRelationController::invalidateSelection );
  QSignalSpy displaySpy( &baseController, &ValueRelationController::displayTextChanged );

  // 1. Missing key, no filter -> presentRawValue; NOT invalidateSelection
  baseController.lookupDisplayTextOnValueChanged( QStringLiteral( "99999" ) );
  QVERIFY( rawSpy.wait( 5000 ) );
  QVERIFY( displaySpy.isEmpty() );  // mDisplayText was already ""; setDisplayText skips emission
  QVERIFY( invalidateSpy.isEmpty() );
  QCOMPARE( baseController.displayText(), QString() );

  // 2. Hotreload without FilterExpression -> early return, no signals
  rawSpy.clear();
  displaySpy.clear();
  baseController.lookupDisplayTextOnHotreload( QStringLiteral( "1" ), QgsFeature() );
  QVERIFY( displaySpy.isEmpty() );

  // 3. Basic lookup: key "1" -> "Cat1-A"
  displaySpy.clear();
  baseController.lookupDisplayTextOnValueChanged( QStringLiteral( "1" ) );
  QVERIFY( displaySpy.wait( 5000 ) );
  QCOMPARE( baseController.displayText(), QStringLiteral( "Cat1-A" ) );

  ValueRelationController filterController;
  filterController.setConfig( filterConfig );

  QSignalSpy filterDisplaySpy( &filterController, &ValueRelationController::displayTextChanged );
  QSignalSpy filterInvalidateSpy( &filterController, &ValueRelationController::invalidateSelection );

  // 4. Hotreload with filter, key valid in context -> displayText updated
  // Form context: cat=1 -> key 1 ("Cat1-A") is reachable (category=1)
  QgsFeature formFeature( formLayer->fields() );
  formFeature.setAttribute( QStringLiteral( "cat" ), 1 );
  formFeature.setValid( true );
  filterController.lookupDisplayTextOnHotreload( QStringLiteral( "1" ), formFeature );
  QVERIFY( filterDisplaySpy.wait( 5000 ) );
  QCOMPARE( filterController.displayText(), QStringLiteral( "Cat1-A" ) );

  // 5. Hotreload with filter, key not in context -> invalidateSelection
  // Form context: cat=2 -> key 1 ("Cat1-A") is NOT reachable (category=1 ≠ 2)
  filterDisplaySpy.clear();
  QgsFeature formFeature2( formLayer->fields() );
  formFeature2.setAttribute( QStringLiteral( "cat" ), 2 );
  formFeature2.setValid( true );
  filterController.lookupDisplayTextOnHotreload( QStringLiteral( "1" ), formFeature2 );
  QVERIFY( filterInvalidateSpy.wait( 5000 ) );

  QgsProject::instance()->removeAllMapLayers();
}
