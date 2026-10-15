/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testfeaturedraftcontroller.h"

#include <QtTest/QtTest>

#include "qgsproject.h"
#include "qgsvectorlayer.h"

#include "featuredraftcontroller.h"
#include "featuredraftstorage.h"
#include "featuredraft.h"

namespace
{
  // Mergin project ids contain a slash
  const QString TEST_PROJECT_ID = QStringLiteral( "testNamespace/testDraftProject" );
  const QString OTHER_PROJECT_ID = QStringLiteral( "testNamespace/otherDraftProject" );

  QgsVectorLayer *addTestLayer()
  {
    QgsVectorLayer *layer = new QgsVectorLayer(
      QStringLiteral( "Point?field=fldtxt:string&field=fldint:integer" ),
      QStringLiteral( "layer" ),
      QStringLiteral( "memory" )
    );
    QgsProject::instance()->addMapLayer( layer );
    return layer;
  }

  QgsFeatureId addTestFeature( QgsVectorLayer *layer, const QString &txt, int number )
  {
    QgsFeature f( layer->fields() );
    f.setAttribute( QStringLiteral( "fldtxt" ), txt );
    f.setAttribute( QStringLiteral( "fldint" ), number );

    QgsFeatureList features { f };
    layer->dataProvider()->addFeatures( features );
    return features.first().id();
  }
}

void TestFeatureDraftController::init()
{
  FeatureDraftStorage::clearDraft( TEST_PROJECT_ID );
  FeatureDraftStorage::clearDraft( OTHER_PROJECT_ID );
}

void TestFeatureDraftController::cleanup()
{
  FeatureDraftStorage::clearDraft( TEST_PROJECT_ID );
  FeatureDraftStorage::clearDraft( OTHER_PROJECT_ID );
  QgsProject::instance()->clear();
}

void TestFeatureDraftController::validNewFeatureGeometryDraftDetected()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::GeometryCapture;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.geometry = QgsGeometry::fromWkt( QStringLiteral( "Point (1 2)" ) );
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );

  QVERIFY( controller.hasDraft() );
  QCOMPARE( controller.draftStage(), FeatureDraftController::GeometryCapture );
  QVERIFY( !controller.draftIsExistingFeature() );
  QCOMPARE( controller.draftLayer(), layer );
  QVERIFY( controller.draftFeatureTitle().isEmpty() );
}

void TestFeatureDraftController::validExistingFeatureAttributeDraftDetected()
{
  QgsVectorLayer *layer = addTestLayer();
  const QgsFeatureId featureId = addTestFeature( layer, QStringLiteral( "one" ), 1 );

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::AttributeForm;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.featureId = featureId;
  draft.attributes.append( { QStringLiteral( "fldtxt" ), QStringLiteral( "string" ), QStringLiteral( "two" ) } );
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );

  QVERIFY( controller.hasDraft() );
  QCOMPARE( controller.draftStage(), FeatureDraftController::AttributeForm );
  QVERIFY( controller.draftIsExistingFeature() );
}

void TestFeatureDraftController::expiredDraftIgnoredAndCleared()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::GeometryCapture;
  draft.timestamp = QDateTime::currentDateTimeUtc().addDays( -11 ); // older than the 10-day guard
  draft.geometry = QgsGeometry::fromWkt( QStringLiteral( "Point (1 2)" ) );
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );

  QVERIFY( !controller.hasDraft() );
  QVERIFY( FeatureDraftStorage::loadDraft( TEST_PROJECT_ID ).isEmpty() );
}

void TestFeatureDraftController::draftWithRemovedFieldIgnored()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::AttributeForm;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.geometry = QgsGeometry::fromWkt( QStringLiteral( "Point (1 2)" ) );
  draft.attributes.append( { QStringLiteral( "doesNotExist" ), QStringLiteral( "string" ), QStringLiteral( "value" ) } );
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );

  QVERIFY( !controller.hasDraft() );
}

