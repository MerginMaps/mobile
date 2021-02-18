#include "projectsproxymodel_future.h"

ProjectsProxyModel_future::ProjectsProxyModel_future( ProjectModelTypes modelType, QObject *parent ) :
  QSortFilterProxyModel( parent ),
  mModelType( modelType )
{

}

bool ProjectsProxyModel_future::filterAcceptsRow( int, const QModelIndex & ) const
{
  // return true if it passes search filter
  return true;
}
