/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testlayertree.h"

#include <QSignalSpy>

#include "qgslayertree.h"

#include "test/testutils.h"

#include "layer/layerdetaildata.h"
#include "layer/layerdetaillegendimageprovider.h"
#include "layer/layertreemodel.h"
#include "layer/layertreesortfiltermodel.h"
#include "layer/layertreemodelpixmapprovider.h"
#include "layer/layertreeflatmodel.h"
#include "layer/layertreeflatsortfiltermodel.h"
#include "layer/layertreeflatmodelpixmapprovider.h"

void TestLayerTree::init()
{

}

void TestLayerTree::cleanup()
{

}

void TestLayerTree::testLayerDetailData()
{
  LayerDetailData *data = new LayerDetailData();

  // load test project
  QString projectDir = TestUtils::testDataDir() + "/planes/quickapp_project.qgs";

  QgsProject *project = new QgsProject();

  QVERIFY( project->read( projectDir ) );

  QgsLayerTree *root = project->layerTreeRoot();
  QList<QgsLayerTreeNode *> nodes = root->children();

  QgsLayerTreeNode *testedNode = nullptr;

  for ( const auto node : nodes )
  {
    if ( node->name() == QStringLiteral( "FlySector" ) )
    {
      testedNode = node;
      break;
    }
  }

  QVERIFY( nodes.size() > 1 );

  QSignalSpy spy1( data, &LayerDetailData::nameChanged );
  QSignalSpy spy2( data, &LayerDetailData::isVectorLayerChanged );
  QSignalSpy spy3( data, &LayerDetailData::isVisibleChanged );
  QSignalSpy spy4( data, &LayerDetailData::vectorLayerChanged );
  QSignalSpy spy5( data, &LayerDetailData::isSpatialChanged );

  data->setLayerTreeNode( testedNode );

  QCOMPARE( spy1.count(), 1 );
  QCOMPARE( spy2.count(), 1 );
  QCOMPARE( spy3.count(), 1 );
  QCOMPARE( spy4.count(), 1 );
  QCOMPARE( spy5.count(), 1 );

  QCOMPARE( data->name(), QStringLiteral( "FlySector" ) );
  QCOMPARE( data->isSpatial(), true );
  QCOMPARE( data->isVisible(), true );
  QCOMPARE( data->isVectorLayer(), true );
  QCOMPARE( data->vectorLayer(), QgsLayerTree::toLayer( testedNode )->layer() );

  delete project;
  delete data;
}

void TestLayerTree::testLayerTreeModel()
{
  // load test project
  QString projectDir = TestUtils::testDataDir() + "/planes/quickapp_project.qgs";
  QgsProject *project = new QgsProject();
  QVERIFY( project->read( projectDir ) );

  LayerTreeModel *model = new LayerTreeModel();

  QSignalSpy spy1( model, &LayerTreeModel::modelInitialized );

  model->setQgsProject( project );

  QCOMPARE( spy1.count(), 1 );

  QCOMPARE( model->rowCount(), 8 ); // including private layer

  // let's check the sort model
  LayerTreeSortFilterModel *sortModel = new LayerTreeSortFilterModel();

  sortModel->setLayerTreeModel( model );

  QCOMPARE( sortModel->rowCount(), 7 ); // excluding the private layer

  // in QML sourceModel is set immediately, so we need to emit
  // the signal here (in order to sort) manually as we assign the source model later
  project->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "SortLayersMethod/Method" ), LayerTreeSortFilterModel::SortMethodEnum::PreserveQgisOrder );
  emit model->modelInitialized();

  QVariant firstNode = sortModel->data( sortModel->index( 0, 0 ), Qt::DisplayRole );
  QCOMPARE( firstNode.toString(), QStringLiteral( "FlySector" ) );

  // the signal here (in order to sort) manually as we assign the source model later
  project->writeEntry( QStringLiteral( "Mergin" ), QStringLiteral( "SortLayersMethod/Method" ), LayerTreeSortFilterModel::SortMethodEnum::Alphabetical );
  emit model->modelInitialized();

  firstNode = sortModel->data( sortModel->index( 0, 0 ), Qt::DisplayRole );
  QCOMPARE( firstNode.toString(), QStringLiteral( "airport-towers" ) );

  delete sortModel;
  delete model;
}

