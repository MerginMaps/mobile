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

class LayerTreeModel : public QgsLayerTreeModel
{
    Q_OBJECT

    Q_PROPERTY( QgsProject *qgsProject READ qgsProject WRITE setQgsProject NOTIFY qgsProjectChanged )

  public:

    enum Roles
    {
      Node = Qt::UserRole + 1,
      NodeType,
      NodePath,
      NodeIsVisible,
      SerializedNode
    };
    Q_ENUM( Roles );

    explicit LayerTreeModel( QObject *parent = nullptr );
    virtual ~LayerTreeModel();

    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;

    QgsProject *qgsProject() const;
    void setQgsProject( QgsProject *newQgsProject );

    QgsLayerTreeModel *qgsModel() const;

    QString serializeNode( QgsLayerTreeNode *node ) const;
    QgsLayerTreeNode *deserializeNode( const QString &nodeId ) const;

    /**
     * Returns QML representation of node's visibility, either 'yes', 'no' or empty string
     * if this layer node is not spatial
     */
    QString visible( QgsLayerTreeNode *node ) const;

    Q_INVOKABLE void reset();

  signals:
    void qgsProjectChanged( QgsProject *qgsProject );
    void modelInitialized();

  protected slots:
    void setupModel();

  private:
    QgsProject *mQgsProject = nullptr; // not owned
};

#endif // LAYERTREEMODEL_H
