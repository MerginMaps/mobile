/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsquickexpressioncontextutils.h"
#include "qgsquickpositionkit.h"

QgsExpressionContextScope *QgsQuickExpressionContextUtils::positionScope( PositionInfo positionInformation, bool usesGpsPosition )
{
  QgsExpressionContextScope *scope = new QgsExpressionContextScope( QObject::tr( "Position" ) );

  const QgsGeometry point = QgsGeometry( new QgsPoint( positionInformation.longitude, positionInformation.latitude, positionInformation.altitude ) );
  addPositionVariable( scope, QStringLiteral( "longitude" ), notNaNorEmpty( positionInformation.longitude ) );
  addPositionVariable( scope, QStringLiteral( "latitude" ), notNaNorEmpty( positionInformation.latitude ) );
  addPositionVariable( scope, QStringLiteral( "altitude" ), notNaNorEmpty( positionInformation.altitude ) );
  addPositionVariable( scope, QStringLiteral( "coordinate" ), QVariant::fromValue<QgsGeometry>( point ) );
  addPositionVariable( scope, QStringLiteral( "direction" ), notNaNorEmpty( positionInformation.direction ) );
  addPositionVariable( scope, QStringLiteral( "ground_speed" ), notNaNorEmpty( positionInformation.groundSpeed ) );
  addPositionVariable( scope, QStringLiteral( "magnetic_variation" ), notNaNorEmpty( positionInformation.magneticVariation ) );
  addPositionVariable( scope, QStringLiteral( "horizontal_accuracy" ), notNaNorEmpty( positionInformation.horizontalAccuracy ) );
  addPositionVariable( scope, QStringLiteral( "vertical_accuracy" ), notNaNorEmpty( positionInformation.verticalAccuracy ) );
  addPositionVariable( scope, QStringLiteral( "vertical_speed" ), notNaNorEmpty( positionInformation.verticalSpeed ) );
  addPositionVariable( scope, QStringLiteral( "from_gps" ), usesGpsPosition );

  return scope;
}

void QgsQuickExpressionContextUtils::addPositionVariable( QgsExpressionContextScope *scope, const QString &name, const QVariant &value, const QVariant &defaultValue )
{
  if ( value.isValid() )
  {
    scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "position_%1" ).arg( name ), value, true, true ) );
  }
  else
  {
    scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "position_%1" ).arg( name ), defaultValue, true, true ) );
  }
}

QVariant QgsQuickExpressionContextUtils::notNaNorEmpty( double value )
{
  return ( std::isnan( value ) ) ? QVariant() : QVariant( value );
}
