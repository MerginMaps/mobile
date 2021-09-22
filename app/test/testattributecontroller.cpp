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
#include <QDesktopWidget>
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
