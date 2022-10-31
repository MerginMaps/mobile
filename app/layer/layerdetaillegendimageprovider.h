/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERDETAILLEGENDIMAGEPROVIDER_H
#define LAYERDETAILLEGENDIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QObject>
#include <qglobal.h>

#include "layerdetaildata.h"

class LayerDetailLegendImageProvider : public QQuickImageProvider
{
    Q_OBJECT
  public:
    explicit LayerDetailLegendImageProvider();
    virtual ~LayerDetailLegendImageProvider();

    QImage requestImage( const QString &id, QSize *size, const QSize &requestedSize ) override;

    Q_INVOKABLE void setData( LayerDetailData *layerDetailData );
    Q_INVOKABLE void reset();

  private:
    LayerDetailData *mLayerDetailData = nullptr; // not owned
};

#endif // LAYERDETAILLEGENDIMAGEPROVIDER_H
