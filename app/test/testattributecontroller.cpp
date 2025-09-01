/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testattributecontroller.h"

#include <QObject>
#include <QApplication>
#include <QScreen>
#include <memory>

#include "testutils.h"

#include "qgsapplication.h"
#include "qgsvectorlayer.h"
#include "qgsproject.h"

#include "attributecontroller.h"
#include "attributetabproxymodel.h"
#include "attributetabmodel.h"
#include "attributeformproxymodel.h"
#include "attributeformmodel.h"
#include "imageutils.h"
#include "inpututils.h"

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
  QString dataDir = TestUtils::testDataDir();
  QString planesVectorFile = dataDir + "/planes/points.shp";
  QString qmlStyle = dataDir + "/planes/SimpleTabsForPlanesLayer.qml";
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

void TestAttributeController::twoGroupsDragAndDropLayout()
{
  QString dataDir = TestUtils::testDataDir();
  QString planesVectorFile = dataDir + "/planes/points.shp";
  QString qmlStyle = dataDir + "/planes/SimpleGroupsForPlanesLayer.qml";
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

  // we dont have tabs, only groups
  QVERIFY( !controller.hasTabs() );
  // Has single default tab
  QVERIFY( controller.tabCount() == 1 );
  const TabItem *tabItem = controller.tabItem( 0 );
  QVERIFY( tabItem );
  QVERIFY( tabItem->name().isEmpty() );

  QSet<QString> groupNames;
  const QVector<QUuid> formItems = tabItem->formItems();
  for ( const QUuid &itemId : formItems )
  {
    const FormItem *item = controller.formItem( itemId );
    QVERIFY( item );
    groupNames.insert( item->groupName() );
  }
  QCOMPARE( groupNames.size(), 2 );
}

void TestAttributeController::tabsAndFieldsMixed()
{
  QString dataDir = TestUtils::testDataDir();
  QString planesVectorFile = dataDir + "/planes/points.shp";
  QString qmlStyle = dataDir + "/planes/MixedTabsFieldsForPlanesLayer.qml";
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

  // Mixed tabs and fields => tabs are ignored
  QVERIFY( !controller.hasTabs() );
  // Has single default tab
  QVERIFY( controller.tabCount() == 1 );

  const TabItem *tabItem = controller.tabItem( 0 );
  Q_ASSERT( tabItem );
  QCOMPARE( tabItem->tabIndex(), 0 );
  QCOMPARE( tabItem->name(), "" );
  QCOMPARE( tabItem->isVisible(), true );
  const QVector<QUuid> formItems = tabItem->formItems();
  QCOMPARE( formItems.size(), 6 );
}

