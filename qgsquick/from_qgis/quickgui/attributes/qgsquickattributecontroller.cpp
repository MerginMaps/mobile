/***************************************************************************
 qgsquickattributecontroller.cpp
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

#include "qgsquickattributecontroller.h"
#include "qgsquickattributeformmodel.h"
#include "qgsquickattributeformproxymodel.h"
#include "qgsquickattributetabmodel.h"
#include "qgsquickattributetabproxymodel.h"
#include "qgsquickrememberattributes.h"

#include <QDebug>
#include <QSet>
#include "qgsvectorlayer.h"
#include "qgsattributeeditorfield.h"
#include "qgsattributeeditorcontainer.h"
#include "qgsvectorlayerutils.h"
#include "qgsmessagelog.h"

// See
// https://github.com/qgis/QGIS/blob/2d1aa68f0d044f2aced7ebeca8d2fa6b754ac970/tests/src/gui/testqgsattributeform.cpp

QgsQuickAttributeController::QgsQuickAttributeController( QObject *parent )
  : QObject( parent )
  , mAttributeTabProxyModel( new QgsQuickAttributeTabProxyModel() )
{
}

void QgsQuickAttributeController::forceClean()
{
  setFeatureLayerPair( QgsQuickFeatureLayerPair() );
}

QgsQuickAttributeController::~QgsQuickAttributeController() = default;


QgsQuickFeatureLayerPair QgsQuickAttributeController::featureLayerPair() const
{
  return mFeatureLayerPair;
}

void QgsQuickAttributeController::setFeatureLayerPair( const QgsQuickFeatureLayerPair &pair )
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
      emit attributeFormPreviewModelChanged();
      emit attributeFormPreviewFieldsChanged();
      emit hasTabsChanged();
    }
    emit featureLayerPairChanged();
  }
}

QgsAttributeEditorContainer *QgsQuickAttributeController::autoLayoutTabContainer() const  //#spellok
{
  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  Q_ASSERT( layer );

  QgsAttributeEditorContainer *root = new QgsAttributeEditorContainer( QLatin1String( "AutoLayoutRoot" ), nullptr );
  QgsFields fields = layer->fields();
  for ( int i = 0; i < fields.size(); ++i )
  {
    // TODO hidden fields -> add them to the mFormItems?
    if ( fields.at( i ).editorWidgetSetup().type() != QLatin1String( "Hidden" ) )
    {
      QgsAttributeEditorField *field = new QgsAttributeEditorField( fields.at( i ).name(), i, root );
      root->addChildElement( field );
    }
  }
  return root;
}

QgsQuickRememberAttributes *QgsQuickAttributeController::rememberAttributes() const
{
  return mRememberAttributes;
}

void QgsQuickAttributeController::setRememberAttributes( QgsQuickRememberAttributes *rememberAttributes )
{
  if ( mRememberAttributes != rememberAttributes )
  {
    mRememberAttributes = rememberAttributes;
    if ( mRememberAttributes && mFeatureLayerPair.layer() )
      mRememberAttributes->storeLayerFields( mFeatureLayerPair.layer() );
    emit rememberAttributesChanged();
  }
}

void QgsQuickAttributeController::flatten(
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
        //if ( !visibilityExpression.isEmpty() )
        //  mVisibilityExpressions.append( qMakePair( QgsExpression( visibilityExpression ), newItems ) );
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

        const QString groupName = container->isGroupBox() ? container->name() : QString();

        // mConstraints.insert( item, field.constraints() );
        std::shared_ptr<QgsQuickFormItemData> formItemData =
          std::shared_ptr<QgsQuickFormItemData>(
            new QgsQuickFormItemData(
              fieldUuid,
              field,
              groupName,
              parentTabRow,
              QgsQuickFormItem::Field,
              layer->attributeDisplayName( fieldIndex ),
              !layer->editFormConfig().readOnly( fieldIndex ),
              layer->editorWidgetSetup( fieldIndex ),
              fieldIndex,
              field.constraints(),
              parentVisibilityExpressions // field doesn't have visibility expression itself
            )
          );

        mFormItemsData[formItemData->id()] = formItemData;


        items.append( fieldUuid );
        break;
      }

      case QgsAttributeEditorElement::AeTypeRelation:
        // todo
        break;

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

void QgsQuickAttributeController::createTab( QgsAttributeEditorContainer *container )
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

  std::shared_ptr<QgsQuickTabItem> tabItem( new QgsQuickTabItem( tabRow,
      container->name(),
      formItemsUuids,
      expr )
                                          );

  mTabItems.push_back( tabItem );
}

void QgsQuickAttributeController::updateOnLayerChange()
{
  // 0) CLEAR
  mExpressionContext = QgsExpressionContext();
  mAttributeFormProxyModelForTabItem.clear();
  mAttributeTabProxyModel.reset( new QgsQuickAttributeTabProxyModel() );
  mAttributeFormPreviewModel.reset();
  mPreviewFields.clear();
  mConstraintsHardValid = false;
  mConstraintsSoftValid = false;
  mFormItemsData.clear();
  mTabItems.clear();

  // 1) DATA
  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  if ( layer )
  {
    if ( layer->editFormConfig().layout() == QgsEditFormConfig::TabLayout )
    {
      QgsAttributeEditorContainer *root = layer->editFormConfig().invisibleRootContainer();
      // Do NOT support more columns!
      root->setColumnCount( 1 );
      for ( QgsAttributeEditorElement *element : root->children() )
      {
        if ( element->type() == QgsAttributeEditorElement::AeTypeContainer )
        {
          QgsAttributeEditorContainer *container = static_cast<QgsAttributeEditorContainer *>( element );
          createTab( container );
        }
        else
        {
          qDebug() << "element in tab layout that is not part of any tab. Ignoring!";
        }
      }
    }
    else
    {
      // Auto-Generated Layout
      // We create fake root tab
      QgsAttributeEditorContainer *tab = autoLayoutTabContainer();
      createTab( tab );
    }

    mExpressionContext = layer->createExpressionContext();
    if ( mRememberAttributes )
      mRememberAttributes->storeLayerFields( layer );
  }

  // 2) MODELS
  // for all other models, ownership is managed by Qt parent system
  QgsQuickAttributeTabModel *tabModel = new QgsQuickAttributeTabModel( mAttributeTabProxyModel.get(), this, mTabItems.size() );
  mAttributeTabProxyModel->setSourceModel( tabModel );
  mAttributeFormProxyModelForTabItem.resize( mTabItems.size() );
  QVector<std::shared_ptr<QgsQuickTabItem>>::iterator tabItemsIterator = mTabItems.begin();
  while ( tabItemsIterator != mTabItems.end() )
  {
    std::shared_ptr<QgsQuickTabItem> item = *tabItemsIterator;
    const QVector<QUuid> &formItems = item->formItems();

    QgsQuickAttributeFormProxyModel *proxyFormModel = new QgsQuickAttributeFormProxyModel( mAttributeTabProxyModel.get() );
    QgsQuickAttributeFormModel *formModel = new QgsQuickAttributeFormModel( mAttributeTabProxyModel.get(), this, formItems );
    proxyFormModel->setSourceModel( formModel );
    mAttributeFormProxyModelForTabItem[item->id()] = proxyFormModel;
    ++tabItemsIterator;
  }
}

void QgsQuickAttributeController::updateOnFeatureChange()
{
  const QgsFeature feature = mFeatureLayerPair.feature();

  QMap<QUuid, std::shared_ptr<QgsQuickFormItemData>>::iterator formItemsDataIterator = mFormItemsData.begin();
  while ( formItemsDataIterator != mFormItemsData.end() )
  {
    std::shared_ptr<QgsQuickFormItemData> itemData = formItemsDataIterator.value();
    if ( itemData->type() == QgsQuickFormItem::Field )
    {
      int fieldIndex = itemData->fieldIndex();
      const QVariant newVal = feature.attribute( fieldIndex );
      mFormItemsData[itemData->id()]->setOriginalValue( newVal );
      if ( mRememberAttributes && FID_IS_NULL( mFeatureLayerPair.feature().id() ) ) // this is a new feature
      {
        QVariant rememberedValue;
        bool shouldUseRememberedValue = mRememberAttributes->rememberedValue(
                                          mFeatureLayerPair.layer(),
                                          fieldIndex,
                                          rememberedValue
                                        );
        if ( shouldUseRememberedValue )
          mFeatureLayerPair.featureRef().setAttribute( fieldIndex, rememberedValue );
      }
    }
    ++formItemsDataIterator;
  }

  recalculateDerivedItems();
}

void QgsQuickAttributeController::recalculateDerivedItems()
{
  QSet<QUuid> changedFormItems;

  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  if ( !layer || !layer->isValid() )
    return;

  QgsFields fields = mFeatureLayerPair.feature().fields();
  mExpressionContext.setFields( fields );
  mExpressionContext.setFeature( featureLayerPair().feature() );

  // Evaluate tab items visiblity
  {
    QVector<std::shared_ptr<QgsQuickTabItem>>::iterator tabItemsIterator = mTabItems.begin();
    while ( tabItemsIterator != mTabItems.end() )
    {
      std::shared_ptr<QgsQuickTabItem> item = *tabItemsIterator;
      QgsExpression exp = item->visibilityExpression();
      exp.prepare( &mExpressionContext );
      bool visible = true;
      if ( exp.isValid() )
        exp.evaluate( &mExpressionContext ).toInt();

      if ( item->isVisible() != visible )
      {
        item->setVisible( visible );
        emit tabDataChanged( item->id() );
      }
      ++tabItemsIterator;
    }
  }

  // Evaluate form items visibility
  {
    QMap<QUuid, std::shared_ptr<QgsQuickFormItemData>>::iterator formItemsDataIterator = mFormItemsData.begin();
    while ( formItemsDataIterator != mFormItemsData.end() )
    {
      std::shared_ptr<QgsQuickFormItemData> item = formItemsDataIterator.value();
      QgsExpression exp = item->visibilityExpression();
      exp.prepare( &mExpressionContext );
      bool visible = true;
      if ( exp.isValid() )
        visible = exp.evaluate( &mExpressionContext ).toInt();
      if ( item->visible() != visible )
      {
        item->setVisible( visible );
        changedFormItems << item->id();
      }
      ++formItemsDataIterator;
    }
  }

  // Evaluate Hard Constrains
  {
    bool allConstraintsHardValid = true;
    {
      QMap<QUuid, std::shared_ptr<QgsQuickFormItemData>>::iterator formItemsDataIterator = mFormItemsData.begin();
      while ( formItemsDataIterator != mFormItemsData.end() )
      {
        std::shared_ptr<QgsQuickFormItemData> item = formItemsDataIterator.value();
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
        ++formItemsDataIterator;
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
      QMap<QUuid, std::shared_ptr<QgsQuickFormItemData>>::iterator formItemsDataIterator = mFormItemsData.begin();
      while ( formItemsDataIterator != mFormItemsData.end() )
      {
        std::shared_ptr<QgsQuickFormItemData> item = formItemsDataIterator.value();
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
        ++formItemsDataIterator;
      }
    }
    if ( allConstraintsSoftValid != constraintsSoftValid() )
    {
      mConstraintsSoftValid = allConstraintsSoftValid;
      emit constraintsSoftValidChanged();
    }
  }

  // Check if we have any changes
  {
    bool anyChanges = false;
    QMap<QUuid, std::shared_ptr<QgsQuickFormItemData>>::iterator formItemsDataIterator = mFormItemsData.begin();
    while ( formItemsDataIterator != mFormItemsData.end() )
    {
      std::shared_ptr<QgsQuickFormItemData> item = formItemsDataIterator.value();
      if ( item->type() == QgsQuickFormItem::Field )
      {
        if ( item->originalValue() != mFeatureLayerPair.feature().attribute( item->fieldIndex() ) )
        {
          anyChanges = true;
          break;
        }
      }

      ++formItemsDataIterator;
    }
    setHasAnyChanges( anyChanges );
  }

  // Emit all signals
  QSet<QUuid>::const_iterator i = changedFormItems.constBegin();
  while ( i != changedFormItems.constEnd() )
  {
    emit formDataChanged( *i );
    ++i;
  }
}

bool QgsQuickAttributeController::constraintsHardValid() const
{
  return mConstraintsHardValid;
}

bool QgsQuickAttributeController::constraintsSoftValid() const
{
  return mConstraintsSoftValid;
}

bool QgsQuickAttributeController::hasTabs() const
{
  return tabCount() > 1;
}

QgsQuickAttributeTabProxyModel *QgsQuickAttributeController::attributeTabProxyModel() const
{
  Q_ASSERT( mAttributeTabProxyModel );
  return mAttributeTabProxyModel.get();
}

int QgsQuickAttributeController::tabCount() const
{
  return mTabItems.size();
}

QgsQuickAttributeFormProxyModel *QgsQuickAttributeController::attributeFormProxyModelForTab( int tabRow ) const
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

QgsQuickAttributeFormModel *QgsQuickAttributeController::attributeFormPreviewModel() const
{
  return mAttributeFormPreviewModel.get();
}

void QgsQuickAttributeController::setAttributeFormPreviewFields( const QStringList &fieldNames )
{
  if ( mPreviewFields != fieldNames || !mAttributeFormPreviewModel )
  {
    QVector<QUuid> previewFieldsUuids( fieldNames.size() );
    QMap<QUuid, std::shared_ptr<QgsQuickFormItemData>>::iterator formItemsDataIterator = mFormItemsData.begin();
    while ( formItemsDataIterator != mFormItemsData.end() )
    {
      std::shared_ptr<QgsQuickFormItemData> item = formItemsDataIterator.value();
      if ( item->type() == QgsQuickFormItem::Field )
      {
        int i = fieldNames.indexOf( item->name() );
        if ( i > 0 )
          previewFieldsUuids[i] = item->id();
      }
      ++formItemsDataIterator;
    }

    mAttributeFormPreviewModel.reset( new QgsQuickAttributeFormModel(
                                        nullptr,
                                        this,
                                        previewFieldsUuids
                                      ) );

    emit attributeFormPreviewModelChanged();
    emit attributeFormPreviewFieldsChanged();
  }
}

QStringList QgsQuickAttributeController::attributeFormPreviewFields() const
{
  return mPreviewFields;
}

bool QgsQuickAttributeController::deleteFeature()
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
                               QStringLiteral( "QgsQuick" ),
                               Qgis::Warning );
  else
  {
    mFeatureLayerPair = QgsQuickFeatureLayerPair();
    emit featureLayerPairChanged();
  }

  return rv;
}

void QgsQuickAttributeController::create()
{
  if ( !mFeatureLayerPair.layer() )
    return;

  startEditing();
  QgsFeature feat = mFeatureLayerPair.feature();
  if ( !mFeatureLayerPair.layer()->addFeature( feat ) )
  {
    QgsMessageLog::logMessage( tr( "Feature could not be added" ),
                               QStringLiteral( "QgsQuick" ),
                               Qgis::Critical );
  }
  commit();

  if ( mRememberAttributes )
  {
    mRememberAttributes->storeFeature( mFeatureLayerPair );
  }
}

bool QgsQuickAttributeController::save()
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
                               QStringLiteral( "QgsQuick" ),
                               Qgis::Warning );

  // This calls lower-level I/O functions which shouldn't be used
  // in a Q_INVOKABLE because they can make the UI unresponsive.
  rv = commit();

  if ( rv )
  {
    QgsFeature feat;
    if ( mFeatureLayerPair.layer()->getFeatures( QgsFeatureRequest().setFilterFid( mFeatureLayerPair.feature().id() ) ).nextFeature( feat ) )
      setFeatureLayerPair( QgsQuickFeatureLayerPair( feat, mFeatureLayerPair.layer() ) );
    else
      QgsMessageLog::logMessage( tr( "Feature %1 could not be fetched after commit" ).arg( mFeatureLayerPair.feature().id() ),
                                 QStringLiteral( "QgsQuick" ),
                                 Qgis::Warning );
  }
  return rv;
}

bool QgsQuickAttributeController::startEditing()
{
  Q_ASSERT( mFeatureLayerPair.layer() );

  // Already an edit session active
  if ( mFeatureLayerPair.layer()->editBuffer() )
    return true;

  if ( !mFeatureLayerPair.layer()->startEditing() )
  {
    QgsMessageLog::logMessage( tr( "Cannot start editing" ),
                               QStringLiteral( "QgsQuick" ),
                               Qgis::Warning );
    return false;
  }
  else
  {
    return true;
  }
}

bool QgsQuickAttributeController::commit()
{
  Q_ASSERT( mFeatureLayerPair.layer() );

  if ( !mFeatureLayerPair.layer()->commitChanges() )
  {
    QgsMessageLog::logMessage( tr( "Could not save changes. Rolling back." ),
                               QStringLiteral( "QgsQuick" ),
                               Qgis::Critical );
    mFeatureLayerPair.layer()->rollBack();
    return false;
  }
  else
  {
    return true;
  }
}

bool QgsQuickAttributeController::hasAnyChanges()
{
  return mHasAnyChanges;
}

void QgsQuickAttributeController::setHasAnyChanges( bool hasChanges )
{
  if ( hasChanges != mHasAnyChanges )
  {
    mHasAnyChanges = hasChanges;
    emit hasAnyChangesChanged();
  }
}

bool QgsQuickAttributeController::isValidFormId( const QUuid &id ) const
{
  return mFormItemsData.contains( id );
}

bool QgsQuickAttributeController::isValidTabId( int id ) const
{
  return ( id >= 0 ) && ( id < tabCount() );
}

QgsQuickFormItem QgsQuickAttributeController::formItem( const QUuid &id ) const
{
  if ( isValidFormId( id ) )
  {
    std::shared_ptr<QgsQuickFormItemData> data = mFormItemsData[id];

    QVariant value = mFeatureLayerPair.feature().attribute( data->fieldIndex() );
    bool shouldRememeberValue = false;
    if ( mRememberAttributes )
      shouldRememeberValue = mRememberAttributes->shouldRememberValue( mFeatureLayerPair.layer(), data->fieldIndex() );
    QgsQuickFormItem item( data, shouldRememeberValue, value );

    return item;
  }
  else
    return QgsQuickFormItem();
}

const QgsQuickTabItem *QgsQuickAttributeController::tabItem( int tabRow ) const
{
  if ( isValidTabId( tabRow ) )
    return mTabItems[tabRow].get();
  else
    return nullptr;
}

bool QgsQuickAttributeController::setFormShouldRememberValue( const QUuid &id, bool shouldRememberValue )
{
  if ( !mRememberAttributes )
    return true; //noop

  if ( isValidFormId( id ) )
  {
    std::shared_ptr<QgsQuickFormItemData> data = mFormItemsData[id];
    bool changed = mRememberAttributes->setShouldRememberValue( mFeatureLayerPair.layer(), data->fieldIndex(), shouldRememberValue );
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

bool QgsQuickAttributeController::setFormValue( const QUuid &id, QVariant value )
{
  if ( isValidFormId( id ) )
  {
    std::shared_ptr<QgsQuickFormItemData> item = mFormItemsData[id];
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
        if ( !val.isNull() )
        {
          emit dataChangedFailed( userFriendlyMsg );
        }
        return false;
      }
      mFeatureLayerPair.featureRef().setAttribute( item->fieldIndex(), val );
      emit formDataChanged( id );
      recalculateDerivedItems();
    }
    return true;
  }
  else
  {
    return false;
  }

}
