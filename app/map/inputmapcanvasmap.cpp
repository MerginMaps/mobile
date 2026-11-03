/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * The source code forked from https://github.com/qgis/QGIS on 25th Nov 2022
 * File: qgsquickmapcanvasmap.cpp by (C) 2014 by Matthias Kuhn
 */

#include <QQuickWindow>
#include <cmath>
#include <QSignalBlocker>
#include <QSGSimpleTextureNode>
#include <QScreen>

#include "qgis.h"
#include "qgsexpressioncontextutils.h"
#include "qgsmaplayertemporalproperties.h"
#include "qgsmaprenderercache.h"
#include "qgsmaprendererparalleljob.h"
#include "qgsmessagelog.h"
#include "qgspallabeling.h"
#include "qgsproject.h"
#include "qgsannotationlayer.h"
#include "qgsvectorlayer.h"
#include "qgslabelingresults.h"

#include "inputmapcanvasmap.h"
#include "inputmapsettings.h"


InputMapCanvasMap::InputMapCanvasMap( QQuickItem *parent )
  : QQuickItem( parent )
  , mMapSettings( std::make_unique<InputMapSettings>() )
  , mCache( std::make_unique<QgsMapRendererCache>() )
{
  connect( this, &QQuickItem::windowChanged, this, &InputMapCanvasMap::onWindowChanged );
  connect( &mRefreshTimer, &QTimer::timeout, this, [ = ] { refreshMap(); } );
  connect( &mMapUpdateTimer, &QTimer::timeout, this, &InputMapCanvasMap::renderJobUpdated );

  connect( mMapSettings.get(), &InputMapSettings::extentChanged, this, &InputMapCanvasMap::onExtentChanged );
  connect( mMapSettings.get(), &InputMapSettings::rotationChanged, this, &InputMapCanvasMap::onExtentChanged );
  connect( mMapSettings.get(), &InputMapSettings::layersChanged, this, &InputMapCanvasMap::onLayersChanged );
  connect( mMapSettings.get(), &InputMapSettings::temporalStateChanged, this, &InputMapCanvasMap::onTemporalStateChanged );

  connect( this, &InputMapCanvasMap::renderStarting, this, &InputMapCanvasMap::isRenderingChanged );
  connect( this, &InputMapCanvasMap::mapCanvasRefreshed, this, &InputMapCanvasMap::isRenderingChanged );

  mMapUpdateTimer.setSingleShot( false );
  mMapUpdateTimer.setInterval( 250 );
  mRefreshTimer.setSingleShot( true );
  mRotationSettleTimer.setSingleShot( true );
  mRotationSettleTimer.setInterval( 400 );
  connect( &mRotationSettleTimer, &QTimer::timeout, this, &InputMapCanvasMap::onRotationSettled );
  setTransformOrigin( QQuickItem::TopLeft );
  setFlags( QQuickItem::ItemHasContents );
}

InputMapCanvasMap::~InputMapCanvasMap()
{
  stopRendering();
}

InputMapSettings *InputMapCanvasMap::mapSettings() const
{
  return mMapSettings.get();
}

void InputMapCanvasMap::zoom( QPointF center, qreal scale )
{
  QgsRectangle extent = mMapSettings->extent();
  QgsPoint oldCenter( extent.center() );
  QgsPoint mousePos( mMapSettings->screenToCoordinate( center ) );

  QgsPointXY newCenter( mousePos.x() + ( ( oldCenter.x() - mousePos.x() ) * scale ),
                        mousePos.y() + ( ( oldCenter.y() - mousePos.y() ) * scale ) );

  // same as zoomWithCenter (no coordinate transformations are needed)
  extent.scale( scale, &newCenter );
  mMapSettings->setExtent( extent );
}

void InputMapCanvasMap::rotate( QPointF center, qreal angle )
{
  const QgsPoint pivot( mMapSettings->screenToCoordinate( center ) );

  {
    // rotation is applied around the center of the map, so we pan afterwards
    // to keep the pivot under the same screen position. Signals are blocked so that
    // nobody (e.g. following of GPS position) sees the intermediate state.
    const QSignalBlocker blocker( mMapSettings.get() );

    mMapSettings->setRotation( mMapSettings->rotation() + angle );

    const QgsPoint pivotNow( mMapSettings->screenToCoordinate( center ) );
    const QgsPoint mapCenter = mMapSettings->center();
    mMapSettings->setCenter( QgsPoint( mapCenter.x() + pivot.x() - pivotNow.x(), mapCenter.y() + pivot.y() - pivotNow.y() ) );
  }

  mRotating = true;
  emit mMapSettings->rotationChanged();
  emit mMapSettings->extentChanged();
  mRotating = false;
}

