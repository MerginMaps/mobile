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

#include <QObject>
#include <qglobal.h>
#include <QAbstractListModel>

class AppSettings;

struct PositionProvider
{
  QString name;
  QString description;
  QString providerType; // either internal or external
  QString providerId; // holds BT address for external provider and simulated/internal for internal provider

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

  PositionProvider() {}
};

class PositionProvidersModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( AppSettings *appSettings READ appSettings WRITE setAppSettings NOTIFY appSettingsChanged )

  public:

    explicit PositionProvidersModel( QObject *parent = nullptr );
    virtual ~PositionProvidersModel();

    enum DataRoles
    {
      ProviderName = Qt::UserRole + 1, //! physical address of BT device
      ProviderDescription,
      ProviderId,
      ProviderType // external (connected) / internal (device)
    };
    Q_ENUM( DataRoles )

    QHash<int, QByteArray> roleNames() const override;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    Q_INVOKABLE void removeProvider( const QString &providerId );
    Q_INVOKABLE void addProvider( const QString &providerName, const QString &providerId );

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
