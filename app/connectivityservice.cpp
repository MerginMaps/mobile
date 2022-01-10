/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "connectivityservice.h"

ConnectivityService::ConnectivityService( QObject *parent ) : QObject( parent )
{
}

bool ConnectivityService::isBluetoothEnabled()
{
  return mBluetooth.hostMode() == QBluetoothLocalDevice::HostConnectable;
}

void ConnectivityService::enableBluetooth()
{
  mBluetooth.setHostMode( QBluetoothLocalDevice::HostConnectable );
}
