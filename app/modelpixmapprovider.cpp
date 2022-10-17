/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "modelpixmapprovider.h"

#include <QIcon>
#include <QPixmap>
#include <QDebug>

ModelPixmapProvider::ModelPixmapProvider( QObject *parent )
  : QObject( parent ), QQuickImageProvider( QQuickImageProvider::Pixmap )
{
}

ModelPixmapProvider::~ModelPixmapProvider()
{
  mModel = nullptr;
}

QPixmap ModelPixmapProvider::requestPixmap( const QString &id, QSize *size, const QSize &requestedSize )
{
  qDebug() << "Asked for image in image provider" << id << *size << requestedSize;

  if ( !mModel )
  {
    qDebug() << "Empty model in image provider!" << id;
    return QPixmap();
  }

  // parse id to get row and column to the model.
  // it should look like <row>/<col>

  QStringList indexes = id.split( '/' );
  if ( indexes.size() != 2 )
  {
    qDebug() << "Invalid index in image provider!" << id;
    return QPixmap();
  }

  bool ok = false;
  int row = indexes[0].toInt( &ok );

  if ( !ok )
  {
    qDebug() << "Invalid row id in image provider!" << id;
    return QPixmap();
  }

  int col = indexes[1].toInt( &ok );

  if ( !ok )
  {
    qDebug() << "Invalid col id in image provider!" << id;
    return QPixmap();
  }


  QModelIndex modelIndex = mModel->index( row, col );
  QIcon icon = mModel->data( modelIndex, Qt::DecorationRole ).value<QIcon>();

  if ( icon.isNull() || icon.availableSizes().isEmpty() )
  {
    qDebug() << "Empty icon in image provider!" << id;
    return QPixmap();
  }

  QSize iconSize = icon.availableSizes().at( 0 );
  QPixmap pixmap = icon.pixmap( iconSize );

  size->setHeight( pixmap.height() );
  size->setWidth( pixmap.width() );

  return pixmap;
}

void ModelPixmapProvider::setModel( QAbstractItemModel *model )
{
  if ( mModel != model )
  {
    mModel = model;
  }
}
