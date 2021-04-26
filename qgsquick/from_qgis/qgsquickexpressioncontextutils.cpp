#include "qgsquickexpressioncontextutils.h"
#include "qgsquickpositionkit.h"

QgsExpressionContextScope *QgsQuickExpressionContextUtils::positionScope( PositionInfo &positionInformation )
{
  QgsExpressionContextScope *scope = new QgsExpressionContextScope( QObject::tr( "Position" ) );
  //const QgsGeometry point = QgsGeometry( new QgsPoint( positionInformation.longitude(), positionInformation.latitude(), positionInformation.elevation() ) );
  //addPositionVariable( scope, QStringLiteral( "coordinate" ), QVariant::fromValue<QgsGeometry>( positionInformation.point ) );

  addPositionVariable( scope, QStringLiteral( "timestamp" ), positionInformation.timestamp );
  addPositionVariable( scope, QStringLiteral( "direction" ), positionInformation.direction );
  addPositionVariable( scope, QStringLiteral( "ground_speed" ), positionInformation.groundSpeed );
  addPositionVariable( scope, QStringLiteral( "magnetic_variation" ), positionInformation.magneticVariation );
  addPositionVariable( scope, QStringLiteral( "horizontal_accuracy" ), positionInformation.horizontalAccuracy );
  addPositionVariable( scope, QStringLiteral( "vertical_accuracy" ), positionInformation.verticalAccuracy );
  addPositionVariable( scope, QStringLiteral( "vertical_speed" ), positionInformation.verticalSpeed );
  addPositionVariable( scope, QStringLiteral( "horizontal_accuracy" ), positionInformation.horizontalAccuracy );
  addPositionVariable( scope, QStringLiteral( "vertical_accuracy" ), positionInformation.verticalAccuracy );

  return scope;
}

QgsExpressionContextScope *QgsQuickExpressionContextUtils::positionScopeTest( const QString &test )
{
  QgsExpressionContextScope *scope = new QgsExpressionContextScope( QObject::tr( "Position" ) );

  addPositionVariable( scope, QStringLiteral( "vertical_speed" ), 42 );
  addPositionVariable( scope, QStringLiteral( "ground_speed" ), 42 );

  return scope;
}

void QgsQuickExpressionContextUtils::addPositionVariable( QgsExpressionContextScope *scope, const QString &name, const QVariant &value, const QVariant &defaultValue )
{
  scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "position_%1" ).arg( name ), value, true, true ) );
}
