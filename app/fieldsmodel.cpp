/***************************************************************************
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "fieldsmodel.h"

FieldsModel::FieldsModel( QObject *parent )
  : QAbstractListModel( parent )
{
  // fill the model with initial data
  addField( tr( "Date" ), "DateTime" );
  addField( tr( "Notes" ), "TextEdit" );
  addField( tr( "Photo" ), "ExternalResource" );
}

bool FieldsModel::addField( const QString &name, const QString &widgetType )
{
  if ( contains( name ) )
  {
    if ( name.isEmpty() )
    {
      emit notifyError( tr( "Please fill a name of previous field before adding a new field." ) );
    }
    else
    {
      emit notifyError( tr( "Field %1 already exists." ).arg( name ) );
    }
    return false;
  }

  beginResetModel();

  FieldConfiguration fc;
  fc.attributeName = name;
  fc.widgetType = widgetType;
  mFields.append( fc );

  endResetModel();
  return true;
}

bool FieldsModel::removeField( const int rowIndex )
{
  if ( rowIndex < 0 || rowIndex >= mFields.count() )
    return false;

  beginResetModel();
  mFields.removeAt( rowIndex );
  endResetModel();
  return true;
}

QList<FieldConfiguration> FieldsModel::fields() const
{
  return mFields;
}

QHash<int, QByteArray> FieldsModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
  roles[AttributeName]  = QByteArrayLiteral( "attributeName" );
  roles[WidgetType]  = QByteArrayLiteral( "widgetType" );

  return roles;
}


int FieldsModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent );
  return static_cast<int>( mFields.count() );
}

QVariant FieldsModel::data( const QModelIndex &index, const int role ) const
{
  const int row = index.row();
  if ( row < 0 || row >= mFields.count() )
    return QVariant();

  FieldConfiguration field = mFields[row];

  switch ( role )
  {
    case AttributeName:
      return field.attributeName;

    case WidgetType:
      return field.widgetType;

    default:
      return QVariant();
  }
}

bool FieldsModel::setData( const QModelIndex &index, const QVariant &value, const int role )
{
  if ( data( index, role ) == value )
    return true;

  const int row = index.row();
  if ( row < 0 || row >= mFields.count() )
    return false;

  switch ( role )
  {
    case AttributeName:
    {
      if ( contains( value.toString() ) )
      {
        emit notifyError( tr( "Field %1 already exists. \nWon't be added to the project." ).arg( value.toString() ) );
      }
      mFields[row].attributeName = value.toString();
      emit dataChanged( index, index, {AttributeName} );
      return true;
    }

    case WidgetType:
    {
      mFields[row].widgetType = value.toString();
      emit dataChanged( index, index, {WidgetType} );
      return true;
    }

    default:
      return false;
  }
}

bool FieldsModel::contains( const QString &name ) const
{
  for ( int i = 0; i < mFields.count(); ++i )
  {
    if ( mFields.at( i ).attributeName == name )
      return true;
  }
  return false;
}
