/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONNECTIVITYSERVICE_H
#define CONNECTIVITYSERVICE_H

#include <QObject>
#include <qglobal.h>

#include <QBluetoothLocalDevice>

class ConnectivityService : public QObject
{
    Q_OBJECT
  public:
    explicit ConnectivityService( QObject *parent = nullptr );

    Q_INVOKABLE bool isBluetoothEnabled();
    Q_INVOKABLE void enableBluetooth();

  signals:
    void bluetoothStateChanged();

  private:
    QBluetoothLocalDevice mBluetooth;
};

#endif // CONNECTIVITYSERVICE_H