void InputMapCanvasMap::pan( QPointF oldPos, QPointF newPos )
{
  QgsPoint start = mMapSettings->screenToCoordinate( oldPos.toPoint() );
  QgsPoint end = mMapSettings->screenToCoordinate( newPos.toPoint() );

  double dx = end.x() - start.x();
  double dy = end.y() - start.y();

  // modify the extent
  QgsRectangle extent = mMapSettings->extent();

  extent.setXMinimum( extent.xMinimum() + dx );
  extent.setXMaximum( extent.xMaximum() + dx );
  extent.setYMaximum( extent.yMaximum() + dy );
  extent.setYMinimum( extent.yMinimum() + dy );

  mMapSettings->setExtent( extent );
}

void InputMapCanvasMap::refreshMap()
{
  stopRendering(); // if any...

  QgsMapSettings mapSettings = mMapSettings->mapSettings();
  if ( !mapSettings.hasValidSettings() )
    return;

  //build the expression context
  QgsExpressionContext expressionContext;
  expressionContext << QgsExpressionContextUtils::globalScope()
                    << QgsExpressionContextUtils::mapSettingsScope( mapSettings );

  QgsProject *project = mMapSettings->project();
  if ( project )
  {
    expressionContext << QgsExpressionContextUtils::projectScope( project );

    mapSettings.setLabelingEngineSettings( project->labelingEngineSettings() );

    // render main annotation layer above all other layers
    QList<QgsMapLayer *> allLayers = mapSettings.layers();
    allLayers.insert( 0, project->mainAnnotationLayer() );
    mapSettings.setLayers( allLayers );
  }

  mapSettings.setExpressionContext( expressionContext );

  mJobOverscan = mOverscan;

  if ( mOverscan )
  {
    // render a square as big as the diagonal of the canvas (same scale, same center), so the picture covers
    // the canvas also when it is rotated by any angle
    const QSize size = mapSettings.outputSize();
    const int side = static_cast<int>( std::ceil( std::hypot( size.width(), size.height() ) ) );
    const QgsPointXY center = mapSettings.extent().center();
    const double halfSize = side * mapSettings.mapUnitsPerPixel() / 2;

    mapSettings.setOutputSize( QSize( side, side ) );
    mapSettings.setExtent( QgsRectangle( center.x() - halfSize, center.y() - halfSize, center.x() + halfSize, center.y() + halfSize ) );
  }

  // enables on-the-fly simplification of geometries to spend less time rendering
  mapSettings.setFlag( Qgis::MapSettingsFlag::UseRenderingOptimization );
  // with incremental rendering - enables updates of partially rendered layers (good for WMTS, XYZ layers)
  // (the picture is not replaced by partial results when the rendering was started by the rotation)
  mRotationRender = mRotationRenderRequested;
  mRotationRenderRequested = false;
  const bool partialOutput = mIncrementalRendering && !mRotationRender;
  mapSettings.setFlag( Qgis::MapSettingsFlag::RenderPartialOutput, partialOutput );

  // create the renderer job
  Q_ASSERT( !mJob );
  mJob = new QgsMapRendererParallelJob( mapSettings );

  if ( partialOutput )
    mMapUpdateTimer.start();

  connect( mJob, &QgsMapRendererJob::renderingLayersFinished, this, &InputMapCanvasMap::renderJobUpdated );
  connect( mJob, &QgsMapRendererJob::finished, this, &InputMapCanvasMap::renderJobFinished );
  mJob->setCache( mCache.get() );

  mJob->start();

  if ( !mSilentRefresh )
  {
    emit renderStarting();
  }
}

void InputMapCanvasMap::setImageFromJob()
{
  mImage = mJob->renderedImage();
  mImageMapSettings = mJob->mapSettings();

  if ( mImageOverscan != mJobOverscan )
  {
    mImageOverscan = mJobOverscan;
    emit overscanReadyChanged();
  }
}

void InputMapCanvasMap::renderJobUpdated()
{
  if ( !mJob || mRotationRender )
    return;

  setImageFromJob();
  mDirty = true;
  // Temporarily freeze the canvas, we only need to reset the geometry but not trigger a repaint
  bool freeze = mFreeze;
  mFreeze = true;
  updateTransform();
  mFreeze = freeze;

  update();
}

