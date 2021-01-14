#include "fieldsmodel.h"
#include <QtDebug>

FieldsModel::FieldsModel( QObject *parent )
  : QAbstractListModel( parent )
{
  initModel();
}

bool FieldsModel::addField( const QString &name, const QString &type, const QString &widgetType )
{
if (mFields.indexFromName(name) >= 0) {
    notify(QString("Field '%1' has not been added due to empty or existing name ").arg(name));
 }

  beginResetModel();
  QgsField field = createField( name, type, widgetType );
  mFields.append( field );
  endResetModel();
  return true;
}

bool FieldsModel::removeField( int row )
{
  if ( row < 0 || row >= mFields.count() )
    return false;

  beginResetModel();
  mFields.remove( row );
  endResetModel();
  return true;
}

QVariantMap FieldsModel::supportedTypes()
{
  QVariantMap supportedTypes;
  supportedTypes.insert( "text", "TextEdit" );
  //supportedTypes.insert("date", "DateTime");
  supportedTypes.insert( "datetime", "DateTime" );
  supportedTypes.insert( "integer", "Range" );
  supportedTypes.insert( "real", "TextEdit" );
  supportedTypes.insert( "bool", "Bool" );
  //supportedTypes.insert("binary", "value_tedt");
  supportedTypes.insert( "text", "ExternalResource" ); // TODO rename

  return supportedTypes;
}

QgsFields FieldsModel::fields()
{
  return mFields;
}

QHash<int, QByteArray> FieldsModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
  roles[AttributeName]  = QByteArrayLiteral( "AttributeName" );
  roles[FieldType]  = QByteArrayLiteral( "FieldType" );
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

  QgsField field = mFields[row];

  switch ( role )
  {
    case AttributeName:
      return field.name();
      break;

    case FieldType:
      return field.typeName();
      break;

    case WidgetType:
      return field.comment();
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
      mFields.rename( row, value.toString() );
      emit dataChanged( index, index, {AttributeName} );
      break;
    }

    case FieldType:
    {
      mFields[row].setTypeName( value.toString() );
      emit dataChanged( index, index, {FieldType} );
      break;
    }
    case WidgetType:
    {
      mFields[row].setComment( value.toString() );
      emit dataChanged( index, index, {WidgetType} );
      break;
    }
  }

  return false;
}

QString FieldsModel::findWidgetTypeByFieldName( const QString name ) const
{
  for ( int i = 0; i < mFields.count(); ++i )
  {
    if ( mFields.at( i ).name() == name )
      return mFields.at( i ).comment();
  }
  return QString( "TextEdit" );
}

void FieldsModel::initModel()
{
  addField( "Date", "datetime", "DateTime" );
  addField( "Notes", "text", "TextEdit" );
  addField( "Photo", "text", "ExternalResource" );
  // ONLY FOR testing
  addField( "Number", "integer", "Range" );
  addField( "Bool", "bool", "CheckBox" );
}

QVariant::Type FieldsModel::parseType( const QString &type )
{
  if ( type == QLatin1String( "text" ) )
    return QVariant::String;
  else if ( type == QLatin1String( "integer" ) )
    return QVariant::Int;
  else if ( type == QLatin1String( "integer64" ) )
    return QVariant::Int;
  else if ( type == QLatin1String( "real" ) )
    return QVariant::Double;
  else if ( type == QLatin1String( "date" ) )
    return QVariant::Date;
  else if ( type == QLatin1String( "datetime" ) )
    return QVariant::DateTime;
  else if ( type == QLatin1String( "bool" ) )
    return QVariant::Bool;
  else if ( type == QLatin1String( "binary" ) )
    return QVariant::Invalid; // TODO
  return QVariant::Invalid;
}

QgsField FieldsModel::createField( const QString &name, const QString &type, const QString &widgetType )
{
  QVariant::Type qtype = parseType( type );
  QgsField field( name, qtype, type );
  field.setComment( widgetType ); // TODO widget type

  return field;
}
