/***************************************************************************
 qgsquickattributetabmodel.h
  --------------------------------------
  Date                 : 20.4.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSQUICKATTRIBUTETABMODEL_H
#define QGSQUICKATTRIBUTETABMODEL_H

#include "qgis_quick.h"
#include <QAbstractListModel>
#include <QUuid>
#include <QVector>

class QgsQuickAttributeController;
class QgsQuickAttributeFormProxyModel;

/**
 * \ingroup quick
 *
 * Model for tabs within Feature Form.
 * This is read-only model
 *
 * \note QML Type: AttributeTabModel
 *
 * \since QGIS 3.22
 */
class QUICK_EXPORT QgsQuickAttributeTabModel : public QAbstractListModel
{
    Q_OBJECT
  public:
    QgsQuickAttributeTabModel( QObject *parent,
                               QgsQuickAttributeController *controller,
                               int tabCount );
    ~QgsQuickAttributeTabModel() override;

    enum AttributeTabRoles
    {
      Name = Qt::UserRole + 1, //!< Tab name
      Visible //!< Tab visible
    };

    Q_ENUM( AttributeTabRoles )

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    Q_INVOKABLE QgsQuickAttributeFormProxyModel *attributeFormProxyModel( int row ) const;

  public slots:
    void onTabDataChanged( int row );
    void onFeatureChanged();

  private:
    const QgsQuickAttributeController *mController = nullptr; // not owned
    const int mTabCount;
};

#endif // QGSQUICKATTRIBUTETABMODEL_H
