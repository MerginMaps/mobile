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

#include <QRegularExpression>
#include <QLocale>

FieldValidator::FieldValidator( QObject *parent ) :
  QObject( parent )
{
}

FieldValidator::ValidationStatus FieldValidator::validate( const FeatureLayerPair &pair, const FormItem &item, QString &validationMessage )
{
  validationMessage = QString();

  // Ignore fid field and relations
  if ( item.type() != FormItem::Field || item.name() == QStringLiteral( "fid" ) )
  {
    return Valid;
  }

  ValidationStatus state = Valid;

  const QgsField field = item.field();
  QVariant value = pair.feature().attribute( item.fieldIndex() );

  bool isNumeric = item.editorWidgetType() == QStringLiteral( "Range" ) || field.isNumeric();
  if ( isNumeric )
  {
    state = validateNumericField( item, value, validationMessage );
  }
  else if ( item.editorWidgetType() == QStringLiteral( "TextEdit" ) )
  {
    state = validateTextField( item, value, validationMessage );
  }
  else
  {
    state = validateGenericField( item, value, validationMessage );
  }

  if ( state != Valid )
    return state;

  // Continue to check hard and soft QGIS constraints
  QStringList errors;

  bool hardConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( pair.layer(),  pair.feature(), item.fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthHard );
  if ( !hardConstraintSatisfied )
  {
    validationMessage = constructConstraintValidationMessage( item, errors );
    return Error;
  }

  errors.clear();

  bool softConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( pair.layer(),  pair.feature(), item.fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthSoft );
  if ( !softConstraintSatisfied )
  {
    validationMessage = constructConstraintValidationMessage( item, errors );
    return Warning;
  }

  return Valid;
}

FieldValidator::ValidationStatus FieldValidator::validateTextField( const FormItem &item, QVariant &value, QString &validationMessage )
{
  const QgsField field = item.field();

  // Check if the text is not too long for the field
  if ( field.length() > 0 )
  {
    const int vLength = value.toString().length();

    if ( vLength > field.length() )
    {
      validationMessage = ValidationTexts::textTooLong.arg( field.length() );
      return Error;
    }
  }

  if ( !field.convertCompatible( value ) )
  {
    validationMessage = ValidationTexts::genericValidationFailed;
    return Error;
  }

  return Valid;
}

FieldValidator::ValidationStatus FieldValidator::validateNumericField( const FormItem &item, QVariant &value, QString &validationMessage )
{
  const QgsField field = item.field();

  if ( value.isNull() )
  {
    return Valid;
  }

  QString errorMessage;

  bool containsDecimals = value.toString().contains( QLocale().decimalPoint() ) || value.toString().contains( "." );

  if ( !field.convertCompatible( value, &errorMessage ) )
  {
    if ( errorMessage.contains( QStringLiteral( "too large" ) ) )
    {
      validationMessage = ValidationTexts::numberExceedingVariableLimits;
    }
    else
    {
      validationMessage = ValidationTexts::numberInvalid;
    }

    return Error;
  }
  else if ( containsDecimals && field.type() != QVariant::Type::Double )
  {
    /* ConverCompatible check passes for doubles written into int fields,
     * however, the value would not be saved and would get replaced by zero,
     * so we need to handle it here and set invalid state for such input.
     */
    validationMessage = ValidationTexts::numberMustBeInt;
    return Error;
  }

  bool isRangeEditable = item.editorWidgetType() == QStringLiteral( "Range" ) &&
                         item.editorWidgetConfig().value( QStringLiteral( "Style" ) ) == QStringLiteral( "SpinBox" );

  // Check min/max range
  if ( isRangeEditable )
  {
    double min = item.editorWidgetConfig().value( "Min" ).toDouble();
    double max = item.editorWidgetConfig().value( "Max" ).toDouble();
    double val = value.toDouble();

    if ( val < min )
    {
      validationMessage = ValidationTexts::numberLowerBoundReached.arg( min );
      return Error;
    }
    else if ( val > max )
    {
      validationMessage = ValidationTexts::numberUpperBoundReached.arg( max );
      return Error;
    }
  }

  return Valid;
}

FieldValidator::ValidationStatus FieldValidator::validateGenericField( const FormItem &item, QVariant &value, QString &validationMessage )
{
  const QgsField field = item.field();

  if ( !field.convertCompatible( value ) )
  {
    validationMessage = ValidationTexts::genericValidationFailed;
    return Error;
  }

  return Valid;
}

QString FieldValidator::constructConstraintValidationMessage( const FormItem &item, const QStringList &unmetConstraints )
{
  /* BEWARE: this method uses QStringList of errors coming from QGIS validation function
   * and does string comparison on them. These error strings are, however, set for translation
   * in QGIS, so comparisons would fail if we would want to translate QGIS strings too.
   */

  const QgsField field = item.field();
  QgsFieldConstraints fldCons = field.constraints();
  QStringList validationMessages;

  bool hasNotNullConstraint = fldCons.constraints() & QgsFieldConstraints::ConstraintNotNull;
  bool notNullViolated = unmetConstraints.contains( QStringLiteral( "value is NULL" ) );

  if ( hasNotNullConstraint && notNullViolated )
  {
    QgsFieldConstraints::ConstraintStrength strength = fldCons.constraintStrength( QgsFieldConstraints::ConstraintNotNull );

    if ( strength == QgsFieldConstraints::ConstraintStrengthHard )
    {
      validationMessages << ValidationTexts::hardNotNullFailed;
    }
    else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
    {
      validationMessages << ValidationTexts::softNotNullFailed;
    }
  }

  bool hasUniqueConstraint = fldCons.constraints() & QgsFieldConstraints::ConstraintUnique;
  bool uniqueViolated = unmetConstraints.contains( QStringLiteral( "value is not unique" ) );

  if ( hasUniqueConstraint && uniqueViolated )
  {
    QgsFieldConstraints::ConstraintStrength strength = fldCons.constraintStrength( QgsFieldConstraints::ConstraintUnique );

    if ( strength == QgsFieldConstraints::ConstraintStrengthHard )
    {
      validationMessages << ValidationTexts::hardUniqueFailed;
    }
    else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
    {
      validationMessages << ValidationTexts::softUniqueFailed;
    }
  }

  bool hasExpressionConstrain = fldCons.constraints() & QgsFieldConstraints::ConstraintExpression;
  bool expressionViolated = unmetConstraints.filter( QRegularExpression( "(parser error|evaluation error|check failed)" ) ).size() > 0;

  if ( hasExpressionConstrain && expressionViolated )
  {
    QgsFieldConstraints::ConstraintStrength strength = fldCons.constraintStrength( QgsFieldConstraints::ConstraintExpression );
    bool containsDescription = !fldCons.constraintDescription().isEmpty();

    if ( containsDescription )
    {
      validationMessages << fldCons.constraintDescription();
    }
    else
    {
      if ( strength == QgsFieldConstraints::ConstraintStrengthHard )
      {
        validationMessages << ValidationTexts::hardExpressionFailed;
      }
      else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
      {
        validationMessages << ValidationTexts::softExpressionFailed;
      }
    }
  }

  if ( validationMessages.size() )
  {
    return validationMessages.join( QStringLiteral( "\n" ) ); // each message on new line
  }

  return QString();
}
