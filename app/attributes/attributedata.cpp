/***************************************************************************
 attributedata.cpp
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


#include "attributedata.h"

FormItem::FormItem(
  const QUuid &id,
  const QgsField &field,
  const QString &groupName,
  const int parentTabId,
  FormItem::FormItemType type,
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

FormItem::FormItemType FormItem::type() const
{
  return mType;
}

QString FormItem::name() const
{
  return mName;
}

bool FormItem::isEditable() const
{
  return mIsEditable;
}

QString FormItem::editorWidgetType() const
{
  return mEditorWidgetSetup.type();
}

QVariantMap FormItem::editorWidgetConfig() const
{
  return mEditorWidgetSetup.config();
}

int FormItem::fieldIndex() const
{
  return mFieldIndex;
}

bool FormItem::constraintSoftValid() const
{
  return mConstraintSoftValid;
}

void FormItem::setConstraintSoftValid( bool constraintSoftValid )
{
  mConstraintSoftValid = constraintSoftValid;
}

bool FormItem::constraintHardValid() const
{
  return mConstraintHardValid;
}

void FormItem::setConstraintHardValid( bool constraintHardValid )
{
  mConstraintHardValid = constraintHardValid;
}

bool FormItem::isVisible() const
{
  return mVisible;
}

void FormItem::setVisible( bool visible )
{
  mVisible = visible;
}

QString FormItem::constraintDescription() const
{
  return mConstraints.constraintDescription();
}

QUuid FormItem::id() const
{
  return mId;
}

int FormItem::parentTabId() const
{
  return mParentTabId;
}

QgsExpression FormItem::visibilityExpression() const
{
  return mVisibilityExpression;
}

bool FormItem::visible() const
{
  return mVisible;
}

QgsField FormItem::field() const
{
  return mField;
}

QString FormItem::groupName() const
{
  return mGroupName;
}

QVariant FormItem::originalValue() const
{
  return mOriginalValue;
}

void FormItem::setOriginalValue( const QVariant &originalValue )
{
  mOriginalValue = originalValue;
}


TabItem::TabItem( const int &id,
                  const QString &name,
                  const QVector<QUuid> &formItems,
                  const QgsExpression &visibilityExpression )
  : mTabIndex( id )
  , mName( name )
  , mFormItems( formItems )
  , mVisibilityExpression( visibilityExpression )
{
}

QString TabItem::name() const
{
  return mName;
}

bool TabItem::isVisible() const
{
  return mVisible;
}

void TabItem::setVisible( bool visible )
{
  mVisible = visible;
}

int TabItem::tabIndex() const
{
  return mTabIndex;
}

QgsExpression TabItem::visibilityExpression() const
{
  return mVisibilityExpression;
}

const QVector<QUuid> TabItem::formItems() const
{
  return mFormItems;
}
