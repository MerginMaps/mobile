#include "fieldsmodel.h"
#include <QtDebug>

FieldsModel::FieldsModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

bool FieldsModel::addField( const QString &name, const QString &widgetType )
{

  if ( contains( name ) )
  {
    if ( name.isEmpty() )
    {
      notify( tr( "Please fill a name of previous field before adding a new field." ) );
    }
    else
    {
      notify( tr( "Field %1 already exists." ).arg( name ) );
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

bool FieldsModel::removeField( int row )
{
  if ( row < 0 || row >= mFields.count() )
    return false;

  beginResetModel();
  mFields.removeAt( row );
  endResetModel();
  return true;
}

QVariantMap FieldsModel::supportedTypes()
{
  QVariantMap supportedTypes;
  supportedTypes.insert( "TextEdit", "Text" );
  supportedTypes.insert( "DateTime", "Date&time" );
  supportedTypes.insert( "Range", "Number" );
  supportedTypes.insert( "CheckBox", "Checkbox" );
  supportedTypes.insert( "ExternalResource", "Photo" );

  return supportedTypes;
}

QList<FieldConfiguration> FieldsModel::fields()
{
  return mFields;
}

QHash<int, QByteArray> FieldsModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
  roles[AttributeName]  = QByteArrayLiteral( "AttributeName" );
  roles[WidgetType]  = QByteArrayLiteral( "WidgetType" );

  return roles;
}


int FieldsModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent );
  return mFields.count();
}

QVariant FieldsModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFields.count() )
    return QVariant();

  FieldConfiguration field = mFields[row];

  switch ( role )
  {
    case AttributeName:
      return field.attributeName;
      break;

    case WidgetType:
      return field.widgetType;
      break;
  }

  return QVariant();
}

bool FieldsModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  if ( data( index, role ) == value )
    return true;

  int row = index.row();
  if ( row < 0 || row >= mFields.count() )
    return false;


  switch ( role )
  {
    case AttributeName:
    {
      if ( contains( value.toString() ) )
      {
        notify( tr( "Field %1 already exists. \nWon't be added to the project." ).arg( value.toString() ) );
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
  }

  return false;
}

void FieldsModel::initModel()
{
  mFields.clear();
  addField( "Date", "DateTime" );
  addField( "Notes", "TextEdit" );
  addField( "Photo", "ExternalResource" );
}

bool FieldsModel::contains( const QString &name )
{
  for ( int i = 0; i < mFields.count(); ++i )
  {
    if ( mFields.at( i ).attributeName == name )
      return true;
  }
  return false;
}
