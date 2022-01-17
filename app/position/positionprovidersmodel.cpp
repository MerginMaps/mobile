/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "positionprovidersmodel.h"
#include "inpututils.h"
#include "appsettings.h"

PositionProvidersModel::PositionProvidersModel( QObject *parent ) : QAbstractListModel( parent )
{
  if ( !InputUtils::isMobilePlatform() )
  {
    PositionProvider simulated( "Simulated provider", "Simulated position around point", "simulated" );

    mProviders.push_front( simulated );
  }

  PositionProvider internal;
  internal.name = tr( "Internal GPS receiver" );
  internal.description = tr( "GPS receiver of this device" );
  internal.providerId = "internal";

  mProviders.push_front( internal );
}

QHash<int, QByteArray> PositionProvidersModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles.insert( DataRoles::ProviderName, QByteArray( "ProviderName" ) );
  roles.insert( DataRoles::ProviderDescription, QByteArray( "ProviderDescription" ) );
  roles.insert( DataRoles::ProviderId, QByteArray( "ProviderId" ) );
  roles.insert( DataRoles::CanBeDeleted, QByteArray( "CanBeDeleted" ) );
  return roles;
}

int PositionProvidersModel::rowCount( const QModelIndex & ) const
{
  return mProviders.count();
}

QVariant PositionProvidersModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  int row = index.row();

  PositionProvider provider = mProviders.at( row );

  if ( row < 0 || row > mProviders.count() )
    return QVariant();

  switch ( role )
  {
    case DataRoles::ProviderName:
      return provider.name;

    case DataRoles::ProviderDescription:
      return provider.description;

    case DataRoles::ProviderId:
      return provider.providerId;

    case DataRoles::CanBeDeleted:
      return !provider.isPermanent();

    default:
      return QVariant();
  }
}

void PositionProvidersModel::removeProvider( const QString &providerId )
{
  if ( providerId.isEmpty() )
    return;

  PositionProvider toRemove;
  toRemove.providerId = providerId;

  if ( !mProviders.contains( toRemove ) )
    return;

  int removeIndex = mProviders.indexOf( toRemove );

  beginRemoveRows( QModelIndex(), removeIndex, removeIndex );

  mProviders.removeAt( removeIndex );

  endRemoveRows();

  // save the new list to persistent QSettings
  if ( mAppSettings )
  {
    mAppSettings->savePositionProviders( toVariantList() );
  }
}

void PositionProvidersModel::addProvider( const QString &name, const QString &providerId )
{
  if ( providerId.isEmpty() )
    return;

  PositionProvider toAdd;
  toAdd.name = name;
  toAdd.providerId = providerId;
  toAdd.description = tr( "Bluetooth GPS receiver" );

  if ( mProviders.contains( toAdd ) )
    return;

  int addIndex = mProviders.count();

  beginInsertRows( QModelIndex(), addIndex, addIndex );

  mProviders.push_back( toAdd );

  endInsertRows();

  // save the new list to persistent QSettings
  if ( mAppSettings )
  {
    mAppSettings->savePositionProviders( toVariantList() );
  }
}

AppSettings *PositionProvidersModel::appSettings() const
{
  return mAppSettings;
}

void PositionProvidersModel::setAppSettings( AppSettings *as )
{
  if ( mAppSettings == as )
    return;

  mAppSettings = as;
  emit appSettingsChanged( mAppSettings );

  // read providers from QSettings
  if ( mAppSettings )
  {
    QVariantList providers = mAppSettings->savedPositionProviders();

    beginResetModel();

    for ( int i = 0; i < providers.count(); ++i )
    {
      if ( providers[i].type() == QVariant::List || providers[i].type() == QVariant::StringList )
      {
        PositionProvider provider;
        provider.name = providers[i].toList()[0].toString();
        provider.providerId = providers[i].toList()[1].toString();
        provider.description = tr( "Bluetooth GPS receiver" );

        mProviders.append( provider );
      }
    }

    endResetModel();
  }
}

QVariantList PositionProvidersModel::toVariantList() const
{
  QVariantList out;

  for ( int i = 0; i < mProviders.count(); ++i )
  {
    if ( mProviders[i].isPermanent() )
      continue;

    out.append( { mProviders[i].name, mProviders[i].providerId } );
  }

  return out;
}
