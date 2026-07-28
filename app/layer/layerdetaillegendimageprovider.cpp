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

LayerDetailLegendImageProvider::LayerDetailLegendImageProvider()
  : QQuickImageProvider( QQuickImageProvider::Image )
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
  const qreal ldpi = screen->logicalDotsPerInch();
  const qreal dpr = screen->devicePixelRatio();

  const qreal dpmm = ldpi * dpr / 25.4;

  // 60 is fallback size, not sure how it came to be
  const int width = requestedSize.isEmpty() ? static_cast<int>( 60 * dpr ) : static_cast<int>( requestedSize.width() * dpr );
  const int height = static_cast<int>( minimumSize.height() * dpmm );

  QImage legend = QImage( width, height, QImage::Format_ARGB32_Premultiplied );

  {
    QPainter painter( &legend );
    painter.setRenderHint( QPainter::Antialiasing );

    QgsRenderContext context = QgsRenderContext::fromQPainter( &painter );

    painter.scale( dpmm, dpmm );

    legend.fill( Qt::transparent );

    renderer->drawLegend( context );
  }

  // Tag with DPR only after painting so QPainter does not auto-scale the
  // coordinate system by dpr (which would otherwise double-apply the factor).
  legend.setDevicePixelRatio( dpr );

  size->setWidth( static_cast<int>( width / dpr ) );
  size->setHeight( static_cast<int>( height / dpr ) );

  return legend;
}

void LayerDetailLegendImageProvider::setData( LayerDetailData *layerDetailData )
{
  mLayerDetailData = layerDetailData;
}

void LayerDetailLegendImageProvider::reset()
{
  mLayerDetailData = nullptr;
}
