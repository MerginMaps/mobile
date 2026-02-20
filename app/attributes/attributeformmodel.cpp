/***************************************************************************
 attributeformmodel.cpp
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

#include "attributeformmodel.h"
#include "attributecontroller.h"
#include "attributedata.h"
#include "mixedattributevalue.h"

AttributeFormModel::AttributeFormModel( QObject *parent, AttributeController *controller, const QVector<QUuid> &data )
  : QAbstractListModel( parent )
  , mController( controller )
  , mData( data )
{
  Q_ASSERT( mController );
  connect( mController, &AttributeController::formDataChanged, this, &AttributeFormModel::onFormDataChanged );
  connect( mController, &AttributeController::featureLayerPairChanged, this, &AttributeFormModel::onFeatureChanged );
}

AttributeFormModel::~AttributeFormModel() = default;

int AttributeFormModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mData.size();
}

QVariant AttributeFormModel::data( const QModelIndex &index, int role ) const
{
  Q_ASSERT( mController );
  if ( !index.isValid() )
    return QVariant();

  const int row = index.row();
  if ( !rowIsValid( row ) )
    return QVariant();

  const QUuid uuid = mData[row];
  const FormItem *item = mController->formItem( uuid );
  if ( !item )
    return QVariant();

  switch ( role )
  {
    case Name:
      return item->name();
    case ShowName:
      return item->showName();
    case Type:
      return item->type();
    case AttributeValue:
      return mController->formValue( item->fieldIndex() );
    case RawValueIsNull:
      return item->rawValue().isNull();
    case AttributeEditable:
      return item->isEditable();
    case AttributeFormModel::EditorWidget:
      return item->editorWidgetType();
    case AttributeFormModel::EditorWidgetConfig:
      return item->editorWidgetConfig();
    case AttributeFormModel::RememberValue:
      return mController->formShouldRememberValue( item->fieldIndex() );
    case AttributeFormModel::Field:
      return item->field();
    case FieldIndex:
      return item->fieldIndex();
    case AttributeFormModel::Group:
      return item->groupName();
    case Visible:
      return item->isVisible();
    case ValidationMessage:
      return item->validationMessage();
    case ValidationStatus:
      return item->validationStatus();
    case Relation:
      return QVariant::fromValue( item->relation() );
    case NmRelation:
      return QVariant::fromValue( item->nmRelation() );
    case RawValue:
      return item->rawValue();
    case HasMixedValues:
      return item->rawValue().userType() == qMetaTypeId< MixedAttributeValue >();

    default:
      return QVariant();
  }
}

void AttributeFormModel::onFormDataChanged( const QUuid id, const QVector<int> roles )
{
  const int row = mData.indexOf( id );
  if ( rowIsValid( row ) )
  {
    const QModelIndex modelIndex = index( row, 0 );
    emit dataChanged( modelIndex, modelIndex, roles );
  }
}

void AttributeFormModel::onFeatureChanged()
{
  if ( rowCount() > 0 )
    emit dataChanged( index( 0, 0 ), index( rowCount() - 1, 0 ) );
}

bool AttributeFormModel::rowIsValid( int row ) const
{
  return ( row >= 0 ) && ( row < mData.size() );
}

QHash<int, QByteArray> AttributeFormModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

  roles[Type] = QByteArray( "Type" );
  roles[Name] = QByteArray( "Name" );
  roles[ShowName] = QByteArray( "ShowName" );
  roles[AttributeValue] = QByteArray( "AttributeValue" );
  roles[RawValueIsNull] = QByteArray( "RawValueIsNull" );
  roles[AttributeEditable] = QByteArray( "AttributeEditable" );
  roles[EditorWidget] = QByteArray( "EditorWidget" );
  roles[EditorWidgetConfig] = QByteArray( "EditorWidgetConfig" );
  roles[RememberValue] = QByteArray( "RememberValue" );
  roles[Field] = QByteArray( "Field" );
  roles[FieldIndex] = QByteArray( "FieldIndex" );
  roles[Group] = QByteArray( "Group" );
  roles[ValidationMessage] = QByteArray( "ValidationMessage" );
  roles[ValidationStatus] = QByteArray( "ValidationStatus" );
  roles[Relation] = QByteArray( "Relation" );
  roles[RawValue] = QByteArray( "RawValue" );
  roles[HasMixedValues] = QByteArray( "HasMixedValues" );
  roles[NmRelation] = QByteArray( "NmRelation" );

  return roles;
}

Qt::ItemFlags AttributeFormModel::flags( const QModelIndex &index ) const
{
  const int row = index.row();
  if ( !rowIsValid( row ) )
  {
    return Qt::ItemFlags();
  }

  Qt::ItemFlags ret = Qt::ItemIsEnabled;
  const QUuid uuid = mData[row];
  const FormItem *item = mController->formItem( uuid );
  if ( item && item->isEditable() )
    ret |= Qt::ItemIsEditable;

  return ret;
}

bool AttributeFormModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  const int row = index.row();
  if ( !rowIsValid( row ) )
  {
    return false;
  }
  const QUuid uuid = mData[row];

  switch ( role )
  {
    case RememberValue:
    {
      bool shouldRememberValue = value.toBool();
      return mController->setFormShouldRememberValue( uuid, shouldRememberValue );
    }

    case AttributeValue:
    {
      const FormItem *item = mController->formItem( uuid );
      //if ( mController->formValue( item->fieldIndex() ) == value )
      if ( item->rawValue() == value )
      {
        return false;
      }
      return mController->setFormValue( uuid, value );
    }

    default:
      return false;
  }
}
