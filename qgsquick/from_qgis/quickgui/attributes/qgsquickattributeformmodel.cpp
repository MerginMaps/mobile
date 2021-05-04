/***************************************************************************
 qgsquickattributeformmodel.cpp
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

#include "qgsquickattributeformmodel.h"
#include "qgsquickattributecontroller.h"
#include "qgsquickattributedata.h"

QgsQuickAttributeFormModel::QgsQuickAttributeFormModel( QObject *parent, QgsQuickAttributeController *controller, const QVector<QUuid> &data )
  : QAbstractListModel( parent )
  , mController( controller )
  , mData( data )
{
  Q_ASSERT( mController );
  connect( mController, &QgsQuickAttributeController::formDataChanged, this, &QgsQuickAttributeFormModel::onFormDataChanged );
  connect( mController, &QgsQuickAttributeController::featureLayerPairChanged, this, &QgsQuickAttributeFormModel::onFeatureChanged );
}

QgsQuickAttributeFormModel::~QgsQuickAttributeFormModel() = default;

int QgsQuickAttributeFormModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )
  return mData.size();
}

QVariant QgsQuickAttributeFormModel::data( const QModelIndex &index, int role ) const
{
  Q_ASSERT( mController );
  if ( !index.isValid() )
    return QVariant();

  const int row = index.row();
  if ( !rowIsValid( row ) )
    return QVariant();

  const QUuid uuid = mData[row];
  const QgsQuickFormItem *item = mController->formItem( uuid );
  if ( !item )
    return QVariant();

  switch ( role )
  {
    case Name:
      return item->name();
    case Type:
      return item->type();
    case AttributeValue:
      return item->value();
    case AttributeEditable:
      return item->isEditable();
    case QgsQuickAttributeFormModel::EditorWidget:
      return item->editorWidgetType();
    case QgsQuickAttributeFormModel::EditorWidgetConfig:
      return item->editorWidgetConfig();
    case QgsQuickAttributeFormModel::RememberValue:
      return item->shouldRememberValue();
    case QgsQuickAttributeFormModel::Field:
      return item->field();
    case FieldIndex:
      return item->fieldIndex();
    case QgsQuickAttributeFormModel::Group:
      return item->groupName();
    case Visible:
      return item->isVisible();
    case ConstraintSoftValid:
      return item->constraintSoftValid();
    case ConstraintHardValid:
      return item->constraintHardValid();
    case ConstraintDescription:
      return item->constraintDescription();
    default:
      return QVariant();
  }
}

void QgsQuickAttributeFormModel::onFormDataChanged( const QUuid id )
{
  const int row = mData.indexOf( id );
  if ( rowIsValid( row ) )
  {
    const QModelIndex modelIndex = index( row, 0 );
    emit dataChanged( modelIndex, modelIndex );
  }
}

void QgsQuickAttributeFormModel::onFeatureChanged()
{
  if ( rowCount() > 0 )
    emit dataChanged( index( 0, 0 ), index( rowCount() - 1, 0 ) );
}

bool QgsQuickAttributeFormModel::rowIsValid( int row ) const
{
  return ( row >= 0 ) && ( row < mData.size() );
}

QHash<int, QByteArray> QgsQuickAttributeFormModel::roleNames() const
{
  QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

  roles[Type]  = QByteArray( "Type" );
  roles[Name]  = QByteArray( "Name" );
  roles[AttributeValue] = QByteArray( "AttributeValue" );
  roles[AttributeEditable] = QByteArray( "AttributeEditable" );
  roles[EditorWidget] = QByteArray( "EditorWidget" );
  roles[EditorWidgetConfig] = QByteArray( "EditorWidgetConfig" );
  roles[RememberValue] = QByteArray( "RememberValue" );
  roles[QgsQuickAttributeFormModel::Field] = QByteArray( "Field" );
  roles[QgsQuickAttributeFormModel::Group] = QByteArray( "Group" );
  roles[ConstraintHardValid] = QByteArray( "ConstraintHardValid" );
  roles[ConstraintSoftValid] = QByteArray( "ConstraintSoftValid" );
  roles[ConstraintDescription] = QByteArray( "ConstraintDescription" );

  return roles;
}

Qt::ItemFlags QgsQuickAttributeFormModel::flags( const QModelIndex &index ) const
{
  const int row = index.row();
  if ( !rowIsValid( row ) )
  {
    return Qt::ItemFlags();
  }

  Qt::ItemFlags ret = Qt::ItemIsEnabled;
  const QUuid uuid = mData[row];
  const QgsQuickFormItem *item = mController->formItem( uuid );

  if ( item->isEditable() )
    ret |= Qt::ItemIsEditable;

  return ret;
}

bool QgsQuickAttributeFormModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
  const int row = index.row();
  if ( !rowIsValid( row ) )
  {
    return false;
  }
  const QUuid uuid = mData[row];

  switch ( role )
  {
    // TODO
    case RememberValue:
    {
      bool shouldRememberValue = value.toBool();
      return mController->setFormShouldRememberValue( uuid, shouldRememberValue );
    }

    case AttributeValue:
    {
      bool shouldRememberValue = value.toBool();
      return mController->setFormValue( uuid, shouldRememberValue );
    }

    default:
      return false;
  }
}
