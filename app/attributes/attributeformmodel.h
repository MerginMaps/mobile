/***************************************************************************
 attributeformmodel.h
  --------------------------------------
  Date                 : 20.4.2021
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

#ifndef ATTRIBUTEFORMMODEL_H
#define ATTRIBUTEFORMMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QUuid>



class AttributeController;

/**
 * This is a model implementation of attribute form of a feature from a vector layer
 * for a SINGLE tab in case of tab layout, or a WHOLE form in case there are no tabs at all.
 * Groups are flattened into a list.
 *
 * Items can be widgets for editing attributes, widgets for relations and containers (groups and tabs) flattened to separators.
 *
 * \note QML Type: AttributeFormModel
 */
class  AttributeFormModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    AttributeFormModel( QObject *parent,
                        AttributeController *controller,
                        const QVector<QUuid> &data );
    ~AttributeFormModel() override;

    enum AttributeFormRoles
    {
      Type = Qt::UserRole + 1, //!< User role used to identify either "field" or "container" type of item
      Name, //!< Field Name
      AttributeValue, //!< Field Value
      AttributeValueIsNull, //!< Whether field value is null (QVariant with set type, but NULL flag)
      AttributeEditable,  //!< Whether is field editable
      EditorWidget, //!< Widget type to represent the data (text field, value map, ...)
      EditorWidgetConfig, //!< Widget configuration
      RememberValue, //!< Remember value (whether to remember the value)
      Field, //!< Field
      FieldIndex, //!< Index
      Group, //!< Group name
      Visible, //!< Field visible
      ValidationMessage,
      ValidationStatus,
      Relation //!< QgsRelation instance for this item, empty if it is not a relation
    };

    Q_ENUM( AttributeFormRoles )

    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags( const QModelIndex &index ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;

  public slots:
    void onFormDataChanged( const QUuid id, QVector<int> roles );
    void onFeatureChanged();

  private:
    bool rowIsValid( int row ) const;

    AttributeController *mController = nullptr; // not owned
    const QVector<QUuid> mData;
};

#endif // ATTRIBUTEFORMMODEL_H
