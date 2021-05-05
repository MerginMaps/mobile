/***************************************************************************
  attributepreviewmodel.h
  --------------------------------------
  Date                 : 5.5.2021
  Copyright            : (C) 2021 by Peter Petrik
  Email                : zilolv@gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef ATTRIBUTEPREVIEWMODEL_H
#define ATTRIBUTEPREVIEWMODEL_H

#include <QAbstractListModel>
#include <QPair>

class QgsQuickFeatureLayerPair;

/**
 * \note QML Type: AttributePreviewModel
 */
class AttributePreviewModel : public QAbstractListModel
{
    Q_OBJECT
  public:
    AttributePreviewModel( QObject *parent = nullptr );
    ~AttributePreviewModel() override;

    enum AttributePreviewRoles
    {
      Name = Qt::UserRole + 1,
      Value
    };

    Q_ENUM( AttributePreviewRoles )

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
    Q_INVOKABLE void resetModel( const QgsQuickFeatureLayerPair &pair );

  private:
    QVector<QPair<QString, QString>> mItems; // name, value
};

#endif // ATTRIBUTEPREVIEWMODEL_H
