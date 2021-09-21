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
#include "appresources.h"

#include "qgsfield.h"
#include "qgsvectorlayerutils.h"

#include <QRegularExpression>
#include <QLocale>

FieldValidator::FieldValidator( QObject *parent ) :
  QObject( parent )
{
}

bool FieldValidator::validate( const FeatureLayerPair &pair, const FormItem &item, QString &validationMessage, ValidationMessageLevel &level )
{
  validationMessage = QString();
  level = Info;

  // Ignore fid field and relations
  if ( item.type() != FormItem::Field || item.name() == QStringLiteral( "fid" ) )
  {
    return true;
  }

  bool valueIsValid = true;

  const QgsField field = item.field();
  QVariant value = pair.feature().attribute( item.fieldIndex() );

  bool isNumeric = item.editorWidgetType() == QStringLiteral( "Range" ) || field.isNumeric();
  if ( isNumeric )
  {
    valueIsValid = validateNumericField( item, value, validationMessage, level );
  }
  else if ( item.editorWidgetType() == QStringLiteral( "TextEdit" ) )
  {
    valueIsValid = validateTextField( item, value, validationMessage, level );
  }
  else
  {
    valueIsValid = validateGenericField( item, value, validationMessage, level );
  }

  if ( !valueIsValid )
    return false;

  // Continue to check hard and soft QGIS constraints
  QStringList errors;

  bool hardConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( pair.layer(),  pair.feature(), item.fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthHard );
  if ( !hardConstraintSatisfied )
  {
    validationMessage = constructConstraintValidationMessage( item, errors );
    level = Error;
    return false;
  }

  errors.clear();

  bool softConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( pair.layer(),  pair.feature(), item.fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthSoft );
  if ( !softConstraintSatisfied )
  {
    validationMessage = constructConstraintValidationMessage( item, errors );
    level = Warning;
    return false;
  }

  return true;
}

bool FieldValidator::validateTextField( const FormItem &item, QVariant &value, QString &validationMessage, ValidationMessageLevel &level )
{
  const QgsField field = item.field();

  if ( !field.convertCompatible( value ) )
  {
    validationMessage = Resources::Texts::Validation::genericValidationFailed;
    level = Error;
    return false;
  }

  // Check if the text is not too long for the field
  if ( field.length() > 0 )
  {
    const int vLength = value.toString().length();

    if ( vLength > field.length() )
    {
      validationMessage = Resources::Texts::Validation::textTooLong.arg( field.length() );
      level = Error;
      return false;
    }
  }
  return true;
}

bool FieldValidator::validateNumericField( const FormItem &item, QVariant &value, QString &validationMessage, ValidationMessageLevel &level )
{
  const QgsField field = item.field();

  if ( value.isNull() )
  {
    return true;
  }

  QString errorMessage;

  bool containsDecimals = value.toString().contains( QLocale().decimalPoint() ) || value.toString().contains( "." );

  if ( !field.convertCompatible( value, &errorMessage ) )
  {
    if ( errorMessage.contains( QStringLiteral( "too large" ) ) )
    {
      validationMessage = Resources::Texts::Validation::numberExceedingVariableLimits;
    }
    else
    {
      validationMessage = Resources::Texts::Validation::numberInvalid;
    }

    level = Error;
    return false;
  }
  else if ( containsDecimals && field.type() != QVariant::Type::Double )
  {
    /* ConverCompatible check passes for doubles written into int fields,
     * however, the value would not be saved and would get replaced by zero,
     * so we need to handle it here and set invalid state for such input.
     */
    validationMessage = Resources::Texts::Validation::numberMustBeInt;
    level = Error;
    return false;
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
      validationMessage = Resources::Texts::Validation::numberLowerBoundReached.arg( min );
      level = Error;
      return false;
    }
    else if ( val > max )
    {
      validationMessage = Resources::Texts::Validation::numberUpperBoundReached.arg( max );
      level = Error;
      return false;
    }
  }

  return true;
}

bool FieldValidator::validateGenericField( const FormItem &item, QVariant &value, QString &validationMessage, ValidationMessageLevel &level )
{
  const QgsField field = item.field();

  if ( !field.convertCompatible( value ) )
  {
    validationMessage = Resources::Texts::Validation::genericValidationFailed;
    level = Error;
    return false;
  }

  return true;
}

QString FieldValidator::constructConstraintValidationMessage( const FormItem &item, const QStringList &unmetConstraints )
{
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
      validationMessages << Resources::Texts::Validation::hardNotNullFailed;
    }
    else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
    {
      validationMessages << Resources::Texts::Validation::softNotNullFailed;
    }
  }

  bool hasUniqueConstraint = fldCons.constraints() & QgsFieldConstraints::ConstraintUnique;
  bool uniqueViolated = unmetConstraints.contains( QStringLiteral( "value is not unique" ) );

  if ( hasUniqueConstraint && uniqueViolated )
  {
    QgsFieldConstraints::ConstraintStrength strength = fldCons.constraintStrength( QgsFieldConstraints::ConstraintUnique );

    if ( strength == QgsFieldConstraints::ConstraintStrengthHard )
    {
      validationMessages << Resources::Texts::Validation::hardUniqueFailed;
    }
    else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
    {
      validationMessages << Resources::Texts::Validation::softUniqueFailed;
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
        validationMessages << Resources::Texts::Validation::hardExpressionFailed;
      }
      else if ( strength == QgsFieldConstraints::ConstraintStrengthSoft )
      {
        validationMessages << Resources::Texts::Validation::softExpressionFailed;
      }
    }
  }

  if ( validationMessages.size() )
  {
    return validationMessages.join( QStringLiteral( "\n" ) ); // each message on new line
  }

  return QString();
}
