/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef UNIQUEVALUESFILTERMODEL_H
#define UNIQUEVALUESFILTERMODEL_H

#include <QAbstractListModel>
#include <QFutureWatcher>
#include <QtQml/qqmlregistration.h>

#include "filtercontroller.h"

class QgsVectorLayer;

// This model loads unique values from the selected layer+field and exposes them via Qt::DisplayRole
class UniqueValuesFilterModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QString layerId READ layerId WRITE setLayerId NOTIFY layerIdChanged )
    Q_PROPERTY( QString fieldName READ fieldName WRITE setFieldName NOTIFY fieldNameChanged )
    Q_PROPERTY( bool isLoading READ isLoading NOTIFY isLoadingChanged )
    Q_PROPERTY( int count READ rowCount NOTIFY countChanged )

  public:
    enum Roles
    {
      ValueRole = Qt::UserRole + 1, // DisplayRole is used for description
    };
    Q_ENUM( Roles )

    explicit UniqueValuesFilterModel( QObject *parent = nullptr );
    ~UniqueValuesFilterModel() override = default;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QString layerId() const;
    void setLayerId( const QString &layerId );

    QString fieldName() const;
    void setFieldName( const QString &fieldName );

    bool isLoading() const;

    Q_INVOKABLE void populate( FilterController *controller );

  signals:
    void layerIdChanged();
    void fieldNameChanged();
    void isLoadingChanged();
    void countChanged();

  public slots:
    void onLoadingFinished();

  private:
    static QVariantList loadUniqueValues( QgsVectorLayer *layer, int fieldIndex );

    QString mLayerId;
    QString mFieldName;

    QVariantList mItems;
    QFutureWatcher<QVariantList> mResultWatcher;
    bool mIsLoading = false;
};

#endif // UNIQUEVALUESFILTERMODEL_H