void TestAttributeController::testValidationMessages()
{
  QString projectDir = TestUtils::testDataDir() + "/planes";
  QString projectName = "quickapp_project.qgs";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *sectorL = QgsProject::instance()->mapLayersByName( QStringLiteral( "FlySector" ) ).at( 0 );
  QgsVectorLayer *flySectorLayer = static_cast<QgsVectorLayer *>( sectorL );

  QVERIFY( flySectorLayer && flySectorLayer->isValid() );

  QgsFeature feat( flySectorLayer->dataProvider()->fields() );
  feat.setValid( true ); // empty feature is by default invalid
  FeatureLayerPair pair( feat, flySectorLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  /* Attributes:
   *  - fid
   *  - Name(txt)           Not NULL - SOFT
   *  - Size(real)          Not NULL - HARD <0; 10000>
   *  - SectorId(int)       Unique - SOFT <-100; 1000>
   *  - Occupied(bool)      Expression, must be TRUE - HARD
   *  - DateTime(datetime)  Not NULL - HARD
   *  - LastEdit(date)      Not NULL - HARD (custom date format)
   *  - Hash(big int)       Unique - HARD
   *  - Code(txt)           Length limit 5
   */

  // there are HARD constraints, so controller should already have validation errors
  QCOMPARE( controller.hasValidationErrors(), true );

  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 9 );

  struct testunit
  {
    QUuid id;
    QVariant value;
    QString expectedValidationMessage;
    FieldValidator::ValidationStatus expectedValidationStatus;
  };

  namespace V = ValidationTexts;

  QList<testunit> testunits
  {
    // Attribute - Name Not NULL - SOFT
    { items.at( 1 ), QVariant(), V::softNotNullFailed, FieldValidator::Warning  },
    { items.at( 1 ), QStringLiteral( "A" ), "", FieldValidator::Valid },
    { items.at( 1 ), QVariant( QString() ), V::softNotNullFailed, FieldValidator::Warning },
    { items.at( 1 ), "abcsd fsdkajf nsa ", "", FieldValidator::Valid },

    // Attribute - Size Not NULL - HARD <0; 10000>
    { items.at( 2 ), QVariant(), V::hardNotNullFailed, FieldValidator::Error },
    { items.at( 2 ), "1", "", FieldValidator::Valid },
    { items.at( 2 ), "1a", V::numberInvalid, FieldValidator::Error },
    { items.at( 2 ), "10001", V::numberUpperBoundReached.arg( 10000 ), FieldValidator::Error },
    { items.at( 2 ), "-1", V::numberLowerBoundReached.arg( 0 ), FieldValidator::Error },
    { items.at( 2 ), "150", "", FieldValidator::Valid },

    // Attribute - SectorId Unique - SOFT <-100; 1000>
    { items.at( 3 ), "1", "", FieldValidator::Valid },
    { items.at( 3 ), "-100", "", FieldValidator::Valid },
    { items.at( 3 ), "13", V::softUniqueFailed, FieldValidator::Warning }, // there should already be feature with such value
    { items.at( 3 ), "14", "", FieldValidator::Valid },
    { items.at( 3 ), "14sad", V::numberInvalid, FieldValidator::Error },
    { items.at( 3 ), "-", V::numberInvalid, FieldValidator::Error },
    { items.at( 3 ), ".", V::numberInvalid, FieldValidator::Error },
    { items.at( 3 ), "14", "", FieldValidator::Valid },

    // Attribute - Occupied Expression, must be TRUE - HARD, expression descriptionn: 'Must be true'
    { items.at( 4 ), false, QStringLiteral( "Must be true" ), FieldValidator::Error },
    { items.at( 4 ), true, "", FieldValidator::Valid },

    // Attribure - DateTime(datetime) Not NULL - HARD, format: yyyy-MM-dd HH:mm:ss (default)
    { items.at( 5 ), QVariant(), V::hardNotNullFailed, FieldValidator::Error },
    { items.at( 5 ), QVariant( QDateTime::fromString( "2020-03-10 10:40:30", "yyyy-MM-dd HH:mm:ss" ) ), "", FieldValidator::Valid },

    // Attribure - LastEdit(date) Not NULL - HARD, format: dd-MM-yyyy (custom)
    { items.at( 6 ), QVariant(), V::hardNotNullFailed, FieldValidator::Error },
    { items.at( 6 ), QVariant( QDateTime::fromString( "29-10-1998", "dd-MM-yyyy" ) ), "", FieldValidator::Valid },

    // Attribute - Hash Unique - HARD
    { items.at( 7 ), QVariant(), "", FieldValidator::Valid },
    { items.at( 7 ), "1", V::hardUniqueFailed, FieldValidator::Error },
    { items.at( 7 ), QVariant(), "", FieldValidator::Valid },
    { items.at( 7 ), "2", "", FieldValidator::Valid },

    // Attribute - Code Length limit 5
    { items.at( 8 ), "", "", FieldValidator::Valid },
    { items.at( 8 ), "f", "", FieldValidator::Valid },
    { items.at( 8 ), "fi", "", FieldValidator::Valid },
    { items.at( 8 ), "five ", "", FieldValidator::Valid },
    { items.at( 8 ), "five chars limit", V::textTooLong.arg( 5 ), FieldValidator::Error },
    { items.at( 8 ), "five ", "", FieldValidator::Valid }
  };

  for ( const testunit &unit : testunits )
  {
    const FormItem *item = controller.formItem( unit.id );
    controller.setFormValue( unit.id, unit.value );

    QCOMPARE( item->validationMessage(), unit.expectedValidationMessage );
    QCOMPARE( item->validationStatus(), unit.expectedValidationStatus );
  }

  QCOMPARE( controller.hasValidationErrors(), false );

  // invalidate some attribute and check if hasValidationErrors responds correctly
  controller.setFormValue( items.at( 8 ), "five chars limit" );
  QCOMPARE( controller.hasValidationErrors(), true );

  controller.setFormValue( items.at( 8 ), "five " );
  QCOMPARE( controller.hasValidationErrors(), false );

  // Try assigning different features and values to see if the state is reseted
  QgsFeature feat2( flySectorLayer->dataProvider()->fields() );
  feat2.setValid( true );
  FeatureLayerPair pair2( feat2, flySectorLayer );

  controller.setFeatureLayerPair( pair2 );

  QCOMPARE( controller.hasValidationErrors(), true );
}