void InputMapCanvasMap::renderJobFinished()
{
  if ( !mJob )
    return;

  const QgsMapRendererJob::Errors errors = mJob->errors();
  for ( const QgsMapRendererJob::Error &error : errors )
  {
    QgsMessageLog::logMessage( QStringLiteral( "%1 :: %2" ).arg( error.layerID, error.message ), QStringLiteral( "Rendering" ) );
  }

  // take labeling results before emitting renderComplete, so labeling map tools
  // connected to signal work with correct results
  delete mLabelingResults;
  mLabelingResults = mJob->takeLabelingResults();

  setImageFromJob();

  // now we are in a slot called from mJob - do not delete it immediately
  // so the class is still valid when the execution returns to the class
  mJob->deleteLater();
  mJob = nullptr;
  mDirty = true;
  mMapUpdateTimer.stop();

  // Temporarily freeze the canvas, we only need to reset the geometry but not trigger a repaint
  bool freeze = mFreeze;
  mFreeze = true;
  updateTransform();
  mFreeze = freeze;

  update();
  if ( !mSilentRefresh )
  {
    emit mapCanvasRefreshed();
  }
  else
  {
    mSilentRefresh = false;
  }

  if ( mDeferredRefreshPending )
  {
    mDeferredRefreshPending = false;
    mSilentRefresh = true;
    refresh();
  }
  else if ( mRotationRender )
  {
    // the map could have been rotated more while it was rendered
    checkRotationRender();
  }
}

void InputMapCanvasMap::layerRepaintRequested( bool deferred )
{
  if ( mMapSettings->outputSize().isNull() )
    return; // the map image size has not been set yet

  if ( !mFreeze )
  {
    if ( deferred )
    {
      if ( !mJob )
      {
        mSilentRefresh = true;
        refresh();
      }
      else
      {
        mDeferredRefreshPending = true;
      }
    }
    else
    {
      refresh();
    }
  }
}

void InputMapCanvasMap::onWindowChanged( QQuickWindow *window )
{
  if ( mWindow == window )
    return;

  if ( mWindow )
    disconnect( mWindow, &QQuickWindow::screenChanged, this, &InputMapCanvasMap::onScreenChanged );

  if ( window )
  {
    connect( window, &QQuickWindow::screenChanged, this, &InputMapCanvasMap::onScreenChanged );
    onScreenChanged( window->screen() );
  }

  mWindow = window;
}

void InputMapCanvasMap::onScreenChanged( QScreen *screen )
{
  if ( screen )
  {
    if ( screen->devicePixelRatio() > 0 )
    {
      mMapSettings->setDevicePixelRatio( screen->devicePixelRatio() );
    }
    mMapSettings->setOutputDpi( screen->physicalDotsPerInch() );
    // Re-apply logical size so setOutputSize re-scales it with the new DPR
    mMapSettings->setOutputSize( boundingRect().size().toSize() );
    refresh();
  }
}

void InputMapCanvasMap::onExtentChanged()
{
  updateTransform();

  if ( mRotating )
  {
    // The map is being rotated continuously (e.g. animation). The rendered picture is rotated, and the map
    // is rendered again only when needed, otherwise the running rendering would be restarted on every step.
    if ( !mFreeze )
      checkRotationRender();
    return;
  }

  // And trigger a new rendering job
  refresh();
}

void InputMapCanvasMap::checkRotationRender()
{
  if ( mFreeze || mMapSettings->outputSize().isNull() )
    return;

  const double diff = rotationDifference();
  if ( diff < 0.5 )
    return;

  if ( mJob )
  {
    // do not cancel the running rendering, wait for it. A rendering started by the rotation checks again when it is
    // finished, for the others we check when the map stops rotating
    if ( !mRotationRender )
      mRotationSettleTimer.start();
    return;
  }

  if ( diff >= mRotationRenderThreshold )
    refreshForRotation();
  else
    mRotationSettleTimer.start(); // restarted on every step, so it fires when the map stops rotating
}

void InputMapCanvasMap::onRotationSettled()
{
  if ( mFreeze || mMapSettings->outputSize().isNull() )
    return;

  if ( mJob )
  {
    // try again when the running rendering is (probably) finished
    mRotationSettleTimer.start();
    return;
  }

  if ( rotationDifference() >= 0.5 )
    refreshForRotation();
}

