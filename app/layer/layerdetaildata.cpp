/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layerdetaildata.h"

#include "qgslayertree.h"
#include "qgslegendsettings.h"
#include "qgslayertreemodel.h"

LayerDetailData::LayerDetailData( QObject *parent )
  : QObject{parent}
{
}

LayerDetailData::~LayerDetailData() = default;

void LayerDetailData::setLayerTreeNode( QgsLayerTreeNode *newLayerTreeNode )
{
  if ( mLayerTreeNode == newLayerTreeNode )
    return;

  if ( mLayerTreeNode )
  {
    disconnect( mLayerTreeNode );
  }

  mLayerTreeNode = newLayerTreeNode;
  emit layerTreeNodeChanged( mLayerTreeNode );

  mName = QString();
  mIsVisible = false;
  mIsSpatial = false;
  mIsVectorLayer = false;

  mLegendRenderer.reset();

  // works only with nodes of type layer
  if ( !mLayerTreeNode || QgsLayerTree::isGroup( mLayerTreeNode ) )
  {
    emit nameChanged( mName );
    emit isVisibleChanged( mIsVisible );
    emit isSpatialChanged( mIsSpatial );
    emit isVectorLayerChanged( mIsVectorLayer );
    emit vectorLayerChanged( nullptr );
    emit mapLayerChanged( nullptr );
    return;
  }

  QgsLayerTreeLayer *nodeLayer = QgsLayerTree::toLayer( mLayerTreeNode );

  if ( !nodeLayer || !nodeLayer->layer() )
  {
    emit nameChanged( mName );
    emit isVisibleChanged( mIsVisible );
    emit isSpatialChanged( mIsSpatial );
    emit isVectorLayerChanged( mIsVectorLayer );
    emit vectorLayerChanged( nullptr );
    emit mapLayerChanged( nullptr );
    return;
  }

  mIsVisible = nodeLayer->isVisible();
  emit isVisibleChanged( mIsVisible );

  mName = nodeLayer->name();
  emit nameChanged( mName );

  mIsSpatial = nodeLayer->layer()->isSpatial();
  emit isSpatialChanged( mIsSpatial );

  mIsVectorLayer = nodeLayer->layer()->type() == QgsMapLayerType::VectorLayer;
  emit isVectorLayerChanged( mIsVectorLayer );

  emit vectorLayerChanged( vectorLayer() );
  emit mapLayerChanged( nodeLayer->layer() );

  // listen on visibility change
  connect( mLayerTreeNode, &QgsLayerTreeNode::visibilityChanged, this, [this]( QgsLayerTreeNode * node )
  {
    if ( node && node == mLayerTreeNode && node->isVisible() != this->isVisible() )
    {
      mIsVisible = node->isVisible();
      emit isVisibleChanged( mIsVisible );
    }
  } );


  // setup render context for legend
  QgsLegendSettings legendSettings;

  mLayerTreeNode->setCustomProperty( QStringLiteral( "legend/title-style" ), QStringLiteral( "hidden" ) );

  QgsLayerTree *tree = new QgsLayerTree();
  tree->setParent( this );
  tree->insertChildNode( 0, nodeLayer->clone() );

  QgsLayerTreeModel *treeModel = new QgsLayerTreeModel( tree );
  treeModel->setParent( this );

  mLegendRenderer = std::make_unique<QgsLegendRenderer>( treeModel, legendSettings );
}

void LayerDetailData::reset()
{
  setLayerTreeNode( nullptr );
}

QgsLayerTreeNode *LayerDetailData::layerTreeNode() const
{
  return mLayerTreeNode;
}

const QString &LayerDetailData::name() const
{
  return mName;
}

QgsVectorLayer *LayerDetailData::vectorLayer() const
{
  if ( !mLayerTreeNode )
  {
    return nullptr;
  }

  QgsLayerTreeLayer *nodeLayer = QgsLayerTree::toLayer( mLayerTreeNode );
  if ( !nodeLayer )
  {
    return nullptr;
  }

  QgsMapLayer *mapLayer = nodeLayer->layer();
  if ( !mapLayer || mapLayer->type() != QgsMapLayerType::VectorLayer )
  {
    return nullptr;
  }

  return qobject_cast<QgsVectorLayer *>( mapLayer );
}

QgsMapLayer *LayerDetailData::mapLayer() const
{
  if ( !mLayerTreeNode )
  {
    return nullptr;
  }

  QgsLayerTreeLayer *nodeLayer = QgsLayerTree::toLayer( mLayerTreeNode );
  if ( !nodeLayer )
  {
    return nullptr;
  }

  QgsMapLayer *mapLayer = nodeLayer->layer();
  if ( !mapLayer )
  {
    return nullptr;
  }

  return mapLayer;
}

bool LayerDetailData::isVisible() const
{
  return mIsVisible;
}

QgsLegendRenderer *LayerDetailData::legendRenderer() const
{
  return mLegendRenderer.get();
}

bool LayerDetailData::isSpatial() const
{
  return mIsSpatial;
}

bool LayerDetailData::isVectorLayer() const
{
  return mIsVectorLayer;
}
