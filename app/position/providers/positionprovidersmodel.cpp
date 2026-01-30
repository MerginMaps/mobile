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
#include "coreutils.h"

#ifdef ANDROID
#include "position/providers/androidpositionprovider.h"
#endif

PositionProvidersModel::PositionProvidersModel( QObject *parent ) : QAbstractListModel( parent )
{
  if ( !InputUtils::isMobilePlatform() )
  {
    PositionProvider simulated( "Simulated provider", "Simulated position around point", "internal", "simulated" );

    mProviders.push_front( simulated );
  }

  // Keep the names of position providers in sync with names
  // used in the constructors of the providers...

  PositionProvider internal;
  internal.name = tr( "Internal" );
  internal.description = tr( "GPS receiver of this device" );
  internal.providerType = "internal";
  internal.providerId = "devicegps";

  mProviders.push_front( internal );

#ifdef ANDROID
  PositionProvider internalFused;
  internalFused.name = tr( "Internal (fused)" );
  if ( AndroidPositionProvider::isFusedAvailable() )
    internalFused.description = tr( "Using GPS, Wifi and sensors" );
  else
    internalFused.description = tr( "Not available (%1)" ).arg( AndroidPositionProvider::fusedErrorString() );
  internalFused.providerType = "internal";
  internalFused.providerId = "android_fused";
  mProviders.push_front( internalFused );

  // This one should have pretty much the same behavior as the provider
  // implemented using Qt Positioning, so let's skip it. When we ditch
  // Qt's implementation on Android, this can be used as a fallback.
#if 0
  PositionProvider internalGps;
  internalGps.name = tr( "Internal (gps)" );
  internalGps.description = tr( "Using GPS only" );
  internalGps.providerType = "internal";
  internalGps.providerId = "android_gps";
  mProviders.push_front( internalGps );
#endif
#endif
}

PositionProvidersModel::~PositionProvidersModel() = default;

QHash<int, QByteArray> PositionProvidersModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles.insert( DataRoles::ProviderName, QByteArray( "ProviderName" ) );
  roles.insert( DataRoles::ProviderDescription, QByteArray( "ProviderDescription" ) );
  roles.insert( DataRoles::ProviderType, QByteArray( "ProviderType" ) );
  roles.insert( DataRoles::ProviderId, QByteArray( "ProviderId" ) );
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

    case DataRoles::ProviderType:
      return provider.providerType;

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

void PositionProvidersModel::addProvider( const QString &name, const QString &providerId, const QString &providerType )
{
  if ( providerId.isEmpty() )
    return;

  PositionProvider toAdd;
  const QString deviceDesc = providerType == QStringLiteral( "external_bt" ) ? tr( " Bluetooth device" ) : tr( " Network device" );
  toAdd.name = name;
  toAdd.providerId = providerId;
  toAdd.description = providerId + " " + deviceDesc;
  toAdd.providerType = providerType;

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
        QVariantList providerData = providers[i].toList();

        if ( providerData.length() < 2 )
        {
          CoreUtils::log( QStringLiteral( "PositionProvidersModel" ), QStringLiteral( "Saved provider does not have sufficient data" ) );
          continue;
        }

        PositionProvider provider;
        const QString providerType = providerData[2].isNull() ? QStringLiteral( "external_bt" ) : QStringLiteral( "external_ip" );
        const QString deviceDesc = providerType == QStringLiteral( "external_bt" ) ? tr( " Bluetooth device" ) : tr( " Network device" );
        provider.name = providerData[0].toString();
        provider.providerId = providerData[1].toString();
        provider.description = provider.providerId + deviceDesc;
        provider.providerType = providerType;

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
    if ( mProviders[i].providerType == QStringLiteral( "internal" ) )
      continue;

    QStringList provider = { mProviders[i].name, mProviders[i].providerId, mProviders[i].providerType };
    out.push_back( provider );
  }

  return out;
}