double InputMapCanvasMap::rotationDifference() const
{
  double diff = std::fmod( mMapSettings->rotation() - mImageMapSettings.rotation(), 360.0 );
  if ( diff > 180 )
    diff -= 360;
  else if ( diff < -180 )
    diff += 360;
  return std::abs( diff );
}

void InputMapCanvasMap::refreshForRotation()
{
  mRotationSettleTimer.stop();
  mRotationRenderRequested = true;
  mRefreshTimer.start( 1 );
}


void InputMapCanvasMap::onTemporalStateChanged()
{
  clearTemporalCache();

  // And trigger a new rendering job
  refresh();
}

void InputMapCanvasMap::updateTransform()
{
  setScale( mImageMapSettings.mapUnitsPerPixel() / mMapSettings->mapSettings().mapUnitsPerPixel() );

  // the rendered image may have been rendered with a different rotation, so we place its top left corner
  // and rotate it (around that corner, see transformOrigin) to match the current view
  const QgsPointXY imageOrigin = mImageMapSettings.mapToPixel().toMapCoordinates( 0, 0 );
  const QgsPointXY pixelPt = mMapSettings->coordinateToScreen( QgsPoint( imageOrigin ) );
  setRotation( mMapSettings->rotation() - mImageMapSettings.rotation() );
  setX( pixelPt.x() );
  setY( pixelPt.y() );
}

int InputMapCanvasMap::mapUpdateInterval() const
{
  return mMapUpdateTimer.interval();
}

void InputMapCanvasMap::setMapUpdateInterval( int mapUpdateInterval )
{
  if ( mMapUpdateTimer.interval() == mapUpdateInterval )
    return;

  mMapUpdateTimer.setInterval( mapUpdateInterval );

  emit mapUpdateIntervalChanged();
}

bool InputMapCanvasMap::incrementalRendering() const
{
  return mIncrementalRendering;
}

void InputMapCanvasMap::setIncrementalRendering( bool incrementalRendering )
{
  if ( incrementalRendering == mIncrementalRendering )
    return;

  mIncrementalRendering = incrementalRendering;
  emit incrementalRenderingChanged();
}

bool InputMapCanvasMap::overscan() const
{
  return mOverscan;
}

void InputMapCanvasMap::setOverscan( bool overscan )
{
  if ( mOverscan == overscan )
    return;

  mOverscan = overscan;

  // render the picture with the new size, it replaces the old one when it is finished
  refreshForRotation();

  emit overscanChanged();
  emit overscanReadyChanged();
}

bool InputMapCanvasMap::overscanReady() const
{
  return !mOverscan || mImageOverscan || mMapSettings->outputSize().isNull();
}

qreal InputMapCanvasMap::rotationRenderThreshold() const
{
  return mRotationRenderThreshold;
}

void InputMapCanvasMap::setRotationRenderThreshold( qreal threshold )
{
  if ( qgsDoubleNear( mRotationRenderThreshold, threshold ) )
    return;

  mRotationRenderThreshold = threshold;
  emit rotationRenderThresholdChanged();
}

bool InputMapCanvasMap::freeze() const
{
  return mFreeze;
}

void InputMapCanvasMap::setFreeze( bool freeze )
{
  if ( freeze == mFreeze )
    return;

  mFreeze = freeze;

  if ( mFreeze )
    stopRendering();
  else
    refresh();

  emit freezeChanged();
}

bool InputMapCanvasMap::isRendering() const
{
  return mJob;
}

