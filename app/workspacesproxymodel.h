/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WORKSPACESPROXYMODEL_H
#define WORKSPACESPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "workspacesmodel.h"

class WorkspacesProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )
    Q_PROPERTY( WorkspacesModel *workspacesSourceModel READ workspacesSourceModel WRITE setWorkspacesSourceModel NOTIFY workspacesSourceModelChanged )

  public:
    explicit WorkspacesProxyModel( QObject *parent = nullptr );
    ~WorkspacesProxyModel() override {};

    QString searchExpression() const;
    WorkspacesModel *workspacesSourceModel() const;

  public slots:
    void setSearchExpression( QString searchExpression );
    void setWorkspacesSourceModel( WorkspacesModel *sourceModel );

  signals:
    void searchExpressionChanged( QString searchExpression );
    void workspacesSourceModelChanged();

  private:
    WorkspacesModel *mModel = nullptr; // not owned by this, needs to be set in order to proxy model to work
    QString mSearchExpression;
};

#endif // WORKSPACESPROXYMODEL_H
