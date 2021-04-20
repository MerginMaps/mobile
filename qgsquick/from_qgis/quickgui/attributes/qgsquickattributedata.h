/***************************************************************************
 qgsquickattributedata.h
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

#ifndef QGSQUICKATTRIBUTEDATA_H
#define QGSQUICKATTRIBUTEDATA_H

#include "qgis_quick.h"
#include <QString>
#include <QVariant>
#include <QVector>
#include <QUuid>
#include <QVariantMap>
#include <QObject>

#include "qgsfieldconstraints.h"
#include "qgseditorwidgetsetup.h"
#include "qgsexpression.h"
#include "qgsfield.h"

class QUICK_EXPORT QgsQuickFormItem
{
    Q_GADGET

  public:
    enum FormItemType
    {
      Container = 1,
      Relation,
      Field
    };
    Q_ENUMS( FormItemType )

    QgsQuickFormItem(
      const QUuid &id,
      const QgsField &field,
      const QString &groupName,
      int parentTabId,
      FormItemType type,
      const QString &name,
      bool isEditable,
      const QgsEditorWidgetSetup &editorWidgetSetup,
      int fieldIndex,
      const QgsFieldConstraints &contraints,
      const QgsExpression &visibilityExpression
    );


    FormItemType type() const;
    QString name() const;
    bool isEditable() const;
    QString editorWidgetType() const;
    QVariantMap editorWidgetConfig() const;
    int fieldIndex() const;

    QVariant value() const;

    // also stores the last value in lastValue()
    void setValue( const QVariant &value );

    bool shouldRememberValue() const;
    void setShouldRememberValue( bool shouldRememberValue );

    bool constraintSoftValid() const;
    void setConstraintSoftValid( bool constraintSoftValid );

    bool constraintHardValid() const;
    void setConstraintHardValid( bool constraintHardValid );

    bool isVisible() const;
    void setVisible( bool visible );

    QString constraintDescription() const;

    QVariant lastValue() const;

    QUuid id() const;

    int parentTabId() const;

    QgsExpression visibilityExpression() const;

    bool visible() const;

    QgsField field() const;

    QString groupName() const;

  private:
    const QUuid mId;
    const QgsField mField;
    const QString mGroupName; //empty for no group, group/tab name if widget is in container
    const int mParentTabId;
    const FormItemType mType;
    const QString mName;
    const bool mIsEditable;
    const QgsEditorWidgetSetup mEditorWidgetSetup;
    const int mFieldIndex;
    const QgsFieldConstraints mConstraints;
    const QgsExpression mVisibilityExpression;

    QVariant mValue;
    QVariant mLastValue;
    bool mShouldRememberValue = false;
    bool mConstraintSoftValid = false;
    bool mConstraintHardValid = false;
    bool mVisible = false;
};

class QUICK_EXPORT QgsQuickTabItem
{
  public:
    QgsQuickTabItem( const int &id,
                     const QString &name,
                     const QVector<QUuid> &formItems,
                     const QgsExpression &visibilityExpression
                   );

    QString name() const;
    const QVector<QUuid> formItems() const;

    bool isVisible() const;
    void setVisible( bool visible );

    int id() const;

    QgsExpression visibilityExpression() const;

  private:
    const int mId;
    const QString mName;
    const QVector<QUuid> mFormItems;
    const QgsExpression mVisibilityExpression;
    bool mVisible = false;
};


#endif // QGSQUICKATTRIBUTEDATA_H
