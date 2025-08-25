/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "trackingutils.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"
#include "qgslinestring.h"

#include "variablesmanager.h"
#include "trackingmanager.h"
#include "featurelayerpair.h"
#include "inpututils.h"

QgsCoordinateReferenceSystem TrackingUtils::crs()
{
  return QgsCoordinateReferenceSystem::fromEpsgId( 4326 );
}

bool TrackingUtils::projectHasTrackingCapability( QgsProject *project )
{
  if ( !project )
  {
    return false;
  }

  return project->readBoolEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/Enabled" ), false );
}

QgsVectorLayer *TrackingUtils::getTrackingLayer( QgsProject *project )
{
  if ( !project )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Received invalid reference to QGIS project" ) );
    return nullptr;
  }

  QString trackingLayerId = project->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/TrackingLayer" ), QStringLiteral() );

  if ( trackingLayerId.isEmpty() )
  {
    // emit trackingErrorOccured( tr( "Could not find tracking layer for the project" ) );

    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Tracking layer not found" ) );
    return nullptr;
  }

  QgsVectorLayer *trackingLayer = project->mapLayer<QgsVectorLayer *>( trackingLayerId );

  if ( !trackingLayer || !trackingLayer->isValid() )
  {
    // emit trackingErrorOccured( tr( "Tracking layer not found or invalid" ) );

    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Tracking layer not found or invalid" ) );
    return nullptr;
  }

  return trackingLayer;
}


TrackingUtils::UpdateFrequency TrackingUtils::getTrackingUpdateFrequency( QgsProject *project )
{
  TrackingUtils::UpdateFrequency frequency = TrackingUtils::BestAccuracy;

  if ( !project )
  {
    return frequency;
  }

  int readFrequency = project->readNumEntry( QStringLiteral( "Mergin" ), QStringLiteral( "PositionTracking/UpdateFrequency" ), 0 );

  if ( readFrequency == 0 )
  {
    frequency = TrackingUtils::BestAccuracy;
  }
  else if ( readFrequency == 1 )
  {
    frequency = TrackingUtils::LessAccuracy;
  }
  else
  {
    frequency = TrackingUtils::EvenWorseAccuracy;
  }

  return frequency;
}

void TrackingUtils::storeTrack( QgsVectorLayer *targetLayer, TrackingManager *trackingManager, VariablesManager *variablesManager )
{
  if ( !targetLayer || !targetLayer->isValid() )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Could not save track, layer is missing or invalid" ) );
    return;
  }

  if ( !trackingManager )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Could not save track, received invalid reference to tracking manager" ) );
    return;
  }

  // convert captured geometry to the destination layer's CRS
  QgsGeometry geometryInLayerCRS = InputUtils::transformGeometry( trackingManager->geometry(), TrackingUtils::crs(), targetLayer );

  // create feature - add tracking variables to scope
  QgsExpressionContextScope *scope = new QgsExpressionContextScope( QStringLiteral( "MM_Tracking" ) );
  scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "tracking_start_time" ), trackingManager->startTime(), true, true ) );
  scope->addVariable( QgsExpressionContextScope::StaticVariable( QStringLiteral( "tracking_end_time" ), QDateTime::currentDateTime(), true, true ) );


  FeatureLayerPair trackedFeature;

  if ( variablesManager ) // TODO: shouldn't the variables manager be mandatory? When would we not have it?
  {
    trackedFeature = InputUtils::createFeatureLayerPair( targetLayer, geometryInLayerCRS, variablesManager, scope );
  }
  else
  {
    trackedFeature = InputUtils::createFeatureLayerPair( targetLayer, geometryInLayerCRS, nullptr, scope );
  }

  if ( !targetLayer->startEditing() )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Could not save track, tracking layer can not be opened for editing" ) );
    return;
  }

  targetLayer->addFeature( trackedFeature.featureRef() );

  if ( !targetLayer->commitChanges() )
  {
    CoreUtils::log( QStringLiteral( "Position tracking" ),
                    QStringLiteral( "Failed to commit track:\n%1" )
                    .arg( targetLayer->commitErrors().join( QLatin1Char( '\n' ) ) ) );

    targetLayer->rollBack();
  }
  else
  {
    targetLayer->triggerRepaint();

    CoreUtils::log( QStringLiteral( "Position tracking" ), QStringLiteral( "Feature %1 commited" ).arg( trackedFeature.featureRef().id() ) );
  }
}

void TrackingUtils::parseAndAppendPositionUpdates( const QString &data, QgsGeometry &geometry )
{
  // We currently support tracking only fro linestrings
  QgsLineString *line = qgsgeometry_cast<QgsLineString *>( geometry.constGet() );
  if ( !line )
  {
    qCritical() << "Error, could not cast tracked geometry to a line!";
    return;
  }

  QStringList positions = data.split( '\n', Qt::SkipEmptyParts );

  if ( positions.isEmpty() )
  {
    return;
  }

  for ( int ix = 0; ix < positions.size(); ix++ )
  {
    QStringList coordinates = positions[ix].split( ' ', Qt::SkipEmptyParts );

    if ( coordinates.size() != 4 )
    {
      continue;
    }

    QgsPoint geop(
      coordinates[0].toDouble(), // long
      coordinates[1].toDouble(), // lat
      coordinates[2].toDouble(), // alt
      coordinates[3].toDouble(), // UTC time in secs
      Qgis::WkbType::PointZM // explicitly mention the point type
    );

    line->addVertex( geop );
  }
}
