/***************************************************************************
  attributetabproxymodel.h
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

#ifndef ATTRIBUTETABPROXYMODEL_H
#define ATTRIBUTETABPROXYMODEL_H

#include <QSortFilterProxyModel>


class AttributeTabModel;
class AttributeFormProxyModel;

/**
 * This is a model for filtering invisible tabs for feature form
 *
 * \note QML Type: AttributeTabProxyModel
 */
class  AttributeTabProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    AttributeTabProxyModel( QObject *parent = nullptr );
    ~AttributeTabProxyModel() override;

    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;

    // Get attribute form proxy model for particular tab index
    Q_INVOKABLE AttributeFormProxyModel *attributeFormProxyModel( int sourceRow ) const;
};

#endif // ATTRIBUTETABPROXYMODEL_H
