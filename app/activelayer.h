/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef ACTIVELAYER_H
#define ACTIVELAYER_H

#include <QObject>
#include "qgsmaplayer.h"
#include "qgsvectorlayer.h"

class ActiveLayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString layerId READ layerId )
    Q_PROPERTY( QString layerName READ layerName )
    Q_PROPERTY( QgsMapLayer *layer WRITE setActiveLayer READ layer NOTIFY activeLayerChanged )
    Q_PROPERTY( QgsVectorLayer *vectorLayer READ vectorLayer )

  public:
    ActiveLayer();

    QString layerId() const;
    QString layerName() const;
    QgsMapLayer *layer() const;
    QgsVectorLayer *vectorLayer() const;

    /**
     * Updates active layer.
     * \param layerName Represents layer to be set as active.
     */
    void setActiveLayer( QgsMapLayer *layer );

  public slots:
    void activeMapThemeChanged();

  signals:
    void activeLayerChanged( const QString &layerName );

  private:
    QgsMapLayer *mLayer;
};

#endif // ACTIVELAYER_H