void TestFeatureDraftController::draftWithChangedFieldTypeIgnored()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::AttributeForm;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.geometry = QgsGeometry::fromWkt( QStringLiteral( "Point (1 2)" ) );
  // fldint is actually an "integer" field - a mismatched recorded type means the schema changed since
  draft.attributes.append( { QStringLiteral( "fldint" ), QStringLiteral( "string" ), QStringLiteral( "1" ) } );
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );

  QVERIFY( !controller.hasDraft() );
}

void TestFeatureDraftController::existingFeatureDraftRequiresLiveFeature()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::AttributeForm;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.featureId = 999999; // no such feature was ever added to the layer
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );

  QVERIFY( !controller.hasDraft() );
}

void TestFeatureDraftController::loadDraftAppliesGeometryAndAttributesButKeepsStorage()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::AttributeForm;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.geometry = QgsGeometry::fromWkt( QStringLiteral( "Point (1 2)" ) );
  draft.attributes.append( { QStringLiteral( "fldtxt" ), QStringLiteral( "string" ), QStringLiteral( "resumed" ) } );
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );
  QVERIFY( controller.hasDraft() );

  const FeatureLayerPair pair = controller.loadDraft();

  QCOMPARE( pair.layer(), layer );
  QCOMPARE( pair.feature().attribute( QStringLiteral( "fldtxt" ) ).toString(), QStringLiteral( "resumed" ) );
  QVERIFY( pair.feature().geometry().asWkt().contains( QStringLiteral( "1 2" ) ) );

  // pending state is cleared, but the draft itself stays in storage until save/rollback clears it
  QVERIFY( !controller.hasDraft() );
  QVERIFY( !FeatureDraftStorage::loadDraft( TEST_PROJECT_ID ).isEmpty() );
}

void TestFeatureDraftController::discardDraftClearsStorage()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::GeometryCapture;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.geometry = QgsGeometry::fromWkt( QStringLiteral( "Point (1 2)" ) );
  FeatureDraftStorage::saveDraft( TEST_PROJECT_ID, draft );

  FeatureDraftController controller;
  controller.checkForDraft( TEST_PROJECT_ID );
  QVERIFY( controller.hasDraft() );

  controller.discardDraft();

  QVERIFY( !controller.hasDraft() );
  QVERIFY( FeatureDraftStorage::loadDraft( TEST_PROJECT_ID ).isEmpty() );
}

void TestFeatureDraftController::draftsAreStoredPerProject()
{
  QgsVectorLayer *layer = addTestLayer();

  FeatureDraft draft;
  draft.layerId = layer->id();
  draft.stage = FeatureDraft::GeometryCapture;
  draft.timestamp = QDateTime::currentDateTimeUtc();
  draft.geometry = QgsGeometry::fromWkt( QStringLiteral( "Point (1 2)" ) );
  FeatureDraftStorage::saveDraft( OTHER_PROJECT_ID, draft );

  FeatureDraftController controller;

  // a draft of a different project is not offered
  controller.checkForDraft( TEST_PROJECT_ID );
  QVERIFY( !controller.hasDraft() );

  // without an active project, nothing is offered and nothing is written
  controller.checkForDraft( QString() );
  QVERIFY( !controller.hasDraft() );
  controller.saveDraft( draft );
  QVERIFY( FeatureDraftStorage::loadDraft( QString() ).isEmpty() );

  // saveDraft() and clearDraft() use the project from checkForDraft()
  controller.checkForDraft( TEST_PROJECT_ID );
  controller.saveDraft( draft );
  QVERIFY( !FeatureDraftStorage::loadDraft( TEST_PROJECT_ID ).isEmpty() );

  controller.clearDraft();
  QVERIFY( FeatureDraftStorage::loadDraft( TEST_PROJECT_ID ).isEmpty() );
  QVERIFY( !FeatureDraftStorage::loadDraft( OTHER_PROJECT_ID ).isEmpty() );
}
