/***************************************************************************
 qgsquickattributeformproxymodel.h
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

#ifndef QGSQUICKATTRIBUTEFORMPROXYMODEL_H
#define QGSQUICKATTRIBUTEFORMPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "qgis_quick.h"

class QgsQuickAttributeFormModel;

/**
 * \ingroup quick
 *
 * \note QML Type: AttributeFormProxyModel
 *
 * \since QGIS 3.4
 */
class QUICK_EXPORT QgsQuickAttributeFormProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    QgsQuickAttributeFormProxyModel( QObject *parent = nullptr );
    ~QgsQuickAttributeFormProxyModel() override;

    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;
};


#endif // QGSQUICKATTRIBUTEFORMPROXYMODEL_H
