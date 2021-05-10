/***************************************************************************
  qgsquickattributetabproxymodel.h
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

#ifndef QGSQUICKATTRIBUTETABPROXYMODEL_H
#define QGSQUICKATTRIBUTETABPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "qgis_quick.h"

class QgsQuickAttributeTabModel;
class QgsQuickAttributeFormProxyModel;

/**
 * \ingroup quick
 *
 * This is a model for filtering invisible tabs for feature form
 *
 * \note QML Type: AttributeTabProxyModel
 *
 * \since QGIS 3.20
 */
class QUICK_EXPORT QgsQuickAttributeTabProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    QgsQuickAttributeTabProxyModel( QObject *parent = nullptr );
    ~QgsQuickAttributeTabProxyModel() override;

    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;

    // Get attribute form proxy model for particular tab index
    Q_INVOKABLE QgsQuickAttributeFormProxyModel *attributeFormProxyModel( int sourceRow ) const;
};

#endif // QGSQUICKATTRIBUTETABPROXYMODEL_H
