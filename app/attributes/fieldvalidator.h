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

    static QString numberInvalid();
    static QString numberUpperBoundReached();
    static QString numberLowerBoundReached();
    static QString numberExceedingVariableLimits();
    static QString numberMustBeInt();
    static QString textTooLong();
    static QString softNotNullFailed();
    static QString hardNotNullFailed();
    static QString softUniqueFailed();
    static QString hardUniqueFailed();
    static QString softExpressionFailed();
    static QString hardExpressionFailed();
    static QString genericValidationFailed();

    friend class TestAttributeController;
    friend class TestFormEditors;
};

#endif // FIELDVALIDATOR_H
