/***************************************************************************
 attributecontroller.h
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

#ifndef ATTRIBUTECONTROLLER_H
#define ATTRIBUTECONTROLLER_H

#include <QSortFilterProxyModel>
#include <QObject>
#include <QVariant>
#include <memory>
#include <QMap>
#include <QVector>
#include <QUuid>

#include "inputconfig.h"
#include "featurelayerpair.h"
#include "attributedata.h"
#include "attributeformproxymodel.h"
#include "attributetabproxymodel.h"
#include "rememberattributescontroller.h"

#include "qgsfeature.h"
#include "qgsproject.h"
#include "qgseditformconfig.h"
#include "qgsexpressioncontext.h"
#include "qgsattributeeditorcontainer.h"
#include "variablesmanager.h"

class AttributeFormModel;
class AttributeTabModel;
class QgsVectorLayer;

/**
 * This is implementation of the controller between Attribute*Model
 * and the QML's .FeatureForm. It is responsible for creation of the
 * models, and CRUD management between QgsFeature and the form. It connects
 * the QgsFeature Fields on the backend side and the QML widgets on the user side
 *
 * Remember values functionality is done by RememberAttributes controller.
 *
 * \note QML Type: AttributeController
 */
class  AttributeController : public QObject
{
    Q_OBJECT

    /**
     * FeatureLayerPair for the model. Input for attributes model.
     */
    Q_PROPERTY( FeatureLayerPair featureLayerPair READ featureLayerPair WRITE setFeatureLayerPair NOTIFY featureLayerPairChanged )

    // this is never nullptr (a form with no tabs will be parsed as having a single tab)
    Q_PROPERTY( AttributeTabProxyModel *attributeTabProxyModel READ attributeTabProxyModel  NOTIFY attributeTabProxyModelChanged )

    //! Returns TRUE if form is in Drag&Drop mode and has any tabs
    Q_PROPERTY( bool hasTabs READ hasTabs NOTIFY hasTabsChanged )

    //! Returns TRUE if has any changes
    Q_PROPERTY( bool hasAnyChanges READ hasAnyChanges NOTIFY hasAnyChangesChanged )

    Q_PROPERTY( RememberAttributesController *rememberAttributesController READ rememberAttributesController WRITE setRememberAttributesController NOTIFY rememberAttributesChanged )

    //! Returns extended expression context for attributes evaluation.
    Q_PROPERTY( VariablesManager *variablesManager READ variablesManager WRITE setVariablesManager NOTIFY variablesManagerChanged )

    //! Returns TRUE if any field do not pass necessary validation, false otherwise
    Q_PROPERTY( bool hasValidationErrors READ hasValidationErrors NOTIFY hasValidationErrorsChanged )

    /**
     * If the featureLayerPair in this controller is a child feature in relation, it will have associated parent AttributeController saved in this property.
     * parent together with linkedRelation is used to automatically prefill fields containing foreign key to parent table (relation reference fields)
     */
    Q_PROPERTY( AttributeController *parentController READ parentController WRITE setParentController NOTIFY parentControllerChanged )

    /**
     * If featureLayerPair is a child feature in relation, it will have associated relation saved in this property
     * see parentController documentation for more info
     */
    Q_PROPERTY( QgsRelation linkedRelation READ linkedRelation WRITE setLinkedRelation NOTIFY linkedRelationChanged )


  public:
    AttributeController( QObject *parent = nullptr );
    ~AttributeController() override;

    //! Restore clean/initial state: no layer, no feature!
    Q_INVOKABLE void reset();

    //! Gets current featureLayerPair
    FeatureLayerPair featureLayerPair() const;
    //! Sets current featureLayerPair
    void setFeatureLayerPair( const FeatureLayerPair &pair );

    bool constraintsHardValid() const;
    bool constraintsSoftValid() const;
    bool hasValidationErrors() const;
    bool hasTabs() const;
    bool hasAnyChanges() const;

    AttributeTabProxyModel *attributeTabProxyModel() const;
    AttributeFormProxyModel *attributeFormProxyModelForTab( int tabRow ) const;

    Q_INVOKABLE bool deleteFeature();
    Q_INVOKABLE bool rollback();
    Q_INVOKABLE bool save();
    Q_INVOKABLE void acquireId();

    int tabCount() const;

    // for data modification use setForm*Value() functions
    const FormItem *formItem( const QUuid &id ) const;

    // for data modification use setTab*Value() functions;
    const TabItem *tabItem( int tabRow ) const;

    // Returns true if successful; otherwise returns false.
    bool setFormShouldRememberValue( const QUuid &id, bool shouldRememberValue );

    bool formShouldRememberValue( int fieldIndex ) const;

    // Returns true if successful; otherwise returns false.
    bool setFormValue( const QUuid &id, QVariant value );

    QVariant formValue( int fieldIndex ) const;

