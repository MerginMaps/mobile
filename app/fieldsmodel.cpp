#include "fieldsmodel.h"
#include <QtDebug>

FieldsModel::FieldsModel( QObject *parent )
  : QAbstractListModel( parent )
{
  initModel();
}

bool FieldsModel::addField( const QString &name, const QString &type )
{
  // TODO check if field with the given name exists
  beginResetModel();

//QVariant::Type qtype = QVariant::nameToType(name.toLocal8Bit().constData());
  QVariant::Type qtype = parseType( type );
  QgsField field( name, qtype, type );

// TODO PROPER widget setup
  if ( type == "text" )
  {
    QgsEditorWidgetSetup setup( QStringLiteral( "TextEdit" ), QVariantMap() );
    field.setEditorWidgetSetup( setup );
  }
  else
  {
    QgsEditorWidgetSetup setup( type, QVariantMap() );
    field.setEditorWidgetSetup( setup );
  }
  qDebug() << "Creating field" << name << qtype << type;

  mFields.append( field );

  endResetModel();
  return true;
  //emit dataChanged();
}

QStringList FieldsModel::supportedTypes()
{
  QStringList supportedTypes;
  supportedTypes << QStringLiteral( "text" );
  supportedTypes << QStringLiteral( "date" );
  supportedTypes << QStringLiteral( "datetime" );
  supportedTypes << QStringLiteral( "integer" );
  supportedTypes << QStringLiteral( "real" );
  supportedTypes << QStringLiteral( "bool" );
  supportedTypes << QStringLiteral( "binary" );

  return supportedTypes;
}

QgsFields FieldsModel::fields()
{
//  QgsFields fields;
//  for ( QgsField field : mFields )
//  {
//    fields.append( field );
//  }
  return mFields;
}

QHash<int, QByteArray> FieldsModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
  roles[AttributeName]  = QByteArrayLiteral( "AttributeName" );
  roles[WidgetType]  = QByteArrayLiteral( "WidgetType" );
  roles[Field] = QByteArrayLiteral( "Field" );

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

//    case FieldType:
//      field.type();
//      break;

    case WidgetType:
      return field.typeName();
      break;

    case Field:
      return field;
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
      mFields[row].setName( value.toString() );
      emit dataChanged( index, index, {AttributeName} );
      break;
    }

    case WidgetType:
    {
      mFields[row].setTypeName( value.toString() );
      emit dataChanged( index, index, {WidgetType} );
      break;
    }
  }

  return false;
}

void FieldsModel::initModel()
{
  addField( "Date", "datetime" );
  addField( "Notes", "text" );
  addField( "Photo", "text" );
  // ONLY FOR testing
  addField( "Number", "integer" );
  addField( "Bool", "bool" );
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
    return QVariant::Invalid;
}

QgsField FieldsModel::createField( const QString &name, const QString &type )
{
  //gsEditorWidgetSetup setup( type_temp, QVariantMap());

  //QgsField field(name, QVariant::TextFormat, type);
  //QString type_temp = QStringLiteral( "string" );

  //field.setName( name );
  //field.setType( QVariant::nameToType( type.toUtf8() ) );

  QVariant::Type qtype;
  if ( type == QLatin1String( "text" ) )
  {
    QgsField field( name, QVariant::String, type );
    QgsEditorWidgetSetup setup( type, QVariantMap() );
    field.setEditorWidgetSetup( setup );
    return field;
  }

  else if ( type == QLatin1String( "integer" ) )
  {
    qtype = QVariant::Int;
  }
//             //return QgsEditorWidgetSetup( "string", QVariantMap());
//           else if ( type == QLatin1String( "integer64" ) )
//             ogrType = OFTInteger64;
//           else if ( type == QLatin1String( "real" ) )
//             ogrType = OFTReal;
//           else if ( type == QLatin1String( "date" ) )
//             ogrType = OFTDate;
//           else if ( type == QLatin1String( "datetime" ) )
//             ogrType = OFTDateTime;
//           else if ( type == QLatin1String( "bool" ) )
//           {
//             ogrType = OFTInteger;
//             isBool = true;
//           }
//    else {
//        return QgsEditorWidgetSetup( "string", QVariantMap());
//    }

  qtype = QVariant::nameToType( name.toUtf8() );
  QgsField field( name, qtype, type );
  QgsEditorWidgetSetup setup( type, QVariantMap() );
  field.setEditorWidgetSetup( setup );
  qDebug() << "Creating field" << name << qtype << type;
  return QgsField( name, qtype, type );
}
