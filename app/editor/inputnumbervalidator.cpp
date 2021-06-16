#include "inputnumbervalidator.h"

#include "qdebug.h"

InputNumberValidator::InputNumberValidator( QObject *parent ) : QDoubleValidator( parent ) {}

InputNumberValidator::~InputNumberValidator() = default;

InputNumberValidator::InputNumberValidator(
  double bottom,
  double top,
  int decimals,
  QObject *parent
) :
  QDoubleValidator(
    bottom,
    top,
    decimals,
    parent
  )
{

}

QValidator::State InputNumberValidator::validate( QString &s, int & ) const
{
  if ( s.isEmpty() )
  {
    // allow empty field
    return QValidator::Acceptable;
  }

  // we accept empty string -> null
  // do not accept more decimal places than allowed



  if ( s.startsWith( "-" ) && s.length() == 1 )
  {
    // if there is only minus sign, check if the min/max values are below zero
    bool isPositiveRange = qMin( bottom(), top() ) >= 0;

    if ( isPositiveRange )
      return QValidator::Invalid; // do not allow minus sign for only positive values
    else
      return QValidator::Intermediate;
  }
  else if ( !s.startsWith( "-" ) && s.length() == 1 )
  {
    // max/min range of only negative numbers must start with minus sign
    bool isNegativeRange = ( bottom() < 0 && top() < 0 );

    if ( isNegativeRange )
      return QValidator::Invalid;
  }

  // check length of decimal places
  QChar point = locale().decimalPoint();
  bool hasDecimalPoint = s.indexOf( point ) != -1;
  if ( hasDecimalPoint )
  {
    int lengthDecimals = s.length() - s.indexOf( point ) - 1;
    if ( lengthDecimals > decimals()  || decimals() == 0 )
    {
      return QValidator::Invalid;
    }

    if ( s.endsWith( point ) )
      return QValidator::Intermediate;
  }

  // check range of value
  bool isNumber;
  double value = locale().toDouble( s, &isNumber );

  if ( isNumber )
  {
    if ( bottom() <= value && value <= top() )
      return QValidator::Acceptable;
    else
    {
      return QValidator::Intermediate;
//      // we are out of range, but we need to figure out whether there is a possibility
//      // to get to the desired range by typing in new numbers. In that case we are lower
//      // than the minimum bound (in positive range, e.g. value = 10, but range is <100;1000>)
//      // or higher than the maximum bound (in negative range, e.g. value = -10, but range is <-1000;-100>)
//      double minAbsBound = qMin( qAbs( bottom() ), qAbs( top() ) );
//      double absValue = qAbs( value );

//      if ( absValue < minAbsBound )
//      {

//        // we also check here if the number already have decimal point and if so, return invalid because with
//        // decimal places it could not reach the bound (if the bound is in distance of 1 and more)
////        if ( !hasDecimalPoint )
////          return QValidator::Intermediate;
////        else
////        {
////          bool isCloseToBoundary = ( absValue + 1 ) >= minAbsBound;
////          if ( !isCloseToBoundary )
////            return QValidator::Invalid;

////          bool filledAllDecimalPlaces = ( s.length() - s.indexOf( point ) - 1 ) == decimals();
////          if ( filledAllDecimalPlaces )
////            return QValidator::Invalid;

////          return QValidator::Intermediate;
////        }
//      }

    }
  }

  return QValidator::Invalid;
}
