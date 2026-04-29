/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSITIONPROVIDERSMODEL_H
#define POSITIONPROVIDERSMODEL_H

#include <qglobal.h>
#include <QAbstractListModel>

#include "appsettings.h"


struct PositionProvider
{
  QString name;
  QString description;
  QString providerType; // can be internal, external_bt, external_ip
  QString providerId; // holds BT address or IP address for external providers and simulated/internal for internal providers

  bool operator==( const PositionProvider &other ) const
  {
    return ( this->providerId == other.providerId );
  }

  bool operator!=( const PositionProvider &other ) const
  {
    return !( *this == other );
  }

  PositionProvider( const QString &name, const QString &desc, const QString &type, const QString &id )
    : name( name ), description( desc ), providerType( type ), providerId( id )
  {}

  PositionProvider() = default;
};

class PositionProvidersModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( AppSettings *appSettings READ appSettings WRITE setAppSettings NOTIFY appSettingsChanged )

  public:

    explicit PositionProvidersModel( QObject *parent = nullptr );
    ~PositionProvidersModel() override;

    enum DataRoles
    {
      ProviderName = Qt::UserRole + 1, // name of bluetooth device or custom name for network device
      ProviderDescription, // device address (IP/BT) + device type
      ProviderId, // device address (IP/BT)
      ProviderType // external_ip (connected) / external_bt (connected) / internal (device) / simulated (device)
    };
    Q_ENUM( DataRoles )

    QHash<int, QByteArray> roleNames() const override;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    Q_INVOKABLE void removeProvider( const QString &providerId );
    Q_INVOKABLE void addProvider( const QString &providerName, const QString &providerId, const QString &providerType );

    AppSettings *appSettings() const;
    void setAppSettings( AppSettings * );

  signals:
    void appSettingsChanged( AppSettings * );

  private:
    QVariantList toVariantList() const;

    AppSettings *mAppSettings = nullptr; // not owned
    QList<PositionProvider> mProviders;
};

#endif // POSITIONPROVIDERSMODEL_H
