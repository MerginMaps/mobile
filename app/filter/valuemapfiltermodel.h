/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VALUEMAPFILTERMODEL_H
#define VALUEMAPFILTERMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>

/**
 * Populates a list model from a QGIS ValueMap editor widget config.
 * Exposes TextRole (display label) and ValueRole (stored key) for each entry.
 */
class ValueMapFilterModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QVariantMap config READ config WRITE setConfig NOTIFY configChanged )
    Q_PROPERTY( QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged )

  public:
    enum Roles
    {
      TextRole = Qt::UserRole + 1,
      ValueRole
    };
    Q_ENUM( Roles )

    explicit ValueMapFilterModel( QObject *parent = nullptr );
    ~ValueMapFilterModel() override = default;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;

    QVariantMap config() const;
    void setConfig( const QVariantMap &config );

    QString searchText() const;
    void setSearchText( const QString &searchText );

  signals:
    void configChanged();
    void searchTextChanged();

  private:
    void populate();
    void applyFilter();

    struct Item
    {
      QString text;
      QString value;
    };

    QVariantMap mConfig;
    QString mSearchText;
    QList<Item> mItems;
    QList<Item> mFilteredItems;
};

#endif // VALUEMAPFILTERMODEL_H
