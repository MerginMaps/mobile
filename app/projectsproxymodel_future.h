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
  public:
    explicit ProjectsProxyModel_future( ProjectsModel_future *projectSourceModel, QObject *parent = nullptr );
    ~ProjectsProxyModel_future() override {};

  Q_PROPERTY( QString searchExpression READ searchExpression WRITE setSearchExpression NOTIFY searchExpressionChanged )

  QString searchExpression() const;

public slots:
  void setSearchExpression(QString SearchExpression);

signals:
  void searchExpressionChanged(QString SearchExpression);

protected:
    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;
//    bool lessThan( const QModelIndex &left, const QModelIndex &right ) const override;

  private:
    ProjectsModel_future *mModel;
    ProjectModelTypes mModelType;
    QString mSearchExpression;
};

#endif // PROJECTSPROXYMODEL_FUTURE_H
