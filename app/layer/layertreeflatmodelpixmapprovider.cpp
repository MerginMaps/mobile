/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layertreeflatmodelpixmapprovider.h"

#include <QIcon>
#include <QPixmap>
#include <QDebug>

LayerTreeFlatModelPixmapProvider::LayerTreeFlatModelPixmapProvider( QObject *parent )
  : QObject( parent ), QQuickImageProvider( QQuickImageProvider::Pixmap )
{
}

LayerTreeFlatModelPixmapProvider::~LayerTreeFlatModelPixmapProvider()
{
  mModel = nullptr;
}

QPixmap LayerTreeFlatModelPixmapProvider::requestPixmap( const QString &id, QSize *size, const QSize &requestedSize )
{
  if ( !mModel )
  {
    qDebug() << "Empty model in image provider!" << id;
    return QPixmap();
  }

  bool ok = false;
  int row = id.toInt( &ok );

  if ( !ok )
  {
    qDebug() << "Invalid row id in image provider!" << id;
    return QPixmap();
  }

  QModelIndex modelIndex = mModel->index( row, 0 );
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

void LayerTreeFlatModelPixmapProvider::setModel( LayerTreeFlatModel *model )
{
  if ( mModel != model )
  {
    mModel = model;
  }
}

void LayerTreeFlatModelPixmapProvider::reset()
{
  mModel = nullptr;
}
