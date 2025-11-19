/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FIELDSMODEL_H
#define FIELDSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QMap>

#include "qgsfield.h"
#include "qgsfields.h"

struct FieldConfiguration
{
  QString attributeName;
  QString fieldType;
  QString widgetType;
};

class FieldsModel: public QAbstractListModel
{
    Q_OBJECT
  public:
    /**
     * Feature roles enum.
     */
    Q_ENUMS( FeatureRoles )

    //! Feature roles
    enum FeatureRoles
    {
      AttributeName = Qt::UserRole + 1,  //!< Attribute's display name (the original field name)
      WidgetType,  //!< Widget type name. Should match QT/QML editor widgets names.
    };

    //! Creates a new fields model
    explicit FieldsModel( QObject *parent = nullptr );

    Q_INVOKABLE bool addField( const QString &name, const QString &widgetType = QString( "TextEdit" ) );
    Q_INVOKABLE bool removeField( int rowIndex );

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex &parent ) const override;
    QVariant data( const QModelIndex &index, int role ) const override;
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole ) override;
    //! Returns fields configuration
    QList<FieldConfiguration> fields() const;

  signals:
    void widgetListChanged();
    void notifyError( const QString &message );

  private:
    QList<FieldConfiguration> mFields;


    bool contains( const QString &name );
};

#endif // FIELDSMODEL_H