void TestAttributeController::testExpressions()
{
  QString projectDir = TestUtils::testDataDir() + "/expressions";
  QString projectName = "project.qgz";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *layer = QgsProject::instance()->mapLayersByName( QStringLiteral( "survey" ) ).at( 0 );
  QgsVectorLayer *surveyLayer = static_cast<QgsVectorLayer *>( layer );

  QVERIFY( surveyLayer && surveyLayer->isValid() );

  QgsFeature feat;
  feat.setValid( true );
  feat.setFields( surveyLayer->fields(), true );
  FeatureLayerPair pair( feat, surveyLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  /* Attributes:
   *  - fid
   *  - text1(string)
   *  - text2(string)
   *  - text_exp(string)    "text1" + "text2"
   *  - num1(float)
   *  - num2(float)
   *  - num_exp(float)       "num1" + "num2"
   *  - text3(string)
   *  - text4(string)    attribute(@current_feature , 'text3')
   */

  QCOMPARE( controller.hasValidationErrors(), false );

  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 9 );

  struct testTwoFields
  {
    QUuid id1;
    QVariant value1;
    QUuid id2;
    QVariant value2;
    QUuid expectedId;
    QVariant expectedValue;
  };

  QList<testTwoFields> unitsTwoFields
  {
    { items.at( 1 ), QVariant( "1" ), items.at( 2 ), QVariant( "2" ), items.at( 3 ), QVariant( "12" ) },
    { items.at( 4 ), QVariant( 1 ), items.at( 5 ), QVariant( 2 ), items.at( 6 ), QVariant( 3 ) },
  };

  for ( const testTwoFields &unit : unitsTwoFields )
  {
    const FormItem *item1 = controller.formItem( unit.id1 );
    const FormItem *item2 = controller.formItem( unit.id2 );
    const FormItem *itemExpected = controller.formItem( unit.expectedId );

    controller.setFormValue( unit.id1, unit.value1 );
    controller.setFormValue( unit.id2, unit.value2 );

    QCOMPARE( controller.featureLayerPair().feature().attribute( item1->fieldIndex() ), unit.value1 );
    QCOMPARE( controller.featureLayerPair().feature().attribute( item2->fieldIndex() ), unit.value2 );
    QCOMPARE( controller.featureLayerPair().feature().attribute( itemExpected->fieldIndex() ), unit.expectedValue );
  }

  struct testSingleField
  {
    QUuid id;
    QVariant value;
    QUuid expectedId;
    QVariant expectedValue;
  };

  QList<testSingleField> unitsSingleField
  {
    { items.at( 7 ), QVariant( "A" ), items.at( 8 ), QVariant( "A" ) },
  };

  for ( const testSingleField &unit : unitsSingleField )
  {
    const FormItem *item = controller.formItem( unit.id );
    const FormItem *itemExpected = controller.formItem( unit.expectedId );

    controller.setFormValue( unit.id, unit.value );

    QCOMPARE( controller.featureLayerPair().feature().attribute( item->fieldIndex() ), unit.value );
    QCOMPARE( controller.featureLayerPair().feature().attribute( itemExpected->fieldIndex() ), unit.expectedValue );
  }

  QString uuid = controller.featureLayerPair().feature().attribute( 9 ).toString();
  pair = controller.featureLayerPair();

  // set another feature
  QgsFeature feat2;
  feat2.setValid( true );
  feat2.setFields( surveyLayer->fields(), true );
  FeatureLayerPair pair2( feat2, surveyLayer );
  controller.setFeatureLayerPair( pair2 );
  for ( int i = 0; i > surveyLayer->fields().count(); i++ )
  {
    QCOMPARE( controller.featureLayerPair().feature().attribute( i ), QVariant() );
  }

  // set initial fetuare again and check that field values are the same
  pair.featureRef().setId( 99 );
  controller.setFeatureLayerPair( pair );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 1 ), QVariant( "1" ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 2 ), QVariant( "2" ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 3 ), QVariant( "12" ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 4 ), QVariant( 1 ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 5 ), QVariant( 2 ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 6 ), QVariant( 3 ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 7 ), QVariant( "A" ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 8 ), QVariant( "A" ) );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 9 ), uuid );
}

