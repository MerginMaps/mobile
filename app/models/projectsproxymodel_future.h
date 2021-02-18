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
    explicit ProjectsProxyModel_future( ProjectModelTypes modelType, QObject *parent = nullptr );
    ~ProjectsProxyModel_future() override {};

  protected:
    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;
//    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

  private:
    ProjectModelTypes mModelType;
};

#endif // PROJECTSPROXYMODEL_FUTURE_H