void TestLayerTree::testLayerTreeFlatModel()
{
  // load test project
  QString projectDir = TestUtils::testDataDir() + "/planes/quickapp_project.qgs";
  QgsProject *project = new QgsProject();
  QVERIFY( project->read( projectDir ) );

  LayerTreeFlatModel *model = new LayerTreeFlatModel();

  QSignalSpy spy1( model, &LayerTreeFlatModel::modelInitialized );

  model->setQgsProject( project );

  QCOMPARE( spy1.count(), 1 );

  QCOMPARE( model->rowCount(), 10 ); // including private layer and child layers

  // let's check the sort model
  LayerTreeFlatSortFilterModel *sortModel = new LayerTreeFlatSortFilterModel();

  sortModel->setLayerTreeFlatModel( model );

  QCOMPARE( sortModel->rowCount(), 9 ); // excluding the private layer

  // in QML sourceModel is set immediately, so we need to emit
  // the signal here (in order to sort) manually as we assign the source model later
  emit model->modelInitialized();

  QVariant firstNode = sortModel->data( sortModel->index( 0, 0 ), Qt::DisplayRole );
  QCOMPARE( firstNode.toString(), QStringLiteral( "airport-towers" ) );

  sortModel->setSearchExpression( QStringLiteral( "lan" ) );

  firstNode = sortModel->data( sortModel->index( 0, 0 ), Qt::DisplayRole );
  QCOMPARE( firstNode.toString(), QStringLiteral( "Land" ) );

  delete sortModel;
  delete model;
}

void TestLayerTree::testLayerTreeImageProviders()
{
  // load test project
  QString projectDir = TestUtils::testDataDir() + "/planes/quickapp_project.qgs";

  QgsProject *project = new QgsProject();

  QVERIFY( project->read( projectDir ) );

  // Render legend

  LayerDetailLegendImageProvider *legendImageProvider = new LayerDetailLegendImageProvider();

  QgsLayerTree *root = project->layerTreeRoot();
  QList<QgsLayerTreeNode *> nodes = root->children();

  QgsLayerTreeNode *testedNode = nullptr;

  for ( const auto node : nodes )
  {
    if ( node->name() == QStringLiteral( "FlySector" ) )
    {
      testedNode = node;
      break;
    }
  }

  LayerDetailData *data = new LayerDetailData();
  data->setLayerTreeNode( testedNode );

  legendImageProvider->setData( data );

  QSize testOutSize;
  QVERIFY( !legendImageProvider->requestImage( "0", &testOutSize, QSize( 100, 100 ) ).isNull() );
  QVERIFY( !testOutSize.isNull() );

  // Layer tree icons in tree view
  LayerTreeModelPixmapProvider *treePixmapProvider = new LayerTreeModelPixmapProvider();

  LayerTreeModel *treeModel = new LayerTreeModel();
  treeModel->setQgsProject( project );

  treePixmapProvider->setModel( treeModel );

  testOutSize = QSize();
  QPixmap out = treePixmapProvider->requestPixmap( "0", &testOutSize, QSize( 20, 20 ) );
  QVERIFY( !out.isNull() );
  QVERIFY( !testOutSize.isNull() );

  // test (de)serialisation of nodes
  testedNode = nullptr;
  QList<QgsLayerTreeNode *> queue = root->children();
  QgsLayerTreeNode *node = nullptr;

  while ( !queue.isEmpty() )
  {
    node = queue.takeFirst();

    if ( node->name() == QStringLiteral( "inner" ) )
    {
      testedNode = node;
      break;
    }

    queue.append( node->children() );
  }

  QVERIFY( testedNode );

  QModelIndex testedIndex = treeModel->node2index( testedNode );
  QString serializedNode = treeModel->data( testedIndex, LayerTreeModel::SerializedNode ).toString();

  QCOMPARE( serializedNode, QStringLiteral( "7-0-0" ) );

  QgsLayerTreeNode *deserializedNode = treeModel->deserializeNode( serializedNode );
  QCOMPARE( deserializedNode, testedNode );

  // Layer tree icons in search (flat list)
  LayerTreeFlatModelPixmapProvider *flatPixmapProvider = new LayerTreeFlatModelPixmapProvider();
  LayerTreeFlatModel *flatModel = new LayerTreeFlatModel();
  flatModel->setQgsProject( project );

  flatPixmapProvider->setModel( flatModel );

  testOutSize = QSize();
  out = flatPixmapProvider->requestPixmap( "0", &testOutSize, QSize( 20, 20 ) );
  QVERIFY( !out.isNull() );
  QVERIFY( !testOutSize.isNull() );

  // ---
  delete treeModel;
  delete flatModel;
  delete data;
  delete treePixmapProvider;
  delete flatPixmapProvider;
  delete legendImageProvider;
}