void TestAttributeController::testRawValue()
{
  QString projectDir = TestUtils::testDataDir() + "/expressions";
  QString projectName = "project.qgz";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *layer = QgsProject::instance()->mapLayersByName( QStringLiteral( "survey" ) ).at( 0 );
  QgsVectorLayer *surveyLayer = static_cast<QgsVectorLayer *>( layer );

  QVERIFY( surveyLayer && surveyLayer->isValid() );

  QgsFeature feat;
  feat.setValid( true );
  feat.setFields( surveyLayer->fields(), true );
  FeatureLayerPair pair( feat, surveyLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  /* Attributes:
   *  - fid
   *  - text1(string)
   *  - text2(string)
   *  - text_exp(string)    "text1" + "text2"
   *  - num1(float)
   *  - num2(float)
   *  - num_exp(float)       "num1" + "num2"
   *  - text3(string)
   *  - text4(string)    attribute(@current_feature , 'text3')
   */

  QCOMPARE( controller.hasValidationErrors(), false );

  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 9 );

  struct testunit
  {
    QUuid id;
    QVariant value;
    QVariant expectedValue;
    QVariant rawValue;
    QString expectedValidationMessage;
    FieldValidator::ValidationStatus expectedValidationStatus;
  };

  namespace V = ValidationTexts;

  QList<testunit> testunits
  {
    { items.at( 1 ), QVariant( 1 ), QVariant( "1" ), QVariant( "1" ), "", FieldValidator::Valid },
    { items.at( 1 ), QVariant( "1" ), QVariant( "1" ), QVariant( "1" ), "", FieldValidator::Valid },
    { items.at( 4 ), QVariant( "a" ), QVariant(), QVariant( "a" ), V::numberInvalid, FieldValidator::Error },
    { items.at( 4 ), QVariant( "1" ), QVariant( 1 ), QVariant( "1" ), "", FieldValidator::Valid },
    { items.at( 4 ), QVariant( 1 ), QVariant( 1 ), QVariant( 1 ), "", FieldValidator::Valid },
  };

  for ( const testunit &unit : testunits )
  {
    const FormItem *item = controller.formItem( unit.id );

    controller.setFormValue( unit.id, unit.value );

    QCOMPARE( controller.featureLayerPair().feature().attribute( item->fieldIndex() ), unit.expectedValue );
    QCOMPARE( item->rawValue(), unit.rawValue );
    QCOMPARE( item->validationMessage(), unit.expectedValidationMessage );
    QCOMPARE( item->validationStatus(), unit.expectedValidationStatus );
  }
}

void TestAttributeController::testFieldsOutsideForm()
{
  QString projectDir = TestUtils::testDataDir() + "/expressions";
  QString projectName = "project.qgz";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *layer = QgsProject::instance()->mapLayersByName( QStringLiteral( "points" ) ).at( 0 );
  QgsVectorLayer *surveyLayer = static_cast<QgsVectorLayer *>( layer );

  QVERIFY( surveyLayer && surveyLayer->isValid() );

  QgsFeature feat;
  feat.setValid( true );
  feat.setFields( surveyLayer->fields(), true );
  FeatureLayerPair pair( feat, surveyLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  QCOMPARE( controller.hasValidationErrors(), false );

  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 8 );

  struct testcase
  {
    QUuid id;
    QVariant value;
    QVariant expectedText;
    QVariant expectedText2;
    QVariant expectedNum;
    QVariant expectedNum2;
  };

  QList<testcase> testCases
  {
    { items.at( 0 ), QVariant( "1" ), QVariant( "1" ), QVariant( "1 on update" ), QVariant(), QVariant() },
    { items.at( 2 ), QVariant( 2 ), QVariant( "1" ), QVariant( "1 on update" ), QVariant( 2 ), QVariant( 102 ) },
  };

  for ( const testcase &t : testCases )
  {
    const FormItem *item = controller.formItem( t.id );

    controller.setFormValue( t.id, t.value );

    QCOMPARE( controller.featureLayerPair().feature().attribute( 1 ), t.expectedText );
    QCOMPARE( controller.featureLayerPair().feature().attribute( 2 ), t.expectedText2 );
    QCOMPARE( controller.featureLayerPair().feature().attribute( 3 ), t.expectedNum );
    QVariant v = controller.featureLayerPair().feature().attribute( 4 );
    if ( v.isNull() )
    {
      QVERIFY( t.expectedNum2.isNull() );
    }
    else
    {
      QCOMPARE( controller.featureLayerPair().feature().attribute( 4 ), t.expectedNum2 );
    }
  }
}