    RememberAttributesController *rememberAttributesController() const;
    void setRememberAttributesController( RememberAttributesController *rememberAttributes );

    VariablesManager *variablesManager() const;
    void setVariablesManager( VariablesManager *variablesManager );

    AttributeController *parentController() const;
    void setParentController( AttributeController *newParentController );

    const QgsRelation &linkedRelation() const;
    void setLinkedRelation( const QgsRelation &newLinkedRelation );

  public slots:
    void onFeatureAdded( QgsFeatureId newFeatureId );

  signals:
    void hasAnyChangesChanged();
    void rememberAttributesChanged();
    void featureLayerPairChanged();
    void attributeTabProxyModelChanged();
    void hasTabsChanged();
    void variablesManagerChanged();
    void hasValidationErrorsChanged();
    void parentControllerChanged();
    void linkedRelationChanged();

    void formDataChanged( QUuid uuid, QVector<int> roles = QVector<int>() );
    void tabDataChanged( int id );
    void formRecalculated();
    void featureIdChanged();
    void changesCommited();
    void commitFailed();

  private:
    void clearAll();

    void setHasAnyChanges( bool hasChanges );
    void setHasValidationErrors( bool hasErrors );
    void discoverRelations( QgsAttributeEditorContainer *container );

    bool isValidTabId( int id ) const;
    bool isValidFormId( const QUuid &id ) const;

    bool startEditing();
    bool commit();

    void updateOnLayerChange();
    void updateOnFeatureChange();

    bool isNewFeature() const;

    /**
     * Recalculates visibility & constrains & default values
     * Note that reevaluate default values is needed only when an attribnute has changed.
     * Evaluation of default values for a new feature is done in digitizing controller when a feature is created.
     * @param isFormValueChange True if recalculation has to be done after an attribute has changed (called by setFormValue function).
     */
    void recalculateDerivedItems( bool isFormValueChange = false, bool isFirstUpdateOfNewFeature = false );
    void recalculateRichTextWidgets( QSet<QUuid> &changedFormItems, QgsExpressionContext &context );
    void recalculateDefaultValues( QSet<QUuid> &changedFormItems, QgsExpressionContext &context, bool isFormValueChange = false, bool isFirstUpdateOfNewFeature = false );
    void recalculateVirtualFields( QSet<QUuid> &changedFormItems, QgsExpressionContext &expressionContext );
    void evaluateExpressionAndUpdateValue( QSet<QUuid> &changedFormItems,
                                           const QString &expressionString, QgsExpressionContext &expressionContext, int fieldIndex, const QgsField &field, std::shared_ptr<FormItem> formItem );

    // generate tab
    void createTab( QgsAttributeEditorContainer *container );

    // flatten groups and tabs within tabs/groups
    void flatten( QgsAttributeEditorContainer *container,
                  int parentTabRow,
                  const QString &parentVisibilityExpressions,
                  QVector<QUuid> &items );

    // Generates fake root tab for auto-layout
    QgsAttributeEditorContainer *autoLayoutTabContainer() const;
    //! Returns editor widget setup according params. If is empty, returns a default setup according field's type.
    QgsEditorWidgetSetup getEditorWidgetSetup( QgsVectorLayer *layer, int fieldIndex ) const;

    //! Fills up relation reference (foreign key) field with parent id
    void prefillRelationReferenceField();

    /**
     * Checks if tab layout is allowed for given container. Function is not recursive and checks only first level elements.
     * @param container Suppose to be the root of attributeEditor container.
     * @return True if there are only tabs and all other elements are within tabs, otherwise false.
     */
    bool allowTabs( QgsAttributeEditorContainer *container );

    //! renames photos if necessary
    void renamePhotos();
    //! save temporary sketched image to original image
    void saveSketches();

    bool mConstraintsHardValid = false;
    bool mConstraintsSoftValid = false;
    bool mHasValidationErrors = false;
    bool mHasAnyChanges = false;
    bool mHasTabs = false;

    FeatureLayerPair mFeatureLayerPair;
    std::unique_ptr<AttributeTabProxyModel> mAttributeTabProxyModel;
    QVector<AttributeFormProxyModel *> mAttributeFormProxyModelForTabItem;
    QMap<QUuid, std::shared_ptr<FormItem>> mFormItems; // order of fields in tab is in tab item
    QVector<std::shared_ptr<TabItem>> mTabItems; // order of tabs by tab row number
    QSet<int> mExpressionFieldsOutsideForm; // indices of fields with expressions which are outside of the form
    QSet<int> mVirtualFieldsOutsideForm; // indices of virtual fields which are outside of the form

    RememberAttributesController *mRememberAttributesController = nullptr; // not owned
    VariablesManager *mVariablesManager = nullptr; // not owned

    AttributeController *mParentController = nullptr; // not owned
    QgsRelation mLinkedRelation;
};
#endif // ATTRIBUTECONTROLLER_H
