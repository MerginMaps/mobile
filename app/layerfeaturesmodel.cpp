#include "layerfeaturesmodel.h"

layerfeaturesmodel::layerfeaturesmodel(QObject *parent)
  : QAbstractItemModel(parent)
{
}

QModelIndex layerfeaturesmodel::index(int row, int column, const QModelIndex &parent) const
{
  // FIXME: Implement me!
}

QModelIndex layerfeaturesmodel::parent(const QModelIndex &index) const
{
  // FIXME: Implement me!
}

int layerfeaturesmodel::rowCount(const QModelIndex &parent) const
{
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

int layerfeaturesmodel::columnCount(const QModelIndex &parent) const
{
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

QVariant layerfeaturesmodel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  // FIXME: Implement me!
  return QVariant();
}
