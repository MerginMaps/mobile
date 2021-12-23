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

#include "QBluetoothUuid"
#include <QBluetoothServiceDiscoveryAgent>
#include "qdebug.h"

BluetoothDiscoveryModel::BluetoothDiscoveryModel( QObject *parent ) : QAbstractListModel( parent )
{
  mDiscoveryAgent = std::unique_ptr<QBluetoothServiceDiscoveryAgent>( new QBluetoothServiceDiscoveryAgent() );

  connect( mDiscoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this, &BluetoothDiscoveryModel::serviceDiscovered );
//  connect( mDiscoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::deviceUpdated, this, &BluetoothDiscoveryModel::deviceUpdated );
  connect( mDiscoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::canceled, this, &BluetoothDiscoveryModel::finishedDiscovery );
  connect( mDiscoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::finished, this, &BluetoothDiscoveryModel::finishedDiscovery );

  connect( mDiscoveryAgent.get(), QOverload<QBluetoothServiceDiscoveryAgent::Error>::of(&QBluetoothServiceDiscoveryAgent::error),
      [=]( QBluetoothServiceDiscoveryAgent::Error error ) {
    qDebug() << error << "occured during discovery, ending..";
    CoreUtils::log( "Bluetooth discovery", QString( "Error occured during device discovery, error code #" ).arg( error ) );
    finishedDiscovery();
  });

  mDiscoveryAgent->setUuidFilter( QBluetoothUuid( QBluetoothUuid::SerialPort ) );
}

BluetoothDiscoveryModel::~BluetoothDiscoveryModel()
{

}

QHash<int, QByteArray> BluetoothDiscoveryModel::roleNames() const
{
  QHash<int, QByteArray> roles;
  roles.insert( DataRoles::DeviceAddress, "DeviceAddress" );
  roles.insert( DataRoles::DeviceName, "DeviceName" );

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

  QBluetoothServiceInfo service = mFoundDevices[deviceIndex];

  switch( role ) {
    case DataRoles::DeviceAddress: {
      return service.device().address().toString();
    }

    case DataRoles::DeviceName: {
      return service.device().name();
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
  }
  else
  {
    mDiscoveryAgent->stop();
  }

  mDiscovering = discovering;
  emit discoveringChanged( mDiscovering );
}

void BluetoothDiscoveryModel::serviceDiscovered( const QBluetoothServiceInfo &info )
{
  int insertIndex = mFoundDevices.count();

  beginInsertRows( index( 0 ), insertIndex, insertIndex ); // or beginResetModel()
  mFoundDevices << info;
  endInsertRows();
}

//void BluetoothDiscoveryModel::deviceUpdated( const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields )
//{
//  qDebug() << "Device has been updated: " << info.deviceUuid().toUInt32() << info.address().toString();
//  for ( int i = 0; i < mFoundDevices.count(); i++ )
//  {
//    if ( mFoundDevices[i].address() == info.address() )
//    {
//      beginResetModel();
//      mFoundDevices[i] = info;
//      endResetModel();
////      dataChanged( index( i ), index( i ) );
//      break;
//    }
//  }
//}

void BluetoothDiscoveryModel::finishedDiscovery()
{
  setDiscovering( false );
}
