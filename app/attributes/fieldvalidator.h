/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FIELDVALIDATOR_H
#define FIELDVALIDATOR_H

#include <QString>
#include <QVariant>
#include <QObject>

class FormItem;
class FeatureLayerPair;

class FieldValidator : public QObject
{
    Q_OBJECT

  public:

    enum ValidationStatus
    {
      Valid = 0, //!< field can be saved, no message
      Warning,  //!< field can be saved, but there are soft constraints not met
      Error     //!< field can not be saved
    };
    Q_ENUMS( ValidationStatus )

    explicit FieldValidator( QObject *parent = nullptr );

    /**
     * \brief validate function validates value saved in feature inside pair for specific FormItem (field), validates value validity
     * checks, hard and soft constraints from QGIS.
     * \param pair feature within layer to be validated
     * \param item form item, it describes which field are we validating
     * \param validationMessage [out] message will be set if there is a problem with field, empty if field has a valid value
     * \return returns state from ValidationStatus enum based on value saved in feature pair
     */
    static ValidationStatus validate( const FeatureLayerPair &pair, const FormItem &item, QString &validationMessage );

    static ValidationStatus validateTextField( const FormItem &item, QVariant &value, QString &validationMessage );
    static ValidationStatus validateNumericField( const FormItem &item, QVariant &value, QString &validationMessage );
    static ValidationStatus validateGenericField( const FormItem &item, QVariant &value, QString &validationMessage );

  private:
    static QString constructConstraintValidationMessage( const FormItem &item, const QStringList &unmetConstraints );
};

namespace ValidationTexts
{

  const QString numberInvalid = QObject::tr( "Value must be a number" );
  const QString numberUpperBoundReached = QObject::tr( "Value must be lower than %1" );
  const QString numberLowerBoundReached = QObject::tr( "Value must be higher than %1" );
  const QString numberExceedingVariableLimits = QObject::tr( "Value is too large" );
  const QString numberMustBeInt = QObject::tr( "Field can not contain decimal places" );

  const QString textTooLong = QObject::tr( "Can not be longer than %1 characters" );

  const QString softNotNullFailed = QObject::tr( "Field should not be empty" );
  const QString hardNotNullFailed = QObject::tr( "Field must not be empty" );
  const QString softUniqueFailed = QObject::tr( "Value should be unique" );
  const QString hardUniqueFailed = QObject::tr( "Value must be unique" );
  const QString softExpressionFailed = QObject::tr( "Unmet QGIS expression constraint" );
  const QString hardExpressionFailed = QObject::tr( "Unmet QGIS expression constraint" );

  const QString genericValidationFailed = QObject::tr( "Not a valid value" );

}

#endif // FIELDVALIDATOR_H
