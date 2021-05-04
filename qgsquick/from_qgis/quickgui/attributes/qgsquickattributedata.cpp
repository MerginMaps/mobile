/***************************************************************************
 qgsquickattributedata.cpp
  --------------------------------------
  Date                 : 22.4.2021
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


#include "qgsquickattributedata.h"

QgsQuickFormItemData::QgsQuickFormItemData(
  const QUuid &id,
  const QgsField &field,
  const QString &groupName,
  const int parentTabId,
  QgsQuickFormItem::FormItemType type,
  const QString &name,
  bool isEditable,
  const QgsEditorWidgetSetup &editorWidgetSetup,
  int fieldIndex,
  const QgsFieldConstraints &contraints,
  const QgsExpression &visibilityExpression
)
  : mId( id )
  , mField( field )
  , mGroupName( groupName )
  , mParentTabId( parentTabId )
  , mType( type )
  , mName( name )
  , mIsEditable( isEditable )
  , mEditorWidgetSetup( editorWidgetSetup )
  , mFieldIndex( fieldIndex )
  , mConstraints( contraints )
  , mVisibilityExpression( visibilityExpression )
{
}

QgsQuickFormItem::FormItemType QgsQuickFormItemData::type() const
{
  return mType;
}

QString QgsQuickFormItemData::name() const
{
  return mName;
}

bool QgsQuickFormItemData::isEditable() const
{
  return mIsEditable;
}

QString QgsQuickFormItemData::editorWidgetType() const
{
  return mEditorWidgetSetup.type();
}

QVariantMap QgsQuickFormItemData::editorWidgetConfig() const
{
  return mEditorWidgetSetup.config();
}

int QgsQuickFormItemData::fieldIndex() const
{
  return mFieldIndex;
}

bool QgsQuickFormItemData::constraintSoftValid() const
{
  return mConstraintSoftValid;
}

void QgsQuickFormItemData::setConstraintSoftValid( bool constraintSoftValid )
{
  mConstraintSoftValid = constraintSoftValid;
}

bool QgsQuickFormItemData::constraintHardValid() const
{
  return mConstraintHardValid;
}

void QgsQuickFormItemData::setConstraintHardValid( bool constraintHardValid )
{
  mConstraintHardValid = constraintHardValid;
}

bool QgsQuickFormItemData::isVisible() const
{
  return mVisible;
}

void QgsQuickFormItemData::setVisible( bool visible )
{
  mVisible = visible;
}

QString QgsQuickFormItemData::constraintDescription() const
{
  return mConstraints.constraintDescription();
}

QUuid QgsQuickFormItemData::id() const
{
  return mId;
}

int QgsQuickFormItemData::parentTabId() const
{
  return mParentTabId;
}

QgsExpression QgsQuickFormItemData::visibilityExpression() const
{
  return mVisibilityExpression;
}

bool QgsQuickFormItemData::visible() const
{
  return mVisible;
}

QgsField QgsQuickFormItemData::field() const
{
  return mField;
}

QString QgsQuickFormItemData::groupName() const
{
  return mGroupName;
}

QVariant QgsQuickFormItemData::originalValue() const
{
  return mOriginalValue;
}

void QgsQuickFormItemData::setOriginalValue( const QVariant &originalValue )
{
  mOriginalValue = originalValue;
}


QgsQuickTabItem::QgsQuickTabItem( const int &id,
                                  const QString &name,
                                  const QVector<QUuid> &formItems,
                                  const QgsExpression &visibilityExpression )
  : mId( id )
  , mName( name )
  , mFormItems( formItems )
  , mVisibilityExpression( visibilityExpression )
{
}

QString QgsQuickTabItem::name() const
{
  return mName;
}

bool QgsQuickTabItem::isVisible() const
{
  return mVisible;
}

void QgsQuickTabItem::setVisible( bool visible )
{
  mVisible = visible;
}

int QgsQuickTabItem::id() const
{
  return mId;
}

QgsExpression QgsQuickTabItem::visibilityExpression() const
{
  return mVisibilityExpression;
}

const QVector<QUuid> QgsQuickTabItem::formItems() const
{
  return mFormItems;
}

QgsQuickFormItem::QgsQuickFormItem()
  : mShouldRememberValue( false )
{
}

QgsQuickFormItem::QgsQuickFormItem(
  std::shared_ptr<QgsQuickFormItemData> item,
  bool shouldRememberValue,
  QVariant value )
  : mItem( item )
  , mShouldRememberValue( shouldRememberValue )
  , mValue( value )
{
}

QgsQuickFormItem::~QgsQuickFormItem() = default;

QVariant QgsQuickFormItem::value() const
{
  return mValue;
}

bool QgsQuickFormItem::shouldRememberValue() const
{
  return mShouldRememberValue;
}

bool QgsQuickFormItem::isModified() const
{
  return mValue != mItem->originalValue();
}

bool QgsQuickFormItem::isValid() const
{
  return mItem.get();
}

QgsQuickFormItem::FormItemType QgsQuickFormItem::type() const
{
  if ( isValid() )
    return mItem->type();
  else
    return FormItemType::Invalid;
}

QString QgsQuickFormItem::name() const
{
  if ( isValid() )
    return mItem->name();
  else
    return QString();
}

bool QgsQuickFormItem::isEditable() const
{
  if ( isValid() )
    return mItem->isEditable();
  else
    return false;
}

QString QgsQuickFormItem::editorWidgetType() const
{
  if ( isValid() )
    return mItem->editorWidgetType();
  else
    return QString();
}

QVariantMap QgsQuickFormItem::editorWidgetConfig() const
{
  if ( isValid() )
    return mItem->editorWidgetConfig();
  else
    return QVariantMap();
}

int QgsQuickFormItem::fieldIndex() const
{
  if ( isValid() )
    return mItem->fieldIndex();
  else
    return -1;
}

bool QgsQuickFormItem::constraintSoftValid() const
{
  if ( isValid() )
    return mItem->constraintSoftValid();
  else
    return false;
}

bool QgsQuickFormItem::constraintHardValid() const
{
  if ( isValid() )
    return mItem->constraintHardValid();
  else
    return false;
}

bool QgsQuickFormItem::isVisible() const
{
  if ( isValid() )
    return mItem->isVisible();
  else
    return false;
}

QString QgsQuickFormItem::constraintDescription() const
{
  if ( isValid() )
    return mItem->constraintDescription();
  else
    return QString();
}

QUuid QgsQuickFormItem::id() const
{
  if ( isValid() )
    return mItem->id();
  else
    return QUuid();
}

int QgsQuickFormItem::parentTabId() const
{
  if ( isValid() )
    return mItem->parentTabId();
  else
    return -1;
}

QgsExpression QgsQuickFormItem::visibilityExpression() const
{
  if ( isValid() )
    return mItem->visibilityExpression();
  else
    return QgsExpression();
}

QgsField QgsQuickFormItem::field() const
{
  if ( isValid() )
    return mItem->field();
  else
    return QgsField();
}

QString QgsQuickFormItem::groupName() const
{
  if ( isValid() )
    return mItem->groupName();
  else
    return QString();
}

QVariant QgsQuickFormItem::originalValue() const
{
  if ( isValid() )
    return mItem->originalValue();
  else
    return QVariant();
}