void TestAttributeController::testPhotoRenaming()
{
  QString projectName = QStringLiteral( "testPhotoRenaming" );
  QString projectDir = QDir::tempPath() + "/MM_test_projects/" + projectName;

  QDir tempDir( projectDir );
  QVERIFY( tempDir.removeRecursively() );

  QVERIFY( InputUtils::cpDir( TestUtils::testDataDir() + "/test_photo_rename", projectDir ) );

  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/image1.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/image2.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/image3.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/image4.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/with_slash/image5.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/with_slash/image6.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/test_photo_rename.qgz" ) ) );

  QVERIFY( !QFile::exists( projectDir + QStringLiteral( "/image_mynotes.jpg" ) ) );
  QVERIFY( !QFile::exists( projectDir + QStringLiteral( "/photos/Survey.jpg" ) ) );
  QVERIFY( !QDir().exists( projectDir + QStringLiteral( "/media/with_slash/custom_photo_name_format" ) ) );
  QVERIFY( !QDir().exists( projectDir + QStringLiteral( "/custom_photo_name_format" ) ) );

  QVERIFY( QgsProject::instance()->read( projectDir + QStringLiteral( "/test_photo_rename.qgz" ) ) );

  QgsMapLayer *layer = QgsProject::instance()->mapLayersByName( QStringLiteral( "Survey" ) ).at( 0 );
  QgsVectorLayer *surveyLayer = static_cast<QgsVectorLayer *>( layer );

  QVERIFY( surveyLayer && surveyLayer->isValid() );

  QgsFeature feat( surveyLayer->fields() );
  FeatureLayerPair pair( feat, surveyLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 9 );

  //
  // Attachment fields:
  // 3 - photo
  // 4 - photo_with_slash
  // 5 - photo_in_subfolder_relative_to_project_path -> subfolder 'media'
  // 6 - photo_in_subfolder_relative_to_default_path -> subfolder 'media'
  // 7 - photo_with_slash_in_subfolder_relative_to_project_path -> subfolder 'media/with_slash'
  // 8 - photo_with_slash_in_subfolder_relative_to_default_path -> subfolder 'media/with_slash'
  //

  controller.setFormValue( items.at( 2 ), QStringLiteral( "mynotes " ) );

  // These are the field values set by QML code after attaching a photo
  controller.setFormValue( items.at( 3 ), QStringLiteral( "image1.jpg" ) );
  controller.setFormValue( items.at( 4 ), QStringLiteral( "image2.jpg" ) );
  controller.setFormValue( items.at( 5 ), QStringLiteral( "media/image3.jpg" ) );
  controller.setFormValue( items.at( 6 ), QStringLiteral( "image4.jpg" ) );
  controller.setFormValue( items.at( 7 ), QStringLiteral( "media/with_slash/image5.jpg" ) );
  controller.setFormValue( items.at( 8 ), QStringLiteral( "image6.jpg" ) );

  controller.save();

  struct testcase
  {
    QString originalPath;
    QString expectedNewPath;
    int fieldIdx;
    QString expectedNewFieldValue;
  };

  QList<testcase> testcases;

  QString originalPath;
  QString expectedNewPath;
  int fieldIdx;
  QString expectedNewFieldValue;

  const FormItem *fieldToTest;

  //
  // Field "photo"
  //  - Contains custom photo name expression
  //  - Does not contain any default path definition
  //  - Stores paths relative to project path
  //

  fieldIdx = 3;
  fieldToTest = controller.formItem( items.at( fieldIdx ) );
  QCOMPARE( fieldToTest->name(), QStringLiteral( "photo" ) );

  originalPath = projectDir + QStringLiteral( "/image1.jpg" );
  expectedNewPath = projectDir + QStringLiteral( "/image_mynotes.jpg" );
  expectedNewFieldValue = QStringLiteral( "image_mynotes.jpg" );

  testcases << testcase{ originalPath, expectedNewPath, fieldIdx, expectedNewFieldValue };

  //
  // Field "photo_with_slash"
  //  - Contains custom photo name expression with subfolder
  //  - Does not contain any default path definition
  //  - Stores paths relative to project path
  //

  fieldIdx = 4;
  fieldToTest = controller.formItem( items.at( fieldIdx ) );
  QCOMPARE( fieldToTest->name(), QStringLiteral( "photo_with_slash" ) );

  originalPath = projectDir + QStringLiteral( "/image2.jpg" );
  expectedNewPath = projectDir + QStringLiteral( "/photos/Survey.jpg" );
  expectedNewFieldValue = QStringLiteral( "photos/Survey.jpg" );

  testcases << testcase{ originalPath, expectedNewPath, fieldIdx, expectedNewFieldValue };

  //
  // Field "photo_in_subfolder_relative_to_project_path"
  //  - Contains custom photo name expression (no subfolder there)
  //  - Images should be stored inside folder 'media' (via default path in the field config)
  //  - Stores paths relative to project path (folder should be part of the field value)
  //

  fieldIdx = 5;
  fieldToTest = controller.formItem( items.at( fieldIdx ) );
  QCOMPARE( fieldToTest->name(), QStringLiteral( "photo_in_subfolder_relative_to_project_path" ) );

  originalPath = projectDir + QStringLiteral( "/media/image3.jpg" );
  expectedNewPath = projectDir + QStringLiteral( "/media/Survey_1.jpg" );
  expectedNewFieldValue = QStringLiteral( "media/Survey_1.jpg" );

  testcases << testcase{ originalPath, expectedNewPath, fieldIdx, expectedNewFieldValue };

  //
  // Field "photo_in_subfolder_relative_to_default_path"
  //  - Contains custom photo name expression (no subfolder there)
  //  - Images should be stored inside folder 'media' (via default path in the field config)
  //  - Stores paths relative to default path (folder should not be part of the field value)
  //

  fieldIdx = 6;
  fieldToTest = controller.formItem( items.at( fieldIdx ) );
  QCOMPARE( fieldToTest->name(), QStringLiteral( "photo_in_subfolder_relative_to_default_path" ) );

  originalPath = projectDir + QStringLiteral( "/media/image4.jpg" );
  expectedNewPath = projectDir + QStringLiteral( "/media/Survey_2.jpg" );
  expectedNewFieldValue = QStringLiteral( "Survey_2.jpg" );

  testcases << testcase{ originalPath, expectedNewPath, fieldIdx, expectedNewFieldValue };

  //
  // Field "photo_with_slash_in_subfolder_relative_to_project_path"
  //  - Contains custom photo name expression with subfolder 'custom_photo_name_format'
  //  - Images should be stored inside folder 'media/with_slash' (via default path in the field config)
  //  - Stores paths relative to project path (folder should be part of the field value)
  //
  //  - as a result, image should be stored in 'media/with_slash/custom_photo_name_format/*'
  //

  fieldIdx = 7;
  fieldToTest = controller.formItem( items.at( fieldIdx ) );
  QCOMPARE( fieldToTest->name(), QStringLiteral( "photo_with_slash_in_subfolder_relative_to_project_path" ) );

  originalPath = projectDir + QStringLiteral( "/media/with_slash/image5.jpg" );
  expectedNewPath = projectDir + QStringLiteral( "/media/with_slash/custom_photo_name_format/Survey_1.jpg" );
  expectedNewFieldValue = QStringLiteral( "media/with_slash/custom_photo_name_format/Survey_1.jpg" );

  testcases << testcase{ originalPath, expectedNewPath, fieldIdx, expectedNewFieldValue };

  //
  // Field "photo_with_slash_in_subfolder_relative_to_default_path"
  //  - Contains custom photo name expression with subfolder 'custom_photo_name_format'
  //  - Images should be stored inside folder 'media/with_slash' (via default path in the field config)
  //  - Stores paths relative to default path (folder should not be part of the field value)
  //
  //  - this is kind-of project misconfiguration, the widget is configured to store photos
  //    in 'media/with_slash', but that is overriden by the custom photo name as it includes
  //    a subfolder and the project is configured to store paths relative to default path.
  //    The value in the field will include the photo name format, even though the widget
  //    will look for images inside 'media/with_slash' folder ... plugin should report this.
  //

  fieldIdx = 8;
  fieldToTest = controller.formItem( items.at( fieldIdx ) );
  QCOMPARE( fieldToTest->name(), QStringLiteral( "photo_with_slash_in_subfolder_relative_to_default_path" ) );

  originalPath = projectDir + QStringLiteral( "/media/with_slash/image6.jpg" );
  expectedNewPath = projectDir + QStringLiteral( "/media/with_slash/custom_photo_name_format/Survey_2.jpg" );
  expectedNewFieldValue = QStringLiteral( "custom_photo_name_format/Survey_2.jpg" );

  testcases << testcase{ originalPath, expectedNewPath, fieldIdx, expectedNewFieldValue };

  const QgsFeature f = controller.featureLayerPair().feature();

  for ( const auto &testcase : std::as_const( testcases ) )
  {
    qInfo() << QStringLiteral( "Testing field #%1" ).arg( testcase.fieldIdx );

    QVERIFY( !QFile::exists( testcase.originalPath ) );
    QVERIFY( QFile::exists( testcase.expectedNewPath ) );

    QCOMPARE( f.attribute( testcase.fieldIdx ), testcase.expectedNewFieldValue );
  }
}

