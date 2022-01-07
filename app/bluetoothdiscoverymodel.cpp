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

#include "qdebug.h"

BluetoothDiscoveryModel::BluetoothDiscoveryModel( QObject *parent ) : QAbstractListModel( parent )
{
  mDiscoveryAgent = std::unique_ptr<QBluetoothDeviceDiscoveryAgent>( new QBluetoothDeviceDiscoveryAgent() );

  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothDiscoveryModel::deviceDiscovered );
  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::deviceUpdated, this, &BluetoothDiscoveryModel::deviceUpdated );
  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::canceled, this, &BluetoothDiscoveryModel::finishedDiscovery );
  connect( mDiscoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothDiscoveryModel::finishedDiscovery );

  connect( mDiscoveryAgent.get(), QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error),
      [=]( QBluetoothDeviceDiscoveryAgent::Error error ) {
    qDebug() << error << "occured during discovery, ending..";
    CoreUtils::log( "Bluetooth discovery", QString( "Error occured during device discovery, error code #" ).arg( error ) );
    finishedDiscovery();
  });

  QBluetoothDeviceInfo demo1( QBluetoothAddress("01:01:01:01:01:01"), "Demo device 1", 1 );
  QBluetoothDeviceInfo demo2( QBluetoothAddress("01:01:01:01:01:02"), "Demo device 2", 3 );
  mFoundDevices << demo1;
  mFoundDevices << demo2;
}

BluetoothDiscoveryModel::~BluetoothDiscoveryModel()
{
}

QHash<int, QByteArray> BluetoothDiscoveryModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles.insert( DataRoles::DeviceAddress, "DeviceAddress" );
  roles.insert( DataRoles::DeviceName, "DeviceName" );
  roles.insert( DataRoles::SignalStrength, "SignalStrength" );

  return roles;
}

int BluetoothDiscoveryModel::rowCount( const QModelIndex & ) const
{
  return mFoundDevices.count();
}

QVariant BluetoothDiscoveryModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  int deviceIndex = index.row();

  if ( deviceIndex < 0 || deviceIndex >= mFoundDevices.count() )
    return QVariant();

  QBluetoothDeviceInfo device = mFoundDevices[deviceIndex];

  switch( role ) {
    case DataRoles::DeviceAddress: {
      return device.address().toString();
    }

    case DataRoles::DeviceName: {
      return device.name();
    }

    case DataRoles::SignalStrength: {
      return device.rssi();
    }

    default: return QVariant();
  }
}

bool BluetoothDiscoveryModel::discovering() const
{
  return mDiscovering;
}

void BluetoothDiscoveryModel::setDiscovering( bool discovering )
{
  if ( mDiscovering == discovering )
    return;

  if ( discovering )
  {
    mDiscoveryAgent->start();
    CoreUtils::log( QStringLiteral( "Bluetooth discovery" ), QStringLiteral( "Started discovering devices, method %1" ).arg( mDiscoveryAgent->supportedDiscoveryMethods() ) );
  }
  else
  {
    mDiscoveryAgent->stop();
  }

  mDiscovering = discovering;
  emit discoveringChanged( mDiscovering );
}

// helper class to print the device
void _printDeviceInfo( const QBluetoothDeviceInfo &device )
{
  qDebug() << "Device" << device.name() << "Address:" << device.address();
  qDebug() << "  uuid:" << device.deviceUuid();
  qDebug() << "  rssi (signal strength):" << device.rssi();
  qDebug() << "  core config:" << device.coreConfigurations();
  qDebug() << "  service classes:" << device.serviceClasses();
  qDebug() << "  service uuids:" << device.serviceUuids();
  qDebug() << "  valid:" << device.isValid();
  qDebug() << "  cached: " << device.isCached();
  qDebug() << "  major class:" << device.majorDeviceClass();
  qDebug() << "  minor class:" << device.minorDeviceClass();
  qDebug() << " --- ";
}

void BluetoothDiscoveryModel::deviceDiscovered( const QBluetoothDeviceInfo &device )
{
  for ( int i = 0; i < mFoundDevices.count(); i++ )
  {
    if ( mFoundDevices[i].address() == device.address() )
    {
      return; // duplicated device
    }
  }

  qDebug() << "Found new device! info below";
  _printDeviceInfo( device );

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
      qDebug() << "Updated info about device! Info below:";
      _printDeviceInfo( device );
//      beginResetModel();
      mFoundDevices[i] = device;
//      endResetModel();
      dataChanged( index( i ), index( i ) );
      break;
    }
  }
}

void BluetoothDiscoveryModel::finishedDiscovery()
{
  setDiscovering( false );
}