QSGNode *InputMapCanvasMap::updatePaintNode( QSGNode *oldNode, QQuickItem::UpdatePaintNodeData * )
{
  if ( mDirty )
  {
    delete oldNode;
    oldNode = nullptr;
    mDirty = false;
  }

  QSGSimpleTextureNode *node = static_cast<QSGSimpleTextureNode *>( oldNode );
  if ( !node )
  {
    node = new QSGSimpleTextureNode();
    QSGTexture *texture = window()->createTextureFromImage( mImage );
    node->setTexture( texture );
    node->setOwnsTexture( true );
  }

  // the picture can have a different size than the canvas (overscan, resize), its top left corner is placed by updateTransform()
  QSizeF size = mImage.size();
  if ( !size.isEmpty() )
    size /= mMapSettings->devicePixelRatio();

  const QRectF rect( QPointF( 0, 0 ), size );

  node->setRect( rect );

  return node;
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
void InputMapCanvasMap::geometryChanged( const QRectF &newGeometry, const QRectF &oldGeometry )
{
  QQuickItem::geometryChanged( newGeometry, oldGeometry );
#else
void InputMapCanvasMap::geometryChange( const QRectF &newGeometry, const QRectF &oldGeometry )
{
  QQuickItem::geometryChange( newGeometry, oldGeometry );
#endif
  if ( newGeometry.size() != oldGeometry.size() )
  {
    mMapSettings->setOutputSize( newGeometry.size().toSize() );

    // the size changed (e.g. the screen was rotated), the old image does not cover the canvas anymore,
    // so render even if the rendering is frozen (e.g. while the map is rotating)
    if ( !mMapSettings->outputSize().isNull() )
      mRefreshTimer.start( 1 );
  }
}

void InputMapCanvasMap::onLayersChanged()
{
  if ( mMapSettings->extent().isEmpty() )
    zoomToFullExtent();

  for ( const QMetaObject::Connection &conn : std::as_const( mLayerConnections ) )
  {
    disconnect( conn );
  }
  mLayerConnections.clear();

  const QList<QgsMapLayer *> layers = mMapSettings->layers();
  for ( QgsMapLayer *layer : layers )
  {
    mLayerConnections << connect( layer, &QgsMapLayer::repaintRequested, this, &InputMapCanvasMap::layerRepaintRequested );
  }

  refresh();
}

void InputMapCanvasMap::destroyJob( QgsMapRendererJob *job )
{
  job->cancel();
  job->deleteLater();
}

void InputMapCanvasMap::stopRendering()
{
  if ( mJob )
  {
    mMapUpdateTimer.stop();

    disconnect( mJob, &QgsMapRendererJob::renderingLayersFinished, this, &InputMapCanvasMap::renderJobUpdated );
    disconnect( mJob, &QgsMapRendererJob::finished, this, &InputMapCanvasMap::renderJobFinished );

    if ( !mJob->isActive() )
      mJob->deleteLater();
    else
      connect( mJob, &QgsMapRendererJob::finished, mJob, &QObject::deleteLater );

    mJob->cancelWithoutBlocking();
    mJob = nullptr;
  }
}

void InputMapCanvasMap::zoomToFullExtent()
{
  QgsRectangle extent;
  const QList<QgsMapLayer *> layers = mMapSettings->layers();
  for ( QgsMapLayer *layer : layers )
  {
    if ( mMapSettings->destinationCrs() != layer->crs() )
    {
      QgsCoordinateTransform transform( layer->crs(), mMapSettings->destinationCrs(), mMapSettings->transformContext() );
      try
      {
        extent.combineExtentWith( transform.transformBoundingBox( layer->extent() ) );
      }
      catch ( const QgsCsException &exp )
      {
        // Ignore extent if it can't be transformed
      }
    }
    else
    {
      extent.combineExtentWith( layer->extent() );
    }
  }
  mMapSettings->setExtent( extent );

  refresh();
}

void InputMapCanvasMap::refresh()
{
  if ( mMapSettings->outputSize().isNull() )
    return; // the map image size has not been set yet

  if ( !mFreeze )
  {
    mRotationRenderRequested = false;
    mRefreshTimer.start( 1 );
  }
}

void InputMapCanvasMap::clearCache()
{
  if ( mCache )
    mCache->clear();
}

void InputMapCanvasMap::clearTemporalCache()
{
  if ( mCache )
  {
    bool invalidateLabels = false;
    const QList<QgsMapLayer *> layerList = mMapSettings->mapSettings().layers();
    for ( QgsMapLayer *layer : layerList )
    {
      if ( layer->temporalProperties() && layer->temporalProperties()->isActive() )
      {
        if ( QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( layer ) )
        {
          if ( vl->labelsEnabled() || vl->diagramsEnabled() )
            invalidateLabels = true;
        }

        if ( layer->temporalProperties()->flags() & QgsTemporalProperty::FlagDontInvalidateCachedRendersWhenRangeChanges )
          continue;

        mCache->invalidateCacheForLayer( layer );
      }
    }

    if ( invalidateLabels )
    {
      mCache->clearCacheImage( QStringLiteral( "_labels_" ) );
      mCache->clearCacheImage( QStringLiteral( "_preview_labels_" ) );
    }
  }
}

