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

    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )
    Q_PROPERTY( LayerTreeModel *layerTreeModel READ layerTreeModel WRITE setLayerTreeModel NOTIFY layerTreeModelChanged )

  public:

    explicit LayerTreeSortFilterModel( QObject *parent = nullptr );
    virtual ~LayerTreeSortFilterModel();

    LayerTreeModel *layerTreeModel() const;
    void setLayerTreeModel( LayerTreeModel *newLayerTreeModel );

    const QString &searchExpression() const;
    void setSearchExpression( const QString &newSearchExpression );

    Q_INVOKABLE QModelIndex getModelIndex( int row, int column, const QModelIndex &parent = QModelIndex() ) const;

    // Converts modelIndex to source model index and returns specific node
    Q_INVOKABLE QgsLayerTreeNode *getNode( QModelIndex modelIndex ) const;

  public slots:
    void onSourceModelInitialized();

  signals:
    void layerTreeModelChanged( LayerTreeModel *layerTreeModel );
    void searchExpressionChanged( const QString &searchExpression );

  private:
    LayerTreeModel *mLayerTreeModel = nullptr;
    QString mSearchExpression;
};

#endif // LAYERTREESORTFILTERMODEL_H
