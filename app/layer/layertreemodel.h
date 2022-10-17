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
#include "qgsproject.h"

class LayerTreeModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )

  public:

    explicit LayerTreeModel( QObject *parent = nullptr );
    virtual ~LayerTreeModel();

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex &parent = QModelIndex() ) const override;
    Q_INVOKABLE QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const override;
    QModelIndex parent( const QModelIndex &child ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

  signals:
    void qgsProjectChanged( QgsProject *qgsProject );

  protected slots:
    void setupModel();
    Q_INVOKABLE void reset();

  private:
    QgsProject *mQgsProject = nullptr; // not owned

    std::unique_ptr<QgsLayerTreeModel> mModel = nullptr;
};

#endif // LAYERTREEMODEL_H
