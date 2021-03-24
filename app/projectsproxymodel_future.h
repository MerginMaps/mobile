/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROJECTSPROXYMODEL_FUTURE_H
#define PROJECTSPROXYMODEL_FUTURE_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "projectsmodel_future.h"

/**
 * @brief The ProjectsProxyModel_future class
 */
class ProjectsProxyModel_future : public QSortFilterProxyModel
{
  Q_OBJECT

  Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )
  Q_PROPERTY( ProjectsModel_future *projectSourceModel READ projectSourceModel WRITE setProjectSourceModel )

public:
    explicit ProjectsProxyModel_future( QObject *parent = nullptr );
    ~ProjectsProxyModel_future() override {};

  QString searchExpression() const;
  ProjectsModel_future *projectSourceModel() const;

public slots:
  void setSearchExpression( QString searchExpression );
  void setProjectSourceModel( ProjectsModel_future *sourceModel );

signals:
  void searchExpressionChanged( QString SearchExpression );

protected:
    bool lessThan( const QModelIndex &left, const QModelIndex &right ) const override;

  private:
    void initialize();

    ProjectsModel_future *mModel;
    ProjectsModel_future::ProjectModelTypes mModelType = ProjectsModel_future::EmptyProjectsModel;
    QString mSearchExpression;
};

#endif // PROJECTSPROXYMODEL_FUTURE_H
