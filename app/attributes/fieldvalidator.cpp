/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "fieldvalidator.h"
#include "attributedata.h"
#include "featurelayerpair.h"

#include "qgsfield.h"
#include "qgsvectorlayerutils.h"

#include "qdebug.h"

FieldValidator::FieldValidator( QObject *parent ) :
  QObject( parent )
{
}

FieldValidator::FieldValueState FieldValidator::validate( const FeatureLayerPair &pair, const FormItem &item )
{
  // Ignore fid field and relations
  if ( item.type() != FormItem::Field || item.name() == QStringLiteral( "fid" ) )
  {
    return ValidValue;
  }

  FieldValueState newState = ValidValue;

  const QgsField field = item.field();
  QVariant value = pair.feature().attribute( item.fieldIndex() );

  bool isNumeric = item.editorWidgetType() == QStringLiteral( "Range" ) || field.isNumeric();
  if ( isNumeric )
  {
    newState = validateNumericField( item, value );
  }
  else if ( item.editorWidgetType() == QStringLiteral( "Text" ) )
  {
    newState = validateTextField( item, value );
  }
  else
  {
    QString errMsg;
    if ( !field.convertCompatible( value, &errMsg ) )
      newState = InvalidValue;
  }

  if ( newState != ValidValue )
    return newState;

  // continue to check hard and soft QGIS constraints
  QStringList errors;

  bool hardConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( pair.layer(),  pair.feature(), item.fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthHard );
  if ( !hardConstraintSatisfied )
    return QgsHardConstraintUnsatisfied;

  bool softConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( pair.layer(),  pair.feature(), item.fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthSoft );
  if ( !softConstraintSatisfied )
    return QgsSoftConstraintUnsatisfied;

  return ValidValue;
}

QString FieldValidator::stateToString( FieldValueState state )
{
  switch( state ) {
    case ValidValue: return QString();
    case InvalidValue: return tr( "Invalid field value" );
    case ValueOutOfRange: return tr( "Value is out of range" );
    default: return QString();
  }
}

FieldValidator::StateImportance FieldValidator::stateToImportance( FieldValueState state )
{
  if ( state == ValidValue )
    return StateImportance::Info;
  else if ( state == QgsSoftConstraintUnsatisfied )
    return StateImportance::Warning;

  return StateImportance::Error;
}

FieldValidator::FieldValueState FieldValidator::validateTextField( const FormItem &item, const QVariant &value )
{
  ( void ) item;
  ( void ) value;
  return ValidValue;
}

FieldValidator::FieldValueState FieldValidator::validateNumericField( const FormItem &item, const QVariant &value )
{
  bool isRangeEditable = item.editorWidgetType() == QStringLiteral( "Range" ) &&
                         item.editorWidgetConfig()[QStringLiteral( "Style" )] == QStringLiteral( "SpinBox" );

  if ( isRangeEditable && !value.isNull() )
  {
    double min = item.editorWidgetConfig()[QStringLiteral( "Min" )].toDouble();
    double max = item.editorWidgetConfig()[QStringLiteral( "Max" )].toDouble();
    double val = value.toDouble();

    if ( !( min <= val && val <= max ) )
    {
      return ValueOutOfRange;
    }
  }

  return ValidValue;
}
