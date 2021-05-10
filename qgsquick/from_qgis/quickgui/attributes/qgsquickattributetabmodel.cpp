/***************************************************************************
 qgsquickattributetabmodel.cpp
  --------------------------------------
  Date                 : 20.4.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsquickattributetabmodel.h"
#include "qgsquickattributecontroller.h"
#include "qgsquickattributedata.h"

QgsQuickAttributeTabModel::QgsQuickAttributeTabModel( QObject *parent, QgsQuickAttributeController *controller, int tabCount )
  : QAbstractListModel( parent )
  , mController( controller )
  , mTabCount( tabCount )
{
  Q_ASSERT( mController );
  connect( mController, &QgsQuickAttributeController::tabDataChanged, this, &QgsQuickAttributeTabModel::onTabDataChanged );
  connect( mController, &QgsQuickAttributeController::featureLayerPairChanged, this, &QgsQuickAttributeTabModel::onFeatureChanged );
}

QHash<int, QByteArray> QgsQuickAttributeTabModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
  roles[QgsQuickAttributeTabModel::Name]  = QByteArray( "Name" );
  roles[QgsQuickAttributeTabModel::Visible] = QByteArray( "Visible" );
  return roles;
}

QgsQuickAttributeTabModel::~QgsQuickAttributeTabModel() = default;

int QgsQuickAttributeTabModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mTabCount;
}

QVariant QgsQuickAttributeTabModel::data( const QModelIndex &index, int role ) const
{
  Q_ASSERT( mController );
  if ( !index.isValid() )
    return QVariant();

  const int row = index.row();
  const QgsQuickTabItem *item = mController->tabItem( row );
  if ( !item )
    return QVariant();

  switch ( role )
  {
    case QgsQuickAttributeTabModel::Name:
      return item->name();
    case QgsQuickAttributeTabModel::Visible:
      return item->isVisible();
    default:
      return QVariant();
  }
}

QgsQuickAttributeFormProxyModel *QgsQuickAttributeTabModel::attributeFormProxyModel( int row ) const
{
  Q_ASSERT( mController );
  return mController->attributeFormProxyModelForTab( row );
}

void QgsQuickAttributeTabModel::onTabDataChanged( int row )
{
  Q_ASSERT( row >= 0 );
  const QModelIndex modelIndex = index( row, 0 );
  emit dataChanged( modelIndex, modelIndex );
}

void QgsQuickAttributeTabModel::onFeatureChanged()
{
  if ( rowCount() > 0 )
    emit dataChanged( index( 0, 0 ), index( rowCount() - 1, 0 ) );
}
