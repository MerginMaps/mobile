#include "layersmodelref.h"

LayersModelRef::LayersModelRef()
{

}

//QVariant LayersModelRef::data(const QModelIndex &index, int role) const
//{
//  /* TODO:
//   *  - check if role is from my role
//   *   - if it is, create respective logic
//   *   - if not, continue
//   *  - return data from base function
//   */
//  return QgsMapLayerProxyModel::data( index, role );
//}

bool LayersModelRef::layersFilter( int source_row, const QModelIndex &source_parent ) const
{
  Q_UNUSED( source_row );
  Q_UNUSED( source_parent );
  return true;
}

bool LayersModelRef::filterAcceptsRow( int source_row, const QModelIndex &source_parent ) const
{
  if ( !QgsMapLayerProxyModel::filterAcceptsRow(source_row, source_parent) )
    return false;

  return layersFilter( source_row, source_parent );
}
