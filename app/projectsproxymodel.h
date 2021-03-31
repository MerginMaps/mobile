/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECTSPROXYMODEL_H
#define PROJECTSPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "projectsmodel.h"

/**
 * @brief The ProjectsProxyModel class
 */
class ProjectsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )
    Q_PROPERTY( ProjectsModel *projectSourceModel READ projectSourceModel WRITE setProjectSourceModel )

  public:
    explicit ProjectsProxyModel( QObject *parent = nullptr );
    ~ProjectsProxyModel() override {};

    QString searchExpression() const;
    ProjectsModel *projectSourceModel() const;

  public slots:
    void setSearchExpression( QString searchExpression );
    void setProjectSourceModel( ProjectsModel *sourceModel );

  signals:
    void searchExpressionChanged( QString SearchExpression );

  protected:
    bool lessThan( const QModelIndex &left, const QModelIndex &right ) const override;

  private:
    void initialize();

    ProjectsModel *mModel;
    ProjectsModel::ProjectModelTypes mModelType = ProjectsModel::EmptyProjectsModel;
    QString mSearchExpression;
};

#endif // PROJECTSPROXYMODEL_H
