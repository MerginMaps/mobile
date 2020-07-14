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

#include "layersproxymodel.h"
#include "appsettings.h"

class ActiveLayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY( int index WRITE setActiveLayer READ index NOTIFY activeLayerChanged)
    Q_PROPERTY( QString layerId READ layerId )
    Q_PROPERTY( QString layerName WRITE setActiveLayer )
    Q_PROPERTY( QModelIndex modelIndex READ modelIndex )
    Q_PROPERTY( QgsVectorLayer *layer READ layer )

  public:
    ActiveLayer( LayersProxyModel &model, AppSettings &appSettings );

    QModelIndex modelIndex() const;
    QgsVectorLayer *layer() const;
    QString layerId() const;
    int index() const;

    /**
     * Updates active layer.
     * \param index Represents row number in the layer model.
     */
    void setActiveLayer( int index );

    /**
     * Updates active layer.
     * \param layerName Represents name of layer to be set as active.
     * If no layer is found with such name, first writable layer is set as active.
     */
    void setActiveLayer( QString layerName );

  public slots:
    void activeMapThemeChanged();

  signals:
    void activeLayerChanged( const QString &layerName );

  private:
    LayersProxyModel &mModel;
    AppSettings &mAppSettings;

    int mIndex;
};

#endif // ACTIVELAYER_H
