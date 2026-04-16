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
#include <QFutureWatcher>
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
    Q_PROPERTY( bool isLoading READ isLoading NOTIFY isLoadingChanged )
    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )

  public:
    enum Roles
    {
      KeyRole = Qt::UserRole + 1, // DisplayRole is used for description
    };
    Q_ENUM( Roles )

    explicit ValueMapFilterModel( QObject *parent = nullptr );
    ~ValueMapFilterModel() override = default;

    bool isLoading() const;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    QHash<int, QByteArray> roleNames() const override;

    QVariantMap config() const;
    void setConfig( const QVariantMap &config );

  signals:
    void configChanged();
    void isLoadingChanged();
    void countChanged();

  private slots:
    void onLoadingFinished();

  private:
    struct Item
    {
      QString description;
      QString key;
    };

    static QList<Item> loadItems( const QVariantMap &config );
    void startLoad();

    QVariantMap mConfig;

    QList<Item> mItems;
    QFutureWatcher<QList<Item>> mResultWatcher;
    bool mIsLoading = false;
    bool mHasPendingLoad = false;
};

#endif // VALUEMAPFILTERMODEL_H
