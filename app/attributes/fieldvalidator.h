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

    enum ValidationMessageLevel
    {
      Info = 0, //!< field can be saved
      Warning,  //!< field can be saved, but there are soft constraints not met
      Error     //!< field can not be saved
    };
    Q_ENUMS( ValidationMessageLevel )

    explicit FieldValidator( QObject *parent = nullptr );

    /**
     * \brief validate function validates value saved in feature inside pair for specific FormItem (field), validates value validity
     * checks, hard and soft constraints from QGIS.
     * \param pair feature within layer to be validated
     * \param item form item, it describes which field are we validating
     * \param validationMessage [out] message will be set if there is a problem with field, empty if field has a valid value
     * \param level [out] importance level of the message, either error (feature can not be saved), warning (can be
     * saved, but have problems) or info either for no error or for helping user understand what field accepts.
     * \return true if have valid value - no error nor warning, false otherwise
     */
    static bool validate( const FeatureLayerPair &pair, const FormItem &item, QString &validationMessage, ValidationMessageLevel &level );

    static bool validateTextField( const FormItem &item, QVariant &value, QString &validationMessage, ValidationMessageLevel &level );
    static bool validateNumericField( const FormItem &item, QVariant &value, QString &validationMessage, ValidationMessageLevel &level );
    static bool validateGenericField( const FormItem &item, QVariant &value, QString &validationMessage, ValidationMessageLevel &level );
    // TODO: what other editors can we validate? value relations maybe?

  private:
    static QString constructConstraintValidationMessage( const FormItem &item );
};

#endif // FIELDVALIDATOR_H
