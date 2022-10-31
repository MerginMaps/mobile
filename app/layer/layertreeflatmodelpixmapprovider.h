/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERTREEFLATMODELPIXMAPPROVIDER_H
#define LAYERTREEFLATMODELPIXMAPPROVIDER_H

#include "layer/layertreeflatmodel.h"

#include <QQuickImageProvider>
#include <QObject>
#include <qglobal.h>

class LayerTreeFlatModelPixmapProvider : public QQuickImageProvider
{
    Q_OBJECT

  public:
    explicit LayerTreeFlatModelPixmapProvider();
    virtual ~LayerTreeFlatModelPixmapProvider();

    QPixmap requestPixmap( const QString &id, QSize *, const QSize & ) override;

    Q_INVOKABLE void setModel( LayerTreeFlatModel *model );
    Q_INVOKABLE void reset();

  private:
    LayerTreeFlatModel *mModel = nullptr; // not owned
};

#endif // LAYERTREEFLATMODELPIXMAPPROVIDER_H
