/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERTREEFLATSORTFILTERMODEL_H
#define LAYERTREEFLATSORTFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QObject>
#include <qglobal.h>

#include "layer/layertreeflatmodel.h"

class LayerTreeFlatSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )
    Q_PROPERTY( LayerTreeFlatModel *layerTreeFlatModel READ layerTreeFlatModel WRITE setLayerTreeFlatModel NOTIFY layerTreeFlatModelChanged )

  public:
    explicit LayerTreeFlatSortFilterModel( QObject *parent = nullptr );
    virtual ~LayerTreeFlatSortFilterModel();

    bool filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const override;

    const QString &searchExpression() const;
    void setSearchExpression( const QString &newSearchExpression );

    LayerTreeFlatModel *layerTreeFlatModel() const;
    void setLayerTreeFlatModel( LayerTreeFlatModel *newLayerTreeFlatModel );

  public slots:
    void onSourceModelInitialized();

  signals:
    void searchExpressionChanged( const QString &searchExpression );
    void layerTreeFlatModelChanged( LayerTreeFlatModel *layerTreeFlatModel );

  private:
    QString mSearchExpression;
    LayerTreeFlatModel *mLayerTreeFlatModel = nullptr;
};

#endif // LAYERTREEFLATSORTFILTERMODEL_H
