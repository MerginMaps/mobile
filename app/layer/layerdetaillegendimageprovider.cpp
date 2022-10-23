/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "layerdetaillegendimageprovider.h"
#include "qgslegendrenderer.h"
#include "qgsrendercontext.h"

#include <QPainter>

#include "QDebug"

LayerDetailLegendImageProvider::LayerDetailLegendImageProvider( QObject *parent )
  : QObject( parent ), QQuickImageProvider( QQuickImageProvider::Image )
{
}

LayerDetailLegendImageProvider::~LayerDetailLegendImageProvider() = default;

QImage LayerDetailLegendImageProvider::requestImage( const QString &id, QSize *size, const QSize &requestedSize )
{
  qDebug() << "Asked for legend with id" << id << ", requested size:" << requestedSize;

  if ( !mLayerDetailData )
  {
    qDebug() << "Empty layer data ptr in image provider!";
    return QImage();
  }

  QgsLegendRenderer *renderer = mLayerDetailData->legendRenderer();

  if ( !renderer )
  {
    qDebug() << "Rendere is not initialized in image provider, returning empty image";
    return QImage();
  }

  QSizeF minimumSize = renderer->minimumSize();

  qDebug() << "Minimum size for this legend:" << minimumSize;

  // We want to take requested width from QML and minimum height from legend renderer.
  // However, minimum height is not calculated with dpr in mind, so we need to multiply
  // it by dpr.

  qreal ppi = 96 * mDpr;
  qreal minWidthMapUnits = minimumSize.width();
  qreal minWidthPixels = minWidthMapUnits * ppi; // TODO: this is wrong conversion - from map units to pixels

  QSize desiredSize( requestedSize );
  if ( desiredSize.isEmpty() )
  {
    // fallback size
    desiredSize = QSize( 60 * mDpr, 60 * mDpr );
  }

  qreal scale = desiredSize.width() / minWidthPixels;

  QImage legend = QImage( desiredSize.width(), minimumSize.height() * ppi * scale, QImage::Format_ARGB32_Premultiplied );

  QPainter painter( &legend );
  painter.setRenderHint( QPainter::Antialiasing );

  QgsRenderContext context = QgsRenderContext::fromQPainter( &painter );
  qDebug() << "min width converted:" << context.convertFromMapUnits( minimumSize.width(), QgsUnitTypes::RenderPixels );


//  qreal scale = legend.width() / ( minimumSize.width() * mDpr );

  qDebug() << "Scale:" << scale << "ppi" << ppi << "ratio" << scale / ppi << "other way" << ppi / scale;

  painter.scale( 6, 6 );

  legend.fill( Qt::white );

  renderer->drawLegend( context );

  size->setHeight( legend.height() );
  size->setWidth( legend.width() );

  return legend;

  // -----

  //ls=QgsLegendSettings()
  //tree=QgsLayerTree()
  //lrl=tree.insertLayer(0, iface.activeLayer())
  //ltm=QgsLayerTreeModel(tree)
  //lr=QgsLegendRenderer(ltm, ls)
  //lr.minimumSize()

  //img=QImage(500,500, QImage.Format_ARGB32_Premultiplied)
  //p=QPainter(img)
  //p.setRenderHint(QPainter.Antialiasing)
  //rc=QgsRenderContext.fromQPainter(p)

  //p.scale(96/25.4, 96/25.4)

  //ltl.setCustomProperty("legend/title-style", "hidden")

  //img.fill(Qt.white)
  //lr.drawLegend(rc)

  //img.save("/tmp/legend.png")

  // -----

//  QSize iconSize( requestedSize );
//  if ( iconSize.isEmpty() )
//  {
//    // fallback size
//    iconSize = QSize( 30 * mDpr, 30 * mDpr );
//  }

//  QPixmap pixmap = icon.pixmap( iconSize );

//  size->setHeight( pixmap.height() );
//  size->setWidth( pixmap.width() );

  //  return pixmap;
}

void LayerDetailLegendImageProvider::setDpr( qreal dpr )
{
  mDpr = dpr;
}

void LayerDetailLegendImageProvider::setData( LayerDetailData *layerDetailData )
{
  mLayerDetailData = layerDetailData;
}

void LayerDetailLegendImageProvider::reset()
{
  mLayerDetailData = nullptr;
}
