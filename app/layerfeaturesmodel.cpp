#include "layerfeaturesmodel.h"
#include <QDebug>

LayerFeaturesModel::LayerFeaturesModel( QObject *parent )
  : QAbstractListModel( parent )
{
  m_features.append(QPair<int, QString> {1, "feat deat"});
  m_features.append(QPair<int, QString> {2, "cucoriedky"});
  m_features.append(QPair<int, QString> {3, "cernice"});
}

int LayerFeaturesModel::rowCount( const QModelIndex &parent ) const
{
  // For list models only the root node (an invalid parent) should return the list's size. For all
  // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
  if ( parent.isValid() )
    return 0;

  return m_features.count();
}

QVariant LayerFeaturesModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= m_features.count() )
    return QVariant( "" );

  if (role < roleNames::id || role > roleNames::displayName )
    return QVariant();

  if ( !index.isValid() )
    return QVariant();

  QPair<int, QString> feat = m_features.at(index.row());

  if ( role == roleNames::id )
    return QVariant(feat.first);
  else
    return QVariant(feat.second);

  return QVariant();
}

bool LayerFeaturesModel::addFeature( const QPair<int, QString> &feature )
{
  m_features.push_back( feature );
  return true;
}

QHash<int, QByteArray> LayerFeaturesModel::roleNames() const
{
  QHash<int, QByteArray> roleNames = QAbstractListModel::roleNames();
  roleNames[id] = "id";
  roleNames[displayName] = "displayName";
  return roleNames;
}

bool LayerFeaturesModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  if ( data( index, role ) != value )
  {
    // FIXME: Implement me!
    emit dataChanged( index, index, QVector<int>() << role );
    return true;
  }
  return false;
}

Qt::ItemFlags LayerFeaturesModel::flags( const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable; // FIXME: Implement me!
}
