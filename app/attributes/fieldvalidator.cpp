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
#include "mixedattributevalue.h"

#include "qgsfield.h"
#include "qgsvectorlayerutils.h"

#include <QRegularExpression>
#include <QLocale>

QString FieldValidator::numberInvalid() { return tr( "Value must be a number" ); };
QString FieldValidator::numberUpperBoundReached() { return tr( "Value must be lower than %1" ); };
QString FieldValidator::numberLowerBoundReached() { return tr( "Value must be higher than %1" ); };
QString FieldValidator::numberExceedingVariableLimits() { return tr( "Value is too large" );};
QString FieldValidator::numberMustBeInt() { return tr( "Field can not contain decimal places" );};
QString FieldValidator::textTooLong() { return tr( "Can not be longer than %1 characters" );};
QString FieldValidator::softNotNullFailed() { return tr( "Field should not be empty" );};
QString FieldValidator::hardNotNullFailed() { return tr( "Field must not be empty" );};
QString FieldValidator::softUniqueFailed() { return tr( "Value should be unique" );};
QString FieldValidator::hardUniqueFailed() { return tr( "Value must be unique" );};
QString FieldValidator::softExpressionFailed() { return tr( "Unmet QGIS expression constraint" );};
QString FieldValidator::hardExpressionFailed() { return tr( "Unmet QGIS expression constraint" );};
QString FieldValidator::genericValidationFailed() { return tr( "Not a valid value" );};

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
  QVariant value = item.rawValue();

  // We also ignore Mixed values when multi-editing, as those fields' values will not be saved to the edited features
  if ( value.userType() == qMetaTypeId<MixedAttributeValue>() )
    return Valid;

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
      validationMessage = textTooLong().arg( field.length() );
      return Error;
    }
  }

  if ( !field.convertCompatible( value ) )
  {
    validationMessage = genericValidationFailed();
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

  // in Qt 6 isNull() does not return true for true if the variant contained an object
  // of a builtin type with an isNull() method that returned true for that object.
  // So isNull() for QVariant( QString() ) will return false and we need to handle this
  // separately.
  if ( value.userType() == QVariant::String && value.toString().isEmpty() )
  {
    return Valid;
  }

  QString errorMessage;

  bool containsDecimals = value.toString().contains( QLocale().decimalPoint() ) || value.toString().contains( "." );

  if ( !field.convertCompatible( value, &errorMessage ) )
  {
    if ( errorMessage.contains( QStringLiteral( "too large" ) ) )
    {
      validationMessage = numberExceedingVariableLimits();
    }
    else
    {
      validationMessage = numberInvalid();
    }

    return Error;
  }
  else if ( containsDecimals && field.type() != QMetaType::Type::Double )
  {
    /* ConverCompatible check passes for doubles written into int fields,
     * however, the value would not be saved and would get replaced by zero,
     * so we need to handle it here and set invalid state for such input.
     */
    validationMessage = numberMustBeInt();
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
      validationMessage = numberLowerBoundReached().arg( min );
      return Error;
    }
    else if ( val > max )
    {
      validationMessage = numberUpperBoundReached().arg( max );
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
    validationMessage = genericValidationFailed();
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
      validationMessages << hardNotNullFailed();
    }
    else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
    {
      validationMessages << softNotNullFailed();
    }
  }

  bool hasUniqueConstraint = fldCons.constraints() & QgsFieldConstraints::ConstraintUnique;
  bool uniqueViolated = unmetConstraints.contains( QStringLiteral( "value is not unique" ) );

  if ( hasUniqueConstraint && uniqueViolated )
  {
    QgsFieldConstraints::ConstraintStrength strength = fldCons.constraintStrength( QgsFieldConstraints::ConstraintUnique );

    if ( strength == QgsFieldConstraints::ConstraintStrengthHard )
    {
      validationMessages << hardUniqueFailed();
    }
    else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
    {
      validationMessages << softUniqueFailed();
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
        validationMessages << hardExpressionFailed();
      }
      else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
      {
        validationMessages << softExpressionFailed();
      }
    }
  }

  if ( validationMessages.size() )
  {
    return validationMessages.join( QStringLiteral( "\n" ) ); // each message on new line
  }

  return QString();
}
