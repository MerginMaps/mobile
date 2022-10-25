/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreemodelpixmapprovider.h"

#include <QIcon>
#include <QPixmap>
#include <QDebug>

LayerTreeModelPixmapProvider::LayerTreeModelPixmapProvider()
  : QQuickImageProvider( QQuickImageProvider::Pixmap )
{
}

LayerTreeModelPixmapProvider::~LayerTreeModelPixmapProvider()
{
  mModel = nullptr;
}

QPixmap LayerTreeModelPixmapProvider::requestPixmap( const QString &id, QSize *size, const QSize &requestedSize )
{
  if ( !mModel )
  {
    return QPixmap();
  }

  QgsLayerTreeNode *node = mModel->deserializeNode( id );

  if ( !node )
  {
    qDebug() << "Invalid deserialized node!" << id;
    return QPixmap();
  }

  QModelIndex modelIndex = mModel->node2index( node );

  if ( !modelIndex.isValid() )
  {
    qDebug() << "Invalid deserialized index!" << id;
    return QPixmap();
  }

  QIcon icon = mModel->data( modelIndex, Qt::DecorationRole ).value<QIcon>();

  if ( icon.isNull() )
  {
    qDebug() << "Empty icon in image provider!" << id;
    return QPixmap();
  }

  QSize iconSize( requestedSize );
  if ( iconSize.isEmpty() )
  {
    // fallback size
    iconSize = QSize( 30, 30 );
  }

  QPixmap pixmap = icon.pixmap( iconSize );

  size->setHeight( pixmap.height() );
  size->setWidth( pixmap.width() );

  return pixmap;
}

void LayerTreeModelPixmapProvider::setModel( LayerTreeModel *model )
{
  if ( mModel != model )
  {
    mModel = model;
  }
}

void LayerTreeModelPixmapProvider::reset()
{
  mModel = nullptr;
}
