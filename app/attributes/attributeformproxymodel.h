/***************************************************************************
 attributeformproxymodel.h
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

#ifndef ATTRIBUTEFORMPROXYMODEL_H
#define ATTRIBUTEFORMPROXYMODEL_H

#include <QSortFilterProxyModel>


class AttributeFormModel;

/**
 * \note QML Type: AttributeFormProxyModel
 */
class  AttributeFormProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    AttributeFormProxyModel( QObject *parent = nullptr );
    ~AttributeFormProxyModel() override;

    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;
};


#endif // ATTRIBUTEFORMPROXYMODEL_H
