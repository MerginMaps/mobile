/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bluetoothdiscoverymodel.h"
#include "coreutils.h"

#ifdef HAVE_BLUETOOTH
#include <QBluetoothUuid>
#endif


BluetoothDiscoveryModel::BluetoothDiscoveryModel( QObject *parent ) : QAbstractListModel( parent )
{
#ifdef HAVE_BLUETOOTH
  mDiscoveryAgent = std::unique_ptr<QBluetoothDeviceDiscoveryAgent>( new QBluetoothDeviceDiscoveryAgent() );

  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothDiscoveryModel::deviceDiscovered );
  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::deviceUpdated, this, &BluetoothDiscoveryModel::deviceUpdated );
  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::canceled, this, &BluetoothDiscoveryModel::finishedDiscovery );
  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothDiscoveryModel::finishedDiscovery );

  connect( mDiscoveryAgent.get(), QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of( &QBluetoothDeviceDiscoveryAgent::error ), this,
           [ = ]( QBluetoothDeviceDiscoveryAgent::Error error )
  {
    CoreUtils::log( "Bluetooth discovery", QString( "Error occured during device discovery, error code #" ).arg( error ) );
    finishedDiscovery();
  } );
#endif
}

BluetoothDiscoveryModel::~BluetoothDiscoveryModel() = default;

QHash<int, QByteArray> BluetoothDiscoveryModel::roleNames() const
{
  QHash<int, QByteArray> roles;

#ifdef HAVE_BLUETOOTH
  roles.insert( DataRoles::DeviceAddress, "DeviceAddress" );
  roles.insert( DataRoles::DeviceName, "DeviceName" );
  roles.insert( DataRoles::SignalStrength, "SignalStrength" );
#endif

  return roles;
}

int BluetoothDiscoveryModel::rowCount( const QModelIndex & ) const
{
#ifdef HAVE_BLUETOOTH
  return mFoundDevices.count();
#else
  return 0;
#endif
}

QVariant BluetoothDiscoveryModel::data( const QModelIndex &index, int role ) const
{
#ifdef HAVE_BLUETOOTH
  if ( !index.isValid() )
    return QVariant();

  int deviceIndex = index.row();

  if ( deviceIndex < 0 || deviceIndex >= mFoundDevices.count() )
    return QVariant();

  QBluetoothDeviceInfo device = mFoundDevices[deviceIndex];

  switch ( role )
  {
    case DataRoles::DeviceAddress:
    {
      return device.address().toString();
    }

    case DataRoles::DeviceName:
    {
      return device.name();
    }

    case DataRoles::SignalStrength:
    {
      return device.rssi();
    }

    default: return QVariant();
  }
#else
  return QVariant();
#endif
}

bool BluetoothDiscoveryModel::discovering() const
{
  return mDiscovering;
}

void BluetoothDiscoveryModel::setDiscovering( bool discovering )
{
  if ( mDiscovering == discovering )
    return;

#ifdef HAVE_BLUETOOTH
  if ( discovering )
  {
    mDiscoveryAgent->start();
    CoreUtils::log( QStringLiteral( "Bluetooth discovery" ), QStringLiteral( "Started discovering devices, method %1" ).arg( mDiscoveryAgent->supportedDiscoveryMethods() ) );
  }
  else
  {
    mDiscoveryAgent->stop();
  }
#endif

  mDiscovering = discovering;
  emit discoveringChanged( mDiscovering );
}

#ifdef HAVE_BLUETOOTH
void BluetoothDiscoveryModel::deviceDiscovered( const QBluetoothDeviceInfo &device )
{
  for ( int i = 0; i < mFoundDevices.count(); i++ )
  {
    if ( mFoundDevices[i].address() == device.address() )
    {
      return; // duplicated device
    }
  }

  // ignore devices with invalid address (apple devices)
  if ( device.address().isNull() )
    return;

  int insertIndex = mFoundDevices.count();
  beginInsertRows( QModelIndex(), insertIndex, insertIndex );

  mFoundDevices << device;

  endInsertRows();
}

void BluetoothDiscoveryModel::deviceUpdated( const QBluetoothDeviceInfo &device, QBluetoothDeviceInfo::Fields )
{
  // ignore devices with invalid address (apple devices)
  if ( device.address().isNull() )
    return;

  for ( int i = 0; i < mFoundDevices.count(); i++ )
  {
    if ( mFoundDevices[i].address() == device.address() )
    {
      mFoundDevices[i] = device;
      emit dataChanged( index( i ), index( i ) );
      break;
    }
  }
}
#endif

void BluetoothDiscoveryModel::finishedDiscovery()
{
  setDiscovering( false );
}