void TestAttributeController::testHtmlAndTextWidgets()
{
  QString projectDir = TestUtils::testDataDir() + "/expressions";
  QString projectName = "project.qgz";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *layer = QgsProject::instance()->mapLayersByName( QStringLiteral( "points" ) ).at( 0 );
  QgsVectorLayer *surveyLayer = static_cast<QgsVectorLayer *>( layer );

  QVERIFY( surveyLayer && surveyLayer->isValid() );

  QgsFeature feat;
  feat.setValid( true );
  feat.setFields( surveyLayer->fields(), true );
  FeatureLayerPair pair( feat, surveyLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  QCOMPARE( controller.hasValidationErrors(), false );

  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 8 );

  const auto htmlItem = controller.formItem( items.at( 3 ) );
  QCOMPARE( htmlItem->editorWidgetType(), "richtext" );
  QCOMPARE( htmlItem->editorWidgetConfig().value( "UseHtml" ).toBool(), true );

  const auto textItem = controller.formItem( items.at( 4 ) );
  QCOMPARE( textItem->editorWidgetType(), "richtext" );
  QCOMPARE( textItem->editorWidgetConfig().value( "UseHtml" ).toBool(), false );

  const auto spacerItem = controller.formItem( items.at( 5 ) );
  QCOMPARE( spacerItem->editorWidgetType(), "spacer" );
  QCOMPARE( spacerItem->editorWidgetConfig().value( "IsHLine" ).toBool(), true );

  const auto hLineItem = controller.formItem( items.at( 6 ) );
  QCOMPARE( hLineItem->editorWidgetType(), "spacer" );
  QCOMPARE( hLineItem->editorWidgetConfig().value( "IsHLine" ).toBool(), false );

  // update one field on which both HTML and Text widgets depends
  auto field = controller.formItem( items.at( 0 ) );
  QCOMPARE( field->name(), "text" );
  controller.setFormValue( field->id(), "my new text" );

  QCOMPARE( htmlItem->rawValue(), "<span>my new text on update</span>" );
  QCOMPARE( textItem->rawValue(), "my new text on update" );
}

