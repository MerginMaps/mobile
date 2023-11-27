#ifndef ENUMHELPER_H
#define ENUMHELPER_H

#pragma once

#include <QDebug>
#include <QMetaEnum>
#include <QString>

/**
 * helper class to use enumerators as strings
 */

namespace EnumHelper
{

  // example to use: ErrorCode::Value myEnum = EnumHelper::fromString<ErrorCode::Value>("StorageLimitHit");
  template <typename E>
  E fromString( const QString &text )
  {
    bool ok;
    auto result = static_cast<E>( QMetaEnum::fromType<E>().keyToValue( text.toUtf8(), &ok ) );
    if ( !ok )
    {
      qDebug() << "Failed to convert enum" << text;
      return {};
    }
    return result;
  }

  // example to use: EnumHelper::toString(ErrorCode::StorageLimitHit);
  template <typename E>
  QString toString( E value )
  {
    const int intValue = static_cast<int>( value );
    return QString::fromUtf8( QMetaEnum::fromType<E>().valueToKey( intValue ) );
  }

  // example to use: EnumHelper::isEqual(myEnum, "StorageLimitHit");
  template <typename E>
  bool isEqual( E value, const QString &text )
  {
    return toString( value ) == text;
  }

  // example to use: EnumHelper::isEqual("StorageLimitHit", myEnum);
  template <typename E>
  bool isEqual( const QString &text, E value )
  {
    return toString( value ) == text;
  }
}

#endif // ENUMHELPER_H
