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

QgsQuickFormItem::QgsQuickFormItem(
  const QUuid &id,
  const QgsField &field,
  const QString &groupName,
  const int parentTabId,
  FormItemType type,
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

QgsQuickFormItem::FormItemType QgsQuickFormItem::type() const
{
  return mType;
}

QString QgsQuickFormItem::name() const
{
  return mName;
}

bool QgsQuickFormItem::isEditable() const
{
  return mIsEditable;
}

QString QgsQuickFormItem::editorWidgetType() const
{
  return mEditorWidgetSetup.type();
}

QVariantMap QgsQuickFormItem::editorWidgetConfig() const
{
  return mEditorWidgetSetup.config();
}

int QgsQuickFormItem::fieldIndex() const
{
  return mFieldIndex;
}

QVariant QgsQuickFormItem::value() const
{
  return mValue;
}

void QgsQuickFormItem::setValue( const QVariant &value )
{
  mLastValue = mValue;
  mValue = value;
}

bool QgsQuickFormItem::shouldRememberValue() const
{
  return mShouldRememberValue;
}

void QgsQuickFormItem::setShouldRememberValue( bool shouldRememberValue )
{
  mShouldRememberValue = shouldRememberValue;
}

bool QgsQuickFormItem::constraintSoftValid() const
{
  return mConstraintSoftValid;
}

void QgsQuickFormItem::setConstraintSoftValid( bool constraintSoftValid )
{
  mConstraintSoftValid = constraintSoftValid;
}

bool QgsQuickFormItem::constraintHardValid() const
{
  return mConstraintHardValid;
}

void QgsQuickFormItem::setConstraintHardValid( bool constraintHardValid )
{
  mConstraintHardValid = constraintHardValid;
}

bool QgsQuickFormItem::isVisible() const
{
  return mVisible;
}

void QgsQuickFormItem::setVisible( bool visible )
{
  mVisible = visible;
}

QString QgsQuickFormItem::constraintDescription() const
{
  return mConstraints.constraintDescription();
}

QVariant QgsQuickFormItem::lastValue() const
{
  return mLastValue;
}

QUuid QgsQuickFormItem::id() const
{
  return mId;
}

int QgsQuickFormItem::parentTabId() const
{
  return mParentTabId;
}

QgsExpression QgsQuickFormItem::visibilityExpression() const
{
  return mVisibilityExpression;
}

bool QgsQuickFormItem::visible() const
{
  return mVisible;
}

QgsField QgsQuickFormItem::field() const
{
  return mField;
}

QString QgsQuickFormItem::groupName() const
{
  return mGroupName;
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
