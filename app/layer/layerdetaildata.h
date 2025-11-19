/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERDETAILDATA_H
#define LAYERDETAILDATA_H

#include <QObject>
#include <qglobal.h>

#include "qgslayertreenode.h"
#include "qgslegendrenderer.h"
#include "qgsvectorlayer.h"

class LayerDetailData : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QgsLayerTreeNode *layerTreeNode READ layerTreeNode WRITE setLayerTreeNode NOTIFY layerTreeNodeChanged )

    // out properties
    Q_PROPERTY( QString name READ name NOTIFY nameChanged )
    Q_PROPERTY( QString layerId READ layerId NOTIFY layerIdChanged )
    Q_PROPERTY( bool isVisible READ isVisible NOTIFY isVisibleChanged )
    Q_PROPERTY( bool isSpatial READ isSpatial NOTIFY isSpatialChanged )
    Q_PROPERTY( bool isVectorLayer READ isVectorLayer NOTIFY isVectorLayerChanged )
    Q_PROPERTY( QgsVectorLayer *vectorLayer READ vectorLayer NOTIFY vectorLayerChanged )
    Q_PROPERTY( QgsMapLayer *mapLayer READ mapLayer NOTIFY mapLayerChanged )

  public:
    explicit LayerDetailData( QObject *parent = nullptr );
    virtual ~LayerDetailData();

    QgsLayerTreeNode *layerTreeNode() const;
    void setLayerTreeNode( QgsLayerTreeNode *newLayerTreeNode );

    bool isVisible() const;
    bool isSpatial() const;
    bool isVectorLayer() const;
    const QString &name() const;
    const QString &layerId() const;
    QgsVectorLayer *vectorLayer() const;
    QgsMapLayer *mapLayer() const;

    QgsLegendRenderer *legendRenderer() const;

    Q_INVOKABLE void reset();

  signals:
    void isSpatialChanged( bool showLegned );
    void isVisibleChanged( bool isVisible );
    void isVectorLayerChanged( bool isVectorLayer );
    void nameChanged( const QString &name );
    void layerIdChanged( const QString &layerId );
    void vectorLayerChanged( QgsVectorLayer *vectorLayer );
    void layerTreeNodeChanged( QgsLayerTreeNode *layerTreeNode );
    void mapLayerChanged( QgsMapLayer *mapLayer );

  private:
    QgsLayerTreeNode *mLayerTreeNode = nullptr; // not owned
    std::unique_ptr<QgsLegendRenderer> mLegendRenderer; // owned

    QString mName;
    QString mLayerId;
    bool mIsSpatial = false;
    bool mIsVisible = false;
    bool mIsVectorLayer = false;
};

#endif // LAYERDETAILDATA_H
