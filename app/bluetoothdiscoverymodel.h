/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BLUETOOTHDISCOVERYMODEL_H
#define BLUETOOTHDISCOVERYMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <qglobal.h>
#include <memory>

#ifdef HAVE_BLUETOOTH
#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#endif

class BluetoothDiscoveryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( bool discovering READ discovering WRITE setDiscovering NOTIFY discoveringChanged )

  public:
    explicit BluetoothDiscoveryModel( QObject *parent = nullptr );
    virtual ~BluetoothDiscoveryModel();

    enum DataRoles
    {
      DeviceAddress = Qt::UserRole + 1, //! physical address of BT device
      DeviceName,
      SignalStrength
    };
    Q_ENUM( DataRoles )

    QHash<int, QByteArray> roleNames() const override;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    bool discovering() const;
    void setDiscovering( bool discovering );

  public slots:

#ifdef HAVE_BLUETOOTH
    void deviceDiscovered( const QBluetoothDeviceInfo &info );
    void deviceUpdated( const QBluetoothDeviceInfo &info, QBluetoothDeviceInfo::Fields updatedFields );
#endif
    void finishedDiscovery();

  signals:
    void discoveringChanged( bool );

  private:
    bool mDiscovering = false;

#ifdef HAVE_BLUETOOTH
    QList<QBluetoothDeviceInfo> mFoundDevices;
    std::unique_ptr<QBluetoothDeviceDiscoveryAgent> mDiscoveryAgent;
#endif
};

#endif // BLUETOOTHDISCOVERYMODEL_H
