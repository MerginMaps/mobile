/***************************************************************************
  identifykit.cpp
 ---------------------
  Date                 : 30.8.2016
  Copyright            : (C) 2016 by Matthias Kuhn
  Email                : matthias (at) opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmessagelog.h"
#include "qgsproject.h"
#include "qgslogger.h"
#include "qgsrenderer.h"
#include "qgsvectorlayer.h"

#include "identifykit.h"
#include "qgsexpressioncontextutils.h"

#include "qgis.h"

IdentifyKit::IdentifyKit( QObject *parent )
  : QObject( parent )
{
}

InputMapSettings *IdentifyKit::mapSettings() const
{
  return mMapSettings;
}

void IdentifyKit::setMapSettings( InputMapSettings *mapSettings )
{
  if ( mapSettings == mMapSettings )
    return;

  mMapSettings = mapSettings;
  emit mapSettingsChanged();
}

FeatureLayerPairs IdentifyKit::identify( const QPointF &point, QgsVectorLayer *layer )
{
  FeatureLayerPairs results;

  if ( !mMapSettings )
  {
    QgsDebugError( QStringLiteral( "Unable to use IdentifyKit without mapSettings property set." ) );
    return results;
  }

  QgsPointXY mapPoint = mMapSettings->screenToCoordinate( point );

  if ( layer )
  {
    QgsFeatureList featureList = identifyVectorLayer( layer, mapPoint );
    for ( const QgsFeature &feature : featureList )
    {
      results.append( FeatureLayerPair( feature, layer ) );
    }
    QgsDebugMsgLevel( QStringLiteral( "IdentifyKit identified %1 results for layer %2" ).arg( results.count() ).arg( layer->name() ), 2 );
  }
  else
  {
    const QString annotationLayer = mMapSettings->project() ? mMapSettings->project()->readEntry( QStringLiteral( "Mergin" ), QStringLiteral( "MapAnnotations/Layer" ) ) : QString();
    for ( QgsMapLayer *layer : mMapSettings->mapSettings().layers() )
    {
      if ( mMapSettings->project() &&
           ( !layer->flags().testFlag( QgsMapLayer::Identifiable )
             || layer->id() == annotationLayer ) )
        continue;

      QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( layer );
      if ( vl )
      {
        QgsFeatureList featureList = identifyVectorLayer( vl, mapPoint );

        for ( const QgsFeature &feature : featureList )
        {
          results.append( FeatureLayerPair( feature, vl ) );
        }
      }
      if ( mIdentifyMode == IdentifyMode::TopDownStopAtFirst && !results.isEmpty() )
      {
        QgsDebugMsgLevel( QStringLiteral( "IdentifyKit identified %1 results with TopDownStopAtFirst mode." ).arg( results.count() ), 2 );
        return results;
      }
    }

    QgsDebugMsgLevel( QStringLiteral( "IdentifyKit identified %1 results" ).arg( results.count() ), 2 );
  }

  return results;
}

static FeatureLayerPair _closestFeature( const FeatureLayerPairs &results, const InputMapSettings &mapSettings, const QPointF &point, double searchRadius )
{
  QgsPointXY mapPoint = mapSettings.screenToCoordinate( point.toPoint() );

  QgsGeometry mapPointGeom( QgsGeometry::fromPointXY( mapPoint ) );

  double distMinPoint = 1e10, distMinLine = 1e10, distMinPolygon = 1e10;
  int iMinPoint = -1, iMinLine = -1, iMinPolygon = -1;
  for ( int i = 0; i < results.count(); ++i )
  {
    const FeatureLayerPair &res = results.at( i );
    QgsGeometry geom( res.feature().geometry() );
    try
    {
      geom.transform( mapSettings.mapSettings().layerTransform( res.layer() ) );
    }
    catch ( QgsCsException &e )
    {
      Q_UNUSED( e )
      // Caught an error in transform
      continue;
    }

    double dist = geom.distance( mapPointGeom );
    Qgis::GeometryType type = geom.type();
    if ( type == Qgis::GeometryType::Point )
    {
      if ( dist < distMinPoint )
      {
        iMinPoint = i;
        distMinPoint = dist;
      }
    }
    else if ( type == Qgis::GeometryType::Line )
    {
      if ( dist < distMinLine )
      {
        iMinLine = i;
        distMinLine = dist;
      }
    }
    else  // polygons
    {
      if ( dist < distMinPolygon )
      {
        iMinPolygon = i;
        distMinPolygon = dist;
      }
    }
  }

  // we give priority to points, then lines, then polygons
  // the rationale is that points in polygon (or on a line) would have nearly
  // always non-zero distance while polygon surrounding it has zero distance,
  // so it would be difficult to identify it
  if ( iMinPoint != -1 && distMinPoint <= searchRadius )
    return results.at( iMinPoint );
  else if ( iMinLine != -1 && distMinLine <= searchRadius )
    return results.at( iMinLine );
  else if ( iMinPolygon != -1 )
    return results.at( iMinPolygon );
  else
    return FeatureLayerPair();
}

FeatureLayerPair IdentifyKit::identifyOne( const QPointF &point, QgsVectorLayer *layer )
{
  FeatureLayerPairs results = identify( point, layer );
  return _closestFeature( results, *mMapSettings, point, searchRadiusMU() );
}

QgsFeatureList IdentifyKit::identifyVectorLayer( QgsVectorLayer *layer, const QgsPointXY &point ) const
{
  QgsFeatureList results;

  if ( !layer || !layer->isSpatial() )
    return results;

  if ( !layer->isInScaleRange( mMapSettings->mapSettings().scale() ) )
    return results;

  QgsFeatureList featureList;

  // toLayerCoordinates will throw an exception for an 'invalid' point.
  // For example, if you project a world map onto a globe using EPSG 2163
  // and then click somewhere off the globe, an exception will be thrown.
  try
  {
    // create the search rectangle
    double searchRadius = searchRadiusMU();

    QgsRectangle r;
    r.setXMinimum( point.x() - searchRadius );
    r.setXMaximum( point.x() + searchRadius );
    r.setYMinimum( point.y() - searchRadius );
    r.setYMaximum( point.y() + searchRadius );

    r = toLayerCoordinates( layer, r );

    QgsFeatureRequest req;
    req.setFilterRect( r );
    req.setLimit( mFeaturesLimit );
    req.setFlags( Qgis::FeatureRequestFlag::ExactIntersect );

    QgsFeatureIterator fit = layer->getFeatures( req );
    QgsFeature f;
    while ( fit.nextFeature( f ) )
      featureList << QgsFeature( f );
  }
  catch ( QgsCsException &cse )
  {
    QgsDebugError( QStringLiteral( "Invalid point, proceed without a found features." ) );
    Q_UNUSED( cse )
  }

  bool filter = false;

  QgsRenderContext context( QgsRenderContext::fromMapSettings( mMapSettings->mapSettings() ) );
  context.expressionContext() << QgsExpressionContextUtils::layerScope( layer );
  QgsFeatureRenderer *renderer = layer->renderer();
  if ( renderer && renderer->capabilities() & QgsFeatureRenderer::ScaleDependent )
  {
    // setup scale for scale dependent visibility (rule based)
    renderer->startRender( context, layer->fields() );
    filter = renderer->capabilities() & QgsFeatureRenderer::Filter;
  }

  for ( const QgsFeature &feature : featureList )
  {
    context.expressionContext().setFeature( feature );

    if ( filter && !renderer->willRenderFeature( const_cast<QgsFeature &>( feature ), context ) )
      continue;

    results.append( feature );
  }

  if ( renderer && renderer->capabilities() & QgsFeatureRenderer::ScaleDependent )
  {
    renderer->stopRender( context );
  }

  return results;
}

double IdentifyKit::searchRadiusMU( const QgsRenderContext &context ) const
{
  return mSearchRadiusMm * context.scaleFactor() * context.mapToPixel().mapUnitsPerPixel();
}

double IdentifyKit::searchRadiusMU() const
{
  QgsRenderContext context = QgsRenderContext::fromMapSettings( mMapSettings->mapSettings() );
  return searchRadiusMU( context );
}

QgsRectangle IdentifyKit::toLayerCoordinates( QgsMapLayer *layer, const QgsRectangle &rect ) const
{
  return mMapSettings->mapSettings().mapToLayerCoordinates( layer, rect );
}

double IdentifyKit::searchRadiusMm() const
{
  return mSearchRadiusMm;
}

void IdentifyKit::setSearchRadiusMm( double searchRadiusMm )
{
  if ( qgsDoubleNear( mSearchRadiusMm, searchRadiusMm ) )
    return;

  mSearchRadiusMm = searchRadiusMm;
  emit searchRadiusMmChanged();
}

int IdentifyKit::featuresLimit() const
{
  return mFeaturesLimit;
}

void IdentifyKit::setFeaturesLimit( int limit )
{
  if ( mFeaturesLimit == limit )
    return;

  mFeaturesLimit = limit;
  emit featuresLimitChanged();
}
