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

#include "qdebug.h"

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
  else if ( item.editorWidgetType() == QStringLiteral( "Text" ) )
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
    validationMessage = constructConstraintValidationMessage( item, true );
    level = Error;
    return false;
  }

  bool softConstraintSatisfied = QgsVectorLayerUtils::validateAttribute( pair.layer(),  pair.feature(), item.fieldIndex(), errors, QgsFieldConstraints::ConstraintStrengthSoft );
  if ( !softConstraintSatisfied )
  {
    validationMessage = constructConstraintValidationMessage( item, false );
    level = Warning;
    return false;
  }

  return true;
}

bool FieldValidator::validateTextField( const FormItem &item, QVariant &value, QString &validationMessage, ValidationMessageLevel &level )
{
  const QgsField field = item.field();

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

  // First check if the value is a number
  if ( !field.convertCompatible( value ) ) // TODO: when value is null, return true
  {
    validationMessage = Resources::Texts::Validation::numberValidationFailed;
    level = Error;
    return false;
  }

  // TODO: you might also want to check if length of the value is not higher than field length (if this is text)

  bool isRangeEditable = item.editorWidgetType() == QStringLiteral( "Range" ) &&
                         item.editorWidgetConfig()[QStringLiteral( "Style" )] == QStringLiteral( "SpinBox" );

  // Check min/max range
  if ( isRangeEditable )
  {
    double min = item.editorWidgetConfig().value( "Min" ).toDouble();
    double max = item.editorWidgetConfig().value( "Max" ).toDouble();
    double val = value.toDouble(); // TODO: is this good conversion to double?

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

  // First check if the value is a number
  if ( !field.convertCompatible( value ) )
  {
    validationMessage = Resources::Texts::Validation::genericValidationFailed;
    level = Error;
    return false;
  }

  return true;
}

QString FieldValidator::constructConstraintValidationMessage( const FormItem &item, bool isHardConstraint )
{
  const QgsField field = item.field();
  QStringList validationMessages;

  if ( field.constraints().constraints() & QgsFieldConstraints::ConstraintUnique )
  {
    QString uniqueMsg;

    if ( isHardConstraint )
    {
      uniqueMsg = Resources::Texts::Validation::hardUniqueFailed;
    }
    else // is soft
    {
      uniqueMsg = Resources::Texts::Validation::softUniqueFailed;
    }
    validationMessages << uniqueMsg;
  }

  if ( field.constraints().constraints() & QgsFieldConstraints::ConstraintNotNull )
  {
    QString notNullMsg;

    if ( isHardConstraint )
    {
      notNullMsg = Resources::Texts::Validation::hardNotNullFailed;
    }
    else // is soft
    {
      notNullMsg = Resources::Texts::Validation::softNotNullFailed;
    }
    validationMessages << notNullMsg;
  }

  QString expression = field.constraints().constraintExpression();
  if ( !expression.isEmpty() )
  {
    // Let's show something only if constraint description is provided
    validationMessages << field.constraints().constraintDescription();
  }

  if ( validationMessages.size() )
  {
    return validationMessages.join( QStringLiteral( "\n" ) );
  }

  return QString();
}
