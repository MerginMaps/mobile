/***************************************************************************
 attributetabmodel.cpp
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

#include "attributetabmodel.h"
#include "attributecontroller.h"
#include "attributedata.h"

AttributeTabModel::AttributeTabModel( QObject *parent, AttributeController *controller, int tabCount )
  : QAbstractListModel( parent )
  , mController( controller )
  , mTabCount( tabCount )
{
  Q_ASSERT( mController );
  connect( mController, &AttributeController::tabDataChanged, this, &AttributeTabModel::onTabDataChanged );
  connect( mController, &AttributeController::featureLayerPairChanged, this, &AttributeTabModel::onFeatureChanged );
}

QHash<int, QByteArray> AttributeTabModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
  roles[AttributeTabModel::Name]  = QByteArray( "Name" );
  roles[AttributeTabModel::Visible] = QByteArray( "Visible" );
  roles[AttributeTabModel::TabIndex] = QByteArray( "TabIndex" );
  return roles;
}

AttributeTabModel::~AttributeTabModel() = default;

int AttributeTabModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mTabCount;
}

QVariant AttributeTabModel::data( const QModelIndex &index, int role ) const
{
  Q_ASSERT( mController );
  if ( !index.isValid() )
    return QVariant();

  const int row = index.row();
  const TabItem *item = mController->tabItem( row );
  if ( !item )
    return QVariant();

  switch ( role )
  {
    case AttributeTabModel::Name:
      return item->name();
    case AttributeTabModel::Visible:
      return item->isVisible();
    case AttributeTabModel::TabIndex:
      return item->tabIndex();
    default:
      return QVariant();
  }
}

AttributeFormProxyModel *AttributeTabModel::attributeFormProxyModel( int row ) const
{
  Q_ASSERT( mController );
  return mController->attributeFormProxyModelForTab( row );
}

void AttributeTabModel::onTabDataChanged( int row )
{
  Q_ASSERT( row >= 0 );
  const QModelIndex modelIndex = index( row, 0 );
  emit dataChanged( modelIndex, modelIndex );
}

void AttributeTabModel::onFeatureChanged()
{
  if ( rowCount() > 0 )
    emit dataChanged( index( 0, 0 ), index( rowCount() - 1, 0 ) );
}
