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
 * \brief The ProjectsProxyModel class used as a proxy filter/sort model for the \see ProjectsModel class.
 *
 * ProjectsProxyModel is a QML type with required property of projectSourceModel. Without source model, this model does nothing (is not initialized).
 * After setting source model, this model starts sorting and allows filtering (search) from view.
 */
class ProjectsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )
    Q_PROPERTY( ProjectsModel *projectSourceModel READ projectSourceModel WRITE setProjectSourceModel )

    //! When true, a project whose Roles::ProjectIsActiveProject is true is always sorted first
    Q_PROPERTY( bool activeProjectAlwaysFirst READ activeProjectAlwaysFirst WRITE setActiveProjectAlwaysFirst )

  public:
    explicit ProjectsProxyModel( QObject *parent = nullptr );
    ~ProjectsProxyModel() override {};

    QString searchExpression() const;
    ProjectsModel *projectSourceModel() const;
    void setActiveProjectAlwaysFirst( bool value );
    bool activeProjectAlwaysFirst() const;

  public slots:
    void setSearchExpression( QString searchExpression );
    void setProjectSourceModel( ProjectsModel *sourceModel );

  signals:
    void searchExpressionChanged( QString SearchExpression );

  protected:
    bool lessThan( const QModelIndex &left, const QModelIndex &right ) const override;

  private:
    void initialize();

    ProjectsModel *mModel = nullptr; // not owned by this, needs to be set in order to proxy model to work
    ProjectsModel::ProjectModelTypes mModelType = ProjectsModel::EmptyProjectsModel;
    QString mSearchExpression;
    bool mActiveProjectAlwaysFirst = false;

    friend class TestModels;
};

#endif // PROJECTSPROXYMODEL_H
