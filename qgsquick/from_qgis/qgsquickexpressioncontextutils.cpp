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
  addPositionVariable( scope, QStringLiteral( "coordinate" ), QVariant::fromValue<QgsGeometry>( point ) );
  //addPositionVariable( scope, QStringLiteral( "timestamp" ), positionInformation.timestamp );
  addPositionVariable( scope, QStringLiteral( "direction" ), positionInformation.direction );
  addPositionVariable( scope, QStringLiteral( "ground_speed" ), positionInformation.groundSpeed );
  addPositionVariable( scope, QStringLiteral( "magnetic_variation" ), positionInformation.magneticVariation );
  addPositionVariable( scope, QStringLiteral( "horizontal_accuracy" ), positionInformation.horizontalAccuracy );
  addPositionVariable( scope, QStringLiteral( "vertical_accuracy" ), positionInformation.verticalAccuracy );
  addPositionVariable( scope, QStringLiteral( "vertical_speed" ), positionInformation.verticalSpeed );
  addPositionVariable( scope, QStringLiteral( "from_gps" ), usesGpsPosition );

  return scope;
}

void QgsQuickExpressionContextUtils::addPositionVariable( QgsExpressionContextScope *scope, const QString &name, const QVariant &value, const QVariant &defaultValue )
{
  // TODO @vsklencar check validity of the value
  if ( true )
  {
    scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "position_%1" ).arg( name ), value, true, true ) );
  }
  else
  {
    scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "position_%1" ).arg( name ), defaultValue, true, true ) );
  }
}
