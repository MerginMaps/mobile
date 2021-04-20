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

#include <QDebug>
#include <QSet>
#include "qgsvectorlayer.h"
#include "qgsattributeeditorfield.h"
#include "qgsattributeeditorcontainer.h"
#include "qgsvectorlayerutils.h"

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
      emit hasTabsChanged();
    }
    emit featureLayerPairChanged();
  }
}

QgsAttributeEditorContainer *QgsQuickAttributeController::autoLayoutTabContainer() const  //#spellok
{
  QgsVectorLayer *layer = mFeatureLayerPair.layer();
  Q_ASSERT( layer );

  QgsAttributeEditorContainer *root = new QgsAttributeEditorContainer( QString(), nullptr );
  QgsFields fields = layer->fields();
  for ( int i = 0; i < fields.size(); ++i )
  {
    // TODO hidden fields -> add them to the mFormItems
    if ( fields.at( i ).editorWidgetSetup().type() != QLatin1String( "Hidden" ) )
    {
      QgsAttributeEditorField *field = new QgsAttributeEditorField( fields.at( i ).name(), i, root );
      root->addChildElement( field );
    }
  }
  return root;
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

        // Qt::CheckState rememberFlag = Qt::Unchecked;
        //if ( rememberValuesAllowed() && mAttributeModel->isFieldRemembered( fieldIndex ) )
        //rememberFlag = Qt::Checked;

        /* QStandardItem *item = new QStandardItem();
        item->setData( layer->attributeDisplayName( fieldIndex ), QgsQuickAttributeFormModel::Name );
        item->setData( mAttributeModel->featureLayerPair().feature().attribute( fieldIndex ), QgsQuickAttributeFormModel::AttributeValue );
        item->setData( !layer->editFormConfig().readOnly( fieldIndex ), QgsQuickAttributeFormModel::AttributeEditable );
        QgsEditorWidgetSetup setup = layer->editorWidgetSetup( fieldIndex );
        item->setData( setup.type(), QgsQuickAttributeFormModel::EditorWidget );
        item->setData( setup.config(), QgsQuickAttributeFormModel::EditorWidgetConfig );
        item->setData( rememberFlag, QgsQuickAttributeFormModel::RememberValue );
        item->setData( layer->fields().at( fieldIndex ), QgsQuickAttributeFormModel::Field );
        item->setData( QStringLiteral( "field" ), QgsQuickAttributeFormModel::ElementType );
        item->setData( fieldIndex, QgsQuickAttributeFormModel::FieldIndex );
        item->setData( container->isGroupBox() ? container->name() : QString(), QgsQuickAttributeFormModel::Group );
        item->setData( true, QgsQuickAttributeFormModel::Visible );
        item->setData( true, QgsQuickAttributeFormModel::ConstraintHardValid );
        item->setData( true, QgsQuickAttributeFormModel::ConstraintSoftValid );
        item->setData( field.constraints().constraintDescription(), QgsQuickAttributeFormModel::ConstraintDescription );
        */
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
        std::shared_ptr<QgsQuickFormItem> formItem =
          std::shared_ptr<QgsQuickFormItem>(
            new QgsQuickFormItem(
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

        mFormItems[formItem->id()] = formItem;


        items.append( fieldUuid );
        // parent->appendRow( item );
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
    // mVisibilityExpressions.append( qMakePair( expr, QVector<QUuid>() << tabUuid ) );
  }

  QVector<QUuid> formItemsUuids;
  flatten( container, tabRow, QString(), formItemsUuids );

  std::shared_ptr<QgsQuickTabItem> tabItem( new QgsQuickTabItem( tabRow,
      QLatin1String( "AutoLayoutRoot" ),
      formItemsUuids,
      expr
                                                               ) );
  mTabItems.push_back( tabItem );
}

void QgsQuickAttributeController::updateOnLayerChange()
{
  // 0) CLEAR
  mExpressionContext = QgsExpressionContext();
  mAttributeFormProxyModelForTabItem.clear();
  mAttributeTabProxyModel.reset( new QgsQuickAttributeTabProxyModel() );
  mAttributeFormPreviewModel.reset();
  mPreviewFieldsUuids.clear();
  mConstraintsHardValid = false;
  mConstraintsSoftValid = false;
  mFormItems.clear();
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

  QMap<QUuid, std::shared_ptr<QgsQuickFormItem>>::iterator formItemsIterator = mFormItems.begin();
  while ( formItemsIterator != mFormItems.end() )
  {
    std::shared_ptr<QgsQuickFormItem> item = formItemsIterator.value();
    if ( item->type() == QgsQuickFormItem::Field )
    {
      const int fieldIndex = item->fieldIndex();
      Q_ASSERT( fieldIndex >= 0 );
      const QVariant newVal = feature.attribute( fieldIndex );
      item->setValue( newVal );
    }
    ++formItemsIterator;
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
    QMap<QUuid, std::shared_ptr<QgsQuickFormItem>>::iterator formItemsIterator = mFormItems.begin();
    while ( formItemsIterator != mFormItems.end() )
    {
      std::shared_ptr<QgsQuickFormItem> item = formItemsIterator.value();
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
      ++formItemsIterator;
    }
  }

  // Evaluate Hard Constrains
  {
    bool allConstraintsHardValid = true;
    {
      QMap<QUuid, std::shared_ptr<QgsQuickFormItem>>::iterator formItemsIterator = mFormItems.begin();
      while ( formItemsIterator != mFormItems.end() )
      {
        std::shared_ptr<QgsQuickFormItem> item = formItemsIterator.value();
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
      QMap<QUuid, std::shared_ptr<QgsQuickFormItem>>::iterator formItemsIterator = mFormItems.begin();
      while ( formItemsIterator != mFormItems.end() )
      {
        std::shared_ptr<QgsQuickFormItem> item = formItemsIterator.value();
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
        ++formItemsIterator;
      }
    }
    if ( allConstraintsSoftValid != constraintsSoftValid() )
    {
      mConstraintsSoftValid = allConstraintsSoftValid;
      emit constraintsSoftValidChanged();
    }
  }

  // Emit all signals
  QSet<QUuid>::const_iterator i = changedFormItems.constBegin();
  while ( i != changedFormItems.constEnd() )
  {
    emit formDataChanged( *i );
    ++i;
  }
}

bool QgsQuickAttributeController::rememberValuesAllowed() const
{
  return mRememberValuesAllowed;
}

void QgsQuickAttributeController::setRememberValuesAllowed( bool rememberValuesAllowed )
{
  if ( mRememberValuesAllowed != rememberValuesAllowed )
  {
    mRememberValuesAllowed = rememberValuesAllowed;
    emit rememberValuesAllowedChanged();
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

void QgsQuickAttributeController::setPreviewFields( const QStringList &fieldNames )
{
  QStringList fieldNamesForPreview;
  for ( const QUuid &id : qAsConst( mPreviewFieldsUuids ) )
  {
    std::shared_ptr<QgsQuickFormItem> item = mFormItems[id];
    fieldNamesForPreview << item->name();
  }

  if ( fieldNamesForPreview != fieldNames || !mAttributeFormPreviewModel )
  {
    QVector<QUuid> previewFields( fieldNames.size() );
    QMap<QUuid, std::shared_ptr<QgsQuickFormItem>>::iterator formItemsIterator = mFormItems.begin();
    while ( formItemsIterator != mFormItems.end() )
    {
      std::shared_ptr<QgsQuickFormItem> item = formItemsIterator.value();
      if ( item->type() == QgsQuickFormItem::Field )
      {
        int i = fieldNames.indexOf( item->name() );
        if ( i > 0 )
          previewFields[i] = item->id();
      }
      ++formItemsIterator;
    }

    mAttributeFormPreviewModel.reset( new QgsQuickAttributeFormModel(
                                        nullptr,
                                        this,
                                        previewFields

                                      ) );
    emit attributeFormPreviewModelChanged();
  }
}

void QgsQuickAttributeController::deleteFeature()
{
  // TODO
  qDebug() << "delete feature TODO";
}

void QgsQuickAttributeController::create()
{
  // TODO
  qDebug() << "create feature TODO";
}

void QgsQuickAttributeController::save()
{
  // TODO
  qDebug() << "save feature TODO";
}

bool QgsQuickAttributeController::hasAnyChanges()
{
  // TODO
  qDebug() << "has changes TODO";
  return false;
}

bool QgsQuickAttributeController::isValidFormId( const QUuid &id ) const
{
  return mFormItems.contains( id );
}

bool QgsQuickAttributeController::isValidTabId( int id ) const
{
  return ( id >= 0 ) && ( id < tabCount() );
}

const QgsQuickFormItem *QgsQuickAttributeController::formItem( const QUuid &id ) const
{
  if ( isValidFormId( id ) )
  {
    return mFormItems[id].get();
  }
  else
  {
    return nullptr;
  }
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
  if ( isValidFormId( id ) )
  {
    bool oldVal = mFormItems[id]->shouldRememberValue();
    if ( shouldRememberValue != oldVal )
    {
      mFormItems[id]->setShouldRememberValue( shouldRememberValue );
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

bool QgsQuickAttributeController::setFormValue( const QUuid &id, QVariant value )
{
  if ( isValidFormId( id ) )
  {
    QVariant oldVal = mFormItems[id]->value();
    if ( value != oldVal )
    {
      mFormItems[id]->setValue( value );
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
