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
#include <QGuiApplication>
#include <QScreen>

#include "QDebug"

LayerDetailLegendImageProvider::LayerDetailLegendImageProvider( QObject *parent )
  : QObject( parent ), QQuickImageProvider( QQuickImageProvider::Image )
{
}

LayerDetailLegendImageProvider::~LayerDetailLegendImageProvider() = default;

QImage LayerDetailLegendImageProvider::requestImage( const QString &, QSize *size, const QSize &requestedSize )
{
  if ( !mLayerDetailData )
  {
    qDebug() << "Empty layer data ptr in image provider!";
    return QImage();
  }

  QgsLegendRenderer *renderer = mLayerDetailData->legendRenderer();

  if ( !renderer )
  {
    qDebug() << "Renderer is not initialized in image provider, returning empty image";
    return QImage();
  }

  QSizeF minimumSize = renderer->minimumSize();

  // We want to take requested width from QML and minimum height from legend renderer.
  // However, minimum height is not calculated with dpr in mind, so we need to multiply
  // it by dpr.

  QScreen *screen = QGuiApplication::screens().at( 0 );
  qreal pdpi = screen->physicalDotsPerInch() * screen->devicePixelRatio();
  qreal dpm = pdpi / 25.4;

  QSize desiredSize( requestedSize );
  if ( desiredSize.isEmpty() )
  {
    // fallback size
    desiredSize = QSize( 60 * mDpr, 60 * mDpr );
  }

  QImage legend = QImage( desiredSize.width(), minimumSize.height() * dpm, QImage::Format_ARGB32_Premultiplied );

  QPainter painter( &legend );
  painter.setRenderHint( QPainter::Antialiasing );

  QgsRenderContext context = QgsRenderContext::fromQPainter( &painter );

  painter.scale( dpm, dpm );

  legend.fill( Qt::transparent );

  renderer->drawLegend( context );

  size->setHeight( desiredSize.height() );
  size->setWidth( desiredSize.width() );

  return legend;
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
