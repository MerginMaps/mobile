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

class QgsVectorLayer;

// This model loads unique values from the selected layer+field and exposes them via Qt::DisplayRole
class UniqueValuesFilterModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY( QgsVectorLayer *layer READ layer WRITE setLayer NOTIFY layerChanged )
    Q_PROPERTY( QString fieldName READ fieldName WRITE setFieldName NOTIFY fieldNameChanged )

  public:
    explicit UniqueValuesFilterModel( QObject *parent = nullptr );
    ~UniqueValuesFilterModel() override = default;

    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

    QgsVectorLayer *layer() const;
    void setLayer( QgsVectorLayer *layer );

    QString fieldName() const;
    void setFieldName( const QString &fieldName );

    Q_INVOKABLE void populate();

  signals:
    void layerChanged();
    void fieldNameChanged();

  public slots:
    void onLoadingFinished();

  private:
    QVariantList loadUniqueValues( QgsVectorLayer *layer, int fieldIndex );

    QgsVectorLayer *mLayer = nullptr;
    QString mFieldName;

    QVariantList mItems;
    QFutureWatcher<QVariantList> mResultWatcher;
};

#endif // UNIQUEVALUESFILTERMODEL_H
