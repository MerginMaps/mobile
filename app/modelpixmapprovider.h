/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODELPIXMAPPROVIDER_H
#define MODELPIXMAPPROVIDER_H

#include "QtCore/qabstractitemmodel.h"
#include <QQuickImageProvider>
#include <QObject>
#include <qglobal.h>

class ModelPixmapProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

  public:
    explicit ModelPixmapProvider( QObject *parent = nullptr );
    virtual ~ModelPixmapProvider();

    QPixmap requestPixmap( const QString &id, QSize *, const QSize & ) override;

    Q_INVOKABLE void setModel( QAbstractItemModel *model );

  private:
    QAbstractItemModel *mModel = nullptr; // not owned
};

#endif // MODELPIXMAPPROVIDER_H