void TestAttributeController::testVirtualFields()
{
  QString projectDir = TestUtils::testDataDir() + "/expressions";
  QString projectName = "project.qgz";

  QVERIFY( QgsProject::instance()->read( projectDir + "/" + projectName ) );

  QgsMapLayer *layer = QgsProject::instance()->mapLayersByName( QStringLiteral( "points" ) ).at( 0 );
  QgsVectorLayer *surveyLayer = static_cast<QgsVectorLayer *>( layer );

  QVERIFY( surveyLayer && surveyLayer->isValid() );

  QgsFeature feat;
  feat.setValid( true );
  feat.setFields( surveyLayer->fields(), true );
  FeatureLayerPair pair( feat, surveyLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );

  QCOMPARE( controller.hasValidationErrors(), false );

  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 8 );

  // A/ virtual field in form
  const auto virtualFieldItem = controller.formItem( items.at( 7 ) );
  QCOMPARE( virtualFieldItem->name(), "vitualfield2" );

  // update  field on which virtualField2 in form depends ("num" field)
  auto field = controller.formItem( items.at( 2 ) );
  QCOMPARE( field->name(), "num" );
  controller.setFormValue( field->id(), 333 );
  QCOMPARE( virtualFieldItem->rawValue(), 333 );

  // B/ virtual field not in form
  QCOMPARE( controller.featureLayerPair().feature().fieldNameIndex( "vitualfield2" ), 6 );
  // update  field on which virtualField in form depends ("text" field)
  auto field2 = controller.formItem( items.at( 0 ) );
  QCOMPARE( field2->name(), "text" );

  controller.setFormValue( field2->id(), "my new text" );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 1 ), "my new text" );
  controller.setFormValue( field2->id(), "my new text2" );
  QCOMPARE( controller.featureLayerPair().feature().attribute( 1 ), "my new text2" );
}

