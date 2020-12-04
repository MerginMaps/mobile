#include "fieldsmodel.h"

FieldsModel::FieldsModel( QObject *parent )
  : QAbstractListModel( parent )
{
}

bool FieldsModel::addField( const QString &name, const QString &type )
{
  // TODO check if field with the given name exists
  beginResetModel();

  QgsField field;
  field.setName( name );
  mFields.append( field );

  endResetModel();
  return true;
  //emit dataChanged();
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
  return mFields.count();
}

QVariant FieldsModel::data( const QModelIndex &index, int role ) const
{
  int row = index.row();
  if ( row < 0 || row >= mFields.count() )
    return QVariant();

  QgsField field = mFields.at( row );

  switch ( role )
  {
    case AttributeName:
      return field.name();
      break;

//    case FieldType:
//      field.type();
//      break;

    case WidgetType:
      QVariant();
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

//  switch ( role )
//  {
//    case AttributeValue:
//    {
//      QVariant val( value );
//      QgsField fld = mFeatureLayerPair.feature().fields().at( index.row() );

//      if ( !fld.convertCompatible( val ) )
//      {
//        QgsMessageLog::logMessage( tr( "Value \"%1\" %4 could not be converted to a compatible value for field %2(%3)." ).arg( value.toString(), fld.name(), fld.typeName(), value.isNull() ? "NULL" : "NOT NULL" ) );
//        return false;
//      }
//      bool success = mFeatureLayerPair.featureRef().setAttribute( index.row(), val );
//      if ( success )
//        emit dataChanged( index, index, QVector<int>() << role );
//      return success;
//      break;
//    }

//    case RememberAttribute:
//    {
//      mRememberedAttributes[ index.row() ] = value.toBool();
//      emit dataChanged( index, index, QVector<int>() << role );
//      break;
//    }
//  }

  return false;
}
