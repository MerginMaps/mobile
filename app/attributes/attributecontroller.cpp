/***************************************************************************
 attributecontroller.cpp
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

#include "attributecontroller.h"
#include "attributeformmodel.h"
#include "attributeformproxymodel.h"
#include "attributetabmodel.h"
#include "attributetabproxymodel.h"
#include "rememberattributescontroller.h"

#include <QDebug>
#include <QSet>
#include "qgsvectorlayer.h"
#include "qgsattributeeditorfield.h"
#include "qgsattributeeditorrelation.h"
#include "qgsattributeeditorcontainer.h"
#include "qgsvectorlayerutils.h"
#include "qgsrelation.h"
#include "qgsmessagelog.h"
#include "inpututils.h"
#include "coreutils.h"

AttributeController::AttributeController( QObject *parent )
  : QObject( parent )
  , mAttributeTabProxyModel( new AttributeTabProxyModel() )
{
}

void AttributeController::reset()
{
  setFeatureLayerPair( FeatureLayerPair() );
}

AttributeController::~AttributeController() = default;


FeatureLayerPair AttributeController::featureLayerPair() const
{
  return mFeatureLayerPair;
}

void AttributeController::setFeatureLayerPair( const FeatureLayerPair &pair )
{
  if ( mFeatureLayerPair != pair )
  {
    // block signals:
    // everything should be invalidated by
    // featureLayerPairChanged & attributeTabProxyModelChanged signals
    blockSignals( true );

    bool hasLayerChanged = mFeatureLayerPair.layer() != pair.layer();
    // Set new active pair
    mFeatureLayerPair = pair;
    if ( hasLayerChanged )
    {
      // layer changed!
      updateOnLayerChange();
    }

    // feature changed!
    updateOnFeatureChange();

    // Done, emit signals
    blockSignals( false );
    if ( hasLayerChanged )
    {
      emit attributeTabProxyModelChanged();
      emit hasTabsChanged();
    }
    emit featureLayerPairChanged();
    emit constraintsHardValidChanged();
    emit constraintsSoftValidChanged();
    emit hasAnyChangesChanged();
  }
}

QgsAttributeEditorContainer *AttributeController::autoLayoutTabContainer() const  //#spellok
{
  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  Q_ASSERT( layer );

  std::unique_ptr<QgsAttributeEditorContainer> root( new QgsAttributeEditorContainer( QLatin1String( "AutoLayoutRoot" ), nullptr ) );
  root->setIsGroupBox( false ); //tab!

  const QgsFields fields = layer->fields();
  for ( int i = 0; i < fields.size(); ++i )
  {
    QgsAttributeEditorField *field = new QgsAttributeEditorField( fields.at( i ).name(), i, root.get() );
    root->addChildElement( field );
  }
  return root.release();
}

QgsEditorWidgetSetup AttributeController::getEditorWidgetSetup( QgsVectorLayer *layer, int fieldIndex ) const
{
  QgsEditorWidgetSetup setup = layer->editorWidgetSetup( fieldIndex );
  if ( setup.type().isEmpty() )
  {
    QgsField field = layer->fields().at( fieldIndex );
    return InputUtils::getEditorWidgetSetup( field );
  }
  return setup;
}

void AttributeController::prefillRelationReferenceField()
{
  if ( !mParentController || !mLinkedRelation.isValid() )
    return;

  const QList<QgsRelation::FieldPair> fieldPairs = mLinkedRelation.fieldPairs();
  for ( const QgsRelation::FieldPair &fieldPair : fieldPairs )
  {
    QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
    while ( formItemsIterator != mFormItems.end() )
    {
      std::shared_ptr<FormItem> itemData = formItemsIterator.value();
      if ( itemData->field().name() == fieldPair.referencingField() )
      {
        setFormValue( itemData->id(), mParentController->featureLayerPair().feature().id() );
        emit formDataChanged( itemData->id() );
        break;
      }
      ++formItemsIterator;
    }
  }
}

bool AttributeController::allowTabs( QgsAttributeEditorContainer *container )
{
  for ( QgsAttributeEditorElement *element : container->children() )
  {
    if ( element->type() == QgsAttributeEditorElement::AeTypeContainer )
    {
      QgsAttributeEditorContainer *elemContainer = static_cast<QgsAttributeEditorContainer *>( element );
      if ( elemContainer->isGroupBox() )
        return false;
    }
    else
      return false;
  }

  return !container->children().isEmpty();
}

VariablesManager *AttributeController::variablesManager() const
{
  return mVariablesManager;
}

void AttributeController::setVariablesManager( VariablesManager *variablesManager )
{
  if ( mVariablesManager != variablesManager )
  {
    mVariablesManager = variablesManager;
    emit variablesManagerChanged();
  }
}

RememberAttributesController *AttributeController::rememberAttributesController() const
{
  return mRememberAttributesController;
}

void AttributeController::setRememberAttributesController( RememberAttributesController *rememberAttributes )
{
  if ( mRememberAttributesController != rememberAttributes )
  {
    mRememberAttributesController = rememberAttributes;
    if ( mRememberAttributesController && mFeatureLayerPair.layer() )
      mRememberAttributesController->storeLayerFields( mFeatureLayerPair.layer() );
    emit rememberAttributesChanged();
  }
}

void AttributeController::flatten(
  QgsAttributeEditorContainer *container,
  int parentTabRow,
  const QString &parentVisibilityExpressions,
  QVector<QUuid> &items
)
{
  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  Q_ASSERT( layer );

  for ( QgsAttributeEditorElement *element : container->children() )
  {
    switch ( element->type() )
    {
      case QgsAttributeEditorElement::AeTypeContainer:
      {
        QString visibilityExpression = parentVisibilityExpressions;
        QgsAttributeEditorContainer *container = static_cast<QgsAttributeEditorContainer *>( element );
        if ( container->visibilityExpression().enabled() )
        {
          if ( visibilityExpression.isNull() )
            visibilityExpression = container->visibilityExpression().data().expression();
          else
            visibilityExpression += " AND " + container->visibilityExpression().data().expression();
        }

        flatten( container, parentTabRow, visibilityExpression, items );
        break;
      }

      case QgsAttributeEditorElement::AeTypeField:
      {
        QUuid fieldUuid = QUuid::createUuid();

        QgsAttributeEditorField *editorField = static_cast<QgsAttributeEditorField *>( element );
        int fieldIndex = editorField->idx();
        if ( fieldIndex < 0 || fieldIndex >= layer->fields().size() )
        {
          qDebug() << "Invalid fieldIndex for editorField!";
          continue;
        }
        QgsField field = layer->fields().at( fieldIndex );
        QStringList expressions;
        QStringList descriptions;
        QString expression = field.constraints().constraintExpression();

        if ( !expression.isEmpty() )
        {
          descriptions << field.constraints().constraintDescription();
          expressions << field.constraints().constraintExpression();
        }

        if ( field.constraints().constraints() & QgsFieldConstraints::ConstraintNotNull )
        {
          descriptions << QObject::tr( "Not NULL" );
        }

        if ( field.constraints().constraints() & QgsFieldConstraints::ConstraintUnique )
        {
          descriptions << QObject::tr( "Unique" );
        }

        bool isReadOnly = ( layer->editFormConfig().readOnly( fieldIndex ) ) ||
                          ( !field.defaultValueDefinition().expression().isEmpty() && field.defaultValueDefinition().applyOnUpdate() );

        const QString groupName = container->isGroupBox() ? container->name() : QString();
        std::shared_ptr<FormItem> formItemData =
          std::shared_ptr<FormItem>(
            FormItem::createFieldItem(
              fieldUuid,
              field,
              groupName,
              parentTabRow,
              FormItem::Field,
              layer->attributeDisplayName( fieldIndex ),
              !isReadOnly,
              getEditorWidgetSetup( layer, fieldIndex ),
              fieldIndex,
              field.constraints(),
              parentVisibilityExpressions // field doesn't have visibility expression itself
            )
          );

        mFormItems[formItemData->id()] = formItemData;


        items.append( fieldUuid );
        break;
      }

      case QgsAttributeEditorElement::AeTypeRelation:
      {
        QUuid widgetUuid = QUuid::createUuid();

        QgsAttributeEditorRelation *relationField = static_cast<QgsAttributeEditorRelation *>( element );
        QgsRelation associatedRelation = relationField->relation();

        bool isNmRelation = layer->editFormConfig().widgetConfig( associatedRelation.id() )[QStringLiteral( "nm-rel" )].toBool();
        if ( isNmRelation )
        {
          CoreUtils::log( "Relations", "Nm relations are not supported" );
          continue;
        }

        const QString groupName = container->isGroupBox() ? container->name() : QString();
        QString label = relationField->label();
        if ( label.isEmpty() )
        {
          label = associatedRelation.name();
        }

        std::shared_ptr<FormItem> formItemData =
          std::shared_ptr<FormItem>(
            FormItem::createRelationItem(
              widgetUuid,
              groupName,
              parentTabRow,
              FormItem::Relation,
              label,
              associatedRelation
            )
          );

        mFormItems[formItemData->id()] = formItemData;
        items.append( widgetUuid );

        break;
      }

      case QgsAttributeEditorElement::AeTypeInvalid:
        // todo
        break;

      case QgsAttributeEditorElement::AeTypeQmlElement:
        // todo
        break;

      case QgsAttributeEditorElement::AeTypeHtmlElement:
        // todo
        break;
    }
  }
}

void AttributeController::createTab( QgsAttributeEditorContainer *container )
{
  Q_ASSERT( container );

  int tabRow = mTabItems.size();

  QgsExpression expr;
  if ( container->visibilityExpression().enabled() )
  {
    expr = container->visibilityExpression().data();
  }

  QVector<QUuid> formItemsUuids;
  flatten( container, tabRow, QString(), formItemsUuids );

  std::shared_ptr<TabItem> tabItem( new TabItem( tabRow,
                                    container->name(),
                                    formItemsUuids,
                                    expr )
                                  );

  mTabItems.push_back( tabItem );
}

void AttributeController::clearAll()
{
  mAttributeFormProxyModelForTabItem.clear();
  mAttributeTabProxyModel.reset( new AttributeTabProxyModel() );
  mConstraintsHardValid = false;
  mConstraintsSoftValid = false;
  mFormItems.clear();
  mTabItems.clear();
  mHasTabs = false;
}

void AttributeController::updateOnLayerChange()
{
  clearAll();

  // 1) DATA
  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  if ( layer )
  {
    if ( layer->editFormConfig().layout() == QgsEditFormConfig::TabLayout )
    {
      QgsAttributeEditorContainer *root = layer->editFormConfig().invisibleRootContainer();
      if ( root->columnCount() > 1 )
      {
        qDebug() << "root tab in manual config has multiple columns. not supported on mobile devices!";
        root->setColumnCount( 1 );
      }

      mHasTabs = allowTabs( root );
      if ( mHasTabs )
      {
        for ( QgsAttributeEditorElement *element : root->children() )
        {
          if ( element->type() == QgsAttributeEditorElement::AeTypeContainer )
          {
            QgsAttributeEditorContainer *container = static_cast<QgsAttributeEditorContainer *>( element );
            if ( container->columnCount() > 1 )
            {
              qDebug() << "tab " << container->name() << " in manual config has multiple columns. not supported on mobile devices!";
              container->setColumnCount( 1 );
            }
            createTab( container );
          }
        }
      }
      else
      {
        createTab( root );
      }
    }
    else
    {
      // Auto-Generated Layout
      // We create fake root tab
      QgsAttributeEditorContainer *tab = autoLayoutTabContainer();
      createTab( tab );
    }

    if ( mRememberAttributesController )
      mRememberAttributesController->storeLayerFields( layer );
  }

  // 2) MODELS
  // for all other models, ownership is managed by Qt parent system
  AttributeTabModel *tabModel = new AttributeTabModel( mAttributeTabProxyModel.get(), this, mTabItems.size() );
  mAttributeTabProxyModel->setSourceModel( tabModel );
  mAttributeFormProxyModelForTabItem.resize( mTabItems.size() );
  QVector<std::shared_ptr<TabItem>>::iterator tabItemsIterator = mTabItems.begin();
  while ( tabItemsIterator != mTabItems.end() )
  {
    std::shared_ptr<TabItem> item = *tabItemsIterator;
    const QVector<QUuid> &formItems = item->formItems();

    AttributeFormProxyModel *proxyFormModel = new AttributeFormProxyModel( mAttributeTabProxyModel.get() );
    AttributeFormModel *formModel = new AttributeFormModel( mAttributeTabProxyModel.get(), this, formItems );
    proxyFormModel->setSourceModel( formModel );
    mAttributeFormProxyModelForTabItem[item->tabIndex()] = proxyFormModel;
    ++tabItemsIterator;
  }
}

void AttributeController::updateOnFeatureChange()
{
  const QgsFeature feature = mFeatureLayerPair.feature();

  QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
  while ( formItemsIterator != mFormItems.end() )
  {
    std::shared_ptr<FormItem> itemData = formItemsIterator.value();
    if ( itemData->type() == FormItem::Field )
    {
      int fieldIndex = itemData->fieldIndex();
      const QVariant newVal = feature.attribute( fieldIndex );
      mFormItems[itemData->id()]->setOriginalValue( newVal );
      if ( mRememberAttributesController && isNewFeature() ) // this is a new feature
      {
        QVariant rememberedValue;
        bool shouldUseRememberedValue = mRememberAttributesController->rememberedValue(
                                          mFeatureLayerPair.layer(),
                                          fieldIndex,
                                          rememberedValue
                                        );
        if ( shouldUseRememberedValue )
          mFeatureLayerPair.featureRef().setAttribute( fieldIndex, rememberedValue );
      }
    }
    ++formItemsIterator;
  }

  recalculateDerivedItems( false, isNewFeature() );
}

bool AttributeController::isNewFeature() const
{
  return FID_IS_NULL( mFeatureLayerPair.feature().id() );
}

void AttributeController::acquireId()
{
  if ( !mFeatureLayerPair.layer() )
    return;

  startEditing();
  QgsFeature feat = mFeatureLayerPair.feature();
  if ( !mFeatureLayerPair.layer()->addFeature( feat ) )
  {
    QgsMessageLog::logMessage( tr( "Feature could not be added" ),
                               QStringLiteral( "Input" ),
                               Qgis::Critical );

  }

  connect( mFeatureLayerPair.layer(), &QgsVectorLayer::featureAdded, this, &AttributeController::onFeatureAdded );
  commit();
  disconnect( mFeatureLayerPair.layer(), &QgsVectorLayer::featureAdded, this, &AttributeController::onFeatureAdded );
}

bool AttributeController::recalculateDefaultValues(
  QSet<QUuid> &changedFormItems,
  QgsExpressionContext &expressionContext,
  bool isFormValueChange,
  bool isFirstUpdateOfNewFeature
)
{
  bool hasChanges = false;
  QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
  while ( formItemsIterator != mFormItems.end() )
  {
    std::shared_ptr<FormItem> item = formItemsIterator.value();
    const QgsField field = item->field();
    const QgsDefaultValue defaultDefinition = field.defaultValueDefinition();

    bool shouldApplyDefaultValue =
      !defaultDefinition.expression().isEmpty() &&
      ( isFirstUpdateOfNewFeature || ( isFormValueChange && defaultDefinition.applyOnUpdate() ) );

    if ( shouldApplyDefaultValue )
    {
      QgsExpression exp( field.defaultValueDefinition().expression() );
      exp.prepare( &expressionContext );
      if ( exp.hasParserError() )
        QgsMessageLog::logMessage( tr( "Default value expression for %1:%2 has parser error: %3" ).arg(
                                     mFeatureLayerPair.layer()->name(),
                                     field.name(),
                                     exp.parserErrorString() ),
                                   QStringLiteral( "Input" ),
                                   Qgis::Warning );

      QVariant value = exp.evaluate( &expressionContext );

      if ( exp.hasEvalError() )
        QgsMessageLog::logMessage( tr( "Default value expression for %1:%2 has evaluation error: %3" ).arg(
                                     mFeatureLayerPair.layer()->name(),
                                     field.name(),
                                     exp.evalErrorString() ),
                                   QStringLiteral( "Input" ),
                                   Qgis::Warning );
      else
      {
        QVariant val( value );
        if ( !field.convertCompatible( val ) )
        {
          QString msg( tr( "Value \"%1\" %4 could not be converted to a compatible value for field %2(%3)." ).arg( value.toString(), field.name(), field.typeName(), value.isNull() ? "NULL" : "NOT NULL" ) );
          QgsMessageLog::logMessage( msg );
        }
        else
        {
          QVariant oldVal = mFeatureLayerPair.feature().attribute( item->fieldIndex() );
          if ( val != oldVal )
          {
            mFeatureLayerPair.featureRef().setAttribute( item->fieldIndex(), val );
            // Update also expression context after an attribute change
            expressionContext.setFeature( featureLayerPair().featureRef() );
            changedFormItems.insert( item->id() );
            hasChanges = true;
          }
        }
      }
    }
    ++formItemsIterator;
  }
  return hasChanges;
}

void AttributeController::recalculateDerivedItems( bool isFormValueChange, bool isFirstUpdateOfNewFeature )
{
  QSet<QUuid> changedFormItems;

  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  if ( !layer || !layer->isValid() )
    return;

  if ( !mFeatureLayerPair.feature().isValid() )
    return;

  // Create context
  QgsFields fields = mFeatureLayerPair.feature().fields();
  QgsExpressionContext expressionContext = layer->createExpressionContext();
  if ( mVariablesManager )
    expressionContext << mVariablesManager->positionScope();

  expressionContext.setFields( fields );
  expressionContext.setFeature( featureLayerPair().featureRef() );

  // Evaluate default values
  // it could be recursive, so
  // let say try few times
  const int LIMIT = 3;
  int tryNumber = 0;
  bool anyValueChanged = true;
  while ( anyValueChanged && tryNumber < LIMIT )
  {
    anyValueChanged = recalculateDefaultValues( changedFormItems, expressionContext, isFormValueChange, isFirstUpdateOfNewFeature );
    ++tryNumber;
  }
  if ( anyValueChanged )
  {
    // ok we cut the loop on limit...
    qDebug() << "Evaluation of default values was not finished in " << LIMIT << " tries. Giving up, sorry!";
  }

  // Evaluate tab items visiblity
  {
    QVector<std::shared_ptr<TabItem>>::iterator tabItemsIterator = mTabItems.begin();
    while ( tabItemsIterator != mTabItems.end() )
    {
      std::shared_ptr<TabItem> item = *tabItemsIterator;
      QgsExpression exp = item->visibilityExpression();
      exp.prepare( &expressionContext );
      bool visible = true;
      if ( exp.isValid() )
      {
        visible = exp.evaluate( &expressionContext ).toBool();
      }

      if ( item->isVisible() != visible )
      {
        item->setVisible( visible );
        emit tabDataChanged( item->tabIndex() );
      }
      ++tabItemsIterator;
    }
  }

  // Evaluate form items visibility
  {
    QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
    while ( formItemsIterator != mFormItems.end() )
    {
      std::shared_ptr<FormItem> item = formItemsIterator.value();
      bool visible = true;
      if ( item->editorWidgetType() == QLatin1String( "Hidden" ) )
      {
        visible = false;
      }
      else
      {
        QgsExpression exp = item->visibilityExpression();
        exp.prepare( &expressionContext );

        if ( exp.isValid() )
          visible = exp.evaluate( &expressionContext ).toInt();
      }

      if ( item->visible() != visible )
      {
        item->setVisible( visible );
        changedFormItems << item->id();
      }
      ++formItemsIterator;
    }
  }

  // Evaluate Hard Constrains
  {
    bool allConstraintsHardValid = true;
    {
      QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
      while ( formItemsIterator != mFormItems.end() )
      {
        std::shared_ptr<FormItem> item = formItemsIterator.value();
        if ( item->type() == FormItem::Field )
        {
          QStringList errors;
          bool hardConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( layer,  featureLayerPair().feature(), item->fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthHard );
          if ( hardConstraintSatisfied != item->constraintHardValid() )
          {
            item->setConstraintHardValid( hardConstraintSatisfied );
            changedFormItems << item->id();
          }
          if ( !hardConstraintSatisfied )
          {
            allConstraintsHardValid = false;
          }
        }
        ++formItemsIterator;
      }
    }
    if ( allConstraintsHardValid != constraintsHardValid() )
    {
      mConstraintsHardValid = allConstraintsHardValid;
      emit constraintsHardValidChanged();
    }
  }

  // Evaluate Soft Constrains
  {
    bool allConstraintsSoftValid = true;
    {
      QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
      while ( formItemsIterator != mFormItems.end() )
      {
        std::shared_ptr<FormItem> item = formItemsIterator.value();
        if ( item->type() == FormItem::Field )
        {
          QStringList errors;
          bool softConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( layer,  featureLayerPair().feature(), item->fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthSoft );
          if ( softConstraintSatisfied != item->constraintSoftValid() )
          {
            item->setConstraintSoftValid( softConstraintSatisfied );
            changedFormItems << item->id();
          }
          if ( !softConstraintSatisfied )
          {
            allConstraintsSoftValid = false;
          }
        }
        ++formItemsIterator;
      }
    }
    if ( allConstraintsSoftValid != constraintsSoftValid() )
    {
      mConstraintsSoftValid = allConstraintsSoftValid;
      emit constraintsSoftValidChanged();
    }
  }

  // Evaluate field values validity
  {
    QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
    while ( formItemsIterator != mFormItems.end() )
    {
      std::shared_ptr<FormItem> item = formItemsIterator.value();
      QVariant value = featureLayerPair().feature().attribute( item->fieldIndex() );

      bool isRangeEditable = item->editorWidgetType() == QStringLiteral( "Range" ) &&
                             item->editorWidgetConfig()[QStringLiteral( "Style" )] == QStringLiteral( "SpinBox" );

      if ( isRangeEditable && !value.isNull() )
      {
        double min = item->editorWidgetConfig()[QStringLiteral( "Min" )].toDouble();
        double max = item->editorWidgetConfig()[QStringLiteral( "Max" )].toDouble();
        double val = featureLayerPair().feature().attribute( item->fieldIndex() ).toDouble();

        if ( !( min <= val && val <= max ) )
        {
          item->setState( FormItem::ValueOutOfRange );
          changedFormItems << item->id();
        }
      }

      if ( !isFormValueChange ) // reset state for new feature layer pair
      {
        item->setState( FormItem::ValidValue );
        changedFormItems << item->id();
      }

      ++formItemsIterator;
    }
    updateFieldValuesValidity();
  }

  // Check if we have any changes
  bool anyChanges = isNewFeature();
  if ( !anyChanges )
  {
    QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
    while ( formItemsIterator != mFormItems.end() )
    {
      std::shared_ptr<FormItem> item = formItemsIterator.value();
      if ( item->type() == FormItem::Field )
      {
        if ( item->originalValue() != mFeatureLayerPair.feature().attribute( item->fieldIndex() ) )
        {
          anyChanges = true;
          break;
        }
      }

      ++formItemsIterator;
    }
  }
  setHasAnyChanges( anyChanges );

  // Emit all signals
  QSet<QUuid>::const_iterator i = changedFormItems.constBegin();
  while ( i != changedFormItems.constEnd() )
  {
    emit formDataChanged( *i );
    ++i;
  }
}

bool AttributeController::constraintsHardValid() const
{
  return mConstraintsHardValid;
}

bool AttributeController::constraintsSoftValid() const
{
  return mConstraintsSoftValid;
}

bool AttributeController::fieldValuesValid() const
{
  return mFieldValuesValid;
}

bool AttributeController::hasTabs() const
{
  return mHasTabs;
}

AttributeTabProxyModel *AttributeController::attributeTabProxyModel() const
{
  Q_ASSERT( mAttributeTabProxyModel );
  return mAttributeTabProxyModel.get();
}

int AttributeController::tabCount() const
{
  return mTabItems.size();
}

AttributeFormProxyModel *AttributeController::attributeFormProxyModelForTab( int tabRow ) const
{
  if ( isValidTabId( tabRow ) )
  {
    return mAttributeFormProxyModelForTabItem[tabRow];
  }
  else
  {
    return nullptr;
  }
}

bool AttributeController::deleteFeature()
{
  if ( !mFeatureLayerPair.layer() )
    return false;

  bool rv = true;

  if ( !startEditing() )
  {
    rv = false;
  }

  bool isDeleted = mFeatureLayerPair.layer()->deleteFeature( mFeatureLayerPair.feature().id() );
  rv = commit();

  if ( !isDeleted )
    QgsMessageLog::logMessage( tr( "Cannot delete feature" ),
                               QStringLiteral( "Input" ),
                               Qgis::Warning );
  else
  {
    mFeatureLayerPair = FeatureLayerPair();
    emit featureLayerPairChanged();
  }

  return rv;
}

bool AttributeController::create()
{
  if ( !mFeatureLayerPair.layer() )
    return false;

  startEditing();
  QgsFeature feat = mFeatureLayerPair.feature();
  if ( !mFeatureLayerPair.layer()->addFeature( feat ) )
  {
    QgsMessageLog::logMessage( tr( "Feature could not be added" ),
                               QStringLiteral( "Input" ),
                               Qgis::Critical );

  }
  connect( mFeatureLayerPair.layer(), &QgsVectorLayer::featureAdded, this, &AttributeController::onFeatureAdded );
  commit();
  disconnect( mFeatureLayerPair.layer(), &QgsVectorLayer::featureAdded, this, &AttributeController::onFeatureAdded );


  if ( mRememberAttributesController )
  {
    mRememberAttributesController->storeFeature( mFeatureLayerPair );
  }
  return true;
}

bool AttributeController::save()
{
  if ( !mFeatureLayerPair.layer() )
    return false;

  bool rv = true;

  if ( !startEditing() )
  {
    rv = false;
  }

  QgsFeature feat = mFeatureLayerPair.feature();
  if ( !mFeatureLayerPair.layer()->updateFeature( feat ) )
    QgsMessageLog::logMessage( tr( "Cannot update feature" ),
                               QStringLiteral( "Input" ),
                               Qgis::Warning );

  // This calls lower-level I/O functions which shouldn't be used
  // in a Q_INVOKABLE because they can make the UI unresponsive.
  rv = commit();

  if ( rv )
  {
    QgsFeature feat;
    if ( mFeatureLayerPair.layer()->getFeatures( QgsFeatureRequest().setFilterFid( mFeatureLayerPair.feature().id() ) ).nextFeature( feat ) )
      setFeatureLayerPair( FeatureLayerPair( feat, mFeatureLayerPair.layer() ) );
    else
      QgsMessageLog::logMessage( tr( "Feature %1 could not be fetched after commit" ).arg( mFeatureLayerPair.feature().id() ),
                                 QStringLiteral( "Input" ),
                                 Qgis::Warning );
  }
  return rv;
}

bool AttributeController::startEditing()
{
  Q_ASSERT( mFeatureLayerPair.layer() );

  // Already an edit session active
  if ( mFeatureLayerPair.layer()->editBuffer() )
    return true;

  if ( !mFeatureLayerPair.layer()->startEditing() )
  {
    QgsMessageLog::logMessage( tr( "Cannot start editing" ),
                               QStringLiteral( "Input" ),
                               Qgis::Warning );
    return false;
  }
  else
  {
    return true;
  }
}

bool AttributeController::commit()
{
  Q_ASSERT( mFeatureLayerPair.layer() );

  if ( !mFeatureLayerPair.layer()->commitChanges() )
  {
    QgsMessageLog::logMessage( tr( "Could not save changes. Rolling back." ),
                               QStringLiteral( "Input" ),
                               Qgis::Critical );
    mFeatureLayerPair.layer()->rollBack();
    return false;
  }
  else
  {
    return true;
  }
}

bool AttributeController::hasAnyChanges() const
{
  return mHasAnyChanges;
}

void AttributeController::setHasAnyChanges( bool hasChanges )
{
  if ( hasChanges != mHasAnyChanges )
  {
    mHasAnyChanges = hasChanges;
    emit hasAnyChangesChanged();
  }
}

void AttributeController::updateFieldValuesValidity()
{
  // loop over items and see if there is a field with invalid state
  QMap<QUuid, std::shared_ptr<FormItem>>::iterator formItemsIterator = mFormItems.begin();
  while ( formItemsIterator != mFormItems.end() )
  {
    std::shared_ptr<FormItem> item = formItemsIterator.value();
    if ( item->valueState() != FormItem::ValidValue )
    {
      setFieldValuesValid( false );
      return;
    }
    ++formItemsIterator;
  }

  setFieldValuesValid( true );
}

void AttributeController::setFieldValuesValid( bool valid )
{
  if ( valid != mFieldValuesValid )
  {
    mFieldValuesValid = valid;
    emit fieldValuesValidChanged();
  }
}

bool AttributeController::isValidFormId( const QUuid &id ) const
{
  return mFormItems.contains( id );
}

bool AttributeController::isValidTabId( int id ) const
{
  return ( id >= 0 ) && ( id < tabCount() );
}

const FormItem *AttributeController::formItem( const QUuid &id ) const
{
  if ( isValidFormId( id ) )
    return mFormItems[id].get();
  else
    return nullptr;
}

const TabItem *AttributeController::tabItem( int tabRow ) const
{
  if ( isValidTabId( tabRow ) )
    return mTabItems[tabRow].get();
  else
    return nullptr;
}

bool AttributeController::setFormShouldRememberValue( const QUuid &id, bool shouldRememberValue )
{
  if ( !mRememberAttributesController )
    return true; //noop

  if ( isValidFormId( id ) )
  {
    std::shared_ptr<FormItem> data = mFormItems[id];
    bool changed = mRememberAttributesController->setShouldRememberValue( mFeatureLayerPair.layer(), data->fieldIndex(), shouldRememberValue );
    if ( changed )
    {
      emit formDataChanged( id );
    }
    return true;
  }
  else
  {
    return false;
  }
}

bool AttributeController::formShouldRememberValue( int fieldIndex ) const
{
  const QgsFeature feat = mFeatureLayerPair.feature();
  const QgsVectorLayer *layer = mFeatureLayerPair.layer();

  if ( !mRememberAttributesController )
    return false;

  return mRememberAttributesController->shouldRememberValue( layer, fieldIndex );

}

bool AttributeController::setFormValue( const QUuid &id, QVariant value )
{
  if ( isValidFormId( id ) )
  {
    std::shared_ptr<FormItem> item = mFormItems[id];
    QVariant oldVal = mFeatureLayerPair.feature().attribute( item->fieldIndex() );
    if ( value != oldVal )
    {
      QVariant val( value );
      const QgsField fld = item->field();
      if ( !fld.convertCompatible( val ) )
      {
        QString msg( tr( "Value \"%1\" %4 could not be converted to a compatible value for field %2(%3)." ).arg( value.toString(), fld.name(), fld.typeName(), value.isNull() ? "NULL" : "NOT NULL" ) );
        QString userFriendlyMsg( tr( "Value %1 is not compatible with field type %2." ).arg( value.toString(), fld.typeName() ) );
        QgsMessageLog::logMessage( msg );

        if ( value.toBool() )
        {
          item->setState( FormItem::InvalidValue );
          setFieldValuesValid( false );
        }
        else
        {
          item->setState( FormItem::ValidValue ); // this is empty field ~ NULL value
          updateFieldValuesValidity();
        }

        emit formDataChanged( id, { AttributeFormModel::ValueValidity } );
        return false;
      }
      mFeatureLayerPair.featureRef().setAttribute( item->fieldIndex(), val );
      item->setState( FormItem::ValidValue );

      emit formDataChanged( id );
      updateFieldValuesValidity();
      recalculateDerivedItems( true, false );
    }
    else
    {
      item->setState( FormItem::ValidValue );
      emit formDataChanged( id, { AttributeFormModel::ValueValidity } );
      updateFieldValuesValidity();
    }
    return true;
  }
  else
  {
    return false;
  }
}

QVariant AttributeController::formValue( int fieldIndex ) const
{
  const QgsFeature feat = mFeatureLayerPair.feature();
  if ( !feat.isValid() ||
       fieldIndex < 0 ||
       fieldIndex >= feat.attributeCount()
     )
    return QVariant();

  return mFeatureLayerPair.feature().attribute( fieldIndex );
}

AttributeController *AttributeController::parentController() const
{
  return mParentController;
}

void AttributeController::setParentController( AttributeController *newParentController )
{
  if ( !newParentController || mParentController == newParentController )
    return;

  mParentController = newParentController;
  emit parentControllerChanged();

  prefillRelationReferenceField();

  if ( mParentController )
  {
    // When parent's feature Id is changed, we want to update the relation reference field
    connect( mParentController, &AttributeController::featureIdChanged, this, &AttributeController::prefillRelationReferenceField );
  }
}

const QgsRelation &AttributeController::linkedRelation() const
{
  return mLinkedRelation;
}

void AttributeController::setLinkedRelation( const QgsRelation &newLinkedRelation )
{
  if ( mLinkedRelation.id() == newLinkedRelation.id() )
    return;

  mLinkedRelation = newLinkedRelation;
  emit linkedRelationChanged();

  prefillRelationReferenceField();
}

void AttributeController::onFeatureAdded( QgsFeatureId newFeatureId )
{
  QgsFeature f = mFeatureLayerPair.layer()->getFeature( newFeatureId );
  setFeatureLayerPair( FeatureLayerPair( f, mFeatureLayerPair.layer() ) );
  emit featureIdChanged();
}