void TestAttributeController::testPhotoSketchingSave()
{
  const QString projectName = QStringLiteral( "testPhotoSketchSaving" );
  const QString projectDir = QDir::tempPath() + "/MM_test_projects/" + projectName;

  QDir tempDir( projectDir );
  QVERIFY( tempDir.removeRecursively() );

  QVERIFY( InputUtils::cpDir( TestUtils::testDataDir() + "/test_photo_rename", projectDir ) );

  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/image1.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/image2.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/image3.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/image4.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/with_slash/image5.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/media/with_slash/image6.jpg" ) ) );
  QVERIFY( QFile::exists( projectDir + QStringLiteral( "/test_photo_rename.qgz" ) ) );

  QVERIFY( !QFile::exists( projectDir + QStringLiteral( "/image_mynotes.jpg" ) ) );
  QVERIFY( !QFile::exists( projectDir + QStringLiteral( "/photos/Survey.jpg" ) ) );
  QVERIFY( !QDir().exists( projectDir + QStringLiteral( "/media/with_slash/custom_photo_name_format" ) ) );
  QVERIFY( !QDir().exists( projectDir + QStringLiteral( "/custom_photo_name_format" ) ) );

  //create "sketches"
  InputUtils::copyFile( QStringLiteral( "%1/image1.jpg" ).arg( projectDir ), QStringLiteral( "%1/%2/image1.jpg" ).arg( QDir::tempPath(), projectName ) );
  ImageUtils::copyExifMetadata( QStringLiteral( "%1/image1.jpg" ).arg( projectDir ), QStringLiteral( "%1/%2/image1.jpg" ).arg( QDir::tempPath(), projectName ) );
  QVERIFY( QFile::exists( QStringLiteral( "%1/%2/image1.jpg" ).arg( QDir::tempPath(), projectName ) ) );
  InputUtils::copyFile( QStringLiteral( "%1/photo.jpg" ).arg( projectDir ), QStringLiteral( "%1/%2/photo.jpg" ).arg( QDir::tempPath(), projectName ) );
  ImageUtils::copyExifMetadata( QStringLiteral( "%1/photo.jpg" ).arg( projectDir ), QStringLiteral( "%1/%2/photo.jpg" ).arg( QDir::tempPath(), projectName ) );
  QVERIFY( QFile::exists( QStringLiteral( "%1/%2/photo.jpg" ).arg( QDir::tempPath(), projectName ) ) );

  QVERIFY( QgsProject::instance()->read( projectDir + QStringLiteral( "/test_photo_rename.qgz" ) ) );

  QgsMapLayer *layer = QgsProject::instance()->mapLayersByName( QStringLiteral( "Survey" ) ).at( 0 );
  QgsVectorLayer *surveyLayer = static_cast<QgsVectorLayer *>( layer );

  QVERIFY( surveyLayer && surveyLayer->isValid() );

  const QgsFeature feat( surveyLayer->fields() );
  const FeatureLayerPair pair( feat, surveyLayer );

  AttributeController controller;
  controller.setFeatureLayerPair( pair );
  const TabItem *tab = controller.tabItem( 0 );
  const QVector<QUuid> items = tab->formItems();
  QCOMPARE( items.size(), 9 );

  // These are the field values set by QML code after attaching a photo
  controller.setFormValue( items.at( 2 ), QStringLiteral( "mynotes " ) );
  controller.setFormValue( items.at( 3 ), QStringLiteral( "image1.jpg" ) );
  controller.setFormValue( items.at( 4 ), QStringLiteral( "photo.jpg" ) );

  controller.save();
  // verify sketches for image1.jpg
  QVERIFY( !QFile::exists( QStringLiteral( "%1/%2/image1.jpg" ).arg( QDir::tempPath(), projectName ) ) );
  QVERIFY( !QFile::exists( QStringLiteral( "%1/image1.jpg" ).arg( projectDir ) ) );
  QVERIFY( QFile::exists( QStringLiteral( "%1/image_mynotes.jpg" ).arg( projectDir ) ) );
  QVERIFY( !TestUtils::testExifPositionMetadataExists( QStringLiteral( "%1/image_mynotes.jpg" ).arg( projectDir ) ) );
  // verify sketches for image3.jpg
  QVERIFY( !QFile::exists( QStringLiteral( "%1/%2/photo.jpg" ).arg( QDir::tempPath(), projectName ) ) );
  QVERIFY( !QFile::exists( QStringLiteral( "%1/photo.jpg" ).arg( projectDir ) ) );
  QVERIFY( QFile::exists( QStringLiteral( "%1/photos/Survey.jpg" ).arg( projectDir ) ) );
  QVERIFY( TestUtils::testExifPositionMetadataExists( QStringLiteral( "%1/photos/Survey.jpg" ).arg( projectDir ) ) );
}
