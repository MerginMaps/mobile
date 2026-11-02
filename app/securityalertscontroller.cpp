/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "securityalertscontroller.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "coreutils.h"
#include "position/positionkit.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"
#include "qgsfeatureiterator.h"
#include "qgsfeaturerequest.h"
#include "qgscoordinatetransform.h"
#include "qgsexpression.h"
#include "qgsexpressioncontextutils.h"
#include "qgsexception.h"

const QString SecurityAlertsController::CONFIG_SCOPE = QStringLiteral( "Mergin" );

SecurityAlertsController::SecurityAlertsController( QObject *parent )
  : QObject( parent )
{
}

void SecurityAlertsController::reload()
{
  mZones.clear();

  if ( mProject && mProject->readBoolEntry( CONFIG_SCOPE, QStringLiteral( "SecurityAlerts/Enabled" ), false ) )
  {
    const QString json = mProject->readEntry( CONFIG_SCOPE, QStringLiteral( "SecurityAlerts/Zones" ) );
    const QJsonArray zones = QJsonDocument::fromJson( json.toUtf8() ).array();
    for ( const QJsonValue &value : zones )
    {
      const QJsonObject zone = value.toObject();
      const QString layerId = zone.value( QStringLiteral( "layer" ) ).toString();
      QgsVectorLayer *layer = mProject->mapLayer<QgsVectorLayer *>( layerId );
      if ( !layer || !layer->isValid() || layer->geometryType() != Qgis::GeometryType::Polygon )
      {
        CoreUtils::log( QStringLiteral( "Security alerts" ), QStringLiteral( "Zone layer %1 not found or not a polygon layer" ).arg( layerId ) );
        continue;
      }
      mZones << Zone { layerId, zone.value( QStringLiteral( "message" ) ).toString(),
                       zone.value( QStringLiteral( "level" ) ).toString( QStringLiteral( "warning" ) ) };
    }
  }

  mActiveKeys.clear();
  mActiveAlerts.clear();
  emit configurationChanged();
  emit activeAlertsChanged();
  checkPosition();
}

void SecurityAlertsController::checkPosition()
{
  if ( !mProject || mZones.isEmpty() || !mPositionKit || !mPositionKit->hasPosition() )
    return;

  const QgsPoint position = mPositionKit->positionCoordinate();
  if ( position.isEmpty() )
    return;

  const QgsCoordinateReferenceSystem wgs84( QStringLiteral( "EPSG:4326" ) );
  QVector<QPair<QString, QVariantMap>> found; // zone key, alert

  for ( const Zone &zone : std::as_const( mZones ) )
  {
    QgsVectorLayer *layer = mProject->mapLayer<QgsVectorLayer *>( zone.layerId );
    if ( !layer )
      continue;

    QgsPointXY point( position.x(), position.y() );
    try
    {
      point = QgsCoordinateTransform( wgs84, layer->crs(), mProject->transformContext() ).transform( point );
    }
    catch ( QgsCsException & )
    {
      continue;
    }

    const QgsGeometry pointGeometry = QgsGeometry::fromPointXY( point );
    QgsExpressionContext context( QgsExpressionContextUtils::globalProjectLayerScopes( layer ) );
    QgsExpression message( zone.message );
    message.prepare( &context );

    QgsFeatureRequest request( QgsRectangle( point.x(), point.y(), point.x(), point.y() ) );
    QgsFeatureIterator it = layer->getFeatures( request );
    QgsFeature feature;
    while ( it.nextFeature( feature ) )
    {
      if ( !feature.geometry().contains( pointGeometry ) )
        continue;

      context.setFeature( feature );
      QString text = message.evaluate( &context ).toString();
      if ( message.hasEvalError() || text.isEmpty() )
        text = zone.message.isEmpty() ? tr( "You are in %1" ).arg( layer->name() ) : zone.message;

      found << qMakePair( QStringLiteral( "%1:%2" ).arg( zone.layerId ).arg( feature.id() ),
                          QVariantMap( { { QStringLiteral( "message" ), text }, { QStringLiteral( "level" ), zone.level },
                            { QStringLiteral( "layer" ), layer->name() } } ) );
    }
  }

  // most severe first
  std::stable_sort( found.begin(), found.end(), []( const QPair<QString, QVariantMap> &a, const QPair<QString, QVariantMap> &b )
  {
    return a.second.value( QStringLiteral( "level" ) ) == QLatin1String( "danger" ) &&
           b.second.value( QStringLiteral( "level" ) ) != QLatin1String( "danger" );
  } );

  QStringList keys;
  QVariantList alerts;
  for ( const auto &entry : std::as_const( found ) )
  {
    keys << entry.first;
    alerts << entry.second;
  }

  if ( keys == mActiveKeys )
    return;

  // notify only about zones that were just entered
  for ( const auto &entry : std::as_const( found ) )
  {
    if ( !mActiveKeys.contains( entry.first ) )
    {
      CoreUtils::log( QStringLiteral( "Security alerts" ), QStringLiteral( "Entered zone %1" ).arg( entry.first ) );
      emit zoneEntered( entry.second.value( QStringLiteral( "message" ) ).toString(), entry.second.value( QStringLiteral( "level" ) ).toString() );
    }
  }

  mActiveKeys = keys;
  mActiveAlerts = alerts;
  emit activeAlertsChanged();
}

QgsProject *SecurityAlertsController::qgsProject() const
{
  return mProject;
}

void SecurityAlertsController::setQgsProject( QgsProject *project )
{
  if ( mProject == project )
    return;
  mProject = project;
  emit qgsProjectChanged();
  reload();
}

PositionKit *SecurityAlertsController::positionKit() const
{
  return mPositionKit;
}

void SecurityAlertsController::setPositionKit( PositionKit *positionKit )
{
  if ( mPositionKit == positionKit )
    return;
  if ( mPositionKit )
    disconnect( mPositionKit, nullptr, this, nullptr );
  mPositionKit = positionKit;
  if ( mPositionKit )
    connect( mPositionKit, &PositionKit::positionChanged, this, &SecurityAlertsController::checkPosition );
  emit positionKitChanged();
}

bool SecurityAlertsController::enabled() const
{
  return !mZones.isEmpty();
}

QVariantList SecurityAlertsController::activeAlerts() const
{
  return mActiveAlerts;
}
