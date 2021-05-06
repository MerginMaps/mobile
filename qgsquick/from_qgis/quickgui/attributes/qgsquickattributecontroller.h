/***************************************************************************
 qgsquickattributecontroller.h
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

#ifndef QGSQUICKATTRIBUTECONTROLLER_H
#define QGSQUICKATTRIBUTECONTROLLER_H

#include <QSortFilterProxyModel>
#include <QObject>
#include <QVariant>
#include <memory>
#include <QMap>
#include <QVector>
#include <QUuid>

#include "qgis_quick.h"
#include "qgsquickfeaturelayerpair.h"
#include "qgsquickattributedata.h"

#include "qgsfeature.h"
#include "qgseditformconfig.h"
#include "qgsexpressioncontext.h"
#include "qgsattributeeditorcontainer.h"

class QgsQuickAttributeFormModel;
class QgsQuickAttributeFormProxyModel;
class QgsQuickAttributeTabModel;
class QgsQuickAttributeTabProxyModel;
class QgsVectorLayer;
class QgsQuickRememberAttributes;

/**
 * \ingroup quick
 *
 * This is implementation of the controller between QgsQuickAttribute*Model
 * and the QML's QgsQuick.FeatureForm. It is responsible for creation of the
 * models, and CRUD management of the edits. It connects the QgsFeature Fields
 * on the backend side and the QML widgets on the user side
 *
 * \note QML Type: AttributeController
 *
 * \since QGIS 3.22
 */
class QUICK_EXPORT QgsQuickAttributeController : public QObject
{
    Q_OBJECT

    /**
     * QgsQuickFeatureLayerPair for the model. Input for attributes model.
     */
    Q_PROPERTY( QgsQuickFeatureLayerPair featureLayerPair READ featureLayerPair WRITE setFeatureLayerPair NOTIFY featureLayerPairChanged )

    // this is newer nullprt
    Q_PROPERTY( QgsQuickAttributeTabProxyModel *attributeTabProxyModel READ attributeTabProxyModel  NOTIFY attributeTabProxyModelChanged )

    //! Returns TRUE if remembering values is allowed
    Q_PROPERTY( bool hasTabs READ hasTabs NOTIFY hasTabsChanged )

    //! Returns TRUE if has any changes
    Q_PROPERTY( bool hasAnyChanges READ hasAnyChanges NOTIFY hasAnyChangesChanged )

    Q_PROPERTY( QgsQuickRememberAttributes *rememberAttributes READ rememberAttributes WRITE setRememberAttributes NOTIFY rememberAttributesChanged )

    //! Returns TRUE if all hard constraints defined on fields are satisfied with the current attribute values
    Q_PROPERTY( bool constraintsHardValid READ constraintsHardValid NOTIFY constraintsHardValidChanged )

    //! Returns TRUE if all soft constraints defined on fields are satisfied with the current attribute values
    Q_PROPERTY( bool constraintsSoftValid READ constraintsSoftValid NOTIFY constraintsSoftValidChanged )

  public:
    QgsQuickAttributeController( QObject *parent = nullptr );
    ~QgsQuickAttributeController() override;

    //! Restore clean/initial state: no layer, no feature!
    Q_INVOKABLE void forceClean();

    //! Gets current featureLayerPair
    QgsQuickFeatureLayerPair featureLayerPair() const;
    //! Sets current featureLayerPair
    void setFeatureLayerPair( const QgsQuickFeatureLayerPair &pair );

    bool constraintsHardValid() const;
    bool constraintsSoftValid() const;
    bool hasTabs() const;
    bool hasAnyChanges();

    QgsQuickAttributeTabProxyModel *attributeTabProxyModel() const;
    QgsQuickAttributeFormProxyModel *attributeFormProxyModelForTab( int tabRow ) const;

    Q_INVOKABLE bool deleteFeature();
    Q_INVOKABLE void create();
    Q_INVOKABLE bool save();

    int tabCount() const;

    // for data modification use setForm*Value() functions
    QgsQuickFormItem formItem( const QUuid &id ) const;

    // for data modification use setTab*Value() functions;
    const QgsQuickTabItem *tabItem( int tabRow ) const;

    // Returns true if successful; otherwise returns false.
    bool setFormShouldRememberValue( const QUuid &id, bool shouldRememberValue );
    // Returns true if successful; otherwise returns false.
    bool setFormValue( const QUuid &id, QVariant value );

    QgsQuickRememberAttributes *rememberAttributes() const;
    void setRememberAttributes( QgsQuickRememberAttributes *rememberAttributes );

  signals:
    void hasAnyChangesChanged();
    void rememberAttributesChanged();
    void featureLayerPairChanged();
    void attributeTabProxyModelChanged();
    void constraintsHardValidChanged();
    void constraintsSoftValidChanged();
    void hasTabsChanged();

    void formDataChanged( QUuid uuid );
    void tabDataChanged( int id );

    void dataChangedFailed( const QString &msg );
    void dataChanged( const QString &msg ); // TODO what is this?? feature for signal...

  private:
    void clearAll();

    void setHasAnyChanges( bool hasChanges );

    bool isValidTabId( int id ) const;
    bool isValidFormId( const QUuid &id ) const;

    bool startEditing();
    bool commit();

    void updateOnLayerChange();
    void updateOnFeatureChange();

    // visibility & constrains & default vals
    void recalculateDerivedItems();
    bool recalculateDefaultValues( QSet<QUuid> &changedFormItems, QgsExpressionContext &context );

    // generate tab
    void createTab( QgsAttributeEditorContainer *container );

    // flatten groups and tabs within tabs/groups
    void flatten( QgsAttributeEditorContainer *container,
                  int parentTabRow,
                  const QString &parentVisibilityExpressions,
                  QVector<QUuid> &items );

    // Generates fake root tab for auto-layout
    QgsAttributeEditorContainer *autoLayoutTabContainer() const;

    bool mConstraintsHardValid = false;
    bool mConstraintsSoftValid = false;
    bool mHasAnyChanges = false;
    bool mHasTabs = false;

    QgsQuickFeatureLayerPair mFeatureLayerPair;
    std::unique_ptr<QgsQuickAttributeTabProxyModel> mAttributeTabProxyModel;
    QVector<QgsQuickAttributeFormProxyModel *> mAttributeFormProxyModelForTabItem;
    QMap<QUuid, std::shared_ptr<QgsQuickFormItemData>> mFormItemsData; // order of fields in tab is in tab item
    QVector<std::shared_ptr<QgsQuickTabItem>> mTabItems; // order of tabs by tab row number

    QgsQuickRememberAttributes *mRememberAttributes; // not owned

};
#endif // QGSQUICKATTRIBUTECONTROLLER_H
