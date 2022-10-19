/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LAYERTREEMODEL_H
#define LAYERTREEMODEL_H

#include <QObject>
#include <qglobal.h>

#include "qgslayertreemodel.h"
#include "qgslayertree.h"
#include "qgsproject.h"

class LayerTree: public QgsLayerTree
{
    Q_OBJECT

  public:
    explicit LayerTree( QObject *parent );
    ~LayerTree();
};

class LayerTreeModel : public QgsLayerTreeModel
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )

  public:

    explicit LayerTreeModel( QObject *parent = nullptr );
    virtual ~LayerTreeModel();

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QgsLayerTreeNode *node( QModelIndex modelIndex ) const;

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

    QgsLayerTreeModel *qgsModel() const;

  signals:
    void qgsProjectChanged( QgsProject *qgsProject );
    void modelInitialized();

  protected slots:
    void setupModel();
    Q_INVOKABLE void reset();

  private:
    QgsProject *mQgsProject = nullptr; // not owned
};

#endif // LAYERTREEMODEL_H
