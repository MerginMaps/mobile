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

    enum FieldValueState {
      ValidValue = 1,
      InvalidValue,     // did not pass convertCompatible check
      ValueOutOfRange,  // number is out of min/max range
      QgsHardConstraintUnsatisfied,
      QgsSoftConstraintUnsatisfied
    };
    Q_ENUMS( FieldValueState )

    enum StateImportance {
      Info = 0, //!< form can be saved
      Warning, //!< form can be saved, but there are soft constraints not met
      Error //!< form can not be saved
    };
    Q_ENUMS( StateImportance )

    explicit FieldValidator( QObject *parent = nullptr );

    static FieldValueState validate( const FeatureLayerPair &pair, const FormItem &item );

    static QString stateToString( FieldValueState state );
    static StateImportance stateToImportance( FieldValueState state );

  private:
    static FieldValueState validateTextField( const FormItem &item, const QVariant &value );
    static FieldValueState validateNumericField( const FormItem &item, const QVariant &value );
};

#endif // FIELDVALIDATOR_H
