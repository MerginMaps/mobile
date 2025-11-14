/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "testmultieditmanager.h"

void TestMultiEditManager::init()
{
  mLayer = std::make_unique<QgsVectorLayer>( QStringLiteral( "point?field=id:integer" ), QStringLiteral( "mem" ), QStringLiteral( "memory" ) );
  QVERIFY( mLayer && mLayer->isValid() );

  QgsFeatureList addFeats;
  for ( int i = 0; i < 6; ++i )
  {
    QgsFeature feature( mLayer->fields() );
    feature.setGeometry( QgsGeometry::fromPointXY( QgsPointXY( i, i ) ) );
    feature.setAttribute( QStringLiteral( "id" ), i + 1 );
    addFeats << feature;
  }
  QVERIFY( mLayer->dataProvider()->addFeatures( addFeats ) );
  QCOMPARE( static_cast<int>( mLayer->featureCount() ), 6 );

  mFeatures.clear();
  QgsFeature feature;
  auto it = mLayer->getFeatures();
  while ( it.nextFeature( feature ) )
  {
    mFeatures << feature;
  }

  //new multiEdit per each test
  mManager = std::make_unique<MultiEditManager>();
}

void TestMultiEditManager::cleanup()
{
  mManager.reset();
  mLayer.reset();
  mFeatures.clear();
}

void TestMultiEditManager::testInitializeSelection()
{
  mManager->initialize( FeatureLayerPair( mFeatures.at( 0 ), mLayer.get() ) );
  mManager->deleteSelectedFeatures();
  QCOMPARE( static_cast<int>( mLayer->featureCount() ), 5 );
}

void TestMultiEditManager::testToggleSelectSameLayer()
{
  mManager->initialize( FeatureLayerPair( mFeatures.at( 0 ), mLayer.get() ) ); // added f1
  mManager->toggleSelect( FeatureLayerPair( mFeatures.at( 1 ), mLayer.get() ) ); // added f2 and,
  mManager->toggleSelect( FeatureLayerPair( mFeatures.at( 1 ), mLayer.get() ) ); // removed f2

  mManager->deleteSelectedFeatures();
  QCOMPARE( static_cast<int>( mLayer->featureCount() ), 5 );
}

void TestMultiEditManager::testDeleteSelectedFeatures()
{
  mManager->initialize( FeatureLayerPair( mFeatures.at( 0 ), mLayer.get() ) );
  mManager->toggleSelect( FeatureLayerPair( mFeatures.at( 1 ), mLayer.get() ) );
  mManager->toggleSelect( FeatureLayerPair( mFeatures.at( 2 ), mLayer.get() ) );

  mManager->deleteSelectedFeatures();

  // three removed out-of 6
  QCOMPARE( static_cast<int>( mLayer->featureCount() ), 3 );

  FeatureLayerPair editable = mManager->editableFeature();
  QVERIFY( !editable.isValid() );
}
