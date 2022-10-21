/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERTREEMODELPIXMAPPROVIDER_H
#define LAYERTREEMODELPIXMAPPROVIDER_H

#include <QQuickImageProvider>
#include <QObject>
#include <qglobal.h>

#include "layer/layertreemodel.h"

class LayerTreeModelPixmapProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

  public:
    explicit LayerTreeModelPixmapProvider( QObject *parent = nullptr );
    virtual ~LayerTreeModelPixmapProvider();

    QPixmap requestPixmap( const QString &id, QSize *, const QSize & ) override;

    Q_INVOKABLE void setModel( LayerTreeModel *model );
    Q_INVOKABLE void reset();

  private:
    qreal mDpr = 1;
    LayerTreeModel *mModel = nullptr; // not owned
};

#endif // LAYERTREEMODELPIXMAPPROVIDER_H
