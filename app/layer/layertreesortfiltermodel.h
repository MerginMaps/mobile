/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERTREESORTFILTERMODEL_H
#define LAYERTREESORTFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <qglobal.h>

#include "qgslayertreenode.h"

#include "layer/layertreemodel.h"

class LayerTreeSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( LayerTreeModel *layerTreeModel READ layerTreeModel WRITE setLayerTreeModel NOTIFY layerTreeModelChanged )

  public:

    enum SortMethodEnum
    {
      PreserveQgisOrder = 0,
      Alphabetical,
    };
    Q_ENUM( SortMethodEnum );

    explicit LayerTreeSortFilterModel( QObject *parent = nullptr );
    virtual ~LayerTreeSortFilterModel();

    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;

    LayerTreeModel *layerTreeModel() const;
    void setLayerTreeModel( LayerTreeModel *newLayerTreeModel );

    // Returns index to passed node
    Q_INVOKABLE QModelIndex node2index( QgsLayerTreeNode *node ) const;

  public slots:
    void onSourceModelInitialized();

  signals:
    void layerTreeModelChanged( LayerTreeModel *layerTreeModel );

  private:
    LayerTreeModel *mLayerTreeModel = nullptr;
};

#endif // LAYERTREESORTFILTERMODEL_H
