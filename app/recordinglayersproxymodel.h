/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/


#ifndef RECORDINGLAYERSPROXYMODEL_H
#define RECORDINGLAYERSPROXYMODEL_H

#include <QObject>

#include "inputconfig.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerproxymodel.h"
#include "qgsvectorlayer.h"
#include "inpututils.h"

#include "layersmodel.h"

class RecordingLayersProxyModel : public QgsMapLayerProxyModel
{
    Q_OBJECT

    Q_PROPERTY( LayersModel *model READ model WRITE setModel NOTIFY modelChanged )

  public:
    Q_INVOKABLE explicit RecordingLayersProxyModel( QObject *parent = nullptr );

    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;

    /**
     * @brief layers method return layers from source model filtered with filter function
     */
    QList<QgsMapLayer *> layers() const;

    //! Getters and setters
    LayersModel *model() const;
    void setModel( LayersModel *model );

  signals:
    void countChanged();
    void qgsProjectChanged();
    void modelTypeChanged();
    void modelChanged();

  public slots:
    void refreshData();

  private:
    LayersModel *mModel;
};

#endif // RECORDINGLAYERSPROXYMODEL_H
