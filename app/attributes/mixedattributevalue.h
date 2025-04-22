/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MIXEDATTRIBUTEVALUE_H
#define MIXEDATTRIBUTEVALUE_H

#include <QVariant>

/**
 * @brief The MixedAttributeValue class is a helper class for multi-feature editing
 *
 * When the edited features do not have the same value, a QVariant of MixedAttributeValue
 * is used instead. The form widgets then can display a placeholder text and eventually
 * the attribute is not saved to the provider.
 */
class MixedAttributeValue
{
  public:
    MixedAttributeValue();

    //! Allows direct construction of QVariants
    operator QVariant() const
    {
      return QVariant::fromValue( *this );
    }

    QString toString() const { return QObject::tr( "* Mixed Values *" ); }

    inline bool operator==( const MixedAttributeValue & ) const { return true; }

    inline bool operator!=( const MixedAttributeValue & ) const { return false; }
};

Q_DECLARE_METATYPE( MixedAttributeValue )

#endif // MIXEDATTRIBUTEVALUE_H
