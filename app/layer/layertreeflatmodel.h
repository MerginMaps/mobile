/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERTREEFLATMODEL_H
#define LAYERTREEFLATMODEL_H

#include <QStandardItemModel>
#include <QObject>
#include <qglobal.h>

#include "qgsproject.h"

#include "layer/layertreemodel.h"

class LayerTreeFlatModel : public QStandardItemModel
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )

  public:
    enum Roles
    {
      // copy roles from layer tree model
      Node = LayerTreeModel::Node,
      NodeType = LayerTreeModel::NodeType,
      NodePath = LayerTreeModel::NodePath,
      NodeIsVisible = LayerTreeModel::NodeIsVisible,
      SerializedNode = LayerTreeModel::SerializedNode
    };
    Q_ENUM( Roles );

    LayerTreeFlatModel( QObject *parent = nullptr );
    virtual ~LayerTreeFlatModel();

    QVariant data( const QModelIndex &index, int role ) const override;
    QHash<int, QByteArray> roleNames() const override;

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

    Q_INVOKABLE void reset();

  public slots:
    void populate();

  signals:
    void qgsProjectChanged( QgsProject *qgsProject );

    void modelInitialized();

  private:
    std::unique_ptr<LayerTreeModel> mLayerTreeModel;

    QgsProject *mQgsProject = nullptr; // not owned
};

#endif // LAYERTREEFLATMODEL_H
